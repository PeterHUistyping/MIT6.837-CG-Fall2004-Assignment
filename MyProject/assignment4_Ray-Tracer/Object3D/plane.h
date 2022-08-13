#ifndef _PLANE_H_
#define _PLANE_H_

#include "object3d.h"
#include "camera.h"
#include <math.h>
class Plane : public Object3D
{
public:
     Plane(Vec3f &n, float dd, Material *mat)
     {
          normal = n;
          d = dd;
          m = mat;
     }
     ~Plane()
     {
     }
     bool intersect(const Ray &r, Hit &h, float tmin)
     {
          Vec3f n = normal;

          float denominator = normal.Dot3(r.getDirection()); // denominator

          if (abs(denominator) < 1e-6) // ray origin outside the shpere tm
               return false;

          float t = (d - normal.Dot3(r.getOrigin())) / denominator; // P`n=d;

          if (t >= tmin && t < h.getT()) // update hit
          {
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
          Vec3f v, b1, b2, a, b, c, e, orgin_p, o;
          if (fabs(normal.y()) < 1e-6 && fabs(normal.z()) < 1e-6) // parallel to x axis
               v.Set(0, 1, 0);
          else
               v.Set(1, 0, 0);
          Vec3f::Cross3(b1, normal, v);
          b1.Normalize();
          Vec3f::Cross3(b2, normal, b1);
          b2.Normalize();
          float t = d / normal.Length() / normal.Length(); // d-o*normal/...
          orgin_p = t * normal;                            // o+..
          float big = 1e6;
          a = orgin_p - b1 * big + b2 * big;
          b = orgin_p - b1 * big - b2 * big;
          c = orgin_p + b1 * big - b2 * big;
          e = orgin_p + b1 * big + b2 * big;
          Vec3f n = normal;
          n.Normalize();
          glBegin(GL_QUADS);
          glNormal3f(n.x(), n.y(), n.z()); // first
          glVertex3f(a.x(), a.y(), a.z());
          glVertex3f(b.x(), b.y(), b.z());
          glVertex3f(c.x(), c.y(), c.z());
          glVertex3f(e.x(), e.y(), e.z());
          glEnd();
     }

private:
     Vec3f normal;
     float d;
};
#endif