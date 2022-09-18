#pragma once
#include <stdio.h>
#include "extern.h"
#include "raytracing_stats.h"
class MarchingInfo
{
public:
    MarchingInfo()
    {
        tmin = 0;
    }
    ~MarchingInfo() {}
    void setSign(int &sign_x, int &sign_y, int &sign_z)
    {
        sign.Set(sign_x, sign_y, sign_z);
    }
    void setd_t(float &dt_x, float &dt_y, float &dt_z)
    {
        d_t.Set(dt_x, dt_y, dt_z);
    }
    void setCurrentCell(int &i_, int &j_, int &k_)
    {
        i = i_, j = j_, k = k_;
    }
    void getCurrentCell(int &i_, int &j_, int &k_) const
    {
        i_ = i, j_ = j, k_ = k;
    }
    void settmin(float &tmin_)
    {
        tmin = tmin_;
    }
    void gettmin(float &tmin_) const
    {
        tmin_ = tmin;
    }
    void set_tnext(Vec3f &tnext_)
    {
        t_next = tnext_;
    }
    void getNormal(Vec3f &n) const
    {
        n = normal;
    }
    void setNormal(Vec3f &n)
    {
        normal = n;
    }
    void nextCell()
    {

        RayTracingStats::IncrementNumGridCellsTraversed();
        float t_next_min = min(min(t_next.x(), t_next.y()), t_next.z());
        if (t_next.x() == t_next_min)
        {
            if (debug)
                printf("Next Cell :x\n");
            if (sign.x() == 1)
                normal.Set(-1, 0, 0);
            else
                normal.Set(1, 0, 0);
            i += sign.x();
            tmin = t_next.x();
            t_next.Set(t_next.x() + d_t.x(), t_next.y(), t_next.z());
        }
        else if (t_next.y() == t_next_min)
        {
            if (debug)
                printf("Next Cell :y\n");
            if (sign.y() == 1)
                normal.Set(0, -1, 0);
            else
                normal.Set(0, 1, 0);
            j += sign.y();
            tmin = t_next.y();
            t_next.Set(t_next.x(), t_next.y() + d_t.y(), t_next.z());
        }
        else if (t_next.z() == t_next_min)
        {
            if (debug)
                printf("Next Cell :z\n");
            if (sign.z() == 1)
                normal.Set(0, 0, -1);
            else
                normal.Set(0, 0, 1);
            k += sign.z();
            tmin = t_next.z();
            t_next.Set(t_next.x(), t_next.y(), t_next.z() + d_t.z());
        }
    }

private:
    float tmin;
    int i, j, k;
    Vec3f normal;
    Vec3f t_next;
    Vec3f d_t; // marching increments
    Vec3f sign;
};