#pragma once

#include <iostream>
#include <algorithm>
#include <string>
using namespace std;

class Garments {
    private:
        inline static int nextId = 1;

    public:
        Garments()
            : Garments(0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                       0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                       0.0f, 0.0f, 0.0f, 0.0f, "", 0, 0.0f) {}

        float neckline;
        float sleeve;
        float length;
        float waist;
        float silhouette;
        float color;
        float temp;
        float reflect;
        float weight;
        float rigid;
        float texture;
        float pattern;
        float hardware;
        float formal;
        string archetype;
        int z;
        float thermal;
        const int id;
        Garments(float n, float slv, float len, float wst, float sil,
            float col, float tmp, float ref,
            float wgt, float rgd, float txt,
            float pat, float hrd,
            float frm, string arch, int z_tier, float thm) : id(nextId++) ,
            neckline(clamp(n, 0.0f, 1.0f)),
            sleeve(clamp(slv, -1.0f, 1.0f)),
            length(clamp(len, -1.0f, 1.0f)),
            waist(clamp(wst, -1.0f, 1.0f)),
            silhouette(clamp(sil, -1.0f, 1.0f)),
            color(clamp(col, 0.0f, 360.0f)),
            temp(clamp(tmp, -1.0f, 1.0f)),
            reflect(clamp(ref, 0.0f, 1.0f)),
            weight(clamp(wgt, 0.0f, 1.0f)),
            rigid(clamp(rgd, -1.0f, 1.0f)),
            texture(clamp(txt, 0.0f, 1.0f)),
            pattern(clamp(pat, 0.0f, 1.0f)),
            hardware(clamp(hrd, 0.0f, 1.0f)),
            formal(clamp(frm, 0.0f, 1.0f)),
            archetype(arch),
            z(clamp(z_tier, 0, 3)),
            thermal(clamp(thm, -1.0f, 1.0f)) {}
        };

class Bottom : public Garments{
    public:
        using Garments::Garments;

        Bottom(){
            neckline = 0.0;
            sleeve = 0.0;
        };

};
class Top : public Garments{
    public:
        using Garments::Garments;

        Top(){
            waist = 0.0;
        };
};
