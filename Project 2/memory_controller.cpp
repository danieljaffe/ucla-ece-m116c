// You have to implement three separate cache models (details later) a
// direct-mapped a fully-associative, and a 4-way set-associative cache, and use
// (ideal) LRU replacement policy (for FA and SA). Each cache has 1 byte per
// block and a total of 16 sets (for DM) and 4 sets (for SA). The main memory is
// 2048 bytes.

#include "memory_controller.h"

// Constructor that initialized the cache
memory_controller::memory_controller(int cache_type) {
    status = 1;
    type = cache_type;
    miss_count = 0;
    hit_count = 0;

    // initializing
    for (auto &i: myCache) {
        i.tag = -1; // -1 indicates that the tag value is invalid. We don't use a separate VALID bit.
        i.lru_position = 0;  // 0 means it is the lowest position
        i.data = 0;
    }
}

// Deconstructor
memory_controller::~memory_controller() = default;

// Representative of one clock cycle. Takes an action based on the "status" of
// the cache along with the passed in parameters.
int memory_controller::clock_cycle(bool cur_MemR, bool cur_MemW, int &cur_data,
                                   int cur_adr) {
    // Stall for a cycle until ready
    if (status < 1) {
        if (status == 0) {
            cur_data = myMem[cur_adr];
        }
        ++status;
    }
        // Memory ready (LW)
    else if (cur_MemR == 1) {
        load_word(cur_data, cur_adr);
    }
        // Memory write (SW)
    else if (cur_MemW == 1) {
        store_word(cur_data, cur_adr);
    }

    return status;
}

// Returns miss count
int memory_controller::get_miss_count() const {
    return miss_count;
}

// Made for debugging. Returns hit count.
__attribute__((unused)) int memory_controller::get_hit_count() const {
    return hit_count;
}

// Handles the load word process for each cache type.
void memory_controller::load_word(int &cur_data, int cur_adr) {
    /////////////////////////
    // direct mapped cache //
    ////////////////////////
    if (type == DM) {
        int cur_tag = cur_adr >> 4;
        int cur_index = cur_adr & 0xF;
        if (myCache[cur_index].tag == cur_tag) { // Cache hit
            cur_data = myCache[cur_index].data;
            hit_count++;
            status = 1;
        } else { // Cache miss
            miss_count++;
            myCache[cur_index].tag = cur_tag;
            myCache[cur_index].data = myMem[cur_adr];
            cur_data = myCache[cur_index].data;
            status = -3;
        }
    }
        // FA

        /////////////////////////////
        // fully-associative cache //
        ////////////////////////////
    else if (type == FA) { // Search entire cache
        int hit = 0;
        int index = 0;
        int temp_lru = 0;
        for (auto &i: myCache) {
            if (hit == 0 and i.tag == cur_adr) { // Cache hit
                temp_lru = i.lru_position;
                i.lru_position = 15;
                cur_data = i.data;
                hit = 1;
                hit_count++;
                status = 1;
                break;
            }
            index++;
        }

        // No hit found so handle a cache miss. Evict LRU
        if (hit == 0) {
            miss_count++;
            index = 0;
            for (auto &i: myCache) {
                if (i.tag == -1 or i.lru_position == 0) {
                    i.data = myMem[cur_adr];
                    i.tag = cur_adr;
                    i.lru_position = 15;
                    status = -3;
                    break;
                }
                index++;
            }
        }

        // Updates the LRU for cache.
        int index2 = 0; // Added to make sure I don't decrement from the newly updated LRU
        for (auto &i: myCache) {
            if (i.tag != -1 and i.lru_position > temp_lru and index != index2) {
                i.lru_position--;
            }
            index2++;
        }
    }

        ///////////////////////////
        // set-associative cache //
        //////////////////////////
    else if (type == SA) {
        int cur_tag = cur_adr >> 2;
        int cur_set_index = (cur_adr & 0x3) * 4;
        int temp_lru = 0;
        int updated = -1;
        int hit = 0;

        // Checks for a possible hit
        for (int i = 0; i < 4; i++) {
            if (myCache[cur_set_index + i].tag == cur_tag) { // Cache hit!
                temp_lru = myCache[cur_set_index + i].lru_position;
                myCache[cur_set_index + i].lru_position = 3;
                cur_data = myCache[cur_set_index + i].data;
                updated = cur_set_index + i;
                hit_count++;
                hit = 1;
                status = 1;
//                std::cerr << "HIT  --> ";
            }
        }

        // No hit was found - Cache miss. Evict LRU.
        if (hit == 0) {
            miss_count++;
            updated = -1;
//            std::cerr << "MISS --> ";

            for (int i = cur_set_index; i < cur_set_index + 4; i++) {
                if (myCache[i].tag == -1 or myCache[i].lru_position == 0) {
                    myCache[i].data = myMem[cur_adr];
                    myCache[i].tag = cur_tag;
                    myCache[i].lru_position = 3;
                    cur_data = myCache[i].data;
                    status = -3;
                    updated = i;
                    break;
                }
            }
        }

        // Updates the LRU for the corresponding blocks.
        for (int i = cur_set_index; i < cur_set_index + 4; i++) {
            if (myCache[i].tag != -1 and myCache[i].lru_position > temp_lru and
                i != updated) {
                myCache[i].lru_position--;
            }
        }

//        std::cerr << "LW--> Current Address: " << cur_adr
//                  << "     Current Data: " << cur_data << "     Set-Index: "
//                  << cur_set_index / 4 << "-" << updated % 4
//                  << "     Current Tag: " << cur_tag << std::endl;
    }
}


// Handles the store word process for each cache type.
void memory_controller::store_word(int &cur_data, int cur_adr) {
    /////////////////////////
    // direct mapped cache //
    ////////////////////////
    if (type == 0) {
        int cur_tag = cur_adr >> 4;
        int cur_index = cur_adr & 0x4;
        if (myCache[cur_index].tag == cur_tag) { // Cache hit
            myCache[cur_index].data = cur_data;
        }
    }
        /////////////////////////////
        // fully-associative cache //
        ////////////////////////////
    else if (type == 1) {
        for (auto &i: myCache) {
            if (i.tag == cur_adr) { // Cache hit
                i.data = cur_data;
            }
        }
    }
        ///////////////////////////
        // set-associative cache //
        //////////////////////////
    else if (type == 2) {
        int cur_tag = cur_adr >> 2;
        int cur_set_index = (cur_adr & 0x3) * 4;
        for (int i = 0; i < 4; i++) {
            if (myCache[cur_set_index + i].tag == cur_tag) {
                myCache[cur_set_index + i].data = cur_data;
            }
        }
    }

//    std::cerr << "SW--> Current Address: " << cur_adr << "     Current Data: "
//              << cur_data << std::endl;
    // Regardless of hit or miss
    myMem[cur_adr] = cur_data;
    status = 1;
    cur_data = 0;
}