#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "object3d.h"
#include "camera.h"
#include <math.h>
extern bool isgouraud;
extern float P_steps, T_steps;
class Sphere : public Object3D
{
public:
    Sphere() {}
    ~Sphere() {}
    Sphere(const Vec3f &c, const float &rad, Material *mat)
    {
        center = c;
        radius = rad;
        m = mat;
    }
    bool intersectG(const Ray &r, Hit &h, float tmin)
    {
        // Geometric
        Vec3f R0 = r.getOrigin() - center;
        float tp = -R0.Dot3(r.getDirection());
        float d2 = R0.Length() * R0.Length() - tp * tp;
        float tt2 = radius * radius - d2;

        float t;
        float tt = sqrt(tt2);
        float tM = tp + tt;
        float tm = tp - tt;

        if (R0.Length() > radius) // ray origin outside the shpere tm
        {
            if (tt2 < 0 || tp < 0) // d2>radius*radius
                return false;
            t = tm;
        }

        else if (R0.Length() == radius) // ray origin on the shpere tp
            t = tp;

        else if (R0.Length() < radius) // ray origin inside the shpere tM
            t = tM;

        if (t >= tmin && t < h.getT()) // update hit
        {
            Vec3f n; // Normal
            Vec3f point;
            h.set(t, m, n, r);
            point = h.getIntersectionPoint();
            n = point - center;
            n.Normalize();
            h.set(t, m, n, r);
            return true;
        }
        else
            return false;
    }
    bool intersect(const Ray &r, Hit &h, float tmin)
    {
        // Algebraic
        Vec3f R0 = r.getOrigin() - center;
        float a = 1;
        float b = 2 * R0.Dot3(r.getDirection());
        float c = R0.Length() * R0.Length() - radius * radius;
        float delta = b * b - 4 * a * c;

        if (delta < 0) // ray origin outside the shpere tm
            return false;
        delta = sqrt(delta);
        float tm = (-b - delta) / 2.0 / a;
        float tM = (-b + delta) / 2.0 / a;
        float t;
        if (tM < 0)
            return false; // tM is positive
        if (tm > 0)
            t = tm; // closest positive
        else
            t = tM;

        if (t > tmin && t < h.getT()) // update hit
        {
            Vec3f n; // Normal
            Vec3f point;
            h.set(t, m, n, r);
            point = h.getIntersectionPoint();
            n = point - center;
            n.Normalize();
            h.set(t, m, n, r);
            return true;
        }
        else
            return false;
    }

    void paint(void)
    {
        m->glSetMaterial();
        const int array_num = 360 / T_steps + 1;
        Vec3f point[array_num], pre;
        for (float iPhi = -90 + P_steps; iPhi < 90; iPhi += P_steps)
        {
            for (float iTheta = 0; iTheta <= 360; iTheta += T_steps)
            // compute appropriate coordinates & normals
            {
                Vec3f x(1, 0, 0), y(0, 0, 1), z(0, 1, 0);
                Vec3f a, b, c, e, xt, yt, zt;
                zt = sin(iPhi * 3.1415926 / 180.0) * z;
                float ztt = cos(iPhi * 3.1415926 / 180.0);
                xt = cos(iTheta * 3.1415926 / 180.0) * ztt * x;
                yt = sin(iTheta * 3.1415926 / 180.0) * ztt * y;
                a = center + radius * (zt + xt + yt);

                int array_i = iTheta / T_steps;
                assert(array_i < array_num);

                if (iTheta == 0) // beginning
                {
                    point[array_num - 1] = pre; // sequence
                    pre = a;
                    continue;
                }
                Vec3f normal, bb, aa, cc, ee;
                if (iPhi == -90 + P_steps)
                {
                    b = pre;
                    c = center;
                    c -= radius * z;
                    bb = b, aa = a, cc = c;
                    Vec3f::Cross3(normal, bb - aa, cc - aa); // normal=ab*bc
                    normal.Normalize();
                    if (!isgouraud)
                    {
                        glBegin(GL_TRIANGLES);
                        glNormal3f(normal.x(), normal.y(), normal.z()); // first
                        glVertex3f(a.x(), a.y(), a.z());
                        glVertex3f(b.x(), b.y(), b.z());
                        glVertex3f(c.x(), c.y(), c.z());
                        glEnd();
                    }
                    else // gouraud
                    {
                        glBegin(GL_TRIANGLES);
                        aa = a, bb = b, cc = c, ee = e;
                        normal = aa - center;
                        normal.Normalize();
                        normal.Negate();
                        glNormal3f(normal.x(), normal.y(), normal.z()); // first
                        glVertex3f(a.x(), a.y(), a.z());

                        normal = bb - center;
                        normal.Normalize();
                        normal.Negate();
                        glNormal3f(normal.x(), normal.y(), normal.z()); // first
                        glVertex3f(b.x(), b.y(), b.z());

                        normal = cc - center;
                        normal.Normalize();
                        normal.Negate();
                        glNormal3f(normal.x(), normal.y(), normal.z()); // first
                        glVertex3f(c.x(), c.y(), c.z());
                        glEnd();
                    }
                }
                else
                {
                    b = pre;
                    c = point[array_i - 1];
                    e = point[array_i];
                    bb = b, aa = a, cc = c, ee = e;
                    Vec3f::Cross3(normal, bb - aa, cc - aa); // normal=ab*bc
                    normal.Normalize();

                    if (!isgouraud)
                    {
                        glBegin(GL_QUADS);
                        glNormal3f(normal.x(), normal.y(), normal.z()); // first
                        glVertex3f(a.x(), a.y(), a.z());
                        glVertex3f(b.x(), b.y(), b.z());
                        glVertex3f(c.x(), c.y(), c.z());
                        glVertex3f(e.x(), e.y(), e.z());
                        glEnd();
                    }
                    else // gouraud
                    {
                        glBegin(GL_QUADS);
                        aa = a, bb = b, cc = c, ee = e;
                        normal = aa - center;
                        normal.Normalize();
                        normal.Negate();
                        glNormal3f(normal.x(), normal.y(), normal.z()); // first
                        glVertex3f(a.x(), a.y(), a.z());

                        normal = bb - center;
                        normal.Normalize();
                        normal.Negate();
                        glNormal3f(normal.x(), normal.y(), normal.z()); // first
                        glVertex3f(b.x(), b.y(), b.z());

                        normal = cc - center;
                        normal.Normalize();
                        normal.Negate();
                        glNormal3f(normal.x(), normal.y(), normal.z()); // first
                        glVertex3f(c.x(), c.y(), c.z());

                        normal = ee - center;
                        normal.Normalize();
                        normal.Negate();
                        glNormal3f(normal.x(), normal.y(), normal.z()); // first
                        glVertex3f(e.x(), e.y(), e.z());
                        glEnd();
                    }
                    if (iPhi == 90 - P_steps) // both
                    {
                        b = pre;
                        c = center;
                        c += radius * z;
                        bb = b, aa = a, cc = c;
                        Vec3f::Cross3(normal, bb - aa, cc - aa); // normal=ab*bc
                        normal.Normalize();
                        if (!isgouraud)
                        {
                            glBegin(GL_TRIANGLES);
                            glNormal3f(normal.x(), normal.y(), normal.z()); // first
                            glVertex3f(a.x(), a.y(), a.z());
                            glVertex3f(b.x(), b.y(), b.z());
                            glVertex3f(c.x(), c.y(), c.z());
                            glEnd();
                        }
                        else // gouraud
                        {
                            glBegin(GL_TRIANGLES);
                            aa = a, bb = b, cc = c, ee = e;
                            normal = aa - center;
                            normal.Normalize();
                            glNormal3f(normal.x(), normal.y(), normal.z()); // first
                            glVertex3f(a.x(), a.y(), a.z());

                            normal = bb - center;
                            normal.Normalize();
                            glNormal3f(normal.x(), normal.y(), normal.z()); // first
                            glVertex3f(b.x(), b.y(), b.z());

                            normal = cc - center;
                            normal.Normalize();
                            glNormal3f(normal.x(), normal.y(), normal.z()); // first
                            glVertex3f(c.x(), c.y(), c.z());
                            glEnd();
                        }
                    }
                }

                point[array_i - 1] = pre;
                pre = a; // updated
            }
        }
    }

private:
    Vec3f center;
    float radius;
};
#endif