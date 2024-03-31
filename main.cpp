#include <iostream>
#include <vector>
#include <queue>

#include "Play.h"
#include "ComparePlay.h"
#include "PlayMaxHeap.h"
#include "Helpers.h"

using namespace std;

int main() {
    const string filename = "pbp-2023 1.csv";

    //for maxHeap
    priority_queue<Play, vector<Play>, ComparePlay> maxHeap;

    //for hash table


    //welcome screen
    cout << "\n============================================= Welcome to the Gridiron Guru! ============================================\n";
    cout << "                                 Developed by Jett Nguyen, Zach Ostroff, and William Shaoul\n\n";
    string input;
    string dataStructure;

    //to read in all inputs from user
    while (true) {
        cout << "Provide the downs, yards to go, time left, and current position on the field ";
        cout << "and we can tell you the best suggested play\nto convert a 1st down or score a touchdown!\n";
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
        cout << "Building...\n";
        if (dataStructure == "1") {
            //for maxHeap
            PlayMaxHeap::readDataAndPushIntoHeap(filename, maxHeap);
        }
        else {
            /*
            //for hash table

            */
        }

        //prompt current down
        cout << "Input current Down as a number 1-4 below:\n";
        cin >> input;
        if (input == "exit") {
            break;
        }
        //validates input for given prompt
        while (!Helpers::validateInput(input, "int", 0, 4)) {
            cout << "Input current Down as a number 1-4 below:\n";
            cin >> input;
        }
        currentSituation.down = stoi(input);

        //prompt yards to go
        cout << "Input yards to go as a number 1-100 below:\n";
        cin >> input;
        if (input == "exit") {
            break;
        }
        //validates input for given prompt
        while (!Helpers::validateInput(input, "int", 0, 100)) {
            cout << "Input yards to go as a number 1-100 below:\n";
            cin >> input;
        }
        currentSituation.toGo = stoi(input);

        //prompt position
        cout << "Input current field position as a number 1-100 below:\n";
        cin >> input;
        if (input == "exit") {
            break;
        }
        //validates input for given prompt
        while (!Helpers::validateInput(input, "int", 0, 100)) {
            cout << "Input current field position as a number 1-100 below:\n";
            cin >> input;
        }
        currentSituation.yardLine = stoi(input);

        //prompt time
        cout << "Input time left in game in mm:ss below\n";
        cin >> input;
        if (input == "exit") {
            break;
        }
        //validates input for given prompt
        while (!Helpers::validateInput(input, "time", 0, 0)) {
            cout << "Input time left in game in mm:ss below\n";
            cin >> input;
        }
        currentSituation.minutes = stoi(to_string(input[0]-'0') + to_string(input[1]-'0'));
        currentSituation.gameSection = Helpers::calculateGameSection(currentSituation.minutes);
        currentSituation.seconds = stoi(to_string(input[3]-'0') + to_string(input[4]-'0'));

        if (currentSituation.down == 0 && currentSituation.toGo == 0 && (currentSituation.yardLine == 98 || currentSituation.yardLine == 99) && currentSituation.playType != "EXTRA POINT") {
            currentSituation.isTwoPointConversion = true;
        }
        else {
            currentSituation.isTwoPointConversion = false;
        }

        cout << "Analyzing over 250,000 plays...\n";

        if (dataStructure == "1") {
            //for maxHeap structure
            PlayMaxHeap::suggestPlayFromHeap(currentSituation, maxHeap);
        }
        else {
            //for hash table
        }

    }
    cout << "Exiting program.\n";

    return 0;
}