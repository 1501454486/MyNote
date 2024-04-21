#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

/*
    知识点：不同算法的效率问题
*/

/*
    程序要求：给定N个整数的序列：A1,A2,...AN,求函数f(i,j)=max{0,A1+...+Ak}的最大值
*/


int main()
{
    
    system("pause");
}

//算法4：在线处理(最优算法)(“在线的意思是指，每输入一个数据就进行即时处理，在任何一个地方终止输入，算法都能正确给出当前的解”)
int MaxSunseqSum4( int A[], int N );
{
    int i;ThisSum = MaxSum = 0;
    for(i=0;i<N;i++){
        ThisSum+=A[i];      //向右累加
        if(ThisSum>MaxSum)
            MaxSum=ThisSum;     //发现更大的时候更新当前结果
        else if(ThisSum<0)
            ThisSum=0;      //则不可能使后面的部分和增大，重新开始
    }
return MaxSum;
}

/*
算法1:
int MaxSubseqSum1(int A[],int N);
{
    int ThisSum,MaxSum=0;
    int i,j,k;
    for(i=0;i<N;i++){    //i是子列左端位置
        ThisSum=0;  //ThisSum是A[i]到A[j]的子列和
        for(k=i;k<=j;k++)
        ThisSum+=A[k];
        if(ThisSunm>MaxSum)   //如果刚得到的这个子列和更大
        MaxSum=ThisSum   //则更新结果
    }    //j循环结束
}    //i循环结束
return MaxSun;                //T=O(N^3)(疑问：复杂度的判断？)

*/

/*
算法2:
int MaxSubseqSum2(int A[],int N)
{
    int ThisSum,MaxSum = 0;
    int i , j;
    for( i = 0; i < N; i++){    //i是子列左端位置
        ThisSum = 0;   //ThisSum是从A[i]到A[j]的子列和
        for( j = i; j < N; j++ ){   //j是子列右端位置
            ThisSum+=A[j];
            //对于相同的i,不同的j,只要在j-1次循环的基础上累加一项即可
            if( ThisSum > MaxSum )   //如果刚得到的这个子列和更大
            MaxSum = ThisSum;   //则更新结果
        }   //j循环结束
    }   //i循环结束
}
return MaxSum;
*/

/*
    
*/