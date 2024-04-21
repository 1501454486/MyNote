#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

/*
    知识点：自动计数的枚举
*/

/*
    程序要求：
*/

enum COLOR {RED, YELLOW, GREEN, NumCOLORS};      //tips:在num的最后加上一个记录枚举数量的变量
//这里如果不指定值，默认从0开始依次往后赋值，比如这里RED是0
//但是可以手动声明值，比如: enum COLOR {RED = 1, YELLOW, GREEN = 19}，此时RED是1,YELLOW是2,中间其他值被跳过了

int main(int argc, char const *argv[])
{
    int color = -1;
    char *ColorNames[NumCOLORS] = {
        "red", "yellow", "green"
    };
    char *colorName = NULL;

    printf("输入你喜欢的颜色代码：");
    scanf("%d",&color);
    if (color >= 0 && color < NumCOLORS) {
        colorName = ColorNames[color];
    } else {
        colorName = "unknown";
    }
    printf("你喜欢的颜色是%s\n",colorName);
    
    system("pause");
}



/*
    
*/