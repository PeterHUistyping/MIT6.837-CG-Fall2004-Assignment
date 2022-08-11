#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <assert.h>
#include "matrix.h"
#include "ray.h"
#include "hit.h"

#define GL_SILENCE_DEPRECATION
//Defined before OpenGL and GLUT includes to avoid deprecation messages 
#include <OpenGL/gl.h> 
#include <OpenGL/glu.h> 
#include <GLUT/glut.h> 

class Camera
{
    public:
    Camera(){}
    virtual ~Camera(){}
    virtual Ray generateRay(Vec2f point) = 0;
    virtual float getTMin() const = 0;

    const Vec3f& getCenter(){return center;}
    const Vec3f& getDirection(){return direction;}
    const Vec3f& getUp(){return up;}
    const Vec3f& getHorizontal(){return horizontal;}
// ====================================================================
// Here are the prototypes for five pure virtual functions for
// initializing, placing, dollying, trucking, and rotating an
// OpenGL camera.  These should be added to the Camera class and
// each of its subclasses
// ====================================================================

    virtual void glInit(int w, int h) = 0;
    virtual void glPlaceCamera(void) = 0;
    virtual void dollyCamera(float dist) = 0;
    virtual void truckCamera(float dx, float dy) = 0;
    virtual void rotateCamera(float rx, float ry) = 0;

    protected:
    Vec3f center;
    Vec3f direction;
    Vec3f up;
    Vec3f horizontal;
    
    
};
class OrthographicCamera: public Camera
{
    public:
    OrthographicCamera(){}
    const float& getSize(){return size;}
    OrthographicCamera(const Vec3f& c,const Vec3f& dir,const Vec3f& u,float s)
    {
        center=c;
        Vec3f a1,a2,a3,b1,b2,b3,tempb1,tempb2;
        a1=dir;
        a2=u;
        //normalizing direction vector
        
        //orthonormalize
        b1=a1;
        tempb1=b1;  //avoid (0,0,0)
        b2=a2- (tempb1*=tempb1.Dot3(a2)/tempb1.Length()/tempb1.Length());   
        tempb1=b1;  //redefine
        tempb2=b2;
        assert(b1.Dot3(b2)<1e-6);
        Vec3f::Cross3(a3,b1,b2); //right hand
        b3=a3- (tempb1*=tempb1.Dot3(a3)/tempb1.Length()/tempb1.Length())- (tempb2*=tempb2.Dot3(a3)/tempb2.Length()/tempb2.Length());
        assert(b1.Dot3(b3)<1e-6);
        assert(b3.Dot3(b2)<1e-6);    
    
        b1.Normalize();
        b2.Normalize();
        b3.Normalize();

        direction=b1;
        up=b2;
        horizontal=b3;
        size=s;
        //b2=a2- (b1*=b1.Dot3(a2)/b1.Length());   
        //   b1/=b1.Dot3(a2)/b1.Length();   //cautious: b1.Dot3(a2)=(0,0,0)
        //Vec3f::Cross3(a3,b1,b2); //right hand
        //b3=a3- (b1*=b1.Dot3(a3)/b1.Length())- (b2*=b2.Dot3(a3)/b2.Length());
        //    b1*= b1.Dot3(a3)/b1.Length();       b2*= b2.Dot3(a3)/b2.Length();
    }
    ~OrthographicCamera(){}
    Ray generateRay(Vec2f point)
    {
        Vec3f Point;  
        float x,y,z,xx,yy;
        xx=point.x()-0.5; //center(0.5,0.5)
        yy=point.y()-0.5;
        xx*=size;  // /2
        yy*=size;
        Vec3f h=horizontal;
        Vec3f u=up;
        h*=xx;
        u*=yy;
        Point=center + h + u;
        Ray r(Point,direction);
        //Ray (const Vec3f &orig, const Vec3f &dir)
        return r;
        
    }
    float getTMin() const 
    {
        return -__FLT_MAX__;
    }
    void glInit(int w, int h) ;
    void glPlaceCamera(void) ;
    void dollyCamera(float dist) ;
    void truckCamera(float dx, float dy) ;
    void rotateCamera(float rx, float ry) ;
    private:
        float size;
};
class PerspectiveCamera:public Camera
{

  public:
    const float& getAngle() {return angle;};
    PerspectiveCamera(){}
    PerspectiveCamera(const Vec3f& c,const Vec3f& dir,const Vec3f& u,float an)
    {
        center=c;
        Vec3f a1,a2,a3,b1,b2,b3,tempb1,tempb2;
        a1=dir;
        a2=u;
        //normalizing direction vector
        
        //orthonormalize
        b1=a1;
        tempb1=b1;  //avoid (0,0,0)
        b2=a2- (tempb1*=tempb1.Dot3(a2)/tempb1.Length()/tempb1.Length());   
        tempb1=b1;  //redefine
        tempb2=b2;
        assert(b1.Dot3(b2)<1e-6);
        Vec3f::Cross3(a3,b1,b2); //right hand
        b3=a3- (tempb1*=tempb1.Dot3(a3)/tempb1.Length()/tempb1.Length())- (tempb2*=tempb2.Dot3(a3)/tempb2.Length()/tempb2.Length());
        assert(b1.Dot3(b3)<1e-6);
        assert(b3.Dot3(b2)<1e-6);    
    
        b1.Normalize();
        b2.Normalize();
        b3.Normalize();

        direction=b1;
        up=b2;
        horizontal=b3;
        angle=an;
     
    }
    ~PerspectiveCamera(){}
    Ray generateRay(Vec2f point)
    {
        float x,y,z,xx,yy;
        xx=point.x()-0.5; //center(0.5,0.5)  //relative to width,2a
        yy=point.y()-0.5;
        float temp=tan(angle/2)*tan(angle/2);  //rad
        temp=temp/(1-temp);
        temp=sqrt(temp);
        temp*=2;   
        xx*=temp;  // /2
        yy*=temp;
        Vec3f h=horizontal;
        Vec3f u=up;
        h*=xx;
        u*=yy;
        Vec3f dir;
        dir=direction+h+u;  
        dir.Normalize();
        Ray r(center,dir);
        //Ray (const Vec3f &orig, const Vec3f &dir)
        return r;
    }
    float getTMin() const 
    {
        return 0;
    }
    void glInit(int w, int h) ;
    void glPlaceCamera(void) ;
    void dollyCamera(float dist) ;
    void truckCamera(float dx, float dy) ;
    void rotateCamera(float rx, float ry) ;
    private:
    float angle;
};
#endif

