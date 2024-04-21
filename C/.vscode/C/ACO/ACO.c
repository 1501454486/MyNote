#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

//-------------------宏定义部分-----------------------------------------
//打开的文件，测试的时候就是small.txt，运行的时候修改这里成ATT48.txt
#define DATA "ATT48.txt"
//蚂蚁数量，测试的时候是45，正式运行的时候改成72
#define ANTNUMBER 72
//城市数量，测试的时候是30，正式运行改成48
#define CITYNUMBER 48
#define ALPHA 1                                //信息重要程度因子
#define BETA 3                                 //启发函数重要程度因子
#define RHO 0.3                                //信息素挥发因子
#define Q 10.0                                 //常系数，每个蚂蚁能产生的信息素的量
#define MAXITERS 10000                         //最大迭代次数

typedef struct ant_inf{
    int current;                               //蚂蚁当前所在节点
    int path[CITYNUMBER];                      //记录蚂蚁走过的路径
    int city_allowed[CITYNUMBER];              //记录对于这只蚂蚁而言，其可以访问的城市列表，0表示可以访问，-1表示已经访问过了
    double Length;                             //这只蚂蚁已经走过的距离长度
    double delta_Phero[CITYNUMBER][CITYNUMBER];//这只蚂蚁走过所有路径后，i、j两个城市间的信息素增量
}ANT;

typedef struct{
    double matrix[CITYNUMBER][CITYNUMBER];     //信息素矩阵，记录各节点之间信息素浓度
} PheromoneMatrix;
//--------------------------------------------------------------------


//-------------------函数声明部分--------------------------------------
//read_data用来读取数据，成功返回0，否则1
int **read_data(FILE *fp);        //返回坐标矩阵
//计算距离，接受城市数和坐标矩阵，输出距离矩阵
double **cal_dist(int **coor);
//初始化信息素浓度函数
void init_Phero(PheromoneMatrix *Pheromones);
//初始化蚂蚁信息的函数
void init(ANT *Ants);
//概率函数，蚂蚁由此判断下一个该去哪个城市
int possibility(ANT *Ants, PheromoneMatrix *Pheromones, int i, double (*Eta)[CITYNUMBER]);
//城市选择函数，采用轮盘赌方法，传入概率数组、可以去的城市数量和最大的城市，传出选择的城市
int ChooseCity(double *probs, int num_city, int max_city);
//用蚁周算法更新信息素函数，返回更新后的信息素
PheromoneMatrix *Update_Pheromones(PheromoneMatrix *Pheromones, ANT *Ants);
//移动蚂蚁函数，跑完一次，需要传入这一轮的信息素、距离矩阵和启发矩阵，跑完这一轮后更新信息素并返回当前最短的蚂蚁的标签
int AntsMove(ANT *Ants, PheromoneMatrix *Pheromones, double (*Eta)[CITYNUMBER], double **dist);
//--------------------------------------------------------------------

//------------------主函数部分-----------------------------------------
int main(int argc, char const *argv[])
{
    srand(time(NULL));
    int i,j;
    int iter;                                   //迭代次数
    int minindex=0;                             //距离最短的蚂蚁的标签
    FILE *fp = fopen(DATA, "r");
    int **coor;                                 //坐标矩阵
    double **dist;                              //距离矩阵
    double Eta[CITYNUMBER][CITYNUMBER];         //启发矩阵，距离的倒数

    coor = read_data(fp);
    dist = cal_dist(coor);
    ANT Ants[ANTNUMBER];                        //蚁群数组
    ANT MinAnt;                                 //用于存储最小的蚂蚁信息
    PheromoneMatrix Pheromones;                 //信息素矩阵
    //用距离矩阵计算出启发矩阵
    for(i=0;i<CITYNUMBER;i++){
        for(j=0;j<CITYNUMBER;j++){
            if(dist[i][j] != 0) Eta[i][j]=1.0/dist[i][j];
            else Eta[i][j] = INFINITY;
        }
    }
    //初始化信息素矩阵
    init_Phero(&Pheromones);
    //开始迭代
    for(iter=0;iter<MAXITERS;iter++){
        init(Ants);                                             //每轮开始的时候都初始化蚂蚁信息
        minindex = AntsMove(Ants,&Pheromones,Eta,dist);
        if(Ants[minindex].Length < MinAnt.Length||iter == 0){   //比较这轮循环后得到的是否比原来短
            memcpy(&MinAnt, &Ants[minindex], sizeof(ANT));      //如果是就拷贝过去
        }
        if(iter<=100){
            if(iter%10==0||iter==0){
                printf("%d次迭代，当前最短路程为：%f\n",iter,MinAnt.Length);
            } 
        }else{
            if(iter%200==0){
                printf("%d次迭代，当前最短路程为：%f\n",iter,MinAnt.Length);
            }
        }
    }

    printf("最短线路为：%d",MinAnt.path[0]);
    for(i=1;i<CITYNUMBER;i++){
        printf("->%d",MinAnt.path[i]);
    }
    printf("\n");
    printf("最短距离为：%f",MinAnt.Length);

    return 0;
}
//------------------------------------------------------------------------

//------------------------函数定义部分-----------------------------------------
int **read_data(FILE *fp)
{
    int city_number;
    int i;
    fscanf(fp,"%d",&city_number);
    int **coor = (int **)malloc(CITYNUMBER * sizeof(int *));     //创建坐标矩阵
    for(i=0 ; i < CITYNUMBER ; i++){
        coor[i] = (int *)malloc(2 * sizeof(int));
        fscanf(fp,"%d",&coor[i][0]);                             //读取x
        fscanf(fp,"%d",&coor[i][1]);                             //读取y
    }
    fclose(fp);
    return coor;
}


double **cal_dist(int **coor)
{
    int i,j;
    double **dist = (double **)malloc(CITYNUMBER * sizeof(double *));
    for(i=0 ; i < CITYNUMBER ; i++){
        dist[i] = (double *)malloc(CITYNUMBER * sizeof(double));
        for(j=i ; j < CITYNUMBER ; j++){
            if( i == j){
                dist[i][j] = 0;
            }else{
                int xi = coor[i][0];
                int yi = coor[i][1];
                int xj = coor[j][0];
                int yj = coor[j][1];
                dist[i][j] = sqrt(pow(xi - xj , 2) + pow(yi - yj , 2));
            }
        }
    }
    for(i = 0 ; i < CITYNUMBER ; i++){
        for(j = 0 ; j < i ; j++){
            dist[i][j] = dist[j][i];
        }
    }
    return dist;
}

void init_Phero(PheromoneMatrix *Pheromones)               //用两个循环初始化信息素矩阵
{
    int i,j;
    for(i=0;i<CITYNUMBER;i++){
        for(j=0;j<CITYNUMBER;j++){
            Pheromones->matrix[i][j] = 1.0;
        }
    }
}

void init(ANT *Ants)
{
    int i,j,k;
    srand(time(NULL));                                      //使用当前时间作为随机数种子
    for(i=0;i<ANTNUMBER;i++){
        Ants[i].current = rand() % CITYNUMBER;              //随机分配蚂蚁初始位置
        Ants[i].Length = 0.0;                               //蚂蚁走过的路初始化为0
        memset(Ants[i].path, -1, sizeof(Ants[i].path));     //将蚂蚁路径初始化为-1
        memset(Ants[i].city_allowed,0,sizeof(Ants[i].city_allowed));//将蚂蚁可访问城市初始化为0
        Ants[i].path[0] = Ants[i].current;                  //将初始位置添加到路径中
        Ants[i].city_allowed[Ants[i].current] = -1;         //-1表示已经去过了某个城市
        for(j=0;j<CITYNUMBER;j++){
            for(k=0;k<CITYNUMBER;k++){
                Ants[i].delta_Phero[j][k] = 0;              //初始化信息素增量
            }
        }
    }
}

//第i只蚂蚁，算出到剩下城市的距离并判断，选择概率最大的，返回该城市序号
//注意不要直接选择概率最大的，采用轮盘赌方法随机选择，避免陷入局部最优
int possibility(ANT *Ants, PheromoneMatrix *Pheromones, int i, double (*Eta)[CITYNUMBER])
{
    int j;
    double pmax = 0, p=0;
    int next_city = 0;                    //下一步要去的城市
    double sum = 0;                       //概率公式的分母
    double probs[CITYNUMBER] = {0};       //下一步要去的城市组成的概率的数组
    int num_city = 0;                     //可以访问的城市的数量
    int max_city;                         //可以访问的最大的城市
    for(j=0;j<CITYNUMBER;j++){
        if(Ants[i].city_allowed[j] == 0){
            //如果==0表示这个城市还没去过，加起来
            sum+=pow(Pheromones->matrix[Ants[i].current][j],ALPHA)*pow(Eta[Ants[i].current][j],BETA);
            num_city++;
            max_city = j;
        }
    }
    //因为最后蚂蚁需要回到原点，这里需要判断是否是最后一次
    if(Ants[i].path[CITYNUMBER-1] != -1){
        next_city = Ants[i].path[0];                //path[0]表示起点
    }else{
        for(j=0;j<CITYNUMBER;j++){                  //j是城市编号
            if(Ants[i].city_allowed[j] == 0){       //如果这个城市还没去过，计算概率
                p = pow(Pheromones->matrix[Ants[i].current][j],ALPHA)*pow(Eta[Ants[i].current][j],BETA)/sum;
                probs[j] = p;
                next_city = ChooseCity(probs, num_city, max_city);
                }
            }
        }
    return next_city;    
}

PheromoneMatrix *Update_Pheromones(PheromoneMatrix *Pheromones, ANT *Ants)
{
    int i,j,k;
    double Ants_delta_Pheromone;
    for(i=0;i<CITYNUMBER;i++){
        for(j=0;j<CITYNUMBER;j++){
            Ants_delta_Pheromone = 0;
            for(k=0;k<ANTNUMBER;k++){
                Ants_delta_Pheromone += Ants[k].delta_Phero[i][j];
            }
            Pheromones->matrix[i][j] = (1-RHO)*Pheromones->matrix[i][j]+Ants_delta_Pheromone;
        }
    }
    return Pheromones;
}

int AntsMove(ANT *Ants, PheromoneMatrix *Pheromones, double (*Eta)[CITYNUMBER], double **dist)
{
    int i,j;
    int next_city;
    int index = 0;                                                //记录最短的蚂蚁的标签
    for(i=0;i<CITYNUMBER;i++){                                    //第i次移动，注意要回到原城市
        for(j=0;j<ANTNUMBER;j++){                                 //对于第j只蚂蚁
            next_city = possibility(Ants, Pheromones, j, Eta);
            //更新需要做的：1、长度累加2、标记这个城市来过了3、路径记录4、移动蚂蚁
            Ants[j].Length += dist[Ants[j].current][next_city];   //目前在的城市和目标城市的距离
            if(i < CITYNUMBER-1){
                Ants[j].path[i+1] = next_city;
                Ants[j].city_allowed[next_city] = -1;             //已经去过了这个城市
            }
            Ants[j].current = next_city;
        }
    }
    //更新信息素增加矩阵
    for(i=0;i<ANTNUMBER;i++){
        for(j=0;j<CITYNUMBER-1;j++){            //由于是对称矩阵，所以对称位置都要增加
            Ants[i].delta_Phero[Ants[i].path[j]][Ants[i].path[j+1]] = Q / Ants[i].Length;
            Ants[i].delta_Phero[Ants[i].path[j+1]][Ants[i].path[j]] = Q / Ants[i].Length;
        }
        Ants[i].delta_Phero[Ants[i].path[j]][Ants[i].path[0]] = Q / Ants[i].Length;
        Ants[i].delta_Phero[Ants[i].path[0]][Ants[i].path[j]] = Q / Ants[i].Length;
    }
    //更新信息素
    Pheromones = Update_Pheromones(Pheromones,Ants);
    //在蚂蚁中比较，找出这轮最短的路径
    for(i=0;i<ANTNUMBER;i++){
        if(Ants[i].Length<Ants[index].Length){
            index = i;
        }
    }
    return index;
}

int ChooseCity(double *probs, int num_city, int max_city)
{
    int i,j;
    int minindex=0;                         //最小下标，最后应该probs[min]<rand
    double random_number = (double)rand() / RAND_MAX;
    double sum = 0.0;                       //概率累计
    for(i=0;i<CITYNUMBER;i++){              //如果在前面某个城市的概率累计比随机数大，就返回
        sum += probs[i];
        if(sum >= random_number){
            return i;
        }
    }
    return max_city;                        //否则返回最后一个城市
}