#include <fstream>
#include <iostream>
#include <cstring>
#include <sstream>
#include <iostream>
#include <vector>


#define CACHE_SETS 16
#define MEM_SIZE 2048
#define CACHE_WAYS 1
#define BLOCK_SIZE 1 // bytes per block
#define DM 0
#define FA 1


using namespace std;
struct cache_set {
    int tag; // you need to compute offset and index to find the tag.
    int lru_position; // for FA only
    int data; // the actual data stored in the cache/memory
    // add more things here if needed
};

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

    if (argc > 2)
        type = stoi(
                argv[2]); // the input could be either 0 or 1 (for DM and FA)
    else type = 0;// the default is DM.


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


    // defining a fully associative or direct-mapped cache
    cache_set myCache[CACHE_SETS]; // 1 set per line. 1B per Block
    int myMem[MEM_SIZE];


    // initializing
    for (int i = 0; i < CACHE_SETS; i++) {
        myCache[i].tag = -1; // -1 indicates that the tag value is invalid. We don't use a separate VALID bit.
        myCache[i].lru_position = 0;  // 0 means lowest position
        myCache[i].data = 0;
    }

    // counters for miss rate
    int accessL = 0; //////
    int accessS = 0;
    int miss = 0; // this has to be updated inside your memory_controller
    int status = 1;
    int clock = 0;
    int traceCounter = 0;
    bool cur_MemR;
    bool cur_MemW;
    int cur_adr;
    int cur_data;
    bool hit;
    // this is the main loop of the code
    while (traceCounter < TraceSize) {
        if (status == 1) {
            cur_MemR = myTrace[traceCounter].MemR;
            cur_MemW = myTrace[traceCounter].MemW;
            cur_data = myTrace[traceCounter].data;
            cur_adr = myTrace[traceCounter].adr;
            hit = false;
            traceCounter += 1;
            if (cur_MemR == 1)
                accessL += 1;
            else if (cur_MemW == 1)
                accessS += 1;
        }
        // YOUR CODE
        status = memory_controller(cur_MemR, cur_MemW, &cur_data, cur_adr,
                                   status, &miss, mode, myCache,
                                   myMem); // in your memory controller you need to implement your FSM, LW, SW, and MM.
        ////////////
        clock += 1;
    }
    // to make sure that the last access is also done
    while (status < 1) {
        // YOUR CODE
        status = memory_controller(cur_MemR, cur_MemW, &cur_data, cur_adr,
                                   status, &miss, mode, myCache,
                                   myMem); // in your memory controller you need to implement your FSM, LW, SW, and MM.
        ////////////
        clock += 1;
    }
    float miss_rate = miss / (float) accessL;

    // printing the final result

    // closing the file
    fin.close();

    return 0;
}
