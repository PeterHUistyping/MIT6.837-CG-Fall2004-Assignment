#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "object3d.h"
#include "camera.h"
#include "grid.h"
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
        Vec3f Left(min(min(a.x(), b.x()), c.x()), min(min(a.y(), b.y()), c.y()), min(min(a.z(), b.z()), c.z()));
        Vec3f Right(max(max(a.x(), b.x()), c.x()), max(max(a.y(), b.y()), c.y()), max(max(a.z(), b.z()), c.z()));
        box = new BoundingBox(Left, Right); // exist later
    }

    ~Triangle()
    {
        delete[] m;
        m = nullptr;
        delete[] box;
        box = nullptr;
    }

    BoundingBox *getBoundingBox()
    {
        return box;
    }
    void insertIntoGrid(Grid *g, Matrix *m)
    {
        m_ = m;
        Vec3f a_ = a, b_ = b, c_ = c;
        if (m != NULL)
        {
            m_ = m;
            m->Transform(a_);
            m->Transform(b_);
            m->Transform(c_);
        }
        Vec3f Left(min(min(a_.x(), b_.x()), c_.x()), min(min(a_.y(), b_.y()), c_.y()), min(min(a_.z(), b_.z()), c_.z()));
        Vec3f Right(max(max(a_.x(), b_.x()), c_.x()), max(max(a_.y(), b_.y()), c_.y()), max(max(a_.z(), b_.z()), c_.z()));
        Vec3f increment = g->getincrement_halfCell();
        float x = increment.x(), y = increment.y(), z = increment.z();
        Vec3f point;
        for (int i = 0; i < g->nx; i++)
        {
            for (int j = 0; j < g->ny; j++)
            {
                for (int k = 0; k < g->nz; k++)
                {
                    point = g->getCenterPosition(i, j, k);

                    if (point.x() >= Left.x() - x && point.y() >= Left.y() - y && point.z() >= Left.z() - z &&
                        point.x() <= Right.x() + x && point.y() <= Right.y() + y && point.z() <= Right.z() + z)
                    {
                        // g->array[i][j][k] = true; // add+  no need to else false
                        g->array_obj[i][j][k].addObject(this);
                    }
                }
            }
        }
    }
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

        if (!(fabs(s) >= 1e-6))
            return false;

        assert(fabs(s) >= 1e-6); // NAN
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