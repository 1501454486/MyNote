#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define maxn 200
#define K 10000
#define min(x, y) ((x) < (y) ? (x) : (y))
typedef enum { false,
               true } Bool;
double Sigmoid(double x) { return 1.0 / (1.0 + exp(-x)); }

/*
 * Store the map in a 2D array,
 * where Map[i][j] is the time to pass the cell (i, j) (1 <= i <= N, 1 <= j <= M)
 * S_x, S_y is the coordinate of the start point, and T_x, T_y is the coordinate of the destination
 * Direction: 0 - right, 1 - down, 2 - left, 3 - up
 * The check(x, y) function is used to check whether the cell (x, y) is valid to pass
 * The time to pass the cell (i, j) is Dis(Map[i][j])
 * 
 * 将地图存储在一个二维数组中，
    其中Map[i][j]表示通过单元格(i, j)所需的时间（1 <= i <= N，1 <= j <= M）
    S_x，S_y是起点的坐标，T_x，T_y是目的地的坐标
    方向：0 - 右，1 - 下，2 - 左，3 - 上
    check(x, y)函数用于检查单元格(x, y)是否可以通过
    通过单元格(i, j)所需的时间为Dis(Map[i][j])
 */

int N, M;
int S_x, S_y, T_x, T_y;
char Map[maxn][maxn];
const int Dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
Bool check(int x, int y) { return (x >= 1 && x <= N && y >= 1 && y <= M && Map[x][y] != '0'); }
int Dis(char c) { return (c == 'T' || c == 'S') ? 0 : 1; }

/*
    * ReadMap:
    * Read the map from the file with the following format:
    * First line:   two integers N and M, the number of rows and columns of the map
    * Next N lines: M characters, each character is in digit '0' / '1', representing the time to pass the cell
                  (Please make sure there is no space in these lines)
    * Mind: The (1, 1) cell should be 'S' and the (N, M) cell should be 'T'

    从文件中按以下格式读取地图：
    第一行：两个整数N和M，表示地图的行数和列数
    接下来的N行：M个字符，每个字符为数字'0' / '1'，表示通过单元格所需的时间
    （请确保这些行中没有空格）
    注意：(1, 1)单元格应为'S'，(N, M)单元格应为'T'
*/

void ReadMap(FILE* fp) {
    fscanf(fp, "%d%d%*c", &N, &M);
    for (int i = 1; i <= N; i++) fscanf(fp, "%s", Map[i] + 1);

    for (int i = 1; i <= N; i++)
        for (int j = 1; j <= M; j++) {
            if (Map[i][j] == 'S') S_x = i, S_y = j;
            if (Map[i][j] == 'T') T_x = i, T_y = j;
        }
    fclose(fp);
    if (!S_x || !T_x) {
        puts("MAP FORMAT ERROR");
        exit(-1);
    }
}

/*
    * The function gives the score of your map by running the BFS algorithm
    * Score:
            1. The count of the steps the algorithm runs
            (The bias of the algorithm's result is not considered, because the BFS algorithm is optimal)
    * Bfs:
            The breadth first search algorithm
            The algorithm works like pouring water into the map
            Every time all the frontiers of the water will expand to the cells next to them
该函数通过运行BFS算法来给出地图的得分

得分：
算法运行的步数
（不考虑算法结果的偏差，因为BFS算法是最优的）
BFS：
广度优先搜索算法
该算法的工作方式类似于将水倒入地图中
每次水的边界都会扩展到其相邻的单元格上
*/

int Answer_Bfs, Count_Bfs;
struct Point {
    int x, y;
} Q[maxn * maxn];
int Dst[maxn][maxn];
typedef struct Point* Pt;
void Bfs() {
    int H = 0, T = 1;
    Q[1] = (struct Point){S_x, S_y}, Dst[S_x][S_y] = 0;
    while (H != T) {
        Pt p = &Q[++H];
        Count_Bfs++;
        if (Map[p->x][p->y] == 'T') return (void)(Answer_Bfs = Dst[p->x][p->y]);
        for (int i = 0; i < 4; i++) {
            int nx = p->x + Dir[i][0], ny = p->y + Dir[i][1];
            if (check(nx, ny) && Map[nx][ny] != 'S' && !Dst[nx][ny]) {
                Dst[nx][ny] = Dst[p->x][p->y] + Dis(Map[nx][ny]);
                Q[++T] = (struct Point){nx, ny};
            }
        }
    }
}
double Judge_Bfs(double Weight, FILE* fp) {
    puts("Judging the algorithm Bfs...");
    Count_Bfs = 0, Answer_Bfs = K * N * M;
    Bfs();
    double Score = Sigmoid(10.0 * Count_Bfs / (N * M)) * Weight;
    printf("You get %.3lf / %.0lf this sub-task!\n", Score, Weight);
    fprintf(fp, "Task1:\t%6.3lf / %.0lf\n", Score, Weight);
    return Score;
}

/*
    * The function gives the score of your map by running the naive DFS algorithm
    * Score:
            1. The count of the steps the algorithm runs to find the shortest path
            2. The bias of the algorithm's result after at most K * N * M steps
                --> *If the program can't find the correct result, you'll get almost full score this sub-task
    * Dfs_naive:
            The naive deep first search algorithm
            Each time, the algorithm will choose the next step in the order of "right - down - left - up"
            Just keep going until getting to the destination
            该函数通过运行朴素的DFS算法来给出地图的得分

得分：
算法运行的步数，用于找到最短路径
在最多K * N * M步之后，算法结果的偏差
--> *如果程序无法找到正确的结果，则在这个子任务中将获得几乎满分的得分
Dfs_naive：
朴素的深度优先搜索算法
每次，算法会按照“右 - 下 - 左 - 上”的顺序选择下一步
一直前进直到到达目的地
*/

int Answer_Dfs_naive, Count_Dfs_naive;
Bool vis[maxn][maxn];
void Dfs_naive(int x, int y, int T) {
    if (T > K * N * M || Answer_Dfs_naive == Answer_Bfs) return;
    Count_Dfs_naive++;
    if (Map[x][y] == 'T') return (void)(Answer_Dfs_naive = min(Answer_Dfs_naive, T));
    for (int i = 0; i < 4; i++) {
        int nx = x + Dir[i][0], ny = y + Dir[i][1];
        if (check(nx, ny) && vis[nx][ny] == false) {
            vis[nx][ny] = true;
            Dfs_naive(nx, ny, T + Dis(Map[nx][ny]));
            vis[nx][ny] = false;
        }
    }
}
double Judge_Dfs_naive(double Weight, FILE* fp) {
    puts("Judging the algorithm Dfs_naive...");
    Count_Dfs_naive = 0, Answer_Dfs_naive = K * N * M;
    Dfs_naive(S_x, S_y, 0);
    double Score = Sigmoid(5.0 * Count_Dfs_naive / (N * M)) * Weight;
    printf("You get %.3lf / %.0lf this sub-task!\n", Score, Weight);
    fprintf(fp, "Task2:\t%6.3lf / %.0lf\n", Score, Weight);
    return Score;
}

/*
 * The function gives the score of your map by running the random DFS algorithm
 * Both score and the implementation of the algorithm are similar to the Dfs_naive
 * But this time, the algorithm will choose the next step randomly
 * 该函数通过运行随机DFS算法来给出地图的得分

得分和算法的实现方式与Dfs_naive类似
但是这次，算法会随机选择下一步

 */

int Answer_Dfs_random, Count_Dfs_random;
Bool vst[maxn][maxn];
void swap(int* x, int* y) {
    int t = *x;
    *x = *y, *y = t;
}
void Dfs_random(int x, int y, int T) {
    if (T > K * N * M || Answer_Dfs_random == Answer_Bfs) return;
    Count_Dfs_random++;
    if (Map[x][y] == 'T') return (void)(Answer_Dfs_random = min(Answer_Dfs_random, T));
    int id[4] = {0, 1, 2, 3};
    for (int i = 0; i < 4; i++) swap(&id[i], &id[rand() % 4]);
    for (int j = 0; j < 4; j++) {
        int i = id[j];
        int nx = x + Dir[i][0], ny = y + Dir[i][1];
        if (check(nx, ny) && vst[nx][ny] == false) {
            vst[nx][ny] = true;
            Dfs_random(nx, ny, T + Dis(Map[nx][ny]));
            vst[nx][ny] = false;
        }
    }
}
double Judge_Dfs_random(double Weight, FILE* fp) {
    puts("Judging the algorithm Dfs_random...");
    srand(2411);  // 'ACEE'
    Count_Dfs_random = 0, Answer_Dfs_random = K * N * M;
    Dfs_random(S_x, S_y, 0);
    double Score = Sigmoid(2.0 * Count_Dfs_random / (N * M)) * Weight;
    printf("You get %.3lf / %.0lf this sub-task!\n", Score, Weight);
    fprintf(fp, "Task3:\t%6.3lf / %.0lf\n", Score, Weight);
    return Score;
}

/*
    * The function gives the score of your map by running the optimized DFS algorithm
    * Both score and the implementation of the algorithm are similar to the Dfs_naive
    * But the algorithm will record the best time to get to each cell (x, y) in Ds[x][y],
      and prune the branch if the time is larger than the recorded time,
      which will optimize the algorithm
      该函数通过运行优化的DFS算法来给出地图的得分

得分和算法的实现方式与Dfs_naive类似
但是算法会记录到达每个单元格(x, y)的最佳时间，并在时间大于记录时间时剪枝，从而优化算法
*/

int Answer_Dfs_optim, Count_Dfs_optim;
int Ds[maxn][maxn];
void Dfs_optim(int x, int y, int T) {
    if (T > K * N * M || Answer_Dfs_optim == Answer_Bfs) return;
    Count_Dfs_optim++;
    if (Map[x][y] != 'S' && T >= Ds[x][y]) return;
    Ds[x][y] = T;
    if (Map[x][y] == 'T') return (void)(Answer_Dfs_optim = T);
    for (int i = 0; i < 4; i++) {
        int nx = x + Dir[i][0], ny = y + Dir[i][1];
        if (check(nx, ny) && Map[nx][ny] != 'S') Dfs_optim(nx, ny, T + Dis(Map[nx][ny]));
    }
}
double Judge_Dfs_optim(double Weight, FILE* fp) {
    puts("Judging the algorithm Dfs_optim...");
    Count_Dfs_optim = 0, Answer_Dfs_optim = K * N * M;
    for (int i = 1; i <= N; i++)
        for (int j = 1; j <= M; j++) Ds[i][j] = K * N * M;
    Ds[S_x][S_y] = 0;
    Dfs_optim(S_x, S_y, 0);
    double Score = Sigmoid(2.0 * Count_Dfs_optim / (N * M)) * Weight;
    printf("You get %.3lf / %.0lf this sub-task!\n", Score, Weight);
    fprintf(fp, "Task4:\t%6.3lf / %.0lf\n", Score, Weight);
    return Score;
}

/*
 * The function gives the score of your map by measuring the flatness
 * Concretely, the function will use 3 * 3 filters to measure the flatness of the map
 * If all the cells in the filter are the same, the flatness will be larger, thus cause a decline in your score
该函数通过测量地图的平坦程度来给出地图的得分

具体而言，该函数将使用3 * 3的滤波器来测量地图的平坦程度
如果滤波器中的所有单元格都相同，则平坦度较大，从而导致得分下降
 */

int Count_Filter;
double Judge_Filter(double Weight, FILE* fp) {
    puts("Judging the flatness of the map...");
    for (int i = 2; i < N; i++)
        for (int j = 2; j < M; j++) {
            Bool flag = true;
            for (int p = -1; p <= 1; p++)
                for (int q = -1; q <= 1; q++)
                    if (Map[i + p][j + q] != Map[i][j]) flag = false;
            if (flag == true) Count_Filter++;
        }
    double Score = Sigmoid(-Count_Filter / 10.0) * 2.0 * Weight;
    printf("You get %.3lf / %.0lf this sub-task!\n", Score, Weight);
    fprintf(fp, "Task5:\t%6.3lf / %.0lf\n", Score, Weight);
    return Score;
}

/*
    * The function gives the score of your map by a randomly roaming robot.
    * The algorithm simulates a robot randomly roaming in the map
        Every time it will choose a valid direction to go and never go back
        Every time it fails(stuck), it will choose the current place as its next start points
    * If it can easily get to the destination, the map is considered not so complicated enough
该函数通过一个随机漫游的机器人来给出地图的得分

该算法模拟了一个随机漫游的机器人在地图中移动
每次它会选择一个有效的方向前进，不会返回之前的位置
每次失败（卡住）时，它将选择当前位置作为下一个起始点
如果机器人能够轻松到达目的地，则认为地图不够复杂
*/
Bool Judge_Random_roam_one_time(int* x, int* y) {
    int px = *x, py = *y;
    while (*x != T_x || *y != T_y) {
        int valid_dir = 0;
        for (int i = 0; i < 4; i++) {
            int nx = *x + Dir[i][0], ny = *y + Dir[i][1];
            if (check(nx, ny) && !(nx == px && ny == py)) valid_dir++;
        }
        if (!valid_dir) return false;
        while (true) {
            int dir = rand() % 4;
            int nx = *x + Dir[dir][0], ny = *y + Dir[dir][1];
            if (check(nx, ny) && !(nx == px && ny == py)) {
                px = *x, py = *y;
                *x = nx, *y = ny;
                break;
            }
        }
    }
    return true;
}
double Judge_Random_roam(double Weight, FILE* fp) {
    puts("Judging the Random_roam...");
    int Count_Random_roam = 0;
    int x = S_x, y = S_y;
    for (int i = 0; i < N * M * 100; i++) {
        if (Judge_Random_roam_one_time(&x, &y))
            Count_Random_roam++, x = S_x, y = S_y;
    }
    double Score = Sigmoid(-Count_Random_roam / 300.0) * 2.0 * Weight;
    printf("You get %.3lf / %.0lf this sub-task!\n", Score, Weight);
    fprintf(fp, "Task6:\t%6.3lf / %.0lf\n", Score, Weight);
    return Score;
}

/*
    * Please call the program in the command line as follows (No ):
        "./Judge <MapFile>"
*/

int main(int argc, char* argv[]) {
    FILE* fp = fopen("B3test.map", "r");
    if (fp == NULL) {
        puts("FILE OPEN ERROR");
        return 1;
    }
    ReadMap(fp);

    FILE* fo = fopen("Scores.txt", "w");
    double Ans = 0;
    Ans += Judge_Bfs(10.0, fo);
    Ans += Judge_Dfs_naive(20.0, fo);
    Ans += Judge_Dfs_random(20.0, fo);
    Ans += Judge_Dfs_optim(20.0, fo);
    Ans += Judge_Filter(10.0, fo);
    Ans += Judge_Random_roam(20.0, fo);

    printf("You successfully get %.3lf!\n", Ans);
    fprintf(fo, "Total:\t%6.3lf\n", Ans);
    return 0;
}