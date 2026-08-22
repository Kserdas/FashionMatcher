#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;
#include "garments.h"
#include "compatibility.cpp"

struct Match {
    const Top* inner_top;
    const Top* jacket;
    const Bottom* bottom;
    float score;
};

vector<Match> find_matches(const vector<Top>& tops,
                           const vector<Bottom>& bottoms,
                           float minimum_score) {
    vector<Match> matches;
    for (const Bottom& bottom : bottoms) {
        for (const Top& top : tops) {
            float score = calculate_compatibility(top, bottom);
            if (score >= minimum_score) {
                matches.push_back({&top, nullptr, &bottom, score});
            }

            for (const Top& jacket : tops) {
                if (top.id == jacket.id) continue;
                score = calculate_compatibility(top, bottom, &jacket);
                if (score >= minimum_score) {
                    matches.push_back({&top, &jacket, &bottom, score});
                }
            }
        }
    }

    sort(matches.begin(), matches.end(), [](const Match& left, const Match& right) {
        return left.score > right.score;
    });
    return matches;
}

void print_matches(const vector<Match>& matches) {
    cout << fixed << setprecision(1);
    if (matches.empty()) {
        cout << "No matches reached the minimum score.\n";
        return;
    }

    for (const Match& match : matches) {
        cout << "Score: " << match.score * 100.0f << "% | Top #"
             << match.inner_top->id << " (" << match.inner_top->archetype << ")";
        if (match.jacket != nullptr) {
            cout << " + Jacket #" << match.jacket->id << " ("
                 << match.jacket->archetype << ")";
        }
        cout << " + Bottom #" << match.bottom->id << " ("
             << match.bottom->archetype << ")\n";
    }
}


int main() {
    vector<Top> tops = {
        Top(0.5f, 0.0f, 0.2f, 0.0f, -0.2f, 20.0f, 0.2f, 0.2f,
            0.2f, 0.0f, 0.3f, 0.1f, 0.0f, 0.4f, "shirt", 1, 0.1f),
        Top(0.4f, 0.0f, 0.0f, 0.0f, 0.1f, 45.0f, 0.1f, 0.1f,
            0.3f, 0.0f, 0.5f, 0.2f, 0.0f, 0.5f, "blouse", 1, 0.1f),
        Top(0.3f, 0.0f, 0.1f, 0.0f, 0.0f, 200.0f, -0.2f, 0.0f,
            0.7f, 0.1f, 0.6f, 0.0f, 0.0f, 0.7f, "jacket", 2, 0.0f)
    };

    vector<Bottom> bottoms = {
        Bottom(0.0f, 0.0f, -0.2f, 0.2f, 0.1f, 210.0f, -0.1f, 0.0f,
               0.5f, 0.1f, 0.4f, 0.1f, 0.0f, 0.5f, "jeans", 0, 0.0f),
        Bottom(0.0f, 0.0f, -0.1f, 0.0f, 0.0f, 30.0f, 0.2f, 0.1f,
               0.3f, 0.0f, 0.3f, 0.0f, 0.0f, 0.4f, "trousers", 0, 0.1f)
    };

    const float minimum_score = 0.55f;
    vector<Match> matches = find_matches(tops, bottoms, minimum_score);
    cout << "Matches scoring at least " << minimum_score * 100.0f << "%:\n";
    print_matches(matches);
    


    return 0;
}