//
// Created by zacho on 4/19/2024.
//

#include "LinkedList.h"

Play* LinkedList::getHead() {
    return head;
}

void LinkedList::setInitialPlay(Play& play){
    head = &play;
    tail = &play;
}

void LinkedList::insert(Play* play) {
    if(head == nullptr){
        head = play;
        return;
    }
    else if(tail == nullptr){
        tail = play;
        head->next = tail;
        return;
    }

    tail->next = play;
    tail = play;
}
