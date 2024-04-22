#include <iostream>
#include <fstream>
#include <sstream>
#include "PlayHashTable.h"


void PlayHashTable::readDataAndPushIntoHashMap(const string &filename, unordered_map<string, LinkedList/*, PlayHash*/> &ht) {

    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Could not open file: " << filename << endl;
        return;
    }

    string line;
    getline(file, line);
    int i = 0;

    while (getline(file, line) /*&& i<3*/) {
        stringstream ss(line);
        string token;

        Play* play = new Play();
        try {
            getline(ss, token, ',');  //read gameID
            play->gameID = stoi(token);
            getline(ss, token, ',');  //read gameDate
            play->gameDate = token;
            getline(ss, token, ',');  //read playType
            // if play type is blank, a no play, or a timeout/punt/extra pt/QB kneel
            // move to next line
            /// DONE TO REDUCE # OF HASH TABLE INSERTIONS
            if(token.empty() || token == "NO PLAY" || token == "TIMEOUT" ||
            token == "KICK OFF" || token == "PUNT" || token == "EXTRA POINT" ||
            token == "QB KNEEL"){
                continue;
            }
            play->playType = token;
            getline(ss, token, ',');  //read qtr
            play->quarter = stoi(token);
            getline(ss, token, ',');  //read minute
            play->minutes = stoi(token);
            getline(ss, token, ',');  //read second
            play->seconds = stoi(token);
            play->timeAsInt = Helpers::timeToInt(play->minutes, play->seconds);
            getline(ss, token, ',');  //read offense
            play->offense = token;
            getline(ss, token, ',');  //read defense
            play->defense = token;
            getline(ss, token, ',');  //read down
            play->down = stoi(token);
            getline(ss, token, ',');  //read toGo
            play->toGo = stoi(token);
            getline(ss, token, ',');  //read yardLine
            play->yardLine = stoi(token);
            getline(ss, token, ',');  //read if result is first down
            play->resultIsFirstDown = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read description
            play->description = token;
            getline(ss, token, ',');  //read resultingYards
            play->resultingYards = stoi(token);
            //give weight based on amount of yards lost/gained
            play->yardsWeight = Helpers::calculateWeight(stoi(token));
            getline(ss, token, ',');  //read formation
            play->formation = token;
            getline(ss, token, ',');  //read if isRush
            play->isRush = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isPass
            play->isPass = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isComplete
            play->isIncomplete = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isTouchdown
            play->isTouchdown = Helpers::booleanResult(stoi(token));
            if (play->isTouchdown) {
                play->touchdownWeight = 10.0f;
            }
            getline(ss, token, ',');  //read passType
            play->passType = token;
            getline(ss, token, ',');  //read if isSack
            play->isSack = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isInterception
            play->isInterception = Helpers::booleanResult(stoi(token));
            if (play->isInterception) {
                play->interceptionWeight = -100.0f;
            }
            getline(ss, token, ',');  //read if isFumble
            play->isFumble = Helpers::booleanResult(stoi(token));
            if (play->isFumble) {
                play->fumbleWeight = -1000.0f;
            }
            getline(ss, token, ',');  //read if isTwoPointConversion
            play->isTwoPointConversion = Helpers::booleanResult(stoi(token));
            getline(ss, token, ',');  //read if isTwoPointConversionSuccessful
            play->isTwoPointConversionSuccessful = Helpers::booleanResult(stoi(token));
            if (play->isTwoPointConversionSuccessful) {
                play->twoPointWeight = 5.0f;
            }
            getline(ss, token, ',');  //read rushDirection
            play->rushDirection = token;
            i++;
        }
            //helps for debugging file
        catch (exception& err) {
            cout << "Error: " << err.what() << " at line " << i << endl;
        }

        //put into hash table
        string playCode = Helpers::generatePlayCode(*play);
        ht[playCode].insert(play);
    }
    file.close();

}
