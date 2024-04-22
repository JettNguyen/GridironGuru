#pragma once
#include <vector>
#include "Play.h"

using namespace std;

class Helpers {
public:
    static bool validateInput(const string& input, const string& inputType, int lowerBound, int upperBound);

    static bool booleanResult(int boolValue);

    static float calculateWeight(int yards);

    static vector<int> calculateTimeBounds(int minute, int second);

    static int timeToInt(int minute, int second);

    static vector<int> calculateToGoBounds(int toGo);

    static vector<int> calculateYardLineBounds(int yardLine);

    static string formatPercentages(float percentage);

    static string formatTime(int minute, int second);

    static string generatePlayCode(Play& play);

};
