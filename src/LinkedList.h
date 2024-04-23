//
// Created by zacho on 4/19/2024.
//

#ifndef PROJECT3_LINKEDLIST_H
#define PROJECT3_LINKEDLIST_H

#include <istream>
#include "Play.h"

class LinkedList {
private:
    Play* head = nullptr;
    Play* tail = nullptr;

public:
    int count = 0;
    LinkedList() { };
    ~LinkedList();

    Play* getHead();
    void setInitialPlay(Play& play);
    void insert(Play* play);
};


#endif //PROJECT3_LINKEDLIST_H
