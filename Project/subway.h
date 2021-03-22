#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define MAX_ROUTE_NODE_COUNT 100
#define MAX_NODE_COUNT 200
#define MAX_LINE_COUNT 9
#define MAX_LINE_NUM 8
#define MAX_LINE_NODE 40
#define MAX_COUNT 225
// Departure_time_interval(min)
#define DTI 5
#define START_HOUR 6
#define END_HOUR 23
#define AN_HOUR_60_MINUTES 60
#define CHANGE_LINE_TIME 3.5
#define MAX_ROUTES_COUNT 3

typedef struct crowdedness {
    int sHour;
    int sMinute;
    int eHour;
    int eMinute;
    // 0-100
    int crowdedness;
    struct crowdedness* nextPeriod;
}Crowdedness;
typedef struct {
    //    char name[50];
    int vNodeSeq;
    int eNodeSeq;
    int line;
    int number;
    float weight;
}Data;
typedef struct enode {
    Data data;
    struct enode* nextNode;
}ENode;
typedef struct {
    int totalECount;
    // char name[50];
    int number;
    struct enode* firstNode;
}VNode;
typedef struct {
    VNode vNodes[MAX_NODE_COUNT];
}Graph;
typedef struct {
    int lineNum;
    int totalNodeCount;
    float length;
    float singleLength;
    int maxPeopleCount;
    int defaultCrowdedness;
    // No error checking
    Crowdedness* firstPeriod;
}Line;
typedef struct path {
    int line[MAX_ROUTE_NODE_COUNT];
    int numbers[MAX_ROUTE_NODE_COUNT];
    // base on 6:00 (min)  arrive time
    int time[MAX_ROUTE_NODE_COUNT];
    int costTime;
    float costMoney;
    float length;
    int nodeCount;
    int valid;
    float weightedTime;
    struct path* nextPath;
}Path;
typedef struct {
    Path* firstPath;
}Paths;


extern int DFSMinLength;
extern int currentHour;
extern int currentMinute;
// 6:00-current (minutes)
extern int currentTime;

// 换算时间
void printTime();
void calculateHourMinute(int time, int* hour, int* minute);

// 初始化功能：读取文件内容 对main函数保存的数据进行初始化

void initLine(Line* line);
void initGraph(Graph* graph, Line line[], int numData[][MAX_LINE_NODE]);
void initStationName(char stationName[][20]);
// ----------------------------------------------------------


// 搜索推荐路线功能

// 主函数直接调用
void searchRoutes(Graph lGraph, Line line[], int numData[][MAX_LINE_NODE], char stationName[][20]);
// 传入几号线几号站点 返回其唯一编号
int searchNumber(int numData[][MAX_LINE_NODE], int line, int lineSeq);
// DFS搜索 递归实现 将所有搜索到的路线保存在Path* L中
void DFS(Graph G, int stack[], int visited[], int now, int destination, int top, Paths* L, int shortest, int avoidLines[]);
// 获得路线后遍历每条路线依次计算路线的相关信息
void calculatePathInfo(Graph G, Paths* paths, Line lines[]);
// 计算从这一站到达到下一站到达的时间和加权时间 （直接通过指针修改time和weightedTime
void getNextStationTime(int currentLine, ENode* p, int* time, Line lines[], float* weightedTime);
// 传入路程长度 返回票价
float calculateMoneyCost(float length);
// 选出推荐前三的函数
void sortByTime(Paths* paths, Path* sortedPaths[]);
void sortByMoney(Paths* paths, Path* sortedPaths[]);
void sortByWeightedTime(Paths* paths, Path* sortedPaths[]);
// 传入一条线路 打印路线的信息
void printRoute(Path* p, char stationName[][20]);
// 加入人流量因素后 调用此函数计算权值
float getWeightValue(int line, Line lines[], int time);
// 选择规避人流量后 调用此函数计算规避路线
void initAvoidLines(Line lines[], int avoidLines[]);
// ----------------------------------------------------------

// 预览功能

// 计算时刻表（即指定站点的进站时间 
void getArrivalTime(Graph G, Line lines[], char stationName[][20], int numData[][40]);
int calculateArrivalTime(int start, int end);
// 指定站点 依次浏览
void getNextStation(Graph G, char stationName[][20], int numData[][40]);
// ----------------------------------------------------------


// 设置功能

// 设置当前时间
void setCurrentTime();
// 打印拥挤度规则
void printLineCrowdedness(Line lines[]);
// 先打印拥挤度规则 再选择是否添加新规则
void checkLineCrowdedness(Line lines[]);
// ----------------------------------------------------------

// 结束 释放申请空间
void freePaths(Paths paths);
void freeMemory(Graph G, Line lines[]);

