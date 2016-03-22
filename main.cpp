#include <stdio.h>
#include "matrices.h"
#include "thrust_mapper.h"

void printVect6(vect6 v);
void printVect8(vect8 v);


ThrustMapper testmapper = ThrustMapper();



int main(void)
{
    matrix8_6 m;
    m.t1 = vect6Make(  272,   732,     0,     0,     0,  -131);
    m.t2 = vect6Make(  272,  -732,     0,     0,     0,   131);
    m.t3 = vect6Make( -272,   765,     0,     0,     0,   131);
    m.t4 = vect6Make( -272,  -765,     0,     0,     0,  -131);
    m.t5 = vect6Make(  -37,    28,   -56,   362,   473,     0);
    m.t6 = vect6Make(    0,   -28,   512,  -362,     0,     0);
    m.t7 = vect6Make(   37,     0,   568,     0,  -473,     0);
    m.t8 = vect6Make(    0,     0,     0,     0,     0,     0);
    
    char fileName[100];
    printf("Enter file name of target forces: ");
    scanf("%s",fileName);
    
    vect6 v[100];
    int i = 0;
    
    FILE *fp = fopen(fileName,"r");
    int Lx, Ly, Lz, Rx, Ry, Rz;
    while(!feof(fp) && i < 99)
    {
        fscanf(fp,"%ld, %ld, %ld, %ld, %ld, %ld",&Lx, &Ly, &Lz, &Rx, &Ry, &Rz);
        v[i] = vect6Make(Lx,Ly,Lz,Rx,Ry,Rz);
        //printVect6(v[i]);
        i++;
    }
    
    int maxval = i;
    
    fclose(fp);
    fp = fopen("output.txt","w");
    for(i = 0; i < maxval; ++i)
    {
        testmapper.calculateThrustMap(v[i]);
        vect8 calculated = testmapper.thrust_map; //testmapper.thrust_map;
        printVect8(matMul_86x61(m,v[i]));
        fprintf(fp,"%ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld\n",calculated.a,calculated.b,calculated.c,
                calculated.d,calculated.e,calculated.f,calculated.g,calculated.h);
    }
    
    fclose(fp);
    
}

void printVect6(vect6 v)
{
    printf("%ld, %ld, %ld, %ld, %ld, %ld\n",v.L.x,v.L.y,v.L.z,v.R.x,v.R.y,v.R.z);
}

void printVect8(vect8 v)
{
    printf("%ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld\n",v.a,v.b,v.c,v.d,v.e,v.f,v.g,v.h);
}
