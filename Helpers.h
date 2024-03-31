#include <vector>

using namespace std;

class Helpers {
public:
    static bool validateInput(const string& input, const string& inputType, int lowerBound, int upperBound);

    static bool booleanResult(int boolValue);

    static float calculateWeight(int yards);

    static int calculateGameSection(int minute);

    static vector<int> calculateToGoBounds(int toGo);

    static vector<int> calculateYardLineBounds(int yardLine);

    static string formatPercentages(float percentage);

    static string formatTime(int min, int sec);

};
