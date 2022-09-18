#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "matrix.h"
#include "ray.h"
#include "hit.h"
#include "extern.h"
#include "perlin_noise.h"
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
  Vec3f getDiffuseColor() const { return diffuseColor; }
  Vec3f getReflectiveColor() const { return reflectiveColor; }
  Vec3f getTransparentColor() const { return transparentColor; }
  float getIndexOfRefraction() const { return indexOfRefraction; }
  virtual void glSetMaterial() const = 0;
  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) = 0; // const = 0;
protected:
  // REPRESENTATION
  Vec3f diffuseColor;
  Vec3f reflectiveColor;
  Vec3f transparentColor;
  float indexOfRefraction;
};

// ====================================================================

class PhongMaterial : public Material
{

public:
  // CONSTRUCTORS & DESTRUCTOR
  PhongMaterial() {}
  PhongMaterial(const Vec3f &d_color) { diffuseColor = d_color; }
  PhongMaterial(const Vec3f &d_color, const Vec3f &s_color, float e)
  {
    diffuseColor = d_color;
    specularColor = s_color;
    exponent = e;
  }

  PhongMaterial(const Vec3f &d_Color, const Vec3f &s_Color,
                float e, const Vec3f &r_Color,
                const Vec3f &t_Color, float index_Refraction)
  {
    diffuseColor = d_Color;
    specularColor = s_Color;
    exponent = e;
    reflectiveColor = r_Color;
    transparentColor = t_Color;
    indexOfRefraction = index_Refraction;
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
    Vec3f normal = hit.getNormal();
    if (shadeback)
    {
      if (hit.getNormal().Dot3(ray.getDirection()) > 0)
        normal.Negate(); // back shading
    }

    Vec3f lcolor = lightColor, pcolor;
    float d = 0;
    assert(fabs(ray.getDirection().Length() - 1) < 1e-6);
    if (normal.Dot3(dirToLight) > 0)
      d = normal.Dot3(dirToLight);

    lcolor *= d;
    lcolor = lcolor * diffuseColor;
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
    d = pow(d, exponent);  // q>1
    l = lcolor * d;        // ignore /r^2
    l = l * specularColor; // specular color
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

// ====================================================================

class Checkerboard : public Material
{
public:
  Checkerboard(Matrix *m, Material *mat1, Material *mat2)
  {
    matrix = m;
    m1 = mat1;
    m2 = mat2;
  }
  ~Checkerboard() {}
  void glSetMaterial(void) const
  {
    m1->glSetMaterial();
  }
  Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor)
  {
    Vec3f p = ray.pointAtParameter(hit.getT());
    matrix->Transform(p);
    if (int(floor(p.x()) + floor(p.y()) + floor(p.z())) & 1)
      return m2->Shade(ray, hit, dirToLight, lightColor);
    else
      return m1->Shade(ray, hit, dirToLight, lightColor);
  }

private:
  Matrix *matrix;
  Material *m1, *m2;
};

// ====================================================================

class Noise : public Material
{
public:
  Noise(Matrix *m, Material *mat1, Material *mat2, int octaves_)
  {
    matrix = m;
    m1 = mat1;
    m2 = mat2;
    octaves = octaves_;
  }
  ~Noise() {}
  void glSetMaterial(void) const { m1->glSetMaterial(); }
  Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor)
  {
    Vec3f p = ray.pointAtParameter(hit.getT());
    matrix->Transform(p);
    float N = 0;
    int e = 1;
    for (int i = 0; i < octaves; i++)
    {
      N += PerlinNoise::noise(e * p.x(), e * p.y(), e * p.z()) / (float)e;
      e *= 2;
    }
    if (N < 0)
      N = 0;
    if (N > 1)
      N = 1;
    return m2->Shade(ray, hit, dirToLight, lightColor) * N + m1->Shade(ray, hit, dirToLight, lightColor) * (1 - N);
  }

private:
  Matrix *matrix;
  Material *m1, *m2;
  int octaves;
};

// ====================================================================

class Marble : public Material
{
public:
  Marble(Matrix *m, Material *mat1, Material *mat2, int octaves_, float frequency_, float amplitude_)
  {
    matrix = m;
    m1 = mat1;
    m2 = mat2;
    octaves = octaves_;
    frequency = frequency_;
    amplitude = amplitude_;
  }
  ~Marble() {}
  void glSetMaterial(void) const { m1->glSetMaterial(); }
  Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor)
  {
    Vec3f p = ray.pointAtParameter(hit.getT());
    matrix->Transform(p);
    float N = 0;
    int e = 1;
    for (int i = 0; i < octaves; i++)
    {
      N += PerlinNoise::noise(e * p.x(), e * p.y(), e * p.z()) / (float)e;
      e *= 2;
    }
    if (N < 0)
      N = 0;
    if (N > 1)
      N = 1;
    float M = 0;
    M = sin(frequency * p.x() + amplitude * N);

    if (M < 0)
      M = 0;
    if (M > 1)
      M = 1;
    return m1->Shade(ray, hit, dirToLight, lightColor) * M + m2->Shade(ray, hit, dirToLight, lightColor) * (1 - M);
  }

private:
  Matrix *matrix;
  Material *m1, *m2;
  int octaves;
  float frequency, amplitude;
};

// ====================================================================

class Wood : public Material
{
public:
  Wood(Matrix *m, Material *mat1, Material *mat2, int octaves_, float frequency_, float amplitude_)
  {
    matrix = m;
    m1 = mat1;
    m2 = mat2;
    octaves = octaves_;
    frequency = frequency_;
    amplitude = amplitude_;
  }
  ~Wood() {}
  void glSetMaterial(void) const { m1->glSetMaterial(); }
  Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor)
  {
    Vec3f p = ray.pointAtParameter(hit.getT());
    matrix->Transform(p);
    float N = 0;
    int e = 1;
    for (int i = 0; i < octaves; i++)
    {
      N += PerlinNoise::noise(e * p.x(), e * p.y(), e * p.z()) / (float)e;
      e *= 2;
    }
    if (N < 0)
      N = 0;
    if (N > 1)
      N = 1;
    float M = 0;
    M = cos(frequency * p.x() + amplitude * N); // cos

    if (M < 0)
      M = 0;
    if (M > 1)
      M = 1;

    return m1->Shade(ray, hit, dirToLight, lightColor) * M + m2->Shade(ray, hit, dirToLight, lightColor) * (1 - M);
  }

private:
  Matrix *matrix;
  Material *m1, *m2;
  int octaves;
  float frequency, amplitude;
};

#endif
