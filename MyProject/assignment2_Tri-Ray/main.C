#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "image.h"
#include "matrix.h"
#include "vectors.h"
#include "scene_parser.h"
#include "material.h"
#include "transform.h"
#include "light.h"
Vec4f& operator*=(Vec4f& v,Matrix& m) {    //supplement
    float x=v.x(),y=v.y(),z=v.z(),w=v.w(),a,b,c,d;
    a = x*m.Get(0,0)+y*m.Get(1,0)+z*m.Get(2,0)+w*m.Get(3,0);
    b = x*m.Get(0,1)+y*m.Get(1,1)+z*m.Get(2,1)+w*m.Get(3,1);
    c = x*m.Get(0,2)+y*m.Get(1,2)+z*m.Get(2,2)+w*m.Get(3,2);
    d = x*m.Get(0,3)+y*m.Get(1,3)+z*m.Get(2,3)+w*m.Get(3,3);
    v.Set(a,b,c,d);
    return v; } 

void GetVec3f(Vec3f &p,Vec4f& v){
    p.Set(v.x(),v.y(),v.z());
}
void RayCasting( SceneParser &parser ,int width,int height,char *output_file,float depth_min,float depth_max ,char *depth_file)
{
     Vec3f Black(0,0,0);
    Image image(width,height);
    image.SetAllPixels(parser.getBackgroundColor());
    Image imageDepth(width,height);
    imageDepth.SetAllPixels(Black);
    
    
    Vec2f Point;
    Vec2f p;
    int flag=0;//debug
    float pixel=1024;

    float i=0.25*pixel,j=0.23*pixel;//debugging
    p.Set(i/pixel,j/pixel);   
    Hit h;
    if(parser.getGroup()->intersect(parser.getCamera()->generateRay(p),h,parser.getCamera()->getTMin()))
    {
        flag++;
        image.SetPixel(width*i/pixel,height*j/pixel,h.getMaterial()->getDiffuseColor());
        if(h.getT()>=depth_min&&h.getT()<=depth_max)
        imageDepth.SetPixel(width*i/pixel,height*j/pixel,h.getMaterial()->getDiffuseColor());
    }

    
    for(float i=0;i<pixel;i++)  //load the camera (0,0)->(1,1) 64*64 Generate Points on the sceen coordinate 
    {
        for(float j=0;j<pixel;j++)
        {
            p.Set(i/pixel,j/pixel);  //Ray - generateRay(p)  //~Camera
            Hit h;
            if(parser.getGroup()->intersect(parser.getCamera()->generateRay(p),h,parser.getCamera()->getTMin()))
            {
                image.SetPixel(width*i/pixel,height*j/pixel,h.getMaterial()->getDiffuseColor());
                Vec3f color;
                float min=0,max=1; //depth 
                if(h.getT()>=depth_min&&h.getT()<=depth_max)
                {
                    float f=1-(max-min)*(h.getT()-depth_min)/(depth_max-depth_min);
                    color.Set(f,f,f);
                }
                else if(h.getT()>depth_max) color.Set(min,min,min);
                
                else if(h.getT()<depth_min) color.Set(max,max,max);
                
                imageDepth.SetPixel(width*i/pixel,height*j/pixel,color);
            }
            
        }
        
    } 
    //cout<<flag;
    image.SaveTGA(output_file);
    imageDepth.SaveTGA(depth_file);
}

void NormalVisualization(SceneParser &parser,int width,int height,char *output_file,char * normals_file)
{
    Image imageNormal(width,height);
    Vec3f Black(0,0,0);
    imageNormal.SetAllPixels(Black);

    Vec2f p;
    float pixel=1024;

    for(float i=0;i<pixel;i++) 
    {
        for(float j=0;j<pixel;j++)
        {
            
            p.Set(i/pixel,j/pixel);  //Ray - generateRay(p)  //~Camera
            Hit h;
            if(parser.getGroup()->intersect(parser.getCamera()->generateRay(p),h,parser.getCamera()->getTMin()))
            { 
                Vec3f normal=h.getNormal();

                if(parser.getCamera()->generateRay(p).getDirection().Dot3(h.getNormal())>0)
                normal.Negate();//back shading
                float min=0,max=1; //depth 
                assert(fabs(normal.Length()-1)<1e-6);
                Vec3f Normalabs(fabs(normal.x()),fabs(normal.y()),fabs(normal.z()));  //absolute value
                imageNormal.SetPixel(width*i/pixel,height*j/pixel,Normalabs);
            }
        }
    } 
    imageNormal.SaveTGA(normals_file);
}

void DiffuseShading( SceneParser &parser ,int width,int height,char *output_file,bool shadeback)
{
    Image image(width,height);
    image.SetAllPixels(parser.getBackgroundColor());

    Vec2f p;
    float pixel=1024;

    //(0,0)->(1,1) 1024*1024 Generate Points on the sceen coordinate
    //load the camera
    
    for(float i=0;i<pixel;i++) 
    {
        for(float j=0;j<pixel;j++)
        {
            p.Set(i/pixel,j/pixel);  //Ray - generateRay(p)  //~Camera
            Hit h;
            if(parser.getGroup()->intersect(parser.getCamera()->generateRay(p),h,parser.getCamera()->getTMin()))
            {
                const Vec3f Ocolor=h.getMaterial()->getDiffuseColor();
                Vec3f am=parser.getAmbientLight();
                Vec3f pcolor(0,0,0);
                Vec3f::Mult(am,am,Ocolor);
                pcolor+=am;
                int lnum= parser.getNumLights();
                
                Vec3f normal=h.getNormal();
                if(parser.getCamera()->generateRay(p).getDirection().Dot3(h.getNormal())>0)
                normal.Negate();//back shading
                for (int l=0;l<lnum;l++)
                {
                    Vec3f ldir,lcolor;
                    parser.getLight(l)->getIllumination(h.getIntersectionPoint(),ldir,lcolor);  
                    // (const Vec3f &p, Vec3f &dir, Vec3f &col)
                    assert(fabs(ldir.Length()-1)<1e-6);
                    float d=0;
                    if(normal.Dot3(ldir)>0) d=normal.Dot3(ldir);
                    lcolor*=d;
                    Vec3f::Mult(lcolor,lcolor,Ocolor);
                    pcolor+=lcolor;
                }
                    assert(fabs(normal.Length()-1)<1e-6);
                    image.SetPixel(width*i/pixel,height*j/pixel,pcolor);
                    Vec3f color;  
            }
            
        }
        
    } 
    image.SaveTGA(output_file);
}


int main(int argc,char *argv[])
{
    // ========================================================
    // ========================================================
    // Some sample code you might like to use for parsing 
    // command line arguments          
    char *input_file = NULL;
    int width = 100;
    int height = 100;
    char *output_file = NULL;
    float depth_min = 0;
    float depth_max = 1;
    char *depth_file = NULL;
    char *normals_file = NULL;
    bool isdepth=false;
    bool isnormal=false;
    bool shadeback=false;
    // sample command line:
    // raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga
    
    for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i],"-input")) {
        i++; assert (i < argc); 
        input_file = argv[i];
    } else if (!strcmp(argv[i],"-size")) {
        i++; assert (i < argc); 
        width = atoi(argv[i]);
        i++; assert (i < argc); 
        height = atoi(argv[i]);
    } else if (!strcmp(argv[i],"-output")) {
        i++; assert (i < argc); 
        output_file = argv[i];
    } else if (!strcmp(argv[i],"-depth")) {
        i++; assert (i < argc); 
        depth_min = atof(argv[i]);
        i++; assert (i < argc); 
        depth_max = atof(argv[i]);
        i++; assert (i < argc); 
        depth_file = argv[i];
        isdepth=true;
    } else if (!strcmp(argv[i],"-normals")) {
        i++; assert (i < argc); 
        normals_file = argv[i];
        isnormal=true;
    } else if (!strcmp(argv[i],"-shade_back")) {
        shadeback=true;
    }
    else {
        printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
        assert(0);
    }
    }
    SceneParser parser(input_file);
    if(isdepth)
    RayCasting(parser,width,height, output_file, depth_min,depth_max ,depth_file);
    
    DiffuseShading(parser,width,height,output_file,shadeback);
    
    if(isnormal)
    NormalVisualization( parser ,width,height, output_file, normals_file);


    // ========================================================
    // ========================================================
  
  
}
