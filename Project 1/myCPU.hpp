#ifndef MYCPU_INCLUDED
#define MYCPU_INCLUDED

using namespace std;

class myCPU 
{
    myCPU();
    ~myCPU();

    void fetch();
    void decode();
    void execute();
    void memory();
    void writeback();
};


#endif 






