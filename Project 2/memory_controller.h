//
// Created by Daniel Jaffe on 12/10/21.
//

#ifndef PROJECT_2_MEMORY_CONTROLLER_H
#define PROJECT_2_MEMORY_CONTROLLER_H

#include <iostream>

#define MEM_SIZE 2048 // bytes
#define CACHE_SETS 16
#define DM 0
#define FA 1
#define SA 2

class memory_controller {

public:
    explicit memory_controller(int cache_type);

    ~memory_controller();

    int clock_cycle(bool cur_MemR, bool cur_MemW, int &cur_data, int cur_adr);

    int get_miss_count() const;

    __attribute__((unused)) int get_hit_count() const;

private:
    struct cache_set {
        int tag; // you need to compute offset and index to find the tag.
        int lru_position; // for FA and SA only
        int data; // the actual data stored in the cache/memory
    };

    int type;
    int status;
    int miss_count;
    int hit_count;
    int myMem[MEM_SIZE]{};
    cache_set myCache[CACHE_SETS]{};

    void load_word(int &cur_data, int cur_adr);

    void store_word(int &cur_data, int cur_adr);
};


#endif //PROJECT_2_MEMORY_CONTROLLER_H
