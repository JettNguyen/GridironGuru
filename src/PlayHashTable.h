#include <iostream>
#include "LinkedList.h"
#include <unordered_map>
#include "Helpers.h"


struct PlayHash {
    size_t operator()(const string& playCode) {
        // using STL's hash function for strings
        std::hash<string> str_hash;
        return str_hash(playCode);
    }

};

class PlayHashTable {

public:
    //read data from file and put into hash map
    static void readDataAndPushIntoHashMap(const string& filename, unordered_map<string, LinkedList/*, PlayHash*/>& ht);


};
