#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include "vectors.h"
#include "matrix.h"
#include "ray.h"
#include "hit.h"
#include "material.h"
#include "boundingbox.h"
class Grid;

class Object3D
{
public:
  Object3D() { m_ = NULL; }
  virtual ~Object3D()
  {
    delete[] m;
    m = nullptr;
    delete[] box;
    box = nullptr;
  }
  virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0; // pure virtual method
  virtual void insertIntoGrid(Grid *g, Matrix *m)
  {
  }
  virtual BoundingBox *getBoundingBox()
  {
    return nullptr;
  }

  virtual void paint(void) = 0;
  // The paint routine is responsible for making the OpenGL calls
  // to draw the object to the screen.
  Material *m;
  Matrix *getMatrix() { return m_; }

protected:
  // Material *m;
  BoundingBox *box;
  Matrix *m_; // add for grid
};

#endif
