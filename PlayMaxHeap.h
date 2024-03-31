#include <iostream>

using namespace std;

class PlayMaxHeap {
public:
    //read data from file and put into heap
    static void readDataAndPushIntoHeap(const string& filename, priority_queue<Play, vector<Play>, ComparePlay>& maxHeap);

    //gives result based on given current situation and all given situations for maxHeap
    static void suggestPlayFromHeap(const Play& currentSituation, priority_queue<Play, vector<Play>, ComparePlay>& maxHeap);
};