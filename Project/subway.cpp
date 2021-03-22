#define _CRT_SECURE_NO_WARNINGS
#include "subway.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

void initLine(Line* lines) {
    FILE* fp = fopen("./line.txt", "r");
    int i;
    for (i = 0; i < MAX_LINE_COUNT; i++) {
        lines[i].lineNum = 0;
        lines[i].defaultCrowdedness = 0;
        lines[i].firstPeriod = NULL;
    }
    while (fscanf(fp, "%d ", &i) != EOF) {
        fscanf(fp, "%d %f %d", &lines[i].maxPeopleCount, &lines[i].length, &lines[i].totalNodeCount);
        lines[i].lineNum = i;
        lines[i].singleLength = lines[i].length / lines[i].totalNodeCount;
        // printf("%d %d %4f %4f %d",line[i].lineNum,line[i].maxPeopleCount,line[i].length,line[i].singleLength,line[i].totalNodeCount);
    }
    fclose(fp);

    FILE* fp2 = fopen("./crowdedness.txt", "r");
    int line;
    int crowdedness;
    // char data[100];
    // while(fgets(data,100,fp2))printf("%s",data);
    while (fscanf(fp2, "%d %d %d%%", &line, &i, &crowdedness) != EOF)
    {

        Crowdedness* q;
        lines[line].defaultCrowdedness = crowdedness;
        while (i--) {
            Crowdedness* p = (Crowdedness*)malloc(sizeof(Crowdedness));
            fscanf(fp2, "%d:%d-%d:%d %d%%\n", &p->sHour, &p->sMinute, &p->eHour, &p->eMinute, &p->crowdedness);
            q = lines[line].firstPeriod;
            lines[line].firstPeriod = p;
            p->nextPeriod = q;
        }
    }
    fclose(fp2);

}
void initGraph(Graph* graph, Line lines[], int numData[][MAX_LINE_NODE]) {
    Graph lGraph;
    FILE* fp = fopen("./graph.txt", "r");
    int i;
    for (i = 0; i < MAX_NODE_COUNT; i++) {
        lGraph.vNodes[i].firstNode = NULL;
        lGraph.vNodes[i].totalECount = 0;
    }

    char name[50];
    int number;
    int line;
    int lineSeq;
    int fNodeNum = 0;
    while (fscanf(fp, "%d %d %d %s ", &number, &line, &lineSeq, &name) != EOF) {
        lGraph.vNodes[number].number = number;
        numData[line - 1][lineSeq - 1] = number;

        ENode* p;
        if (lineSeq > 1) {
            // f
            lGraph.vNodes[number].totalECount++;
            if (lGraph.vNodes[number].firstNode == NULL) {
                lGraph.vNodes[number].firstNode = (ENode*)malloc(sizeof(ENode));
                p = lGraph.vNodes[number].firstNode;

            }
            else {
                p = lGraph.vNodes[number].firstNode;
                while (p->nextNode != NULL) p = p->nextNode;
                p->nextNode = (ENode*)malloc(sizeof(ENode));
                p = p->nextNode;
            }
            p->data.number = fNodeNum;
            p->data.line = line;
            p->data.vNodeSeq = lineSeq;
            p->data.eNodeSeq = lineSeq - 1;
            p->data.weight = lines[line].singleLength;
            p->nextNode = NULL;

            // n
            lGraph.vNodes[fNodeNum].totalECount++;
            if (lGraph.vNodes[fNodeNum].firstNode == NULL) {
                lGraph.vNodes[fNodeNum].firstNode = (ENode*)malloc(sizeof(ENode));
                p = lGraph.vNodes[fNodeNum].firstNode;

            }
            else {
                p = lGraph.vNodes[fNodeNum].firstNode;
                while (p->nextNode != NULL) p = p->nextNode;
                p->nextNode = (ENode*)malloc(sizeof(ENode));
                p = p->nextNode;
            }
            p->data.number = number;
            p->data.line = line;
            p->data.vNodeSeq = lineSeq - 1;
            p->data.eNodeSeq = lineSeq;
            p->data.weight = lines[line].singleLength;
            p->nextNode = NULL;

        }
        fNodeNum = number;
    }
    fclose(fp);
    *graph = lGraph;

    // ENode* test=lGraph.vNodes[numData[2-1][24-1]].firstNode;
    // while(test!=0) {printf("%d %d %d\n",test->data.line,test->data.vNodeSeq,test->data.eNodeSeq);test=test->nextNode;}
}
void initStationName(char stationName[][20]) {
    FILE* fp = fopen("./station.txt", "r");
    // ASNI
    int index = 0;
    char name[50];
    while (fscanf(fp, "%s ", stationName[index]) != EOF) index++;
    fclose(fp);
}
void searchRoutes(Graph lGraph, Line lines[], int numData[][MAX_LINE_NODE], char stationName[][20]) {
    DFSMinLength = 200;
    int inputLine, inputLineSeq;
    printf("start:");
    scanf("%d %d", &inputLine, &inputLineSeq);
    if (inputLine == 5) {
        printf("No such line.");
        return;
    }

    int startNode = searchNumber(numData, inputLine, inputLineSeq);
    printf("end:");
    scanf("%d %d", &inputLine, &inputLineSeq);
    if (inputLine == 5) {
        printf("No such line.");
        return;
    }
    int endNode = searchNumber(numData, inputLine, inputLineSeq);

    // if wrong
    if (endNode == startNode) {
        printf("Please enter different station\n");
        return;
    }
    printf("from %s to %s\n", stationName[startNode], stationName[endNode]);

    // if in the same line
    int shortest = 0;
    ENode* s = lGraph.vNodes[startNode].firstNode;
    while (s != NULL) {
        ENode* t = lGraph.vNodes[endNode].firstNode;
        while (t != NULL) {
            if (t->data.line == s->data.line) shortest = abs(t->data.vNodeSeq - s->data.vNodeSeq);
            t = t->nextNode;
        }
        s = s->nextNode;
    }

    int stack[MAX_NODE_COUNT] = { 0 };
    int visited[MAX_NODE_COUNT] = { 0 };
    int avoidLines[MAX_LINE_COUNT] = { 0 };
    Paths paths;
    paths.firstPath = NULL;
    int top = 0;
    initAvoidLines(lines, avoidLines);
    DFS(lGraph, stack, visited, startNode, endNode, top, &paths, shortest, avoidLines);
    calculatePathInfo(lGraph, &paths, lines);
    Path* timePaths[MAX_ROUTES_COUNT];
    Path* moneyPaths[MAX_ROUTES_COUNT];
    Path* weightedPaths[MAX_ROUTES_COUNT];
    sortByTime(&paths, timePaths);
    sortByMoney(&paths, moneyPaths);
    sortByWeightedTime(&paths, weightedPaths);
    int index = 0;

    printf("---------------------------------------------------\n");
    printf("According to money cost:\n");
    for (index = 0; index < MAX_ROUTES_COUNT; index++) {
        printRoute(moneyPaths[index], stationName);
    }
    printf("---------------------------------------------------\n");
    printf("\nAccording to time cost\n");
    for (index = 0; index < MAX_ROUTES_COUNT; index++) {
        printRoute(timePaths[index], stationName);

    }
    printf("---------------------------------------------------\n");
    printf("\nAccording to weighted time cost\n");
    for (index = 0; index < MAX_ROUTES_COUNT; index++) {
        printRoute(weightedPaths[index], stationName);
    }
}
void printRoute(Path* p, char stationName[][20]) {
    if (p == NULL) {
        printf("No recommended route.\n");
        return;
    }
    int hour = currentHour;
    int minute = currentMinute;
    int i = 0;
    int line = 0;
    // printf("(line)number:StationName(arrivaltime)");
    while (p->numbers[i] != -1) {
        if (line != p->line[i]) {
            line = p->line[i];
            printf("\nline %d\t", line);
        }
        int number = p->numbers[i];
        calculateHourMinute(p->time[i], &hour, &minute);
        printf("(%d)%d:%s(%d:%d)->", p->line[i], number, stationName[number], hour, minute);
        i++;
    }
    printf("\nall stations:\t%d\tlength:%.4f(km)\n", p->nodeCount, p->length);
    printf("all cost:\ttime-%d(min)\tweightedtime-%.4f\tmoney-%.4f\n", p->costTime, p->weightedTime, p->costMoney);
    printf("\n");
}
// 参数一：图  参数2：栈  参数3：是否访问  参数4：当前位置  参数5：到达的位置  参数6：栈顶  参数7：所有路径的链表
void DFS(Graph G, int stack[], int visited[], int now, int destination, int top, Paths* paths, int shortest, int avoidLines[]) {
    int i;
    ENode* p;
    // cut
    if (top > DFSMinLength) return;
    // avoid line

    stack[top] = now;
    Path* path;
    if (now == destination) {
        // printf("%d %d\n",min,top);
        if (top<DFSMinLength && top>shortest) DFSMinLength = top;
        path = (Path*)malloc(sizeof(Path));
        int i;
        for (i = 0; i <= MAX_NODE_COUNT; i++) path->numbers[i] = -1;
        for (i = 0; i <= top; i++) path->numbers[i] = stack[i];
        // from 0 to top = top+1
        path->nodeCount = top + 1;
        path->nextPath = paths->firstPath;
        paths->firstPath = path;
        return;
    }
    else visited[now] = 1;
    p = G.vNodes[now].firstNode;
    while (p != NULL) {
        // avoid
        if (avoidLines[p->data.line]) {
            p = p->nextNode;
            continue;
        }
        if (!visited[p->data.number])
            DFS(G, stack, visited, p->data.number, destination, top + 1, paths, shortest, avoidLines);
        p = p->nextNode;
    }
    stack[top] = 0;
    visited[now] = 0;
}
int searchNumber(int numData[][MAX_LINE_NODE], int line, int lineSeq) {
    return numData[line - 1][lineSeq - 1];
}
void getNextStation(Graph G, char stationName[][20], int numData[][40]) {
    int number;
    int line, lineSeq;
    printf("station:");
    scanf("%d %d", &line, &lineSeq);

    number = searchNumber(numData, line, lineSeq);

    int direction = 0;
    while (direction != -1) {
        ENode* p = G.vNodes[number].firstNode;
        printf("%d-%s\n", number, stationName[number]);
        int count = G.vNodes[number].totalECount;
        int head = 0;
        while (p != NULL) {
            printf("%d ", head);
            int line = p->data.line;
            int vSeq = p->data.vNodeSeq;
            int eSeq = p->data.eNodeSeq;
            if (eSeq > vSeq) printf("back");
            else printf("front");

            printf(":%d-%d-%d-%s\n", p->data.number, line, eSeq, stationName[p->data.number]);
            head++;
            p = p->nextNode;
        }
        p = G.vNodes[number].firstNode;
        scanf("%d", &direction);
        if (direction > head || direction < 0) {
            printf("Wrong.Try again.\n");
            return;
        }
        int i = direction;
        while (i) {
            i--;
            p = p->nextNode;
        }
        number = p->data.number;
    }
}
void getArrivalTime(Graph G, Line lines[], char stationName[][20], int numData[][40]) {
    int number;
    int line, lineSeq;
    printf("station:");
    scanf("%d %d", &line, &lineSeq);
    getchar();

    number = searchNumber(numData, line, lineSeq);
    printf("%s\n", stationName[number]);
    ENode* p = G.vNodes[number].firstNode;
    if (currentHour<START_HOUR && currentHour>END_HOUR) {
        printf("No subway now");
    }
    else {
        while (p != NULL) {
            int arrivalHour;
            int arrivalMinute;
            int time;
            if (p->data.eNodeSeq > p->data.vNodeSeq) {
                time = calculateArrivalTime(1, p->data.vNodeSeq);
                printf("Line %d(from start):", p->data.line);
            }
            else {
                time = calculateArrivalTime(lines[p->data.line].totalNodeCount, p->data.vNodeSeq);
                printf("Line %d(from end):", p->data.line);
            }
            printf("%d", time);

            arrivalHour = START_HOUR + time / AN_HOUR_60_MINUTES;
            arrivalMinute = time % AN_HOUR_60_MINUTES;
            if (arrivalHour > currentHour || (arrivalHour == currentHour && arrivalMinute >= currentMinute)) printf("arrival time: %d:%d", arrivalHour, arrivalMinute);
            else {
                int beforeH = arrivalHour;
                int beforM = arrivalMinute;
                while (!(arrivalHour == currentHour && arrivalMinute >= currentMinute) && !(arrivalHour > currentHour)) {
                    beforeH = arrivalHour;
                    beforM = arrivalMinute;
                    // 啊垃圾代码 我英文不好甚至只能用中文吐槽 呕吐了 怎么写能这么垃圾恶心的代码 顶不住
                    arrivalMinute += DTI;
                    arrivalHour += arrivalMinute / AN_HOUR_60_MINUTES;
                    arrivalMinute %= AN_HOUR_60_MINUTES;
                }
                printf("arrival time : %d:%d(before) %d:%d(after)", beforeH, beforM, arrivalHour, arrivalMinute);
            }
            printf("\n");
            p = p->nextNode;
        }

    }
}
int calculateArrivalTime(int start, int end) {
    int time;
    if (start = end) time = 0;
    else    time = abs(start - end) * 3 - 1;
    return time;
}
void setCurrentTime() {
    printf("Set current time:  hour:minute (24h)\n");
    // wrong 0-24 0-60
    int ch, cm;
    scanf("%d:%d", &ch, &cm);
    if (ch > 23 || ch < 0 || cm>59 || cm < 0) {
        printf("Wrong.Try again.\n");
        return;
    }
    currentHour = ch;
    currentMinute = cm;
    currentTime = (currentHour - START_HOUR) * 60 + currentMinute;
}
void calculatePathInfo(Graph G, Paths* paths, Line lines[]) {
    Path* currentPath = paths->firstPath;

    while (currentPath != NULL) {
        int number, next;
        int nodeCount = currentPath->nodeCount;
        int index;
        float length = 0;
        float weightedTime = 0;
        int time = 0;
        int currentLine = 0;
        for (index = 0; index < currentPath->nodeCount; index++) {
            // arrive at this station
            currentPath->time[index] = time;
            number = currentPath->numbers[index];
            if (index == currentPath->nodeCount - 1) {
                break;
            }
            else {
                next = currentPath->numbers[index + 1];
                ENode* p = G.vNodes[number].firstNode;
                while (p->data.number != next) p = p->nextNode;
                length += p->data.weight;
                currentPath->line[index] = currentLine;
                getNextStationTime(currentLine, p, &time, lines, &weightedTime);
                currentLine = p->data.line;
            }

        }
        currentPath->line[0] = currentPath->line[1];
        currentPath->line[index] = currentLine;
        currentPath->costTime = time;
        currentPath->weightedTime = weightedTime;
        currentPath->length = length;
        currentPath->costMoney = calculateMoneyCost(length);
        if ((time + currentMinute) / AN_HOUR_60_MINUTES + currentHour >= END_HOUR)   currentPath->valid = 0;
        else   currentPath->valid = 1;
        currentPath = currentPath->nextPath;


    }
}
void getNextStationTime(int currentLine, ENode* p, int* time, Line lines[], float* weightedTime) {
    int timeBackUp = *time;
    // no need to weight
    if (currentLine && currentLine == p->data.line) {
        *time = *time + 1;
    }
    else if (currentLine == 0) {
        // start waiting
        int currentStationTime = currentTime;
        int aTime;
        if (p->data.eNodeSeq > p->data.vNodeSeq) {
            aTime = calculateArrivalTime(1, p->data.vNodeSeq);
        }
        else {
            aTime = calculateArrivalTime(lines[p->data.line].totalNodeCount, p->data.vNodeSeq);
        }

        if (aTime > currentStationTime) *time = aTime - currentTime;
        else {
            int t = (((currentStationTime - aTime) / DTI) * DTI + aTime);
            while (t < currentStationTime) t += DTI;
            *time = t - currentTime;
        }
    }
    else {
        // change line 3.5min (currentStationTime+3.5)
        int currentStationTime = currentTime + *time;

        int aTime;
        // eNode>vNode  1 v->e n
        if (p->data.eNodeSeq > p->data.vNodeSeq) {
            aTime = calculateArrivalTime(1, p->data.vNodeSeq);
        }
        else {
            aTime = calculateArrivalTime(lines[p->data.line].totalNodeCount, p->data.vNodeSeq);
        }
        if ((float)aTime > ((float)currentStationTime + CHANGE_LINE_TIME)) *time = aTime - currentTime;
        else {
            int t = (((currentStationTime - aTime) / DTI) * DTI + aTime);
            while ((float)t < ((float)currentStationTime + CHANGE_LINE_TIME)) t += DTI;
            *time = t - currentTime;
        }
    }
    *weightedTime = *weightedTime + *time - timeBackUp + 2 * getWeightValue(p->data.line, lines, *time);
    *time = *time + 2;
}
float calculateMoneyCost(float length) {
    // basic 2
    float calLength = length;
    float costMoney = 0;
    if (calLength <= 4) {
        costMoney = 2;
    }
    else {
        costMoney = 2;
        if (calLength <= 12) {
            costMoney += (calLength - 4.0) / 4.0;
        }
        else {
            costMoney += 2;
            if (calLength <= 24) {
                costMoney += (calLength - 12.0) / 6.0;
            }
            else {
                costMoney += 2;
                if (calLength <= 40) {
                    costMoney += (calLength - 24.0) / 8.0;
                }
                else {
                    costMoney += 2;
                    if (calLength <= 50) {
                        costMoney += (calLength - 40.0) / 10.0;
                    }
                    else {
                        costMoney += 1;
                        costMoney += (calLength - 50.0) / 20;
                    }
                }
            }
        }
    }
    return costMoney;
}
void sortByTime(Paths* paths, Path* sortedPaths[]) {
    int i = 0;
    int min = -1;
    int max;
    for (i = 0; i < MAX_ROUTES_COUNT; i++) {
        max = 300;
        Path* p = paths->firstPath;
        Path* pSave = NULL;
        while (p != NULL) {
            if (p->costTime < max && p->costTime >= min && p->valid) {
                if (p->costTime == min && i > 0) {
                    int same = 1;
                    int k=0;
                    while (p->numbers[k] > 0) {
                        if (p->numbers[k] != sortedPaths[i - 1]->numbers[k]) {
                            same = 0;
                            break;
                        }
                        k++;
                    }
                    if (same) {
                        p = p->nextPath;
                        continue;
                    }
                }
                pSave = p;
                max = p->costTime;
            }
            p = p->nextPath;
        }
        sortedPaths[i] = pSave;
        min = max;
    }
    // for(i=0;i<MAX_ROUTES_COUNT;i++)
    //     if(sortedPaths[i]!=NULL)sortedPaths[i]->valid=1;

}
void sortByMoney(Paths* paths, Path* sortedPaths[]) {
    int i = 0;
    float min = 0;
    float max;
    for (i = 0; i < MAX_ROUTES_COUNT; i++) {
        max = 50;
        Path* p = paths->firstPath;
        Path* pSave = NULL;
        while (p != NULL) {
            if (p->costMoney<max && p->costMoney>min && p->costTime != -1 && p->valid) {
                pSave = p;
                max = p->costMoney;
            }
            p = p->nextPath;
        }
        sortedPaths[i] = pSave;
        min = max;
    }
    // for(i=0;i<MAX_ROUTES_COUNT;i++){
    //     if(sortedPaths[i]!=NULL)sortedPaths[i]->valid=1;}
}
void sortByWeightedTime(Paths* paths, Path* sortedPaths[]) {
    int i = 0;
    float min = -1;
    float max;
    for (i = 0; i < MAX_ROUTES_COUNT; i++) {
        max = 300;
        Path* p = paths->firstPath;
        Path* pSave = NULL;
        while (p != NULL) {
            if (p->weightedTime<max && p->weightedTime>min && p->costTime != -1 && p->valid) {
                pSave = p;
                max = p->weightedTime;
            }
            p = p->nextPath;
        }
        sortedPaths[i] = pSave;
        min = max;
    }
    // for(i=0;i<MAX_ROUTES_COUNT;i++){
    //     if(sortedPaths[i]!=NULL)sortedPaths[i]->valid=1;}
}
void calculateHourMinute(int time, int* hour, int* minute) {
    *hour = (time + currentMinute) / AN_HOUR_60_MINUTES + currentHour;
    *minute = (time + currentMinute) % AN_HOUR_60_MINUTES;
}
// float weighted value calculate
void checkLineCrowdedness(Line lines[]) {

    printLineCrowdedness(lines);

    int notQuit = 1;
    printf("change Crowdedness? Yes(1):No(0)\n");
    scanf("%d", &notQuit);
    while (notQuit) {
        printf("enter Line Number:");
        int line;
        scanf("%d", &line);
        getchar();

        Crowdedness* p = (Crowdedness*)malloc(sizeof(Crowdedness));
        printf("enter start time and end time(6:0-23:0) and crowdedness(0-100):");
        scanf("%d:%d %d:%d %d", &p->sHour, &p->sMinute, &p->eHour, &p->eMinute, &p->crowdedness);
        if (p->eMinute > 59 || p->eMinute < 0 || p->sMinute>59 || p->sMinute < 0 || p->sHour>23 || p->sHour < 6 || p->eHour>23 || p->eHour<6 || p->sHour>p->eHour || (p->eHour == p->sHour && p->sMinute > p->eMinute) || (p->eHour == 23 && p->eMinute != 0) || p->crowdedness > 100 || p->crowdedness < 0) {
            printf("Please enter time or crowdedness according to specifications");
            continue;
        }
        getchar();

        Crowdedness* q = lines[line].firstPeriod;
        lines[line].firstPeriod = p;
        p->nextPeriod = q;

        printLineCrowdedness(lines);

        printf("change Crowdedness? Yes(1):No(0)\n");
        scanf("%d", &notQuit);
    }
}
void printLineCrowdedness(Line lines[]) {
    int i;
    for (i = 0; i < MAX_LINE_COUNT; i++) {
        if (lines[i].lineNum != 0) {
            printf("line %d: Default Crowdedness:%d%%\n", lines[i].lineNum, lines[i].defaultCrowdedness);
            Crowdedness* p = lines[i].firstPeriod;
            int index = 0;
            while (p != NULL) {
                printf("Period %d\t%d:%d-%d:%d\t%d%%\n", index, p->sHour, p->sMinute, p->eHour, p->eMinute, p->crowdedness);
                p = p->nextPeriod;
                index++;
            }
        }
    }
}
float getWeightValue(int line, Line lines[], int time) {
    int hour = currentHour + time / AN_HOUR_60_MINUTES;
    int minute = currentMinute + time % AN_HOUR_60_MINUTES;
    float weight = 1.0;
    Crowdedness* p;
    int crowdedness = lines[line].defaultCrowdedness;
    p = lines[line].firstPeriod;
    while (p != NULL) {
        if ((p->sHour<hour && p->eHour>hour) || (p->eHour == hour && p->eMinute > minute) || (p->sHour == hour && p->sMinute < minute)) {
            crowdedness = p->crowdedness;
            break;
        }
        p = p->nextPeriod;
    }

    if (crowdedness <= 20) weight = 0.6;
    else if (crowdedness > 70) weight = 1.5;
    else weight = 1.0;

    return weight;
}
void initAvoidLines(Line lines[], int avoidLines[]) {
    int choice;
    printf("Including line crowdedness as a factor in recommended routes?Yes(1):No(0)");
    int hour = currentHour;
    int minute = currentMinute;
    scanf("%d", &choice);
    if (choice) {
        int crowdedness;
        printf("Avoid crowdedness above?(0-100)");
        scanf("%d", &crowdedness);
        int line;
        int lineCrowdedness[MAX_LINE_COUNT];
        // calculate base on crowdedness now

        for (line = 0; line < MAX_LINE_COUNT; line++) {
            if (lines[line].lineNum) {
                Crowdedness* p;
                lineCrowdedness[line] = lines[line].defaultCrowdedness;
                p = lines[line].firstPeriod;
                while (p != NULL) {
                    if ((p->sHour<hour && p->eHour>hour) || (p->eHour == hour && p->eMinute > minute) || (p->sHour == hour && p->sMinute < minute)) {
                        lineCrowdedness[line] = p->crowdedness;
                        break;
                    }
                    p = p->nextPeriod;
                }

                avoidLines[line] = lineCrowdedness[line] >= crowdedness ? 1 : 0;
            }
        }
        for (line = 0; line < MAX_LINE_COUNT; line++)
            if (avoidLines[line]) printf("avoid line %d (crowdedness:%d%%)\n", line, lineCrowdedness[line]);
    }

    // avoid-1
}

void freeMemory(Graph G, Line lines[]) {
    int i;
    for (i = 0; i < MAX_LINE_COUNT; i++) {
        if (lines[i].firstPeriod != NULL) {
            Crowdedness* p = lines[i].firstPeriod;
            Crowdedness* q;
            while (p != NULL) {
                q = p->nextPeriod;
                free(p);
                p = q;
            }
        }
    }
    for (i = 0; i < MAX_NODE_COUNT; i++) {
        if (G.vNodes[i].firstNode != NULL) {
            ENode* p = G.vNodes[i].firstNode;
            ENode* q;
            while (p != NULL) {
                q = p->nextNode;
                free(p);
                p = q;
            }
        }
    }
}
void freePaths(Paths paths) {
    Path* p = paths.firstPath;
    Path* q;
    while (p != NULL) {
        q = p->nextPath;
        free(p);
        p = q;
    }
}
void printTime() {
    printf("CurrentTime-----%d:%d\n", currentHour, currentMinute);
}