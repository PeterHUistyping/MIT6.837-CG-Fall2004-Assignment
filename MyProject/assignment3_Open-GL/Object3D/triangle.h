#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "object3d.h"
#include "camera.h"
#include <math.h>

class Triangle : public Object3D
{
public:
    Triangle(Vec3f &aa, Vec3f &bb, Vec3f &cc, Material *mat)
    {
        a = aa, b = bb, c = cc, m = mat;

        Vec3f::Cross3(normal, bb - aa, cc - aa); // normal=ab*bc
        normal.Normalize();
        // normal.Write(); // debug
        // cautious:bb&cc have been modified.
    }

    ~Triangle() {}

    float det2(float a, float b,
               float c, float d)
    {
        return a * d - b * c;
    }

    float det3(float a1, float a2, float a3,
               float b1, float b2, float b3,
               float c1, float c2, float c3)
    {
        return a1 * det2(b2, b3, c2, c3) - b1 * det2(a2, a3, c2, c3) + c1 * det2(a2, a3, b2, b3);
    }

    bool intersect(const Ray &r, Hit &h, float tmin)
    {
        // Changed
        float t, s, x, y, z;
        Vec3f n = normal;
        if (n.Dot3(r.getDirection()) > 0)
            n.Negate(); // back shading

        //  s=det3(1,1,1,
        //           b.x()-a.x(),b.y()-a.y(),b.z()-a.z(),
        //           c.x()-a.x(),c.y()-a.y(),c.z()-a.z());
        //  s/=2;
        //  s=fabs(s);
        s = det3(a.x() - b.x(), a.x() - c.x(), r.getDirection().x(),
                 a.y() - b.y(), a.y() - c.y(), r.getDirection().y(),
                 a.z() - b.z(), a.z() - c.z(), r.getDirection().z());

        x = det3(a.x() - r.getOrigin().x(), a.x() - c.x(), r.getDirection().x(),
                 a.y() - r.getOrigin().y(), a.y() - c.y(), r.getDirection().y(),
                 a.z() - r.getOrigin().z(), a.z() - c.z(), r.getDirection().z());

        y = det3(a.x() - b.x(), a.x() - r.getOrigin().x(), r.getDirection().x(),
                 a.y() - b.y(), a.y() - r.getOrigin().y(), r.getDirection().y(),
                 a.z() - b.z(), a.z() - r.getOrigin().z(), r.getDirection().z());

        z = det3(a.x() - b.x(), a.x() - c.x(), a.x() - r.getOrigin().x(),
                 a.y() - b.y(), a.y() - c.y(), a.y() - r.getOrigin().y(),
                 a.z() - b.z(), a.z() - c.z(), a.z() - r.getOrigin().z());

        if (fabs(s) < 1e-6)
            return false;
        assert(fabs(s) >= 1e-6);
        float w, p, q;
        p = x / s;
        q = y / s;
        w = 1 - p - q;
        t = z / s;

        if (p <= 0 || q <= 0 || w <= 0) // ray origin outside the shpere tm
        {
            return false;
        }
        if (t >= tmin && t < h.getT()) // update hit
        {
            h.set(t, m, n, r);
            return true;
        }
        else
            return false;
    }

    void paint(void)
    {
        m->glSetMaterial();
        glBegin(GL_TRIANGLES);
        glNormal3f(normal.x(), normal.y(), normal.z()); // first
        glVertex3f(a.x(), a.y(), a.z());
        glVertex3f(b.x(), b.y(), b.z());
        glVertex3f(c.x(), c.y(), c.z());
        glEnd();
    }

private:
    Vec3f a;
    Vec3f b;
    Vec3f c;
    Vec3f normal; // cache
};
#endif