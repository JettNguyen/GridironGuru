#pragma once
#include <string>

using namespace std;

//represents a play
struct Play {
    Play* next;
    int gameID;
    string gameDate;
    int quarter;
    int minutes;
    int seconds;
    int timeAsInt;
    string offense;
    string defense;
    int down;
    int toGo;
    int yardLine;
    bool resultIsFirstDown;
    float firstDownWeight;
    string description;
    int resultingYards;
    float yardsWeight;
    string formation;
    string playType;
    bool isRush;
    bool isPass;
    bool isIncomplete;
    bool isTouchdown;
    float touchdownWeight;
    string passType;
    bool isSack;
    bool isInterception;
    float interceptionWeight;
    bool isFumble;
    float fumbleWeight;
    bool isTwoPointConversion;
    bool isTwoPointConversionSuccessful;
    float twoPointWeight;
    string rushDirection;


public:
    Play();

    bool operator==(const Play& comparedPlay);
};