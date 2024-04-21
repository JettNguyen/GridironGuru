//
// Created by zacho on 4/19/2024.
//

#ifndef PROJECT3_NODEPLAY_H
#define PROJECT3_NODEPLAY_H

#include <istream>
#include "Play.h"

class NodePlay {
private:
    Play* thisPlay;
    NodePlay* nextPlay;
public:
    NodePlay();
    void setPlay(Play* thisPlay);
    void setNext(NodePlay* nextPlay);
    bool isNullPlayPtr();
};


#endif //PROJECT3_NODEPLAY_H
