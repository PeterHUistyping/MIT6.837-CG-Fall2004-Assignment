#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include "vectors.h"
#include "ray.h"
#include "hit.h"

class Object3D
{
public:
    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0; // pure virtual method

    Object3D() {}
    virtual ~Object3D() {}

protected:
    Material *m; // materials are very simple and consist of a single color - assignment1
};

#endif