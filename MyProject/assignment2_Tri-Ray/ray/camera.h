#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <assert.h>
#include "vectors.h"
#include "ray.h"
#include "hit.h"
class Camera
{
public:
    Camera() {}
    virtual ~Camera() {}
    virtual Ray generateRay(Vec2f point) = 0;
    virtual float getTMin() const = 0;

    const Vec3f &getCenter() { return center; }
    const Vec3f &getDirection() { return direction; }
    const Vec3f &getUp() { return up; }
    const Vec3f &getHorizontal() { return horizontal; }
    const float &getSize() { return size; }

protected:
    Vec3f center;
    Vec3f direction;
    Vec3f up;
    Vec3f horizontal;
    float size;
    float angle;
};
class OrthographicCamera : public Camera
{
public:
    OrthographicCamera() {}
    OrthographicCamera(const Vec3f &c, const Vec3f &dir, const Vec3f &u, float s)
    {
        center = c;
        Vec3f a1, a2, a3, b1, b2, b3, tempb1, tempb2;
        a1 = dir;
        a2 = u;
        // normalizing direction vector

        // orthonormalize
        b1 = a1;
        tempb1 = b1; // avoid (0,0,0)
        b2 = a2 - (tempb1 *= tempb1.Dot3(a2) / tempb1.Length() / tempb1.Length());
        tempb1 = b1; // redefine
        tempb2 = b2;
        assert(b1.Dot3(b2) < 1e-6);
        Vec3f::Cross3(a3, b1, b2); // right hand
        b3 = a3 - (tempb1 *= tempb1.Dot3(a3) / tempb1.Length() / tempb1.Length()) - (tempb2 *= tempb2.Dot3(a3) / tempb2.Length() / tempb2.Length());
        assert(b1.Dot3(b3) < 1e-6);
        assert(b3.Dot3(b2) < 1e-6);

        b1.Normalize();
        b2.Normalize();
        b3.Normalize();

        direction = b1;
        up = b2;
        horizontal = b3;
        size = s;
        // b2=a2- (b1*=b1.Dot3(a2)/b1.Length());
        //    b1/=b1.Dot3(a2)/b1.Length();   //cautious: b1.Dot3(a2)=(0,0,0)
        // Vec3f::Cross3(a3,b1,b2); //right hand
        // b3=a3- (b1*=b1.Dot3(a3)/b1.Length())- (b2*=b2.Dot3(a3)/b2.Length());
        //     b1*= b1.Dot3(a3)/b1.Length();       b2*= b2.Dot3(a3)/b2.Length();
    }
    ~OrthographicCamera() {}
    Ray generateRay(Vec2f point)
    {
        Vec3f Point;
        float x, y, z, xx, yy;
        xx = point.x() - 0.5; // center(0.5,0.5)
        yy = point.y() - 0.5;
        xx *= size; // /2
        yy *= size;
        Vec3f h = horizontal;
        Vec3f u = up;
        h *= xx;
        u *= yy;
        Point = center + h + u;
        Ray r(Point, direction);
        // Ray (const Vec3f &orig, const Vec3f &dir)
        return r;
    }
    float getTMin() const
    {
        return -__FLT_MAX__;
    }
};
class PerspectiveCamera : public Camera
{

public:
    PerspectiveCamera() {}
    PerspectiveCamera(const Vec3f &c, const Vec3f &dir, const Vec3f &u, float an)
    {
        center = c;
        Vec3f a1, a2, a3, b1, b2, b3, tempb1, tempb2;
        a1 = dir;
        a2 = u;
        // normalizing direction vector

        // orthonormalize
        b1 = a1;
        tempb1 = b1; // avoid (0,0,0)
        b2 = a2 - (tempb1 *= tempb1.Dot3(a2) / tempb1.Length() / tempb1.Length());
        tempb1 = b1; // redefine
        tempb2 = b2;
        assert(b1.Dot3(b2) < 1e-6);
        Vec3f::Cross3(a3, b1, b2); // right hand
        b3 = a3 - (tempb1 *= tempb1.Dot3(a3) / tempb1.Length() / tempb1.Length()) - (tempb2 *= tempb2.Dot3(a3) / tempb2.Length() / tempb2.Length());
        assert(b1.Dot3(b3) < 1e-6);
        assert(b3.Dot3(b2) < 1e-6);

        b1.Normalize();
        b2.Normalize();
        b3.Normalize();

        direction = b1;
        up = b2;
        horizontal = b3;
        angle = an;
    }
    ~PerspectiveCamera() {}
    Ray generateRay(Vec2f point)
    {
        Vec3f Point;
        float x, y, z, xx, yy;
        xx = point.x() - 0.5; // center(0.5,0.5)
        yy = point.y() - 0.5;
        xx *= size; // /2
        yy *= size;
        Vec3f h = horizontal;
        Vec3f u = up;
        h *= xx;
        u *= yy;
        Point = center + h + u;
        Ray r(Point, direction);
        // Ray (const Vec3f &orig, const Vec3f &dir)
        return r;
    }
    float getTMin() const
    {
        return 0;
    }
};
#endif
