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

// ����ʱ��
void printTime();
void calculateHourMinute(int time, int* hour, int* minute);

// ��ʼ�����ܣ���ȡ�ļ����� ��main������������ݽ��г�ʼ��

void initLine(Line* line);
void initGraph(Graph* graph, Line line[], int numData[][MAX_LINE_NODE]);
void initStationName(char stationName[][20]);
// ----------------------------------------------------------


// �����Ƽ�·�߹���

// ������ֱ�ӵ���
void searchRoutes(Graph lGraph, Line line[], int numData[][MAX_LINE_NODE], char stationName[][20]);
// ���뼸���߼���վ�� ������Ψһ���
int searchNumber(int numData[][MAX_LINE_NODE], int line, int lineSeq);
// DFS���� �ݹ�ʵ�� ��������������·�߱�����Path* L��
void DFS(Graph G, int stack[], int visited[], int now, int destination, int top, Paths* L, int shortest, int avoidLines[]);
// ���·�ߺ����ÿ��·�����μ���·�ߵ������Ϣ
void calculatePathInfo(Graph G, Paths* paths, Line lines[]);
// �������һվ���ﵽ��һվ�����ʱ��ͼ�Ȩʱ�� ��ֱ��ͨ��ָ���޸�time��weightedTime
void getNextStationTime(int currentLine, ENode* p, int* time, Line lines[], float* weightedTime);
// ����·�̳��� ����Ʊ��
float calculateMoneyCost(float length);
// ѡ���Ƽ�ǰ���ĺ���
void sortByTime(Paths* paths, Path* sortedPaths[]);
void sortByMoney(Paths* paths, Path* sortedPaths[]);
void sortByWeightedTime(Paths* paths, Path* sortedPaths[]);
// ����һ����· ��ӡ·�ߵ���Ϣ
void printRoute(Path* p, char stationName[][20]);
// �������������غ� ���ô˺�������Ȩֵ
float getWeightValue(int line, Line lines[], int time);
// ѡ������������ ���ô˺���������·��
void initAvoidLines(Line lines[], int avoidLines[]);
// ----------------------------------------------------------

// Ԥ������

// ����ʱ�̱���ָ��վ��Ľ�վʱ�� 
void getArrivalTime(Graph G, Line lines[], char stationName[][20], int numData[][40]);
int calculateArrivalTime(int start, int end);
// ָ��վ�� �������
void getNextStation(Graph G, char stationName[][20], int numData[][40]);
// ----------------------------------------------------------


// ���ù���

// ���õ�ǰʱ��
void setCurrentTime();
// ��ӡӵ���ȹ���
void printLineCrowdedness(Line lines[]);
// �ȴ�ӡӵ���ȹ��� ��ѡ���Ƿ�����¹���
void checkLineCrowdedness(Line lines[]);
// ----------------------------------------------------------

// ���� �ͷ�����ռ�
void freePaths(Paths paths);
void freeMemory(Graph G, Line lines[]);

