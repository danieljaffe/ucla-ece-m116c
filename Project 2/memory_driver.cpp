#include <fstream>
#include <iostream>
#include <cstring>
#include <sstream>
#include <iostream>
#include <vector>

#include "memory_controller.h"

#define CACHE_SETS 16
#define MEM_SIZE 2048 // bytes
#define CACHE_WAYS 1
#define BLOCK_SIZE 1 // bytes per block
#define DM 0
#define FA 1
#define SA 2


using namespace std;

struct trace {
    bool MemR;
    bool MemW;
    int adr;
    int data;
};

/*
Either implement your memory_controller here or use a separate .cpp/.c file for memory_controller and all the other functions inside it (e.g., LW, SW, Search, Evict, etc.)
*/


int main(int argc,
         char *argv[]) // the program runs like this: ./program <filename> <mode>
{
    // input file (i.e., test.txt)
    string filename = argv[1];

    // mode for replacement policy
    int type;

    ifstream fin;

    // opening file
    fin.open(filename.c_str());
    if (!fin) { // making sure the file is correctly opened
        cout << "Error opening " << filename << endl;
        exit(1);
    }

    if (argc > 2) {
        type = stoi(
                argv[2]); // the input could be either 0, 1, or 2 (for DM, FA, and SA)
    } else {
        type = 0; // the default is DM.
    }


    // reading the text file
    string line;
    vector<trace> myTrace;
    int TraceSize = 0;
    string s1, s2, s3, s4;
    while (getline(fin, line)) {
        stringstream ss(line);
        getline(ss, s1, ',');
        getline(ss, s2, ',');
        getline(ss, s3, ',');
        getline(ss, s4, ',');
        myTrace.push_back(trace());
        myTrace[TraceSize].MemR = stoi(s1);
        myTrace[TraceSize].MemW = stoi(s2);
        myTrace[TraceSize].adr = stoi(s3);
        myTrace[TraceSize].data = stoi(s4);
        //cout<<myTrace[TraceSize].MemW << endl;
        TraceSize += 1;
    }

    auto mc = memory_controller(type);

    // counters for miss rate
    int accessL = 0;
    int accessS = 0;
    int status = 1;
    int clock = 0;
    int traceCounter = 0;
    bool cur_MemR;
    bool cur_MemW;
    int cur_adr;
    int cur_data;
    // this is the main loop of the code
    while (traceCounter < TraceSize) {
        if (status == 1) {
            cur_MemR = myTrace[traceCounter].MemR;
            cur_MemW = myTrace[traceCounter].MemW;
            cur_data = myTrace[traceCounter].data;
            cur_adr = myTrace[traceCounter].adr;
            traceCounter += 1;
            if (cur_MemR == 1)
                accessL += 1;
            else if (cur_MemW == 1)
                accessS += 1;
        }
        // YOUR CODE
        status = mc.clock_cycle(cur_MemR, cur_MemW, cur_data, cur_adr);
        clock += 1;
    }

    while (status < 1) { // to make sure that the last access is also done
        status = mc.clock_cycle(cur_MemR, cur_MemW, cur_data, cur_adr);
        clock += 1;
    }
    float miss_rate = mc.get_miss_count() / (float) accessL;

    // printing the final result
    std::cout << "(" << clock << ", " << miss_rate << ")" << endl;
    std::cerr << "Miss count: " << mc.get_miss_count() << endl;
    std::cerr << "Miss rate: " << miss_rate << endl;
    std::cerr << "Total cycles: " << clock << endl;

    // closing the file
    fin.close();

    return 0;
}
