//
// Created by zacho on 4/19/2024.
//

#ifndef PROJECT3_LINKEDLIST_H
#define PROJECT3_LINKEDLIST_H


#include "Play.h"

class LinkedList {
private:

public:
    LinkedList() {};

    Play* getHead();

    void insert(Play* play);

    Play* head = nullptr;

    Play* tail = nullptr;
};

#endif //PROJECT3_LINKEDLIST_H
