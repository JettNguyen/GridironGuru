//
// Created by zacho on 4/19/2024.
//

#ifndef PROJECT3_NODEPLAY_H
#define PROJECT3_NODEPLAY_H

#include <istream>
#include "Play.h"

class NodePlay {
public:
//private:
    Play* thisPlay;
    NodePlay* nextPlay;
//public:
    NodePlay();
    NodePlay(Play* thisPlay);
};


#endif //PROJECT3_NODEPLAY_H
