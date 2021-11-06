#include <myCPU.hpp>

/* 
Add all the required standard and developed libraries here. 
Remember to include all those files when you are submitting the project. 
*/
#include <cstdint>
#include <fstream>
#include <iostream>
// #include <string>
#include <vector>

/*
Put/Define any helper function/definitions you need here
*/


int main (int argc, char* argv[]) // your project should be executed like this: ./cpusim filename.txt and should print (a0,a1) 
{
	/* This is the front end of your project. 
	You need to first read the instructions that are stored in a file and load them into an instruction memory. 
	*/

	// Checks for correct arguments
	if (1 != argc) 
	{
		std::cerr << argv[0] << " takes 1 argument: FILE_NAME" << std::endl;
		return -1;
	}

	// Opens the file for reading
	std::ifstream instructions;
	if (!instructions) 
	{
		cout << "Error opening input file" << endl;
		return -1;
	}





	FILE* fin  = stdin; // or use ifstream

	/* Define your Instruction memory here. Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4MB (i.e., 4096 lines). Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode). 
	
	Each line in the input file is stored as an unsigned char and is 1 byte (each four lines are one instruction). You need to read the file line by line and store it into the memory. You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc. 
	*/ 
	instMem = ... 

	

	/* OPTIONAL: Instantiate your Instruction object here. */
	Instruction myInst; 

	/* OPTIONAL: Instantiate your CPU object here. */
	CPU myCPU(); 

	// Clock and PC
	uint64_t myClock = 0; // data-types can be changed! This is just a suggestion. 
	uint64_t myPC = 0; 



	while (1) // processor's main loop. Each iteration is equal to one clock cycle.  
	{
		//fetch
		... = myCPU.fetch(...) // fetching the instruction

		// decode
		... = myCPU.decode(...) // decoding

		// execute

		// memory

		// writeback


		// _next values should be written to _current values here:

		//
		myClock += 1; 
		myPC += 4; // for now we can assume that next PC is always PC + 4
		// we should break the loop if ALL instructions in the pipelinehas opcode==0 instruction 
		(if ...)
			break; 
	}

	// clean up the memory (if any)

	// print the stats

	return 0; 
	
}


