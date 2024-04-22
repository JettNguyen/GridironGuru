//
// Created by zacho on 4/19/2024.
//

#include "LinkedList.h"

Play* LinkedList::getHead() {
    return head;
}

Play *LinkedList::getTail() {
    return tail;
}

void LinkedList::setHead(Play* play){
    head = play;
}
void LinkedList::setTail(Play* play){
    tail = play;
}

void LinkedList::insert(Play *play) {

    if(head == nullptr){
        head = play;
        tail = play;
        return;
    }

    tail->next = play;
    tail = play;
}
