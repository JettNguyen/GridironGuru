//
// Created by zacho on 4/19/2024.
//

#include "NodePlay.h"

NodePlay::NodePlay() {
    this->thisPlay = nullptr;
    this->nextPlay = nullptr;

}

void NodePlay::setPlay(Play *thisPlay) {
    this->thisPlay = thisPlay;
}

void NodePlay::setNext(NodePlay *nextPlay) {
    this->nextPlay = nextPlay;
}

bool NodePlay::isNullPlayPtr() {
    if(this->thisPlay == nullptr){
        return true;
    }
    return false;
}

bool NodePlay::isNullNextPtr() {
    if(this->nextPlay == nullptr){
        return true;
    }
    return false;
}
