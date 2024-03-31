
#include <string>
#include "Play.h"

using namespace std;


Play::Play(){
    gameID = 0;
    gameDate = "";
    quarter = -1;
    minutes = 0;
    gameSection = 0;
    seconds = 0;
    offense = "";
    defense = "";
    down = -1;
    toGo = -1;
    yardLine = -1;
    resultIsFirstDown = false;
    firstDownWeight = 0.0f;
    description = "";
    resultingYards = -1;
    yardsWeight = 0.0f;
    formation = "";
    playType = "";
    isRush = false;
    isPass = false;
    isIncomplete = true;
    isTouchdown = false;
    touchdownWeight = 0.0f;
    passType = "";
    isSack = false;
    isInterception = true;
    interceptionWeight = 0.0f;
    isFumble = true;
    isTwoPointConversion = false;
    isTwoPointConversionSuccessful = false;
    twoPointWeight = 0.0f;
    rushDirection = 0.0f;
}