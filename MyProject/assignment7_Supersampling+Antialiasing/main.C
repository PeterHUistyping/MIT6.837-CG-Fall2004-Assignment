#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "image.h"
#include "scene_parser.h"
#include "material.h"
#include "transform.h"
#include "light.h"
#include "glCanvas.h"
#include "raytracer.h"
#include "boundingbox.h"
#include "film.h"
#include "sampler.h"
#include "filter.h"
Vec4f &operator*=(Vec4f &v, Matrix &m)
{ // supplement
    float x = v.x(), y = v.y(), z = v.z(), w = v.w(), a, b, c, d;
    a = x * m.Get(0, 0) + y * m.Get(1, 0) + z * m.Get(2, 0) + w * m.Get(3, 0);
    b = x * m.Get(0, 1) + y * m.Get(1, 1) + z * m.Get(2, 1) + w * m.Get(3, 1);
    c = x * m.Get(0, 2) + y * m.Get(1, 2) + z * m.Get(2, 2) + w * m.Get(3, 2);
    d = x * m.Get(0, 3) + y * m.Get(1, 3) + z * m.Get(2, 3) + w * m.Get(3, 3);
    v.Set(a, b, c, d);
    return v;
}

void GetVec3f(Vec3f &p, Vec4f &v)
{
    p.Set(v.x(), v.y(), v.z());
}

char *input_file = NULL;
int width = 100;
int height = 100;
char *output_file = NULL;
float depth_min = 0;
float depth_max = 1;
char *depth_file = NULL;
char *normals_file = NULL;
bool isdepth = false;
bool is_normal = false;
bool shadeback = false;
bool gui = false;
bool isgouraud = false;
bool shadows = false;
int max_bounces = 0;
float cutoff_weight = 0;
float P_steps = 0, T_steps = 0;
Grid *grid = NULL;
int nx = 0, ny = 0, nz = 0;
bool visualize_grid = false;
SceneParser *parser; // pointer!!
GLCanvas canvas;
RayTracer *ray_tracer;
float epsilon_shadow = 1e-5;
float epsilon = 1e-5; // reflection &refraction
bool debug = false;
bool stats = false;
char *samples_file = NULL;
int samples_zoom = 0;
char *filter_file = NULL;
int filter_zoom = 0;
bool random_samples = false, uniform_samples = false, jittered_samples = false;
int num_samples = 0;
bool box_filter = false, tent_filter = false, gaussian_filter = false;
bool render_filter = false, render_samples = false;
float num_filter = 0;
Sampler *sampler = new UniformSampler(num_samples); // default
Filter *filter = nullptr;
void RayCasting()
{
    Vec3f Black(0, 0, 0);
    Image image(width, height);
    image.SetAllPixels(parser->getBackgroundColor());
    Image imageDepth(width, height);
    imageDepth.SetAllPixels(Black);

    Vec2f Point;
    Vec2f p;
    int flag = 0; // debug

    for (float i = 0; i < width; i++) // load the camera (0,0)->(1,1) width*height Generate Points on the sceen coordinate
    {
        for (float j = 0; j < height; j++)
        {
            p.Set(i / width, j / height); // Ray - generateRay(p)  //~Camera
            Hit h;
            if (parser->getGroup()->intersect(parser->getCamera()->generateRay(p), h, parser->getCamera()->getTMin()))
            {
                image.SetPixel(i, j, h.getMaterial()->getDiffuseColor());
                Vec3f color;
                float min = 0, max = 1; // depth
                if (h.getT() >= depth_min && h.getT() <= depth_max)
                {
                    float f = 1 - (max - min) * (h.getT() - depth_min) / (depth_max - depth_min);
                    color.Set(f, f, f);
                }
                else if (h.getT() > depth_max)
                    color.Set(min, min, min);

                else if (h.getT() < depth_min)
                    color.Set(max, max, max);

                imageDepth.SetPixel(i, j, color);
            }
        }
    }
    // cout<<flag;
    image.SaveTGA(output_file);
    imageDepth.SaveTGA(depth_file);
}

void NormalVisualization()
{
    Image imageNormal(width, height);
    Vec3f Black(0, 0, 0);
    imageNormal.SetAllPixels(Black);

    Vec2f p;

    for (float i = 0; i < width; i++)
    {
        for (float j = 0; j < height; j++)
        {
            p.Set(i / width, j / height); // Ray - generateRay(p)  //~Camera
            Hit h;
            if (parser->getGroup()->intersect(parser->getCamera()->generateRay(p), h, parser->getCamera()->getTMin()))
            {
                Vec3f normal = h.getNormal();
                float min = 0, max = 1; // depth
                assert(fabs(normal.Length() - 1) < 1e-6);
                Vec3f Normalabs(fabs(normal.x()), fabs(normal.y()), fabs(normal.z())); // absolute value
                imageNormal.SetPixel(i, j, Normalabs);
            }
        }
    }
    imageNormal.SaveTGA(normals_file);
}

void traceRayFunction(float x, float y)
{
    Vec2f p;
    p.Set(x, y); // Ray - generateRay(p)  //~Camera
    Hit h;
    Ray ray = parser->getCamera()->generateRay(p);
    Vec3f pcolor = ray_tracer->traceRay(ray, epsilon, 0, 1, 0, h);
}

// STORE in IMAGE
void Rendering()
{
    Image image(width, height); // image.SetAllPixels(parser->getBackgroundColor());
    Vec2f p;                    //(0,0)->(1,1) width*height Generate Points on the sceen coordinate
    Vec3f pcolor;
    Ray ray;
    if (grid != NULL)
    {
        RayTracingStats::Initialize(width, height, grid->getBoundingBox(), nx, ny, nz);
    }
    else
    {
        RayTracingStats::Initialize(width, height, NULL, nx, ny, nz);
    }

    for (float i = 0; i < width; i++)
    { // load the camera
        for (float j = 0; j < height; j++)
        {
            Vec3f pcolor;
            p.Set(i / width, j / height); // Ray - generateRay(p)  //~Camera
            Hit h;
            ray = parser->getCamera()->generateRay(p);
            pcolor += ray_tracer->traceRay(ray, epsilon, 0, 1, 0, h);
            image.SetPixel(i, j, pcolor);
        }
    }
    if (stats)
    {
        RayTracingStats::PrintStatistics();
    }

    image.SaveTGA(output_file);
}

// STORE in FILM
void Supersampling()
{
    Film film(width, height, num_samples);
    Image image(width, height);
    Vec2f p, p2;
    Vec3f pcolor;
    Ray ray;
    if (grid != NULL)
    {
        RayTracingStats::Initialize(width, height, grid->getBoundingBox(), nx, ny, nz);
    }
    else
    {
        RayTracingStats::Initialize(width, height, NULL, nx, ny, nz);
    }
    for (float i = 0; i < width; i++)
    { // load the camera
        for (float j = 0; j < height; j++)
        {
            for (int k = 0; k < num_samples; k++)
            {
                pcolor.Set(0, 0, 0);
                p2.Set(i, j);
                p = sampler->getSamplePosition(k); // Ray - generateRay(p)  //~Camera
                p2 += p;
                p2.Set(p2.x() / width, p2.y() / height);
                Hit h;
                ray = parser->getCamera()->generateRay(p2);
                pcolor += ray_tracer->traceRay(ray, epsilon, 0, 1, 0, h);
                film.setSample(i, j, k, p, pcolor);
            }
        }
    }
    if (stats)
    {
        RayTracingStats::PrintStatistics();
    }
    if (render_samples)
    {
        film.renderSamples(samples_file, samples_zoom);
    }
    else if (render_filter)
    {
        film.renderFilter(filter_file, filter_zoom, filter);
    }
    else
    {
        if (filter != nullptr)
        {
            for (float i = 0; i < width; i++)
            { // load the camera
                for (float j = 0; j < height; j++)
                {
                    image.SetPixel(i, j, filter->getColor(i, j, &film));
                }
            }
            image.SaveTGA(output_file);
        }
        else
        {
            Rendering();
        }
    }
}

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-input"))
        {
            i++;
            assert(i < argc);
            input_file = argv[i];
        }
        else if (!strcmp(argv[i], "-size"))
        {
            i++;
            assert(i < argc);
            width = atoi(argv[i]);
            i++;
            assert(i < argc);
            height = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-output"))
        {
            i++;
            assert(i < argc);
            output_file = argv[i];
        }
        else if (!strcmp(argv[i], "-depth"))
        {
            i++;
            assert(i < argc);
            depth_min = atof(argv[i]);
            i++;
            assert(i < argc);
            depth_max = atof(argv[i]);
            i++;
            assert(i < argc);
            depth_file = argv[i];
            isdepth = true;
        }
        else if (!strcmp(argv[i], "-normals"))
        {
            i++;
            assert(i < argc);
            normals_file = argv[i];
            is_normal = true;
        }
        else if (!strcmp(argv[i], "-shade_back"))
        {
            shadeback = true;
        }
        else if (!strcmp(argv[i], "-gui"))
        {
            gui = true;
        }
        else if (!strcmp(argv[i], "-tessellation"))
        {
            i++;
            assert(i < argc);
            T_steps = atoi(argv[i]);
            assert(360 % (int)T_steps == 0);
            T_steps = 360.0 / T_steps;
            i++;
            assert(i < argc);
            P_steps = atoi(argv[i]);
            assert(180 % (int)P_steps == 0);
            P_steps = 180.0 / P_steps;
        }
        else if (!strcmp(argv[i], "-gouraud"))
        {
            isgouraud = true;
        }
        else if (!strcmp(argv[i], "-shadows"))
        {
            shadows = true;
        }
        else if (!strcmp(argv[i], "-bounces"))
        {
            i++;
            assert(i < argc);
            max_bounces = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-weight"))
        {
            i++;
            assert(i < argc);
            cutoff_weight = (float)atof(argv[i]);
        }
        else if (!strcmp(argv[i], "-grid"))
        {
            i++;
            assert(i < argc);
            nx = atoi(argv[i]);
            i++;
            assert(i < argc);
            ny = atoi(argv[i]);
            i++;
            assert(i < argc);
            nz = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-visualize_grid"))
        {
            visualize_grid = true;
        }
        else if (!strcmp(argv[i], "-stats"))
        {
            stats = true;
        }
        else if (!strcmp(argv[i], "-render_samples"))
        {
            i++;
            assert(i < argc);
            samples_file = argv[i];
            i++;
            assert(i < argc);
            samples_zoom = atoi(argv[i]);
            render_samples = true;
        }
        else if (!strcmp(argv[i], "-render_filter"))
        {
            i++;
            assert(i < argc);
            filter_file = argv[i];
            i++;
            assert(i < argc);
            filter_zoom = atoi(argv[i]);
            render_filter = true;
        }
        else if (!strcmp(argv[i], "-random_samples"))
        {
            i++;
            assert(i < argc);
            num_samples = atoi(argv[i]);
            random_samples = true;
            sampler = new RandomSampler(num_samples);
        }
        else if (!strcmp(argv[i], "-uniform_samples"))
        {
            i++;
            assert(i < argc);
            num_samples = atoi(argv[i]);
            uniform_samples = true;
            sampler = new UniformSampler(num_samples);
        }
        else if (!strcmp(argv[i], "-jittered_samples"))
        {
            i++;
            assert(i < argc);
            num_samples = atoi(argv[i]);
            jittered_samples = true;
            sampler = new JitteredSampler(num_samples);
        }

        else if (!strcmp(argv[i], "-box_filter"))
        {
            i++;
            assert(i < argc);
            num_filter = atof(argv[i]);
            box_filter = true;
            filter = new BoxFilter(num_filter);
        }
        else if (!strcmp(argv[i], "-tent_filter"))
        {
            i++;
            assert(i < argc);
            num_filter = atof(argv[i]);
            tent_filter = true;
            filter = new TentFilter(num_filter);
        }
        else if (!strcmp(argv[i], "-gaussian_filter"))
        {
            i++;
            assert(i < argc);
            num_filter = atof(argv[i]);
            gaussian_filter = true;
            filter = new GaussianFilter(num_filter);
        }
        else
        {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }
    SceneParser pa(input_file);
    parser = &pa;
    Grid gr(parser->getGroup()->getBoundingBox(), nx, ny, nz);
    if (nx != 0)
    {
        grid = &gr;
    }
    else
    {
        grid = NULL;
    }
    RayTracer r_tracer(parser, max_bounces, cutoff_weight, shadows, grid);
    ray_tracer = &r_tracer;

    if (isdepth)
        RayCasting();
    if (is_normal)
        NormalVisualization();
    // Rendering();
    if (!random_samples && !uniform_samples && !jittered_samples)
    {
        uniform_samples = true; // default
        num_samples = 1;
        samples_file = output_file;
    }
    Supersampling();

    if (gui)
    {
        glutInit(&argc, argv);
        canvas.initialize(parser, Rendering, traceRayFunction, grid, visualize_grid);
    }
}
