#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"subway.h"
#include"login.h"

int DFSMinLength;
int currentHour;
int currentMinute;
// 6:00-current (minutes)
int currentTime;

int main() {

    if (!loginInterface()) return 0;
    printf("\n----------------------------------------------------------------------\n");
    // init
    Line lines[MAX_LINE_COUNT];
    initLine(lines);
    Graph lGraph;
    // numData[line-1][lineSeq-1]=number;
    int numData[MAX_LINE_NUM][40] = { 0 };
    initGraph(&lGraph, lines, numData);
    char stationName[MAX_NODE_COUNT][20];
    initStationName(stationName);
    currentHour = 6;
    currentMinute = 0;
    // 6:00-current (minutes)
    currentTime = 0;


    int choice = 1;
    while (choice) {
        printTime();
        printf("1.setCurrentTime\n2.searchRoute\n3.viewStationInfo\n4.changeCrowdednessInfo\n5.getArrivalTime\n0.exit\n");
        scanf("%d", &choice);
        switch (choice)
        {
        case 0:
            break;
        case 1:
            setCurrentTime();
            break;
        case 2:
            searchRoutes(lGraph, lines, numData, stationName);
            break;
        case 3:
            getNextStation(lGraph, stationName, numData);
            break;
        case 4:
            checkLineCrowdedness(lines);
            break;
        case 5:
            getArrivalTime(lGraph, lines, stationName, numData);
        default:
            printf("Please Try Again.\n");
            break;
        }
    }
    freeMemory(lGraph, lines);
    return 0;

}
