#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ifs.h"
#include "matrix.h"
#include "image.h"
void IFS::input(int i, const float &probability, const Matrix &m)
{
    pro[i] = probability;
    trans[i] = m;
}

void IFS::render(Image &image, const int &num_points, const int &num_iters)
{
    double r = drand48();
    double lower = 0;
    int flag = 0, st = 0;
    Vec3f color(0, 255, 0);
    for (int j = 0; j < num_points; j++)
    {

        Vec2f vec(drand48(), drand48());
        for (int k = 0; k < num_iters; k++)
        {
            lower = 0;
            flag = 0, st = 0;
            for (int i = 0; i < num; i++)
            {
                r = drand48();
                if (r >= lower && r < lower + pro[i])
                {
                    flag = i;
                    st = 1;
                }
                lower += pro[i];
                if (flag == i && st == 1)
                    break;
            }

            trans[flag].Transform(vec); // void Transform(Vec2f &v);
        }
        if (vec.x() >= 0 && vec.x() < 1 && vec.y() >= 0 && vec.y() < 1)
            image.SetPixel(400 * vec.x(), 400 * vec.y(), color);
    }
}