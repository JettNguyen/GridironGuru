#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <map>

#include "Play.h"
#include "ComparePlay.h"
#include "Helpers.h"
#include "PlayMaxHeap.h"

using namespace std;

//read data from file and put into heap
void PlayMaxHeap::readDataAndPushIntoHeap(const string& filename, priority_queue<Play, vector<Play>, ComparePlay>& maxHeap) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open file: " << filename << endl;
        return;
    }

    string line;
    getline(file, line);
    int i = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;

        Play play;
        try {
            getline(ss, token, ',');  //read gameID
            play.gameID = stoi(token);
            getline(ss, token, ',');  //read gameDate
            play.gameDate = token;
            getline(ss, token, ',');  //read qtr
            play.quarter = stoi(token);
            getline(ss, token, ',');  //read minute
            play.minutes = stoi(token);
            play.gameSection = Helpers::calculateGameSection(play.minutes);
            getline(ss, token, ',');  //read second
            play.seconds = stoi(token);
            getline(ss, token, ',');  //read offense
            play.offense = token;
            getline(ss, token, ',');  //read defense
            play.defense = token;
            getline(ss, token, ',');  //read down
            play.down = stoi(token);
            getline(ss, token, ',');  //read toGo
            play.toGo = stoi(token);
            getline(ss, token, ',');  //read yardLine
            play.yardLine = stoi(token);
            getline(ss, token, ',');  //read if result is first down
            play.resultIsFirstDown = Helpers::booleanResult(stoi(token));
            if (Helpers::booleanResult(stoi(token))) {
                play.firstDownWeight = 10.0f;
            }
            getline(ss, token, ',');  //read description
            play.description = token;
            getline(ss, token, ',');  //read resultingYards
            play.resultingYards = stoi(token);
            //give weight based on amount of yards lost/gained
            play.yardsWeight = Helpers::calculateWeight(stoi(token));
            getline(ss, token, ',');  //read formation
            play.formation = token;
            getline(ss, token, ',');  //read playType
            play.playType = token;
            getline(ss, token, ',');  //read if isRush
            play.isRush = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isPass
            play.isPass = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isComplete
            play.isIncomplete = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isTouchdown
            play.isTouchdown = Helpers::booleanResult(stoi(token));
            if (Helpers::booleanResult(stoi(token))) {
                play.touchdownWeight = 10.0f;
            }
            getline(ss, token, ',');  //read passType
            play.passType = token;
            getline(ss, token, ',');  //read if isSack
            play.isSack = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isInterception
            play.isInterception = Helpers::booleanResult(stoi(token));
            if (Helpers::booleanResult(stoi(token))) {
                play.interceptionWeight = -100.0f;
            }
            getline(ss, token, ',');  //read if isFumble
            play.isFumble = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isTwoPointConversion
            play.isTwoPointConversion = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isTwoPointConversionSuccessful
            play.isTwoPointConversionSuccessful = Helpers::booleanResult(stoi(token));
            if (Helpers::booleanResult(stoi(token))) {
                play.twoPointWeight = 5.0f;
            }
            getline(ss, token, ',');  //read rushDirection
            play.rushDirection = token;
            i++;
        }
        catch (exception& err) {
            cout << "Error: " << err.what() << " at line " << i << endl;

        }

        //put into maxHeap
        maxHeap.push(play);
    }
    file.close();
}

//gives result based on given current situation and all given situations for maxHeap
void PlayMaxHeap::suggestPlayFromHeap(const Play& currentSituation, priority_queue<Play, vector<Play>, ComparePlay>& maxHeap) {
    //copies it so that the heap can be reused multiple times each run
    priority_queue<Play, vector<Play>, ComparePlay> modifiableHeap = maxHeap;
    Play bestPlay = modifiableHeap.top();

    //stores similar situations
    priority_queue<Play, vector<Play>, ComparePlay> tempHeap;

    //initialize bounds
    int toGoLowerBound;
    int toGoUpperBound;
    int yardLineLowerBound;
    int yardLineUpperBound;

    //initialize counters
    int firstDowns = 0;
    int touchdowns = 0;
    int conversions = 0;
    int fieldGoals = 0;
    int firstDownPasses = 0;
    int firstDownRushes = 0;
    int touchdownPasses = 0;
    int touchdownRushes = 0;
    int twoPointPasses = 0;
    int twoPointRushes = 0;
    //map<playType, map<subPlayType, numOfSuccesses>>
    map<string, map<string,int>> playTypeSuccessMap = {};

    while (!modifiableHeap.empty()) {
        Play currentPlay = modifiableHeap.top();
        //bounds for toGo
        toGoLowerBound = Helpers::calculateToGoBounds(currentPlay.toGo)[0];
        toGoUpperBound = Helpers::calculateToGoBounds(currentPlay.toGo)[1];
        //bounds for yardLine
        yardLineLowerBound = Helpers::calculateYardLineBounds(currentPlay.yardLine)[0];
        yardLineUpperBound = Helpers::calculateYardLineBounds(currentPlay.yardLine)[1];

        modifiableHeap.pop();

        if (currentPlay.down == currentSituation.down && currentSituation.toGo > toGoLowerBound
            && currentSituation.toGo < toGoUpperBound && currentSituation.yardLine > yardLineLowerBound
            && currentSituation.yardLine < yardLineUpperBound
            && currentSituation.gameSection == currentPlay.gameSection) {

            tempHeap.push(currentPlay);
            if (currentSituation.isTwoPointConversion && currentPlay.isTwoPointConversion) {
                //calculating likelihood of successful conversion in situation
                if (currentPlay.isTwoPointConversionSuccessful) {
                    conversions++;
                    //from https://stackoverflow.com/questions/2340281/check-if-a-string-contains-a-string-in-c
                    if (currentPlay.description.find("PASS") != string::npos) {
                        twoPointPasses++;
                        playTypeSuccessMap[currentPlay.playType]["PASS"]++; //for specific formation
                    } else if (currentPlay.description.find("RUSH") != string::npos) {
                        twoPointRushes++;
                        playTypeSuccessMap[currentPlay.playType]["RUSH"]++; //for specific formation
                    }
                }
            }
            else {
                if (currentSituation.isTwoPointConversion && currentPlay.playType == "PASS") {
                    cout << currentPlay.playType << ": " << currentPlay.description << endl;
                }

                //calculating likelihood of first down in situation
                if (currentPlay.resultIsFirstDown) {
                    firstDowns++;
                    if (currentPlay.isPass) {
                        firstDownPasses++;
                        playTypeSuccessMap[currentPlay.playType][currentPlay.passType]++; //for specific pass type
                    }
                    else if (currentPlay.isRush) {
                        firstDownRushes++;
                        playTypeSuccessMap[currentPlay.playType][currentPlay.rushDirection]++; //for specific rush dir
                    }
                }

                //calculating likelihood of TD in situation
                if (currentPlay.isTouchdown) {
                    touchdowns++;
                    if (currentPlay.isPass) {
                        touchdownPasses++;
                        playTypeSuccessMap[currentPlay.playType][currentPlay.passType]++; //for specific pass type
                    } else if (currentPlay.isRush) {
                        touchdownRushes++;
                        playTypeSuccessMap[currentPlay.playType][currentPlay.rushDirection]++; //for specific rush dir
                    }
                }

                //calculating likelihood of successful field goal in situation
                if (currentPlay.playType == "FIELD GOAL") {
                    //from https://stackoverflow.com/questions/2340281/check-if-a-string-contains-a-string-in-c
                    if (currentPlay.description.find("IS GOOD") != string::npos) {
                        fieldGoals++;
                        playTypeSuccessMap[currentPlay.playType][currentPlay.formation]++; //for specific formation
                    }
                }
            }
        }
    }

    if (tempHeap.empty()) {
        cout << "No Match Found! Good Luck!\n\n";
        return;
    }

    bestPlay = tempHeap.top();

    map<string, float> likelihoods;
    multimap<float, string, greater<float>> sortedLikelihoods;

    float likelihoodFirstDown = (static_cast<float>(firstDowns)/tempHeap.size())*100;
    likelihoods["First Down: "] = likelihoodFirstDown;
    float likelihoodFirstDownPass = 0;
    float likelihoodFirstDownRush = 0;
    if (firstDowns != 0) {
        likelihoodFirstDownPass = (static_cast<float>(firstDownPasses)/firstDowns)*100;
        likelihoodFirstDownRush = (static_cast<float>(firstDownRushes)/firstDowns)*100;
    }

    float likelihoodTouchdown = (static_cast<float>(touchdowns)/tempHeap.size())*100;
    likelihoods["Touchdown: "] = likelihoodTouchdown;
    float likelihoodTouchdownPass = 0;
    float likelihoodTouchdownRush = 0;
    if (touchdowns != 0) {
        likelihoodTouchdownPass = (static_cast<float>(touchdownPasses)/touchdowns)*100;
        likelihoodTouchdownRush = (static_cast<float>(touchdownRushes)/touchdowns)*100;
    }

    float likelihoodTwoPoint = (static_cast<float>(conversions)/tempHeap.size())*100;
    float likelihoodTwoPointPass = 0;
    float likelihoodTwoPointRush = 0;
    if (conversions != 0) {
        likelihoodTwoPointPass = (static_cast<float>(twoPointPasses)/conversions)*100;
        likelihoodTwoPointRush = (static_cast<float>(twoPointRushes)/conversions)*100;
    }

    float likelihoodFieldGoal = 0.0f;
    if (fieldGoals != 0) {
        likelihoodFieldGoal = (static_cast<float>(fieldGoals)/tempHeap.size())*100;
    }
    likelihoods["Field Goal: "] = likelihoodFieldGoal;

    for (auto iter = likelihoods.begin(); iter != likelihoods.end(); iter++) {
        sortedLikelihoods.insert({iter->second, iter->first});
    }

    int mostSuccesses = -1;
    //map<amtOfSuccess, map<playType, subPlayType>>
    //logic from https://www.geeksforgeeks.org/descending-order-map-multimap-c-stl/
    map<int, pair<string, string>, greater<int>> successfulPlayMap = {};

    for (auto iter = playTypeSuccessMap.begin(); iter != playTypeSuccessMap.end(); iter++) {
        for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++) {
            if (iter2->second > mostSuccesses) {
                successfulPlayMap[iter2->second] = {iter->first, iter2->first};
                mostSuccesses = iter2->second;
            }
        }
    }
    cout << "\nOUT OF " << tempHeap.size() << " SIMILAR SITUATIONS, ";
    cout << "THE IDEAL PLAY(S)' LIKELIHOODS ARE:\n";

    //prints ideal plays in descending order by iterating through map of occurrences with their respective plays
    for (auto iter = successfulPlayMap.begin(); iter != successfulPlayMap.end(); iter++) {
        float subPlayLikelihood = (static_cast<float>(iter->first)/tempHeap.size())*100;
        if (iter->second.first == "FIELD GOAL") {
            cout << "    " << iter->second.first << " IN " << iter->second.second << " FORMATION: ";
        }
        else {
            cout << "    " << iter->second.first << " " << iter->second.second << ": ";
        }
        cout << Helpers::formatPercentages(subPlayLikelihood) << "%\n";
    }

    cout << "\nLIKELIHOODS:\n";
    if (currentSituation.isTwoPointConversion) {
        cout << "    Two Point Conversion: " << Helpers::formatPercentages(likelihoodTwoPoint) << "%\n";
        cout << "        Two Point Pass: " << Helpers::formatPercentages(likelihoodTwoPointPass) << "%\n";
        cout << "        Two Point Rush: " << Helpers::formatPercentages(likelihoodTwoPointRush) << "%\n";
    }
    else {
        //orders from greatest to least likelihoods in printing
        for (auto iter = sortedLikelihoods.begin(); iter != sortedLikelihoods.end(); iter++) {
            //ignores likelihoods of 0%
            if (iter->first != 0) {
                cout << "    " << iter->second << Helpers::formatPercentages(iter->first) << "%\n";
                //if it's a first down
                if (iter->second.find("Fir") != string::npos) {
                    cout << "        Passing: " << Helpers::formatPercentages(likelihoodFirstDownPass) << "%\n";
                    cout << "        Rushing: " << Helpers::formatPercentages(likelihoodFirstDownRush) << "%\n";
                }
                //if it's a touchdown
                else if (iter->second.find("To") != string::npos){
                    cout << "        Passing: " << Helpers::formatPercentages(likelihoodTouchdownPass) << "%\n";
                    cout << "        Rushing: " << Helpers::formatPercentages(likelihoodTouchdownRush) << "%\n";
                }
            }
        }
    }

    cout << "\nBEST HISTORICAL PLAY: " << bestPlay.playType;
    if (bestPlay.isPass) {
        cout << " " << bestPlay.passType;
    }
    else if (bestPlay.isRush) {
        cout << " " << bestPlay.rushDirection;
    }
    cout << " with a(n) " << bestPlay.formation << " formation\n";
    cout << "    On " << bestPlay.gameDate << " " << bestPlay.offense << " gained " << bestPlay.resultingYards;
    cout << " yards against " << bestPlay.defense << " during quarter " << bestPlay.quarter << " on down " << bestPlay.down;
    cout << " on the " << bestPlay.yardLine << " yard line with " << bestPlay.toGo << " yards to go at time ";
    cout << Helpers::formatTime(bestPlay.minutes, bestPlay.seconds) << ".\n";
    cout << "The play ";
    if (bestPlay.isTwoPointConversion) {
        cout << "was a Two Point Conversion and was";
        if (bestPlay.isTwoPointConversionSuccessful) {
            cout << " ";
        }
        else {
            cout << "not ";
        }
        cout << "successful.";
    }
    else {
        if (bestPlay.isTouchdown) {
            cout << "resulted in a touchdown.";
        }
        else {
            cout << "did not result in a touchdown";
            if (bestPlay.resultIsFirstDown) {
                cout << " but it did result in a first down.";
            }
            else {
                cout << " or a first down.";
            }
        }
    }
    cout << "\nDescription: " << bestPlay.description << endl;
    cout << "Game ID: " << bestPlay.gameID << endl << endl;
    cout << "\n============================================= Welcome back to the Gridiron Guru! ============================================\n";

}
