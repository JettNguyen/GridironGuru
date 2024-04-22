//
// Created by zacho on 4/19/2024.
//

#ifndef PROJECT3_LINKEDLIST_H
#define PROJECT3_LINKEDLIST_H

#include <istream>
#include "Play.h"

class LinkedList {
private:
    Play* head;
    Play* tail;
public:
    LinkedList(Play* initialPlay = nullptr) {
        head = initialPlay;
        tail = initialPlay;
    };

    Play* getHead();
    Play* getTail();
    void setHead(Play* play);
    void setTail(Play* play);
    void insert(Play* play);
};


#endif //PROJECT3_LINKEDLIST_H
