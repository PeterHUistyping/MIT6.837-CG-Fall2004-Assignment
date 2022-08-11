#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include "vectors.h"
#include "ray.h"
#include "hit.h"
#include "material.h"
#define GL_SILENCE_DEPRECATION
// Defined before OpenGL and GLUT includes to avoid deprecation messages
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

class Object3D
{
public:
  virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0; // pure virtual method

  Object3D() {}
  virtual ~Object3D() {}

  virtual void paint(void) = 0;
  // The paint routine is responsible for making the OpenGL calls
  // to draw the object to the screen.

protected:
  Material *m; // materials are very simple and consist of a single color - assignment1
};

#endif