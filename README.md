# P2 - Memory Management and Layering

## Synopsis
The goal of this project is to create a MemoryManager class that will be used as a replacement for the C++ "new" keyword. The class will handle the allocation and deallocation of sections of memory, in addition to storing a list of holes in the blocks of memory. The data structure used to track blocks and holes is a linked list. The list of holes is used to find the best fit for memory allocation as well as the worst fit.

## Description

### file_1: MemoryManager.h
The header file declares the MemoryManager class as well as the Node class which constructs the linked list.

### file_2: MemoryManager.cpp
This file defines the functions declared in MemoryManager.h.

### public MemoryManager(...)
This constructor sets native word size and default allocator.

### public ~MemoryManager()
This destructor calls shutdown function.

### public void initialize(...)
Function cleans up previous block. Initializes block of requested size using UNIX sbrk.

### public void shutdown()
Deallocates memory block using UNIX brk and resets variables to default values. While loop resets linked list.

### public void *allocate(...)
Function returns pointer of allocated space. default_Alloc finds the offset and while loop iterates through linked list to find node. When the size variable in the node matches, bool curr_hole changes. When the size does not match, a node is created and the node changes size and offset of memory blocks.

### public void free(...)
Function finds offset to free memory block. While loop iterates through linked list to target the matching node, and the node updates to a hole. Another while loop iterates through linked list to find more consecutive holes, and hole values merge. The second hole is removed.

### public void setAllocator(...)
This is an allocator function setter.

### public int dumpMemoryMap(char *filename)
open() call opens file and permits read/write, create, and truncate. write() call writes the output of string holesList to a file. close() saves changes.

### public void *getList()
Function iterates through linked list. It returns the list of holes.

### public void *getBitmap()
Returns an array of string of 0's and 1's that show words in memory blocks. The first two bytes are found by using substring to separate the first and last 8 bytes, using to_string on byteSize, and using bitset. Binary arithmetic finds the decimal value of each 8 bits.

### public unsigned getWordSize()
This function returns the number of bytes in one word.

### public void *getMemoryStart()
This function returns the memory address of starting block.

### public unsigned getMemoryLimit()
This function returns the maximum bytes allowed in memory block.

### int bestFit(...)
Iterates through holesList to find smallest hole size that can fit desired sizeInWords.

### int worstFit(...)
Iterates through holesList to find largest hole size that can fit desired sizeInWords.

## Testing
Makefile was used to compile the class before linking against the library. Tests were executed and the score was shown. Valgrind was installed and used to test for leaks and errors. Gdb was used for debugging.

## Bugs
N/A

## Demo
https://youtu.be/zDCN5p4pONs

## References
P2: Memory Management, COP4600 Project PDF, University of Florida

MemoryManager https://people.kth.se/~johanmon/courses/id2206/lectures/management-handout.pdf

## Author
Obssa Said
