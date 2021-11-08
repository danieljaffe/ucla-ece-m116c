#include <CPU.hpp>

/* 
Add all the required standard and developed libraries here. 
Remember to include all those files when you are submitting the project. 
*/
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

/*
Put/Define any helper function/definitions you need here
*/


int main (int argc, char* argv[]) // your project should be executed like this: ./cpusim filename.txt and should print (a0,a1) 
{
	// Checks for correct number of arguments
	if (2 != argc) {
        std::cerr << argv[0] << " takes 1 argument: FILE_NAME" << std::endl;
        return -1;
    }

	// This creates a vector for the instruction memory bytes and then uses std iterators to copy each line of the file.
	// I read them in as uint16_t and then copy them as uint8_t. The reason this is necessary is because uint8_t would
	// be read as a char and only grab one digit of each numbers at a time.
	std::ifstream ifs(argv[1]);
	std::vector<uint8_t> instMem;
    std::istream_iterator<uint16_t> input(ifs);
    std::copy(input, std::istream_iterator<uint16_t>(), std::back_inserter(instMem));

    // This vector is for the data memory.
    std::vector<uint8_t> dataMem;

	// Instantiate CPU object
    auto myCPU = CPU(std::move(instMem), std::move(dataMem));

	while (1) // processor's main loop. Each iteration is equal to one clock cycle.
	{
		//fetch
		myCPU.fetch();

		// decode
		myCPU.decode();

		// execute
        myCPU.execute();

		// memory
        myCPU.memory();

		// writeback
        myCPU.writeback();

		// _next values should be written to _current values here:
        myCPU.clockTick();

		// Break the loop if ALL instructions in the pipeline has opcode==0 instruction
		if (myCPU.isFinished()) { break; }
	}

	// print the stats
    myCPU.printStats();

	return 0;
}



