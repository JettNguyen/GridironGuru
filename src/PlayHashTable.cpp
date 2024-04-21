#include <iostream>
#include <fstream>
#include <sstream>

#include "Helpers.h"
#include "PlayHashTable.h"

void PlayHashTable::readDataAndPushIntoHashMap(const string &filename, pair<NodePlay, NodePlay> *hashArray) {
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
//            if (play.resultIsFirstDown) {
//                play.firstDownWeight = 10.0f;
//            }
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
//            if (play.isTouchdown) {
//                play.touchdownWeight = 10.0f;
//            }
            getline(ss, token, ',');  //read passType
            play.passType = token;
            getline(ss, token, ',');  //read if isSack
            play.isSack = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isInterception
            play.isInterception = Helpers::booleanResult(stoi(token));
//            if (play.isInterception) {
//                play.interceptionWeight = -100.0f;
//            }
            getline(ss, token, ',');  //read if isFumble
            play.isFumble = Helpers::booleanResult(stoi(token));
//            if (play.isFumble) {
//                play.fumbleWeight = -1000.0f;
//            }
            getline(ss, token, ',');  //read if isTwoPointConversion
            play.isTwoPointConversion = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isTwoPointConversionSuccessful
            play.isTwoPointConversionSuccessful = Helpers::booleanResult(stoi(token));
//            if (play.isTwoPointConversionSuccessful) {
//                play.twoPointWeight = 5.0f;
//            }
            getline(ss, token, ',');  //read rushDirection
            play.rushDirection = token;
            i++;
        }
            //helps for debugging file
        catch (exception& err) {
            cout << "Error: " << err.what() << " at line " << i << endl;
        }

        NodePlay nodePlay;

        // create NodePlay object only if it is a play type we are considering
        if(play.down >= 1 && (play.playType == "SCRAMBLE" || play.playType == "PASS" || play.playType == "RUSH" || play.playType == "SACK")){
            nodePlay.setPlay(&play);
        }
        else if (play.down == 0 && play.isTwoPointConversion){
            nodePlay.setPlay(&play);
        }
        else{
            continue;
        }

        //get variables for hashing

        //same code used to create a hash value for each play object
        //is to be used to get a hash value for the data input by the user

        int downRep; //represents the down of the play
        int toGoRep; //represents the yards to go of the play
        int posRep; //represents the position of the play
        int timeRep; //represents the time in the game of the play


        //calculating rep values for non-two point conversion plays
        if(play.down >= 1) {

            //determining the downRep
            downRep = play.down - 1;
            // start at index 0 for first down, 1 for second down and so on

            //determining the toGoRep
            if (play.toGo == 1) {
                toGoRep = 0;
            } else if (play.toGo == 2 || play.toGo == 3) {
                toGoRep = 1;
            } else if (play.toGo >= 4 && play.toGo <= 6) {
                toGoRep = 2;
            } else if (play.toGo >= 7 && play.toGo <= 10) {
                toGoRep = 3;
            } else if (play.toGo >= 11 && play.toGo <= 16) {
                toGoRep = 4;
            } else if (play.toGo >= 17) {
                toGoRep = 5;
            }

            //determining the posRep
            posRep = play.yardLine / 10;

            //determining the timeRep
            int secondsLeft = play.minutes * 60 + play.seconds;
            if (play.quarter == 1 || play.quarter == 3 || (play.quarter == 2 && secondsLeft > 300) ||
                (play.quarter == 4 && secondsLeft > 600)) {
                timeRep = 0;
                //we decided that time is not a relevant factor in determining the situation besides when time is under five minutes in
                //the second quarter, and under 10 minutes in the
            }
            if (play.quarter == 2 && secondsLeft <= 300) {
                timeRep = 1 + (secondsLeft - 1) / 30;
            }
            if (play.quarter == 4 && secondsLeft <= 600) {
                timeRep = 11 + (secondsLeft - 1) / 30;
            }
        }
        int hashVal;

        if(play.down != 0){
            hashVal = downRep * 1860 + toGoRep * 310 + posRep * 31 + timeRep * 1;
//            if (hashVal == 3348){
//                cout << "yipee" << endl;
//            }
        }
        else{
            hashVal = 7440;
            //if yardLine == 99, hashVal doesn't change
            if(play.yardLine == 98){
                hashVal += 1;
            }
            else if(play.yardLine <= 97 && play.yardLine >= 90){
                hashVal +=2;
            }
            else if(play.yardLine <= 89){
                hashVal += 3;
            }
        }
        hashVal %= 24;
        //take this out after

        //hashVal now calculated

        //put nodes into the HashMap
        if(hashArray[hashVal].first.isNullPlayPtr()){
            hashArray[hashVal].first = nodePlay;
            hashArray[hashVal].second = nodePlay;
        }
        else{
            hashArray[hashVal].second.setNext(&nodePlay);
            hashArray[hashVal].second = nodePlay;
        }
    }
    file.close();
}
