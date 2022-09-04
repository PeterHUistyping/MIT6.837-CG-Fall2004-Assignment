
#ifndef _GRID_H_
#define _GRID_H_
#include "object3d.h"
#include "object3dvector.h"
#include "camera.h"
#include "marchinginfo.h"
#include "rayTree.h"
#include <math.h>
#include "extern.h"
void GetVec3f(Vec3f &p, Vec4f &v);
enum Kind
{
    up,
    down,
    Left,
    Right,
    in,
    out,
    all
};
class Grid : public Object3D
{
public:
    Grid() {}
    Grid(BoundingBox *bb) { box = bb; }
    Grid(BoundingBox *bb, int nxx, int nyy, int nzz)
    {
        box = bb, nx = nxx, ny = nyy, nz = nzz;
        /* array = new bool **[nx];
         for (int j = 0; j < nx; j++) // 1D
         {
             array[j] = new bool *[ny];   // 2D
             for (int k = 0; k < ny; k++) // 2D
             {
                 array[j][k] = new bool[nz]; // 3D
             }
         }*/

        array_obj = new Object3DVector **[nx];
        for (int j = 0; j < nx; j++) // 1D
        {
            array_obj[j] = new Object3DVector *[ny]; // 2D
            for (int k = 0; k < ny; k++)             // 2D
            {
                array_obj[j][k] = new Object3DVector[nz]; // 3D
            }
        }
    }
    BoundingBox *getBoundingBox() const
    {
        return box;
    }
    ~Grid()
    {
        /*for (int j = 0; j < nx; j++) // 1D
        {
            for (int k = 0; k < ny; k++) // 2D
            {
                delete[] array[j][k]; // 3D
                array[j][k] = nullptr;
            }
            delete[] array[j]; // 2D
            array[j] = nullptr;
        }
        delete[] array;*/
        for (int j = 0; j < nx; j++) // 1D
        {
            for (int k = 0; k < ny; k++) // 2D
            {
                delete[] array_obj[j][k]; // 3D
                array_obj[j][k] = nullptr;
            }
            delete[] array_obj[j]; // 2D
            array_obj[j] = nullptr;
        }
        delete array_obj;
        array_obj = nullptr;
        delete m;
        m = nullptr;
        delete box;
        box = nullptr;
    }
    virtual void insertIntoGrid(Grid *g, Matrix *m) {}

    Vec3f getincrement() const
    {
        Vec3f max = this->getBoundingBox()->getMax();
        Vec3f min = this->getBoundingBox()->getMin();
        Vec3f increment = max - min;
        return increment;
    }
    Vec3f getincrement_halfCell() const
    {
        Vec3f increment = getincrement();
        increment.Set(increment.x() / nx / 2, increment.y() / ny / 2, increment.z() / nz / 2);
        return increment;
    }
    bool isInsideGird(const Vec3f &point) const
    {
        Vec3f max = this->getBoundingBox()->getMax();
        Vec3f min = this->getBoundingBox()->getMin();
        return point.x() >= min.x() && point.y() >= min.y() && point.z() >= min.z() &&
               point.x() <= max.x() && point.y() <= max.y() && point.z() <= max.z();
    }
    bool isInsideGird_(int i, int j, int k) const
    {
        return i >= 0 && j >= 0 && k >= 0 &&
               i < nx && j < ny && k < nz;
    }

    Vec3f getCenterPosition(int i, int j, int k) const
    {
        Vec3f min = this->getBoundingBox()->getMin();
        Vec3f increment = getincrement();
        Vec3f start = min + getincrement_halfCell();
        // Vec3f start = min + Vec3f(increment.x() / nx / 2, increment.y() / ny / 2, increment.z() / nz / 2);
        return start + Vec3f(i * increment.x() / nx, j * increment.y() / ny, k * increment.z() / nz);
    }
    bool isInsideCell(int i, int j, int k, const Vec3f &point)
    {
        Vec3f center = getCenterPosition(i, j, k);
        Vec3f min = center - getincrement_halfCell();
        Vec3f max = center + getincrement_halfCell();
        return point.x() >= min.x() && point.y() >= min.y() && point.z() >= min.z() &&
               point.x() <= max.x() && point.y() <= max.y() && point.z() <= max.z();
    }
    void getIJK_Normal(int &i_, int &j_, int &k_, Vec3f &normal, const Vec3f &p_) const
    {
        // assert(isInsideGird(p_));
        Vec3f maxp = this->getBoundingBox()->getMax();
        Vec3f minp = this->getBoundingBox()->getMin();
        Vec3f p = p_ - minp;

        Vec3f increment = maxp - minp;
        increment.Set(increment.x() / nx, increment.y() / ny, increment.z() / nz);
        i_ = p.x() / increment.x();
        if (i_ == nx)
            i_--;
        j_ = p.y() / increment.y();
        if (j_ == ny)
            j_--;
        k_ = p.z() / increment.z();
        if (k_ == nz)
            k_--;
        Vec3f p_n = p_ - getCenterPosition(i_, j_, k_);
        Vec3f increment_half = getincrement_halfCell();
        if (fabs(increment_half.x() - fabs(p_n.x())) < 1e-6) // rounding error
        {
            if (p_n.x() > 0)
                normal.Set(1, 0, 0);
            else
                normal.Set(-1, 0, 0);
        }
        if (fabs(increment_half.y() - fabs(p_n.y())) < 1e-6)
        {
            if (p_n.y() > 0)
                normal.Set(0, 1, 0);
            else
                normal.Set(0, -1, 0);
        }
        if (fabs(increment_half.z() - fabs(p_n.z())) < 1e-6)
        {
            if (p_n.z() > 0)
                normal.Set(0, 0, 1);

            else
                normal.Set(0, 0, -1);
        }
        assert(normal.Length() >= 1e-6);
    }

    void addEnteredFace(int i, int j, int k, Vec3f normal, Material *m)
    {
        Kind dir;
        if (normal.x() == -1)
        {
            dir = in;
        }
        if (normal.x() == 1)
        {
            dir = out;
        }
        if (normal.z() == 1)
        {
            dir = up;
        }
        if (normal.x() == -1)
        {
            dir = down;
        }
        if (normal.y() == 1)
        {
            dir = Right;
        }
        if (normal.y() == -1)
        {
            dir = Left;
        }
        Vec3f increment = getincrement();
        Vec3f center = this->getCenterPosition(i, j, k);
        Vec3f A = center + Vec3f(increment.x() / nx / 2, -increment.y() / ny / 2, increment.z() / nz / 2);
        Vec3f D = center + Vec3f(-increment.x() / nx / 2, -increment.y() / ny / 2, increment.z() / nz / 2);
        Vec3f B = center + Vec3f(increment.x() / nx / 2, increment.y() / ny / 2, increment.z() / nz / 2);
        Vec3f C = center + Vec3f(-increment.x() / nx / 2, increment.y() / ny / 2, increment.z() / nz / 2);
        Vec3f A_ = center + Vec3f(increment.x() / nx / 2, -increment.y() / ny / 2, -increment.z() / nz / 2);
        Vec3f D_ = center + Vec3f(-increment.x() / nx / 2, -increment.y() / ny / 2, -increment.z() / nz / 2);
        Vec3f B_ = center + Vec3f(increment.x() / nx / 2, increment.y() / ny / 2, -increment.z() / nz / 2);
        Vec3f C_ = center + Vec3f(-increment.x() / nx / 2, increment.y() / ny / 2, -increment.z() / nz / 2);
        Vec3f a, b, c, d, n, aa, bb, cc;

        if (dir == up)
        {
            a = C, b = D, c = A, d = B;
        }
        else if (dir == out)
        {
            a = B, b = A, c = A_, d = B_;
        }
        else if (dir == Right)
        {
            a = C, b = B, c = B_, d = C_;
        }
        else if (dir == down)
        {
            a = D_, b = C_, c = B_, d = A_;
        }
        else if (dir == Left)
        {
            a = A, b = D, c = D_, d = A_;
        }
        else if (dir == in)
        {
            a = D, b = C, c = C_, d = D_;
        }
        else if (dir == all)
        {
            for (Kind dir = up; dir <= out; dir = (Kind)(dir + 1)) // dir++ won't work
            {
                paintEachSide(i, j, k, dir);
            }
        }
        aa = a, bb = b, cc = c;
        Vec3f::Cross3(n, bb - aa, cc - aa); // normal=ab*ac

        RayTree::AddEnteredFace(a, b, c, d, normal, m);
    }

    void addHitCellFace(int i, int j, int k, Kind dir, Material *m)
    {
        Vec3f increment = getincrement();
        Vec3f center = this->getCenterPosition(i, j, k);
        Vec3f A = center + Vec3f(increment.x() / nx / 2, -increment.y() / ny / 2, increment.z() / nz / 2);
        Vec3f D = center + Vec3f(-increment.x() / nx / 2, -increment.y() / ny / 2, increment.z() / nz / 2);
        Vec3f B = center + Vec3f(increment.x() / nx / 2, increment.y() / ny / 2, increment.z() / nz / 2);
        Vec3f C = center + Vec3f(-increment.x() / nx / 2, increment.y() / ny / 2, increment.z() / nz / 2);
        Vec3f A_ = center + Vec3f(increment.x() / nx / 2, -increment.y() / ny / 2, -increment.z() / nz / 2);
        Vec3f D_ = center + Vec3f(-increment.x() / nx / 2, -increment.y() / ny / 2, -increment.z() / nz / 2);
        Vec3f B_ = center + Vec3f(increment.x() / nx / 2, increment.y() / ny / 2, -increment.z() / nz / 2);
        Vec3f C_ = center + Vec3f(-increment.x() / nx / 2, increment.y() / ny / 2, -increment.z() / nz / 2);
        Vec3f a, b, c, d, n, aa, bb, cc;

        if (dir == up)
        {
            a = C, b = D, c = A, d = B;
        }
        else if (dir == out)
        {
            a = B, b = A, c = A_, d = B_;
        }
        else if (dir == Right)
        {
            a = C, b = B, c = B_, d = C_;
        }
        else if (dir == down)
        {
            a = D_, b = C_, c = B_, d = A_;
        }
        else if (dir == Left)
        {
            a = A, b = D, c = D_, d = A_;
        }
        else if (dir == in)
        {
            a = D, b = C, c = C_, d = D_;
        }
        else if (dir == all)
        {
            for (Kind dir = up; dir <= out; dir = (Kind)(dir + 1)) // dir++ won't work
            {
                addHitCellFace(i, j, k, dir, m);
            }
        }
        aa = a, bb = b, cc = c;
        Vec3f::Cross3(n, bb - aa, cc - aa); // normal=ab*ac
        RayTree::AddHitCellFace(a, b, c, d, n, m);
    }

    bool intersect(const Ray &r, Hit &h, float tmin)
    {
        Vec3f maxp = this->getBoundingBox()->getMax();
        Vec3f minp = this->getBoundingBox()->getMin();
        bool result = false;
        for (int i = 0; i < plane_obj.getNumObjects(); i++)
        {
            if (plane_obj.getObject(i)->intersect(r, h, tmin))
                result = true;
        }
        MarchingInfo mi;
        if (r.getDirection().x() == 0)
        {
            if (r.getOrigin().x() < minp.x() || r.getOrigin().x() > maxp.x())
                return result;
        }
        if (r.getDirection().y() == 0)
        {

            if (r.getOrigin().y() < minp.y() || r.getOrigin().y() > maxp.y())
                return result;
        }
        if (r.getDirection().z() == 0)
        {
            if (r.getOrigin().z() < minp.z() || r.getOrigin().z() > maxp.z())
                return result;
        }
        Vec3f t1, t2;
        t1 = minp - r.getOrigin();
        assert(fabs(r.getDirection().Length() - 1) < 1e-6);

        t2 = maxp - r.getOrigin();
        float t1_x, t1_y, t1_z, t2_x, t2_y, t2_z;
        t1_x = t1.x() / r.getDirection().x();
        t2_x = t2.x() / r.getDirection().x();
        t1_y = t1.y() / r.getDirection().y();
        t2_y = t2.y() / r.getDirection().y();
        t1_z = t1.z() / r.getDirection().z();
        t2_z = t2.z() / r.getDirection().z();
        if (t2_x < t1_x)
            swap(t2_x, t1_x); // else swap
        if (t2_y < t1_y)
            swap(t2_y, t1_y); // else swap
        if (t2_z < t1_z)
            swap(t2_z, t1_z); // else swap
        float tnear, tfar, tintersect;
        tnear = max(max(t1_x, t1_y), t1_z);
        tfar = min(min(t2_x, t2_y), t2_z);
        if (tnear > tfar)
            return result;
        if (tfar < tmin)
            return result;
        if (tnear > tmin)
            tintersect = tnear;
        else
        {
            tintersect = tfar;
        }
        initializeRayMarch(mi, r, tintersect);
        // float t_current, t_cur;
        Material *m, *m_temp = new PhongMaterial(Vec3f(0, 1, 0));
        int i_, j_, k_;
        Vec3f normal;
        mi.getCurrentCell(i_, j_, k_);
        if (debug)
        {
            printf("i %d/%d,j %d/%d,k %d/%d\n", i_, nx, j_, ny, k_, nz);
        }
        while (isInsideGird_(i_, j_, k_))
        {
            // mi.gettmin(t_current);
            mi.getNormal(normal);
            if (array_obj[i_][j_][k_].getNumObjects() != 0) //&& isInsideCell(i_, j_, k_, r.pointAtParameter(tintersect + t_current)))
            {
                m = array_obj[i_][j_][k_].getObject(0)->m;
                h.set(tintersect, m, normal, r);

                for (int l = 1; l < array_obj[i_][j_][k_].getNumObjects(); l++) // more than 1
                {
                    if (array_obj[i_][j_][k_].getObject(l)->intersect(r, h, tmin))
                    {
                        h.set(h.getT(), h.getMaterial(), normal, r);
                    }
                }

                RayTree::AddShadowSegment(r, 0, 200);
                addEnteredFace(i_, j_, k_, normal, m);
                addHitCellFace(i_, j_, k_, all, m);
                return true;
            }

            RayTree::AddShadowSegment(r, 0, 200);
            addEnteredFace(i_, j_, k_, normal, m_temp);
            addHitCellFace(i_, j_, k_, all, m_temp);
            mi.nextCell();
            mi.getCurrentCell(i_, j_, k_);
        }
        return result;
    }
    void initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const
    {
        int sign_x = -1, sign_y = -1, sign_z = -1;
        if (r.getDirection().x() > 0) // r_dx=0 sign=-1;
        {
            sign_x = 1;
        }
        if (r.getDirection().y() > 0)
        {
            sign_y = 1;
        }
        if (r.getDirection().z() > 0)
        {
            sign_z = 1;
        }
        mi.setSign(sign_x, sign_y, sign_z);

        Vec3f point = r.pointAtParameter(tmin);
        float dt_x, dt_y, dt_z, tnext_x = tmin, tnext_y = tmin, tnext_z = tmin;
        int i_, j_, k_;
        Vec3f normal;
        getIJK_Normal(i_, j_, k_, normal, point);
        mi.setNormal(normal);
        mi.setCurrentCell(i_, j_, k_);

        Vec3f center_cell = getCenterPosition(i_, j_, k_);
        Vec3f increment_half = getincrement_halfCell();
        Vec3f increment_each = increment_half * 2;
        Vec3f min_ = center_cell - increment_half;
        Vec3f max_ = center_cell + increment_half;
        float f_x, f_y, f_z;
        if (sign_x == -1)
        {
            f_x = point.x() - min_.x();
        }
        else if (sign_x == 1)
        {
            f_x = max_.x() - point.x();
        }
        if (sign_y == -1)
        {
            f_y = point.y() - min_.y();
        }
        else if (sign_y == 1)
        {
            f_y = max_.y() - point.y();
        }
        if (sign_z == -1)
        {
            f_z = point.z() - min_.z();
        }
        else if (sign_z == 1)
        {
            f_z = max_.z() - point.z();
        }
        assert(fabs(r.getDirection().Length() - 1) < 1e-6);

        if (fabs(r.getDirection().x()) >= 1e-6)
        {
            dt_x = increment_each.x() / fabs(r.getDirection().x()); //*|dir|
            f_x /= fabs(r.getDirection().x());
        }
        else
        {
            tnext_x = __FLT_MAX__;
        }
        if (fabs(r.getDirection().y()) >= 1e-6)
        {
            dt_y = increment_each.y() / fabs(r.getDirection().y()); ///*|dir|
            f_y /= fabs(r.getDirection().y());
        }
        else
        {
            tnext_y = __FLT_MAX__;
        }
        if (fabs(r.getDirection().z()) >= 1e-6)
        {
            dt_z = increment_each.z() / fabs(r.getDirection().z()); //*|dir|
            f_z /= fabs(r.getDirection().z());
        }
        else
        {
            tnext_z = __FLT_MAX__;
        }
        mi.setd_t(dt_x, dt_y, dt_z);
        if (debug)
        {
            printf("sign %d,%d,%d\n", sign_x, sign_y, sign_z);
            min_.Write();
            point.Write();
            printf("d_t %f,%f,%f\n", dt_x, dt_y, dt_z);
            printf("tnext_orgin %f,%f,%f\n", tnext_x, tnext_y, tnext_z);
            normal.Write();
        }
        if (normal.x() == -1 || normal.x() == 1)
        {
            tnext_x += dt_x;
            tnext_z += f_z;
            tnext_y += f_y;
        }
        else if (normal.y() == -1 || normal.y() == 1)
        {
            tnext_x += f_x;
            tnext_y += dt_y;
            tnext_z += f_z;
        }
        else if (normal.z() == -1 || normal.z() == 1)
        {
            tnext_x += f_x;
            tnext_y += f_y;
            tnext_z += dt_z;
        }
        if (debug)
        {
            printf("tnext %f,%f,%f\n", tnext_x, tnext_y, tnext_z);
        }
        Vec3f t_next(tnext_x, tnext_y, tnext_z);
        mi.set_tnext(t_next);
    }

    void paintEachSide(int i, int j, int k, Kind dir)
    {
        Vec3f increment = getincrement();
        Vec3f center = this->getCenterPosition(i, j, k);
        Vec3f A = center + Vec3f(increment.x() / nx / 2, -increment.y() / ny / 2, increment.z() / nz / 2);
        Vec3f D = center + Vec3f(-increment.x() / nx / 2, -increment.y() / ny / 2, increment.z() / nz / 2);
        Vec3f B = center + Vec3f(increment.x() / nx / 2, increment.y() / ny / 2, increment.z() / nz / 2);
        Vec3f C = center + Vec3f(-increment.x() / nx / 2, increment.y() / ny / 2, increment.z() / nz / 2);
        Vec3f A_ = center + Vec3f(increment.x() / nx / 2, -increment.y() / ny / 2, -increment.z() / nz / 2);
        Vec3f D_ = center + Vec3f(-increment.x() / nx / 2, -increment.y() / ny / 2, -increment.z() / nz / 2);
        Vec3f B_ = center + Vec3f(increment.x() / nx / 2, increment.y() / ny / 2, -increment.z() / nz / 2);
        Vec3f C_ = center + Vec3f(-increment.x() / nx / 2, increment.y() / ny / 2, -increment.z() / nz / 2);
        Vec3f a, b, c, d, n, aa, bb, cc;

        if (dir == up)
        {
            a = C, b = D, c = A, d = B;
        }
        else if (dir == out)
        {
            a = B, b = A, c = A_, d = B_;
        }
        else if (dir == Right)
        {
            a = C, b = B, c = B_, d = C_;
        }
        else if (dir == down)
        {
            a = D_, b = C_, c = B_, d = A_;
        }
        else if (dir == Left)
        {
            a = A, b = D, c = D_, d = A_;
        }
        else if (dir == in)
        {
            a = D, b = C, c = C_, d = D_;
        }
        else if (dir == all)
        {
            for (Kind dir = up; dir <= out; dir = (Kind)(dir + 1)) // dir++ won't work
            {
                paintEachSide(i, j, k, dir);
            }
        }
        aa = a, bb = b, cc = c;
        Vec3f::Cross3(n, bb - aa, cc - aa); // normal=ab*ac

        glBegin(GL_QUADS);
        glNormal3f(n.x(), n.y(), n.z()); // first
        glVertex3f(a.x(), a.y(), a.z());
        glVertex3f(b.x(), b.y(), b.z());
        glVertex3f(c.x(), c.y(), c.z());
        glVertex3f(d.x(), d.y(), d.z());
        glEnd();
    }
    void paint_Every() // every six sides
    {
        for (int i = 0; i < nx; i++)
        {
            for (int j = 0; j < ny; j++)
            {
                for (int k = 0; k < nz; k++)
                {
                    // if (array[i][j][k])
                    if (array_obj[i][j][k].getNumObjects() != 0)
                    {
                        paintEachSide(i, j, k, all);
                    }
                }
            }
        }
    }
    void paint_helper1(Kind k1, Kind k2)
    {
        bool pre, current;
        for (int i = 0; i < nx; i++)
        {
            for (int j = 0; j < ny; j++)
            {
                for (int k = 0; k < nz; k++)
                {
                    if (k == 0)
                    {
                        // pre = array[i][j][k];
                        pre = (array_obj[i][j][k].getNumObjects() != 0);
                        // if (array[i][j][k])
                        if (array_obj[i][j][k].getNumObjects() != 0)
                        {
                            paintEachSide(i, j, k, k1);
                        }
                    }
                    else
                    {
                        // current = array[i][j][k];
                        current = (array_obj[i][j][k].getNumObjects() != 0);
                        if (pre == true && current == false)
                        {
                            paintEachSide(i, j, k - 1, k2);
                        }
                        else if (pre == false && current == true)
                        {
                            paintEachSide(i, j, k, k1);
                        }
                        pre = current;
                    }
                    if (k == nz - 1)
                    {
                        // if (array[i][j][k])
                        if (array_obj[i][j][k].getNumObjects() != 0)
                        {
                            paintEachSide(i, j, k, k2);
                        }
                    }
                }
            }
        }
    }
    void paint_helper2(Kind k1, Kind k2)
    {
        bool pre, current;
        for (int k = 0; k < nz; k++)
        {
            for (int i = 0; i < nx; i++)
            {
                for (int j = 0; j < ny; j++)
                {
                    if (j == 0)
                    {
                        // pre = array[i][j][k];
                        pre = (array_obj[i][j][k].getNumObjects() != 0);
                        // if (array[i][j][k])
                        if (array_obj[i][j][k].getNumObjects() != 0)
                        {
                            paintEachSide(i, j, k, k1);
                        }
                    }
                    else
                    {
                        // current = array[i][j][k];
                        current = (array_obj[i][j][k].getNumObjects() != 0);
                        if (pre == true && current == false)
                        {
                            paintEachSide(i, j - 1, k, k2);
                        }
                        else if (pre == false && current == true)
                        {
                            paintEachSide(i, j, k, k1);
                        }
                        pre = current;
                    }
                    if (j == ny - 1)
                    {
                        // if (array[i][j][k])
                        if (array_obj[i][j][k].getNumObjects() != 0)
                        {
                            paintEachSide(i, j, k, k2);
                        }
                    }
                }
            }
        }
    }
    void paint_helper3(Kind k1, Kind k2)
    {
        bool pre, current;
        for (int k = 0; k < nz; k++)
        {
            for (int j = 0; j < ny; j++)
            {
                for (int i = 0; i < nx; i++)
                {
                    if (i == 0)
                    {
                        // pre = array[i][j][k];
                        pre = (array_obj[i][j][k].getNumObjects() != 0);
                        // if (array[i][j][k])
                        if (array_obj[i][j][k].getNumObjects() != 0)
                        {
                            paintEachSide(i, j, k, k1);
                        }
                    }
                    else
                    {
                        // current = array[i][j][k];
                        current = (array_obj[i][j][k].getNumObjects() != 0);
                        if (pre == true && current == false)
                        {
                            paintEachSide(i - 1, j, k, k2);
                        }
                        else if (pre == false && current == true)
                        {
                            paintEachSide(i, j, k, k1);
                        }
                        pre = current;
                    }
                    if (i == nx - 1)
                    {
                        // if (array[i][j][k])
                        if (array_obj[i][j][k].getNumObjects() != 0)
                        {
                            paintEachSide(i, j, k, k2);
                        }
                    }
                }
            }
        }
    }

    void paint() // only those between occupied & unoccupied
    {
        paint_helper1(down, up);
        paint_helper2(Left, Right);
        paint_helper3(in, out);
    }
    friend class Sphere;
    friend class Triangle;
    friend class Plane;

private:
    int nx;
    int ny;
    int nz;
    // bool ***array; // opaque or transparent
    Object3DVector ***array_obj;
    Object3DVector plane_obj;
};
#endif