#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#define MAXK 1e7

/*
    知识点：
*/

/*
    程序要求：写程序计算给定多项式在给定点x处的值
*/

clock_t start,stop;
double duration;
double f(int n,double a[],double x);
int main()
{
    int n,i,j;
    printf("Enter n:");
    scanf("%d",&n);
    double a[n];
    printf("Enter a[n]:");
    for(i=0;i<n;i++){
        scanf("%lf",&a[i]);
    }
    double x;
    printf("Enter x;");
    scanf("%lf",&x);
    
    start=clock();
    for(j=0;j<MAXK;j++){
        double f(n,a,x);
    }
    stop=clock();
    duration=((double)(stop-start))/CLK_TCK;
    printf("time:%6.2e",duration);
    system("pause");
}

double f(int n,double a[],double x)
{
    int i;
    double p=a[n];
    for(i=n;i>0;i--)
        p=a[i-1]+x*p;
    return p;
}

/*
    
*/