#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*
    知识点：比较两种算法差别
*/

/*
    程序要求：写程序实现一个函数PrintN,使得传入一个正整数为N的参数后，能顺序打印从1到N的全部正整数
*/

void PrintN(int N);
int main()
{
    int N;
    scanf("%d",&N);
    PrintN(N);
    system("pause");
}

//void PrintN(int N)
//{
//   if(N==1)printf("1\n");
//    else{
//        PrintN(N-1);
//        printf("%d\n",N);
//    }
//}

void PrintN(int N)
{
    int i;
    for(i=1;i<=N;i++){
        printf("%d\n",i);
    }
}

/*
    可以注意到，在数据较小的时候两种算法没有差别，在数据较大的时候，第一种可以正常运行，而递归无法正常运行
*/