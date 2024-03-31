#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "Helpers.h"

using namespace std;

bool Helpers::validateInput(const string& input, const string& inputType, int lowerBound, int upperBound) {
    if (inputType == "int") {
        try {
            //attempts to convert input into an integer
            stoi(input);
            //checks bounds of input
            if (stoi(input) < lowerBound || stoi(input) > upperBound) {
                cout << "Error: Input is out of bounds!\n";
                return false;
            }
        }
        catch (exception& err) {
            //should catch if input is not an integer
            cout << "Error: err.what()\n";
            return false;
        }
    }

    if (inputType == "time") {
        //checks if input of time is 5 characters long
        if (input.length() != 5) {
            cout << "Error: Time format is invalid! Enter like this: 11:24 or 03:20\n";
            return false;
        }

        //will check if each character is of the right type
        try {
            stoi(to_string(input[0]));
            stoi(to_string(input[1]));
            //checks if minutes are greater than 15 mins
            if (stoi(to_string(input[0]-'0') + to_string(input[1]-'0')) > 15) {
                cout << "Error: Time format is invalid! Minutes exceed 15!\n";
                return false;
            }
            //checks if 3rd char is a :
            if (input[2] != ':') {
                cout << "Error: Time format is invalid! Enter like this: 11:24 or 03:20\n";
                return false;
            }

            stoi(to_string(input[3]));
            stoi(to_string(input[4]));
            //checks if seconds are greater than 59 secs
            if (stoi(to_string(input[3]-'0') + to_string(input[4]-'0')) > 59) {
                cout << "Error: Time format is invalid! Seconds exceed 59!\n";
                return false;
            }
            //checks if total time is greater than 15 mins
            if (stoi(to_string(input[0]-'0') + to_string(input[1]-'0') + to_string(input[3]-'0') + to_string(input[4]-'0')) > 1500) {
                cout << "Error: Time format is invalid! Time Exceeds 15:00!\n";
                return false;
            }
        }
        catch (exception& err) {
            cout << "Error: Time format is invalid! Enter like this: 11:24 or 03:20\n";
            return false;
        }
    }
    return true;
}

bool Helpers::booleanResult(int boolValue) {
    if (boolValue == 1) {
        return true;
    }
    else {
        return false;
    }
}

float Helpers::calculateWeight(int yards) {
    if (yards < 0) {
        return 0.0f - pow(yards, 0.75);
    }
    else {
        return pow(yards, 0.75);
    }
}

int Helpers::calculateGameSection(int minute) {
    if (0 <= minute && minute < 3) {
        return 1;
    }
    else if (3 <= minute && minute < 6) {
        return 2;
    }
    else if (6 <= minute && minute < 9) {
        return 3;
    }
    else if (9 <= minute && minute < 12) {
        return 4;
    }
    else if (12 <= minute && minute <= 15) {
        return 5;
    }
}

vector<int> Helpers::calculateToGoBounds(int toGo) {
    vector<int> toGoBounds = {};
    //can't go below 0
    if (toGo < 1) {
        toGoBounds.push_back(0);
    }
    else {
        toGoBounds.push_back(toGo-1);
    }
    //cant go above 100
    if (toGo > 99) {
        toGoBounds.push_back(100);
    }
    else {
        toGoBounds.push_back(toGo+1);
    }
    return toGoBounds;
}

vector<int> Helpers::calculateYardLineBounds(int yardLine) {
    vector<int> yardLineBounds = {};
    //can't go below 0
    if (yardLine < 5) {
        yardLineBounds.push_back(0);
    }
    else {
        yardLineBounds.push_back(yardLine-5);
    }
    //cant go above 100
    if (yardLine > 95) {
        yardLineBounds.push_back(100);
    }
    else {
        yardLineBounds.push_back(yardLine+5);
    }
    return yardLineBounds;
}

string Helpers::formatPercentages(float percentage) {
    string newPercentage = to_string(percentage);
    if (percentage >= 10) {
        newPercentage.erase(newPercentage.begin() + 5, newPercentage.end());
    }
    else {
        newPercentage.erase(newPercentage.begin() + 4, newPercentage.end());
    }
    return newPercentage;
}

string Helpers::formatTime(int min, int sec) {
    string timeString = "";

    //for minutes
    if (min < 10) {
        timeString += "0" + to_string(min);
    }
    else {
        timeString += to_string(min);
    }

    timeString += ":";

    //for seconds
    if (sec < 10) {
        timeString += "0" + to_string(sec);
    }
    else {
        timeString += to_string(sec);
    }

    return timeString;
}
