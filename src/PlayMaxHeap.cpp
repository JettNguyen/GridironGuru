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

//read data from file and put into maxHeap
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
            getline(ss, token, ',');  //read second
            play.seconds = stoi(token);
            play.timeAsInt = Helpers::timeToInt(play.minutes, play.seconds);
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
            if (play.resultIsFirstDown) {
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
            if (play.isTouchdown) {
                play.touchdownWeight = 10.0f;
            }
            getline(ss, token, ',');  //read passType
            play.passType = token;
            getline(ss, token, ',');  //read if isSack
            play.isSack = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isInterception
            play.isInterception = Helpers::booleanResult(stoi(token));
            if (play.isInterception) {
                play.interceptionWeight = -100.0f;
            }
            getline(ss, token, ',');  //read if isFumble
            play.isFumble = Helpers::booleanResult(stoi(token));
            if (play.isFumble) {
                play.fumbleWeight = -1000.0f;
            }
            getline(ss, token, ',');  //read if isTwoPointConversion
            play.isTwoPointConversion = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isTwoPointConversionSuccessful
            play.isTwoPointConversionSuccessful = Helpers::booleanResult(stoi(token));
            if (play.isTwoPointConversionSuccessful) {
                play.twoPointWeight = 5.0f;
            }
            getline(ss, token, ',');  //read rushDirection
            play.rushDirection = token;
            i++;
        }
        //helps for debugging file
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

    //handles if it is not determining a 2 point conversion
    if (currentSituation.down != 0) {
        //bounds for toGo have 1 yard leeway
        toGoLowerBound = Helpers::calculateToGoBounds(currentSituation.toGo)[0];
        toGoUpperBound = Helpers::calculateToGoBounds(currentSituation.toGo)[1];

        //bounds for yardLine have 5 yard leeway
        yardLineLowerBound = Helpers::calculateYardLineBounds(currentSituation.yardLine)[0];
        yardLineUpperBound = Helpers::calculateYardLineBounds(currentSituation.yardLine)[1];
    }
    //if it is determining a 2 point conversion
    else {
        toGoLowerBound = 0;
        toGoUpperBound = 0;
        yardLineLowerBound = 98;
        yardLineUpperBound = 99;
    }

    //bounds for time always have 1:30 leeway
    int timeLowerBound = Helpers::calculateTimeBounds(currentSituation.minutes, currentSituation.seconds)[0];
    int timeUpperBound = Helpers::calculateTimeBounds(currentSituation.minutes, currentSituation.seconds)[1];

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

        modifiableHeap.pop();

        //checks if quarter and down are same, toGo is within 1 yard inclusive
        //yardLine is within 5 yards inclusive, and time is within 1:30 inclusive
        //then adds current iteration of play into tempHeap if all are true
        if (currentPlay.quarter == currentSituation.quarter && currentPlay.down == currentSituation.down
            && currentPlay.toGo >= toGoLowerBound && currentPlay.toGo <= toGoUpperBound
            && currentPlay.yardLine >= yardLineLowerBound && currentPlay.yardLine <= yardLineUpperBound
            && currentPlay.timeAsInt >= timeLowerBound && currentPlay.timeAsInt <= timeUpperBound) {

            tempHeap.push(currentPlay);

            //if it's determining a two point conversion
            if (currentSituation.isTwoPointConversion && currentPlay.isTwoPointConversion && currentPlay.playType != "EXTRA POINT") {

                //calculating likelihood of successful conversion in situation
                if (currentPlay.isTwoPointConversionSuccessful) {
                    conversions++;
                    //from https://stackoverflow.com/questions/2340281/check-if-a-string-contains-a-string-in-c
                    //uses this because .csv doesn't specify if pass or rush directly if it's a conversion
                    if (currentPlay.description.find("PASS") != string::npos) {
                        twoPointPasses++;
                        playTypeSuccessMap[currentPlay.playType]["PASS"]++;  //for specific formation
                    }
                    else if (currentPlay.description.find("RUSH") != string::npos) {
                        twoPointRushes++;
                        playTypeSuccessMap[currentPlay.playType]["RUSH"]++;  //for specific formation
                    }
                }
            }
            //if it's not determining a two point conversion
            else {
                if (currentSituation.isTwoPointConversion && currentPlay.playType == "PASS") {
                    cout << currentPlay.playType << ": " << currentPlay.description << endl;
                }

                //calculating likelihood of first down in situation
                if (currentPlay.resultIsFirstDown) {
                    firstDowns++;
                    if (currentPlay.isPass) {
                        firstDownPasses++;
                        playTypeSuccessMap[currentPlay.playType][currentPlay.passType]++;  //for specific pass type
                    }
                    else if (currentPlay.isRush) {
                        firstDownRushes++;
                        playTypeSuccessMap[currentPlay.playType][currentPlay.rushDirection]++;  //for specific rush dir
                    }
                }

                //calculating likelihood of TD in situation
                if (currentPlay.isTouchdown) {
                    touchdowns++;
                    if (currentPlay.isPass) {
                        touchdownPasses++;
                        playTypeSuccessMap[currentPlay.playType][currentPlay.passType]++;  //for specific pass type
                    } else if (currentPlay.isRush) {
                        touchdownRushes++;
                        playTypeSuccessMap[currentPlay.playType][currentPlay.rushDirection]++;  //for specific rush dir
                    }
                }

                //calculating likelihood of successful field goal in situation
                if (currentPlay.playType == "FIELD GOAL") {
                    //from https://stackoverflow.com/questions/2340281/check-if-a-string-contains-a-string-in-c
                    //uses this because .csv file doesn't directly specify if field goal is good or not
                    if (currentPlay.description.find("IS GOOD") != string::npos) {
                        fieldGoals++;
                        playTypeSuccessMap[currentPlay.playType][currentPlay.formation]++; //for specific formation
                    }
                }
            }
        }
    }

    //if there are no similar situations within bounds given

    if (tempHeap.empty()) {
        cout << "No Match Found! Good Luck!\n\n";
        return;
    }
    //if there is a similar situation but the attempt was unsuccessful
    if (tempHeap.top().isIncomplete || tempHeap.top().isInterception || tempHeap.top().resultingYards < 0) {
        if (tempHeap.size() > 1) {
            cout << "Matches found, but with no gain. Here are their game IDs:\n";
            for (int i = 0; i < tempHeap.size(); i++) {
                cout << tempHeap.top().gameID << endl;
                tempHeap.pop();
            }
        }
        else {
            cout << "Match found, but with no gain. Here is its game ID:\n";
            cout << tempHeap.top().gameID << endl;
        }
        cout << endl;
        return;
    }

    //the best play is at the top of the max Heap depending on rating given by ComparePlay
    bestPlay = tempHeap.top();

    map<string, float> likelihoods;

    //likelihoods for first downs
    float likelihoodFirstDown = (static_cast<float>(firstDowns)/static_cast<float>(tempHeap.size()))*100;
    //puts likelihood of first down into map that's unsorted by likelihood
    likelihoods["First Down: "] = likelihoodFirstDown;
    float likelihoodFirstDownPass = 0;
    float likelihoodFirstDownRush = 0;
    //so that it doesn't divide by 0
    if (firstDowns != 0) {
        likelihoodFirstDownPass = (static_cast<float>(firstDownPasses)/static_cast<float>(firstDowns))*100;
        likelihoodFirstDownRush = (static_cast<float>(firstDownRushes)/static_cast<float>(firstDowns))*100;
    }

    //likelihoods for touchdowns
    float likelihoodTouchdown = (static_cast<float>(touchdowns)/static_cast<float>(tempHeap.size()))*100;
    //puts likelihood of touchdown into map that's unsorted by likelihood
    likelihoods["Touchdown: "] = likelihoodTouchdown;
    float likelihoodTouchdownPass = 0;
    float likelihoodTouchdownRush = 0;
    //so that it doesn't divide by 0
    if (touchdowns != 0) {
        likelihoodTouchdownPass = (static_cast<float>(touchdownPasses)/static_cast<float>(touchdowns))*100;
        likelihoodTouchdownRush = (static_cast<float>(touchdownRushes)/static_cast<float>(touchdowns))*100;
    }

    //likelihoods for conversions
    float likelihoodTwoPoint = (static_cast<float>(conversions)/static_cast<float>(tempHeap.size()))*100;
    //likelihood of conversion not into map because if calculating for conversion, will be the only one printed
    //...so it will not need to be sorted
    float likelihoodTwoPointPass = 0;
    float likelihoodTwoPointRush = 0;
    //so that it doesn't divide by 0
    if (conversions != 0) {
        likelihoodTwoPointPass = (static_cast<float>(twoPointPasses)/static_cast<float>(conversions))*100;
        likelihoodTwoPointRush = (static_cast<float>(twoPointRushes)/static_cast<float>(conversions))*100;
    }

    //likelihood for field goals
    float likelihoodFieldGoal = 0.0f;
    //so that it doesn't divide by 0
    if (fieldGoals != 0) {
        likelihoodFieldGoal = (static_cast<float>(fieldGoals)/static_cast<float>(tempHeap.size()))*100;
    }
    //puts likelihood of field goal into map that's unsorted by likelihood
    likelihoods["Field Goal: "] = likelihoodFieldGoal;


    //logic for multimap inspired by https://www.educative.io/answers/how-to-sort-a-map-by-value-in-cpp
    //is so that the likelihoods map can be sorted by value
    //logic of descending sorted map from https://www.geeksforgeeks.org/descending-order-map-multimap-c-stl/
    multimap<float, string, greater<float>> sortedLikelihoods;
    //will sort the likelihoods map by the key's value
    for (auto iter = likelihoods.begin(); iter != likelihoods.end(); iter++) {
        sortedLikelihoods.insert({iter->second, iter->first});
    }

    int mostSuccesses = -1;
    //map<amtOfSuccess, map<playType, subPlayType>>
    //logic of descending sorted map from https://www.geeksforgeeks.org/descending-order-map-multimap-c-stl/
    map<int, pair<string, string>, greater<int>> successfulPlayMap = {};

    //puts successful play(s) into separate map
    //keeps 1 if it has the most successes and keeps multiple if successes are the same for 2 different plays
    for (auto iter = playTypeSuccessMap.begin(); iter != playTypeSuccessMap.end(); iter++) {
        for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++) {
            if (iter2->second > mostSuccesses) {
                successfulPlayMap[iter2->second] = {iter->first, iter2->first};
                mostSuccesses = iter2->second;
            }
        }
    }

    cout << "\nOUT OF " << tempHeap.size() << " SIMILAR SITUATIONS, ";
    //if plural amount of successful plays
    if (successfulPlayMap.size() > 1) {
        cout << "THE IDEAL PLAYS' LIKELIHOODS ARE:\n";
    }
    else {
        cout << "THE IDEAL PLAY'S LIKELIHOOD IS:\n";
    }


    //prints ideal plays in descending order by iterating through map of occurrences with their respective plays
    for (auto iter = successfulPlayMap.begin(); iter != successfulPlayMap.end(); iter++) {
        float subPlayLikelihood = (static_cast<float>(iter->first)/static_cast<float>(tempHeap.size()))*100;
        if (iter->second.first == "FIELD GOAL") {
            cout << "    " << iter->second.first << " IN " << iter->second.second << " FORMATION: ";
        }
        else {
            cout << "    " << iter->second.first << " " << iter->second.second << ": ";
        }
        cout << Helpers::formatPercentages(subPlayLikelihood) << "%\n";
    }

    cout << "\nLIKELIHOODS:\n";

    //only prints 2 pt conversion if the inputted situation prompted for 2 pt conversion likelihood
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

    //if best play starts with a vowel
    if (bestPlay.formation[0] == 'A' || bestPlay.formation[0] == 'E' || bestPlay.formation[0] == 'I'
        || bestPlay.formation[0] == 'O' || bestPlay.formation[0] == 'U') {
        cout << " with an " << bestPlay.formation << " formation\n";
    }
    else {
        cout << " with a " << bestPlay.formation << " formation\n";
    }

    //formatted output in paragraph form prints
    cout << "    On " << bestPlay.gameDate << ", " << bestPlay.offense;
    if (bestPlay.isTwoPointConversion && bestPlay.isTwoPointConversionSuccessful) {
        cout << " scored 2 points ";
        cout << "against " << bestPlay.defense << " during quarter " << bestPlay.quarter;
        cout << " at time " << Helpers::formatTime(bestPlay.minutes, bestPlay.seconds) << ".\n";
    }
    else {
        cout << " gained " << bestPlay.resultingYards << " yards ";
        cout << "against " << bestPlay.defense << " during quarter " << bestPlay.quarter << " on down " << bestPlay.down;
        cout << " on the " << bestPlay.yardLine << " yard line with " << bestPlay.toGo << " yards to go at time ";
        cout << Helpers::formatTime(bestPlay.minutes, bestPlay.seconds) << ".\n";
    }
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
                cout << ", but it did result in a first down.";
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
