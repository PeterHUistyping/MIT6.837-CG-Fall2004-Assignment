#ifndef _GROUP_H_
#define _GROUP_H_

#include "object3d.h"
#include "sphere.h"
#include "triangle.h"
#include "plane.h"
#include "grid.h"

class Group : public Object3D
{
public:
    Group(int number)
    {
        const int num = number;
        numObjects = num;
        group = new Object3D *[num];
    }
    ~Group()
    {
        delete[] group;
        group = nullptr;
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
        for (int i = 0; i < numObjects; i++)
        {
            group[i]->insertIntoGrid(g, m);
        }
    }

    bool
    intersect(const Ray &r, Hit &h, float tmin)
    {
        bool inter = false;
        for (int i = 0; i < numObjects; i++)
        {
            if (group[i]->intersect(r, h, tmin))
                inter = true;
        }
        return inter;
    }
    bool intersectShadowRay(const Ray &r, Hit &h, float tmin)
    {
        bool inter = false;
        for (int i = 0; i < numObjects; i++)
        {
            if (group[i]->intersect(r, h, tmin))
            {
                inter = true;
                break;
            }
        }
        return inter;
    }

    void addObject(int index, Object3D *obj)
    {
        group[index] = obj;
        if (obj->getBoundingBox() != NULL)
        {
            if (index == 0)
            {
                box = group[index]->getBoundingBox();
            }
            else
            {
                assert(index < numObjects);
                box->Extend(group[index]->getBoundingBox());
            }
        }
    }

    void paint(void)
    {
        for (int i = 0; i < numObjects; i++)
        {
            group[i]->paint();
        }
    }

private:
    int numObjects;
    Object3D **group; // pointer ->NULL  Object3D* [num] num-1000
};

#endif