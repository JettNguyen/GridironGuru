//
// Created by zacho on 4/19/2024.
//

#include "NodePlay.h"

NodePlay::NodePlay(Play* thisPlay) {
    this->thisPlay = thisPlay;
    this->nextPlay = nullptr;
}

NodePlay::NodePlay() {
    this->thisPlay = nullptr;
    this->nextPlay = nullptr;
}
