#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vectors.h"
#include "ray.h"
#include "hit.h"
// ====================================================================
// ====================================================================

// You will extend this class in later assignments

class Material
{

public:
  // CONSTRUCTORS & DESTRUCTOR
  Material() {}
  Material(const Vec3f &d_color) { diffuseColor = d_color; }
  virtual ~Material() {} // for base

  // ACCESSORS
  virtual Vec3f getDiffuseColor() const { return diffuseColor; }
  virtual void glSetMaterial() const = 0;
  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) = 0; // const = 0;
protected:
  // REPRESENTATION
  Vec3f diffuseColor;
};

// ====================================================================
// ====================================================================
class PhongMaterial : public Material
{

public:
  // CONSTRUCTORS & DESTRUCTOR
  PhongMaterial()
  {
  }
  PhongMaterial(const Vec3f &d_color, const Vec3f &s_color, float e)
  {
    diffuseColor = d_color;
    specularColor = s_color;
    exponent = e;
  }
  ~PhongMaterial() {} // for base

  // ACCESSORS
  Vec3f getSpecularColor() const
  {
    return specularColor;
  }

  void glSetMaterial(void) const;

  Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) // const
  {
    Vec3f lcolor = lightColor, pcolor;
    float d = 0;
    assert(fabs(ray.getDirection().Length() - 1) < 1e-6);
    if (hit.getNormal().Dot3(dirToLight) > 0)
      d = hit.getNormal().Dot3(dirToLight);

    lcolor *= d;
    Vec3f::Mult(lcolor, lcolor, diffuseColor);
    pcolor += lcolor; // diffuse color
                      //------------------------------------------------------//
    Vec3f h, l;
    lcolor = lightColor;
    assert(fabs(dirToLight.Length() - 1) < 1e-6);
    assert(fabs(ray.getDirection().Length() - 1) < 1e-6);
    h = dirToLight - ray.getDirection(); // negative of ray dir
    h.Normalize();
    d = h.Dot3(hit.getNormal());
    if (d < 0)
    {
      d = 0; // not fix
    }
    d = pow(d, exponent);             // q>1
    l = lcolor * d;                   // ignore /r^2
    Vec3f::Mult(l, l, specularColor); // specular color
    /*if (d < 0)
    {
      l *= dirToLight.Dot3(hit.getNormal()); // fix
      // multiplied by the dot product of the normal and direction to the light
    }*/
    pcolor += l;

    return pcolor;
  }

private:
  Vec3f specularColor;
  float exponent;
};
#endif
