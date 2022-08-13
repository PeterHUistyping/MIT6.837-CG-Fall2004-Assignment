#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_
#include "scene_parser.h"
#include "rayTree.h"
extern float epsilon;
extern bool shadeback;
extern float epsilon_shadow;
class RayTracer
{
public:
    RayTracer()
    {
    }
    RayTracer(SceneParser *s, int max_b, float cutoff_wt, bool shad)
    {
        parser = s;
        max_bounces = max_b;
        cutoff_weight = cutoff_wt;
        shadows = shad;
    }
    ~RayTracer() {}
    Vec3f mirrorDirection(const Vec3f &normal, const Vec3f &incoming)
    {
        Vec3f out;
        out = incoming - 2 * normal.Dot3(incoming) * normal;
        out.Normalize();
        return out;
    }
    bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming, float index_i, float index_t, Vec3f &transmitted)
    {
        float nr, cosi, sini;
        nr = index_i / index_t;
        Vec3f income = incoming;
        income.Negate();
        cosi = normal.Dot3(income);
        sini = sqrt(1 - cosi * cosi);
        if (index_i > index_t && sini >= index_t / index_i) // total internal reflection
            return false;
        transmitted = (nr * cosi - sqrt(1 - nr * nr * (1 - cosi * cosi))) * normal - nr * income; // incoming opp;
        transmitted.Normalize();
        return true;
    }

    Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const
    {
        Vec3f pcolor;                                         // point_color
        if (bounces > max_bounces || weight <= cutoff_weight) // end of recursion
            return pcolor;
        RayTracer tracer_temp;
        if (parser->getGroup()->intersect(ray, hit, epsilon_shadow)) // parser->getCamera()->getTMin()
        {
            Vec3f am = parser->getAmbientLight();
            am = am * hit.getMaterial()->getDiffuseColor();
            pcolor += am; // ambient color
            indexOfRefraction = hit.getMaterial()->getIndexOfRefraction();
            RayTree::SetMainSegment(ray, 0, hit.getT());
            int lnum = parser->getNumLights();
            for (int l = 0; l < lnum; l++)
            {
                Vec3f dirToLight, lcolor, normal_temp;
                float distanceToLight = 0;
                parser->getLight(l)->getIllumination(hit.getIntersectionPoint(), dirToLight, lcolor, distanceToLight);
                if (shadows)
                {
                    Ray r2(hit.getIntersectionPoint(), dirToLight); // Shadow
                    Hit h2(distanceToLight, NULL, normal_temp);
                    if (!parser->getGroup()->intersectShadowRay(r2, h2, epsilon_shadow)) // epsilon self-shadowing
                    {                                                                    // Diffuse  Color + Specular Color
                        pcolor += hit.getMaterial()->Shade(ray, hit, dirToLight, lcolor);
                        RayTree::AddShadowSegment(r2, 0, h2.getT());
                    }
                }
                else // no -shadows mode
                {
                    pcolor += hit.getMaterial()->Shade(ray, hit, dirToLight, lcolor);
                }
            }

            if (hit.getMaterial()->getReflectiveColor().Length() != 0 && bounces < max_bounces)
            { // reflection
                Vec3f normal = hit.getNormal();
                if (shadeback)
                {
                    if (hit.getNormal().Dot3(ray.getDirection()) > 0)
                        normal.Negate(); // back shading
                }
                assert(fabs(normal.Length() - 1) < 1e-6);
                assert(fabs(ray.getDirection().Length() - 1) < 1e-6);
                Vec3f out = tracer_temp.mirrorDirection(normal, ray.getDirection());
                Ray ray_out(hit.getIntersectionPoint(), out);
                Hit h;
                pcolor += traceRay(ray_out, epsilon, bounces + 1, weight, indexOfRefraction, h) * hit.getMaterial()->getReflectiveColor();
                RayTree::AddReflectedSegment(ray_out, 0, h.getT());
            }

            if (hit.getMaterial()->getTransparentColor().Length() != 0 && bounces < max_bounces)
            { // refraction
                Vec3f transmitted;
                Vec3f normal = hit.getNormal();
                assert(fabs(hit.getNormal().Length() - 1) < 1e-6);
                assert(fabs(ray.getDirection().Length() - 1) < 1e-6);
                if (hit.getNormal().Dot3(ray.getDirection()) < 0) // air -> object
                {
                    if (shadeback)
                    {
                        if (hit.getNormal().Dot3(ray.getDirection()) > 0)
                            normal.Negate(); // back shading
                    }
                    if (tracer_temp.transmittedDirection(normal, ray.getDirection(), 1, indexOfRefraction, transmitted))
                    {
                        Ray ray_out(hit.getIntersectionPoint(), transmitted);
                        Hit h;
                        Vec3f RefractionColor;
                        RefractionColor = traceRay(ray_out, epsilon, bounces + 1, weight, indexOfRefraction, h) * hit.getMaterial()->getTransparentColor();
                        pcolor += RefractionColor;
                        RayTree::AddTransmittedSegment(ray_out, 0, h.getT());
                    }
                }
                else //  object->air
                {
                    if (shadeback)
                    {
                        if (hit.getNormal().Dot3(ray.getDirection()) > 0)
                            normal.Negate(); // back shading
                    }
                    if (tracer_temp.transmittedDirection(normal, ray.getDirection(), indexOfRefraction, 1, transmitted))
                    {
                        Ray ray_out(hit.getIntersectionPoint(), transmitted);
                        Hit h;
                        Vec3f RefractionColor;
                        RefractionColor = traceRay(ray_out, epsilon, bounces + 1, weight, 1, h) * hit.getMaterial()->getTransparentColor();
                        pcolor += RefractionColor;
                        RayTree::AddTransmittedSegment(ray_out, 0, h.getT());
                    }
                }
            }
        }
        else
        {
            pcolor = parser->getBackgroundColor();
        }

        return pcolor;
    }

private:
    SceneParser *parser;
    int max_bounces;
    float cutoff_weight;
    bool shadows;
};
#endif