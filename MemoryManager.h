#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <functional>
#include <bitset>
#include <cmath>
#include <cstring>
#include <algorithm>

class Node{
    public:
        int size;
        int index;
        bool curr_hole;
        Node* next;

    Node(Node* next, int size, int index, bool curr_hole){
        this->size = size;
        this->index = index;
        this->curr_hole = curr_hole;
        this->next = next;
    }
};
class MemoryManager{
    public:
        MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator);
        ~MemoryManager();
        void initialize(size_t sizeInWords);
        void shutdown();
        void *allocate(size_t sizeInBytes);
        void free(void *address);
        void setAllocator(std::function<int(int, void *)> allocator);
        int dumpMemoryMap(char *filename);
        void *getList();
        void *getBitmap();
        unsigned getWordSize();
        void *getMemoryStart();
        unsigned getMemoryLimit();

    private:
        std::function<int(int, void*)> default_Alloc;
        uint16_t* holeList = nullptr;
        uint8_t* bitMap = nullptr;

        unsigned int wordSize;
        uint8_t* mem = nullptr;
        size_t sizeInWords;

        Node* head = nullptr;

        int memLimit;
};

int bestFit(int sizeInWords, void *list);
int worstFit(int sizeInWords, void *list);