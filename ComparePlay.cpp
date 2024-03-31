#include "Play.h"
#include "ComparePlay.h"
//borrowed logic from Discussion 6 - Heaps & Priority Queues (slide 74)
bool ComparePlay::operator()(const Play& play1, const Play& play2) {
    //calculates favorable outcomes by calculating weights for relevant attributes
    //first downs get 10 points, yards range from -99^0.75 to 99^0.75 points, touchdowns get 10, interceptions get -100,
    //two pt conversions get 5
    float playRating1 = play1.firstDownWeight + play1.yardsWeight + play1.touchdownWeight + play1.interceptionWeight + play1.twoPointWeight;
    float playRating2 = play2.firstDownWeight + play2.yardsWeight + play2.touchdownWeight + play2.interceptionWeight + play2.twoPointWeight;

    return playRating1 < playRating2;
}
