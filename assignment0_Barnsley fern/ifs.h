#ifndef _IFS_H_
#define _IFS_H_
#include <string.h>
#include <cstdio>
#include <stdlib.h>
#include "matrix.h"
#include "image.h"
const int n=10;
class IFS{
public:
    int num;
    //Matrix trans[n];  //trans[num] non static num
    //float pro[n];
    Matrix *trans;
    float *pro;
    IFS()
    {
        num=0;
    }
    IFS(const int& num_trans) 
    { 
        num=num_trans;   
        trans=new Matrix[num];
        pro=new float[num];
    //Matrix size!=0 //memset(trans,0,sizeof(trans));    //memset(pro,0,sizeof(pro));
    //'memset' call operates on objects of type 'float' while the size is based on a different type 'float *' [-Wsizeof-pointer-memaccess]
    }
       
    void input(int i,const float& probability ,const Matrix & m);
   
    void render(Image& image, const int & num_points,const int & num_iters);
     
    ~IFS()
    {
        delete [] trans;
        delete [] pro;
    }
};
#endif