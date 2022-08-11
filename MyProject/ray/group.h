#ifndef _GROUP_H_
#define _GROUP_H_

#include "object3d.h"
#include "sphere.h"
#include "triangle.h"
#include "plane.h"
class Group : public Object3D
{
public:
    ~Group()
    {
        delete[] group;
    }

    Group(int number)
    {
        const int num = number;
        numObjects = num;
        group = new Object3D *[num];
    }

    bool intersect(const Ray &r, Hit &h, float tmin)
    {
        bool inter = false;
        int count = 0; // debug
        for (int i = 0; i < numObjects; i++)
        {
            if (group[i]->intersect(r, h, tmin))
                inter = true, count++;
        }
        return inter;
    }

    void addObject(int index, Object3D *obj)
    {
        group[index] = obj;
    }

    void paint(void)
    {
        for (int i = 0; i < numObjects; i++)
        {

            group[i]->paint();
        }
    }

private: // int n;
    // vector <Object3D> group(n);
    int numObjects;
    Object3D **group; // pointer ->NULL  Object3D* [num] num-1000
};

#endif