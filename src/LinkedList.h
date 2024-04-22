//
// Created by zacho on 4/19/2024.
//

#ifndef PROJECT3_LINKEDLIST_H
#define PROJECT3_LINKEDLIST_H

#include <istream>
#include "Play.h"

class LinkedList {
private:

public:
    LinkedList() { };

    Play* getHead();
    void setInitialPlay(Play& play);
    void insert(Play* play);

    Play* head = nullptr;
    Play* tail = nullptr;

};


#endif //PROJECT3_LINKEDLIST_H
