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
        count++;
        return;
    }
    else if(tail == nullptr){
        tail = play;
        head->next = tail;
        count++;
        return;
    }

    tail->next = play;
    tail = play;
    count++;
}

LinkedList::~LinkedList() {
    while (head != nullptr && head->next != tail){
        Play* temp = head;
        head = head->next;
        delete temp;
    }

};