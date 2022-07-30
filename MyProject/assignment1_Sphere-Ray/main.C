#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "image.h"
#include "matrix.h"
#include "vectors.h"
#include "scene_parser.h"
#include "group.h"
#include "material.h"
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
    } else {
        printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
        assert(0);
    }
    }
    SceneParser parser(input_file);

    Image image(width,height);
    image.SetAllPixels(parser.getBackgroundColor());
    Image imageDepth(width,height);
    imageDepth.SetAllPixels(parser.getBackgroundColor());
 

    Vec2f Point;
    Vec2f p;
    int flag=0;//debug
    float pixel=1024;

    float i=0.75*pixel,j=0.50*pixel;//debugging
    p.Set(i/pixel,j/pixel);   
    Hit h;
    if(parser.getGroup()->intersect(parser.getCamera()->generateRay(p),h,parser.getCamera()->getTMin()))
    {
        flag++;
        image.SetPixel(width*i/pixel,height*j/pixel,h.getMaterial()->getDiffuseColor());
        if(h.getT()>=depth_min&&h.getT()<=depth_max)
        imageDepth.SetPixel(width*i/pixel,height*j/pixel,h.getMaterial()->getDiffuseColor());
    }

    //(0,0)->(1,1) 64*64 Generate Points on the sceen coordinate
    //load the camera
    
    for(float i=0;i<pixel;i++) 
        {
            for(float j=0;j<pixel;j++)
            {
                p.Set(i/pixel,j/pixel);  //Ray - generateRay(p)  //~Camera
                Hit h;
                if(parser.getGroup()->intersect(parser.getCamera()->generateRay(p),h,parser.getCamera()->getTMin()))
                {
                     flag++;
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

    // ========================================================
    // ========================================================
 
    image.SaveTGA(output_file);
    imageDepth.SaveTGA(depth_file);

}