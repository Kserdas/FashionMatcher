#include <iostream>
#include <algorithm>
#include <cmath>
#include "garments.h"
#include <emscripten/bind.h>
using namespace emscripten;

float calculate_compatibility(const Top& t, const Bottom& b,
                              const Top* jacket = nullptr);

EMSCRIPTEN_BINDINGS(wardrobe_module) {
    // Expose Garments base class members if needed
    class_<Garments>("Garments")
        .property("id", &Garments::id)
        .property("formal", &Garments::formal)
        .property("thermal", &Garments::thermal)
        .property("archetype", &Garments::archetype);

    // Expose Top class & constructor
    class_<Top, base<Garments>>("Top")
        .constructor<float, float, float, float, float,
                     float, float, float, float, float,
                     float, float, float, float, string, int, float>();

    // Expose Bottom class & constructor
    class_<Bottom, base<Garments>>("Bottom")
        .constructor<float, float, float, float, float,
                     float, float, float, float, float,
                     float, float, float, float, string, int, float>();

    // Expose compatibility functions to JavaScript
    emscripten::function("calculate_compatibility", &calculate_compatibility,
                         allow_raw_pointers());
}

// Helper function to calculate the Gaussian decay curve (Bell Curve)
// This translates human "sweet spots" into math.
float gaussian(float x, float mu, float sigma) {
    return std::exp(-std::pow(x - mu, 2.0f) / (2.0f * std::pow(sigma, 2.0f)));
}

// Evaluates compatibility. Pass a pointer to 'jacket' if there is a z=2 layer.
float calculate_compatibility(const Top& t, const Bottom& b, const Top* jacket) {
    
    // =========================================================
    // PHASE 1: HARD VETO GATES (Prune impossible/terrible fits)
    // =========================================================
    float formal_diff = std::abs(t.formal - b.formal);
    if (formal_diff > 0.5f) return 0.0f; // Semantic clash (e.g., tuxedo + sweatpants)

    float thermal_diff = std::abs(t.thermal - b.thermal);
    if (thermal_diff > 1.2f) return 0.0f; // Seasonal clash (e.g., winter coat + bikini)

    // =========================================================
    // PHASE 2: CALCULATE PSYCHOLOGICAL SUB-SCORES [0.0 to 1.0]
    // =========================================================

    // 1. Morphology (Proportions & Golden Ratio)
    // Optimal when top.length + b.waist == 0
    float s_prop = gaussian(t.length + b.waist, 0.0f, 0.4f);
    // Silhouette balance (Inverse)
    float s_vol = gaussian(t.silhouette + b.silhouette, 0.0f, 0.5f);
    float s_morph = (s_prop * 0.6f) + (s_vol * 0.4f);

    // 2. Chromatics (Color Wheel Angular Harmony)
    float hue_diff = std::abs(t.color - b.color);
    float delta_theta = std::min(hue_diff, 360.0f - hue_diff);
    
    // Check against Monochromatic (0), Analogous (30), Complementary (180)
    float s_hue = std::max({
        gaussian(delta_theta, 0.0f, 15.0f),         // Monochromatic
        0.90f * gaussian(delta_theta, 30.0f, 10.0f), // Analogous
        0.95f * gaussian(delta_theta, 180.0f, 20.0f) // Complementary
    });
    
    // Undertone/Temp alignment
    float s_temp = 1.0f - (std::abs(t.temp - b.temp) / 2.0f);
    float s_chroma = (s_hue * 0.7f) + (s_temp * 0.3f);

    // 3. Materiality & Texture (Wundt Curve - rewards moderate contrast)
    // Peak harmony when difference is around 0.35
    float s_texture = gaussian(std::abs(t.texture - b.texture), 0.35f, 0.25f);

    // 4. Visual Complexity (Pattern Clash)
    // Power of 1.5 allows one micro-print and one macro-print, but penalizes two busy prints.
    float s_pattern = 1.0f - std::pow(t.pattern * b.pattern, 1.5f);

    // 5. Context & Formality
    // Smooth plateau for slight style mixing, sharp drop-off for extremes
    float s_formality = 1.0f - std::pow(formal_diff, 2.5f);

    // --- BASE 2-PIECE SCORE ---
    float base_score = (s_morph * 0.25f) + 
                       (s_chroma * 0.25f) + 
                       (s_texture * 0.15f) + 
                       (s_pattern * 0.15f) + 
                       (s_formality * 0.20f);

    // =========================================================
    // PHASE 3: LAYERED LOGIC (If a Jacket / z=2 is provided)
    // =========================================================
    if (jacket != nullptr) {
        // Physical Layering Checks
        if (jacket->z <= t.z) return 0.0f; // Jacket must be an outer layer
        if (jacket->weight < t.weight - 0.1f) return 0.0f; // Outerwear must be heavier/equal

        // Evaluate the Jacket with the Bottom
        float jacket_bottom_score = calculate_compatibility(*jacket, b, nullptr);

        // Evaluate Jacket interacting with the Inner Top (Patterns & Textures)
        float jacket_top_pattern = 1.0f - std::pow(t.pattern * jacket->pattern, 1.5f);
        float jacket_top_hue_diff = std::abs(t.color - jacket->color);
        float jacket_top_theta = std::min(jacket_top_hue_diff, 360.0f - jacket_top_hue_diff);
        float jacket_top_color = std::max({
            gaussian(jacket_top_theta, 0.0f, 15.0f),
            0.9f * gaussian(jacket_top_theta, 30.0f, 10.0f),
            0.95f * gaussian(jacket_top_theta, 180.0f, 20.0f)
        });

        // The jacket dictates the final silhouette, so it absorbs much of the weight
        float final_layered_score = (base_score * 0.3f) + 
                                    (jacket_bottom_score * 0.5f) + 
                                    (jacket_top_pattern * 0.1f) + 
                                    (jacket_top_color * 0.1f);
                                    
        return std::clamp(final_layered_score, 0.0f, 1.0f);
    }

    return std::clamp(base_score, 0.0f, 1.0f);
}