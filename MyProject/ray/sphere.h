#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "object3d.h"
#include "camera.h"
#include <math.h>
class Sphere: public Object3D
{
   public:
    
    Sphere(){}
    ~Sphere(){}
    Sphere(const Vec3f &c,const float &rad,Material * mat)
    {
        center=c;
        radius=rad;
        m=mat;
    }
    bool intersectG(const Ray& r, Hit &h,float tmin)
    {
            //Geometric 
            Vec3f R0=r.getOrigin()-center;
            float tp=-R0.Dot3(r.getDirection());
            float d2=R0.Length()*R0.Length()-tp*tp;
            float tt2=radius*radius-d2;

            float t;
            float tt=sqrt(tt2);
            float tM=tp+tt;
            float tm=tp-tt;

            if(R0.Length()>radius) //ray origin outside the shpere tm
            {
                if(tt2<0||tp<0)//d2>radius*radius
                return false;
                t=tm;
            }


            else if(R0.Length()==radius) //ray origin on the shpere tp
                t=tp;
            
            else if(R0.Length()<radius) //ray origin inside the shpere tM
                t=tM;
            
            if(t>tmin&&t<h.getT())  //update hit
            {
                Vec3f n; //Normal
                h.set(t,m,n,r);
                return true;
                
            }
            else return false;
               
    }
     bool intersect(const Ray& r, Hit &h,float tmin)
    {
            //Algebraic 
            Vec3f R0=r.getOrigin()-center;
            float a=1;
            float b=2*R0.Dot3(r.getDirection());
            float c=R0.Length()*R0.Length()-radius*radius;
            float delta=b*b-4*a*c;

            if(delta<0) //ray origin outside the shpere tm
                 return false;
            delta=sqrt(delta);
            float tm=(-b-delta)/2.0/a;
            float tM=(-b+delta)/2.0/a;
            float t;
            if (tM<0) return false;  //tM is positive
            if(tm>0) t=tm; //closest positive
            else t=tM;

            if(t>tmin&&t<h.getT())//update hit
                 {
                    Vec3f n; //Normal
                    h.set(t,m,n,r);
                    tmin=t;
                    return true;
                   
                 }
            else return false;
               
    }
    private:
    Vec3f center;
    float radius;
};
#endif