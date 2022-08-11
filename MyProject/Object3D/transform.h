#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "group.h"
void GetVec3f(Vec3f &p, Vec4f &v);
class Transform : public Object3D
{
public:
    ~Transform() {}
    Transform(Matrix &m, Object3D *o)
    {
        matrix = m;
        object = o;
    }

    bool intersect(const Ray &r, Hit &h, float tmin)
    {
        bool inter = false;

        Vec4f Origin_Os(r.getOrigin(), 1); // point
        Vec4f Dir_Os(r.getDirection(), 0);
        Vec3f Origin_Ob;
        Vec3f Dir_Ob;
        Matrix matrixInverse = matrix;
        assert(matrixInverse.Inverse(matrixInverse, 1e-6));
        // int Matrix::Inverse(Matrix &m, float epsilon)
        Origin_Os *= matrixInverse; // ADD
        Dir_Os *= matrixInverse;
        GetVec3f(Origin_Ob, Origin_Os);
        GetVec3f(Dir_Ob, Dir_Os); // overload
        Vec3f Dir_Obo = Dir_Ob;
        Dir_Ob.Normalize();
        float delta = 1 / Dir_Obo.Length(); //.x()  =0
        Ray ray(Origin_Ob, Dir_Ob);
        h.set(h.getT() / delta, h.getMaterial(), h.getNormal(), r);

        if (object->intersect(ray, h, tmin))
        {
            inter = true;
            Vec4f Normal(h.getNormal(), 1);
            Matrix matrixInverseT = matrixInverse;
            matrixInverseT.Transpose(matrixInverseT);
            Normal *= matrixInverseT;
            Vec3f n;
            GetVec3f(n, Normal);
            n.Normalize();
            h.set(h.getT(), h.getMaterial(), n, r);
            // h.set(h.getT(),h.getMaterial(),Normal,ray);
            // assert(fabs(r.pointAtParameter(h.getT()*delta).x()-ray.pointAtParameter(h.getT()).x())<1e-6);
        }
        h.set(h.getT() * delta, h.getMaterial(), h.getNormal(), r);
        return inter;
    }

    void paint(void)
    {
        glPushMatrix();
        GLfloat *glMatrix = matrix.glGet();
        glMultMatrixf(glMatrix);
        delete[] glMatrix;
        object->paint();
        glPopMatrix(); // restore from the stack
    }

private:
    Matrix matrix;
    Object3D *object;
};
#endif
