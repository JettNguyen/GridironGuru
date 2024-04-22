#include <iostream>
#include <vector>
#include <queue>


#include "Play.h"
#include "ComparePlay.h"
#include "PlayMaxHeap.h"
#include "PlayHashTable.h"
#include "Helpers.h"

using namespace std;


int main() {
    string filename;

    //for maxHeap
    priority_queue<Play, vector<Play>, ComparePlay> maxHeap;

    //hash map
    //keys are play code strings, values are Linked Lists of Plays
    priority_queue<Play, vector<Play>, ComparePlay> hashMaxHeap;
    vector<LinkedList> hashTable(500, LinkedList());

    //welcome screen
    cout << "\n============================================= Welcome to the Gridiron Guru! =============================================\n";
    cout << "                                 Developed by Jett Nguyen, Zach Ostroff, and William Shaoul\n\n";
    string input;
    string dataStructure;
    static bool heapUsed = false;
    static bool hashTableUsed = false;

    //to read in all inputs from user
    while (true) {
        cout << "Provide the quarter, down, yards to go, time left, and current position on the field ";
        cout << "and we can tell you the best\nsuggested play(s) to convert a 1st down, score a touchdown, ";
        cout << "field goal, or two point conversion!\n";
        cout << "For two point conversions, please input the down and yards to go as \"0\".\n";
        cout << "(Input \"exit\" at any time to exit the program)\n\n";

        Play currentSituation;

        //prompt data structure
        cout << "Input \"1\" to use a maxHeap or \"2\" to use a hashTable below:\n";
        cin >> dataStructure;
        if (dataStructure == "exit") {
            break;
        }
        //validates input for given prompt
        while (!Helpers::validateInput(dataStructure, "int", 1, 2)) {
            cout << "Input the number 1 or 2  below:\n";
            cin >> dataStructure;
        }

        if (dataStructure == "1" && !heapUsed) {
            //for maxHeap
            filename = "../files/pbp2013-2023.csv";
            cout << "Building Heap...\n";
            //so that the heap is not built again during the run
            heapUsed = true;
            PlayMaxHeap::readDataAndPushIntoHeap(filename, maxHeap);
        }
        else if (dataStructure == "2" && !hashTableUsed){
            filename = "../files/pbp2013-2023-modified.csv";
            cout << "Building Hash Table...\n";
            //so that the hash table is not built again during the run
            hashTableUsed = true;
            PlayHashTable::readDataAndPushIntoHashMap(filename, hashTable);
        }

        //prompt current qtr
        cout << "Input current QUARTER as a number 1-4 below:\n";
        cin >> input;
        if (input == "exit") {
            break;
        }
        //validates input for given prompt
        while (!Helpers::validateInput(input, "int", 1, 4)) {
            cout << "Input current QUARTER as a number 1-4 below:\n";
            cin >> input;
        }
        currentSituation.quarter = stoi(input);

        //prompt current down
        cout << "Input current DOWN as a number 0-4 below:\n";
        cin >> input;
        if (input == "exit") {
            break;
        }
        //validates input for given prompt
        while (!Helpers::validateInput(input, "int", 0, 4)) {
            cout << "Input current DOWN as a number 0-4 below:\n";
            cin >> input;
        }
        currentSituation.down = stoi(input);

        //prompt yards to go
        cout << "Input YARDS TO GO as a number 0-99 below:\n";
        cin >> input;
        if (input == "exit") {
            break;
        }
        //validates input for given prompt
        while (!Helpers::validateInput(input, "int", 0, 99)) {
            cout << "Input YARDS TO GO as a number 0-99 below:\n";
            cin >> input;
        }
        currentSituation.toGo = stoi(input);

        //prompt position
        cout << "Input current FIELD POSITION as a number 1-99 below (0 is your goal line, 100 is opponent's goal line):\n";
        cin >> input;
        if (input == "exit") {
            break;
        }
        //validates input for given prompt
        while (!Helpers::validateInput(input, "int", 1, 99)) {
            cout << "Input current FIELD POSITION as a number 1-99 below (0 is your goal line, 100 is opponent's goal line):\n";
            cin >> input;
        }
        currentSituation.yardLine = stoi(input);

        //prompt time
        cout << "Input TIME LEFT in quarter in mm:ss below\n";
        cin >> input;
        if (input == "exit") {
            break;
        }
        //validates input for given prompt
        while (!Helpers::validateInput(input, "time", 0, 0)) {
            cout << "Input TIME LEFT in quarter in mm:ss below\n";
            cin >> input;
        }
        currentSituation.minutes = stoi(to_string(input[0]-'0') + to_string(input[1]-'0'));
        currentSituation.seconds = stoi(to_string(input[3]-'0') + to_string(input[4]-'0'));
        currentSituation.timeAsInt = Helpers::timeToInt(currentSituation.minutes, currentSituation.seconds);

        //checks if inputs from user qualifies for two point conversion
        if (currentSituation.down == 0 && currentSituation.toGo == 0 && (currentSituation.yardLine == 98 || currentSituation.yardLine == 99)) {
            currentSituation.isTwoPointConversion = true;
        }
        else {
            currentSituation.isTwoPointConversion = false;
        }

        cout << "Analyzing over 485,000 plays from 2013-2023...\n";

        //depending on chosen data structure, will suggest plays differently
        if (dataStructure == "1") {
            //for maxHeap structure
            PlayMaxHeap::suggestPlayFromHeap(currentSituation, maxHeap);
        }
        else {
            string currentHashCode = Helpers::generatePlayCode(currentSituation);

            Helpers temp;
            int index = temp.hash_func(currentHashCode, hashTable);
            LinkedList plays = hashTable[index];
            Play* currentPlay = plays.head;

            while (currentPlay != nullptr) {
                hashMaxHeap.push(*currentPlay);
                currentPlay = currentPlay->next;
            }

            PlayMaxHeap::suggestPlayFromHeap(currentSituation, hashMaxHeap);


        }
    }
    cout << "Exiting program.\n";

    return 0;
}