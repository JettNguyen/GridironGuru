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
        return 0.0f - static_cast<float>(pow(yards, 0.75));
    }
    else {
        return static_cast<float>(pow(yards, 0.75));
    }
}

vector<int> Helpers::calculateTimeBounds(int minute ,int second) {
    //returns vector of time bounds that are +- 1:30 from given time
    //this range is tight enough to give a very similar situation but loose enough to find a good amount
    //[lowerTimeAsInt, upperTimeAsInt]
    vector<int> timeBounds;
    int lowerMinute = 0;
    int upperMinute;
    int lowerSecond;
    int upperSecond;


    //push back lower bound
    //will have to subtract 2 minutes if condition is met
    if (second < 30) {
        if (minute < 2) {
            lowerMinute = 0;
        }
        else {
            lowerMinute += minute-2;
        }
        lowerSecond = 60+(second-30);
    }
    else {
        lowerMinute = minute-1;
        lowerSecond = second-30;
    }
    timeBounds.push_back(timeToInt(lowerMinute, lowerSecond));

    //push back upper bound
    if (second >= 30) {
        if (minute > 13) {
            upperMinute = 15;
        }
        else {
            upperMinute = minute+2;
        }
        upperSecond = (second+30)-60;
    }
    else {
        upperMinute = minute+1;
        upperSecond = second+30;
    }
    timeBounds.push_back(timeToInt(upperMinute, upperSecond));

    return timeBounds;
}

int Helpers::timeToInt(int minute, int second) {
    string returnString;
    int time;

    returnString += to_string(minute);
    if (second < 10) {
        returnString += "0" + to_string(second);
    }
    else {
        returnString += to_string(second);
    }

    time = stoi(returnString);
    return time;
}

vector<int> Helpers::calculateToGoBounds(int toGo) {
    vector<int> toGoBounds = {};

    //for two point conversion cases, it will always be 0
    if (toGo == 0) {
        return {0, 0};
    }

    //can't go below 1, 0 toGo in .csv file is for placeholders
    if (toGo < 2) {
        toGoBounds.push_back(1);
    }
    else {
        toGoBounds.push_back(toGo-1);
    }
    //can't go above 99
    if (toGo > 98) {
        toGoBounds.push_back(99);
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
    //can't go above 100
    //but 100 is for placeholders in the .csv file, so limit to 99
    if (yardLine > 94) {
        yardLineBounds.push_back(99);
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

string Helpers::formatTime(int minute, int second) {
    string timeString;

    //for minutes
    if (minute < 10) {
        timeString += "0" + to_string(minute);
    }
    else {
        timeString += to_string(minute);
    }

    timeString += ":";

    //for seconds
    if (second < 10) {
        timeString += "0" + to_string(second);
    }
    else {
        timeString += to_string(second);
    }

    return timeString;
}
