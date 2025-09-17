#include "MemoryManager.h"
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include <fcntl.h>
#include <string>
#include <sstream>
#include <iostream>
#include <array>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <bitset>
#include <cstring>
using namespace std;

MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator){
    // Store wordSize and the passed in default allocator function as member variables.
    this->default_Alloc = allocator;
    this->wordSize = wordSize;
}

MemoryManager::~MemoryManager(){
   // Deletes all heap memory when the object falls out of scope. Never call the destructor directly.
    shutdown();
}

void MemoryManager::initialize(size_t sizeInWords){
// Instantiates contiguous array of size (sizeInWords * wordSize) amount of bytes.
// ● If initialize is called on an already initialized object, call shutdown then reinitialize.
// ● Most of your other functions should not work before this is called. 
// They should return the relevant error for the data type, such as void, -1, nullptr, etc.
    memLimit = sizeInWords * wordSize;
    if (sizeInWords <= 65536){
        this->sizeInWords = sizeInWords;
        if (this->mem != nullptr) {
            shutdown();
        }
        // allocate your initial block of memory without the use of new or any stdlib functions
        this->mem = (uint8_t*) sbrk(this->sizeInWords * this->wordSize);
    } 
}

void MemoryManager::shutdown(){
    sizeInWords = 0;
    brk(mem);
    mem = nullptr;
    Node* curr = head;
    Node* next;
    while (curr != nullptr){
        next = curr->next;
        delete curr;
        curr = next;
    }
    head = nullptr;
}

int bestFit(int sizeInWords, void *list){
    // Finds a hole in the list that best fits the given sizeInWords
    if (list == nullptr) {
        return 0;
    }
    uint16_t* holesList = static_cast<uint16_t*>(list);
    int size = 1 + holesList[0] * 2;
    int index = 0;
    int diff = 10000000;
    bool is_hole = false;
    for (int i = 2; i < size; i = i + 2) {
        if ((holesList[i] - sizeInWords) >= 0) {
            is_hole = true;
            if (diff > (holesList[i] - sizeInWords)) {
                diff = holesList[i] - sizeInWords;
                index = i - 1;
            }
        }
    }
    int offset = holesList[index];
    if (!is_hole) {
        return -1;
    }
    return offset;
}

int worstFit(int sizeInWords, void *list){
    // finds largest possible hole
    auto* holesList = static_cast<uint16_t*>(list);
    uint16_t size = *holesList++;
    int offset = -1;
    int s = -1;
    for(uint16_t i = 1; i < (size) * 2; i+= 2)
    {
        if(holesList[i] >= sizeInWords && offset == -1)
        {
            offset = holesList[i - 1];
            s = holesList[i];
        }
        else if(s < holesList[i] && holesList[i] >= sizeInWords)
        {
            offset = holesList[i - 1];
            s = holesList[i];
        }
    }


    return offset;
}

void* MemoryManager::allocate(size_t sizeInBytes){   
       if (head == nullptr){
        head = new Node(nullptr, sizeInWords, 0, true);
    }
    unsigned words = ((sizeInBytes) / (this->wordSize));
    if ((sizeInBytes) % (this->wordSize) != 0){
        words += 1;
    }
    int index = this->default_Alloc(words, (void*) getList());
    delete[] holeList;
    if (index == -1) {
        return nullptr;
    }
    Node* curr = head;
    while (curr != nullptr && curr->index != index){
        curr = curr->next;
    }
    if (curr->size == words){
        curr->curr_hole = false;
    }
    else {
        Node* next = curr->next;
        curr->next = new Node(next, curr->size - words, curr->index + words, true);
        curr->size = words;
        curr->curr_hole = false;
    }
    return mem + (this->wordSize * index);
}

void MemoryManager::free(void *address){
    // Frees the memory block within the memory manager so that it can be reused.
    unsigned offset = ((uint8_t*)address - mem) / wordSize;
    Node* curr = head;
    while (curr->index != offset && curr != nullptr){
        curr = curr->next;
    }
    curr->curr_hole = true;
    curr = head;
    Node* next = nullptr;

    while (curr != nullptr){
        while (curr->next != nullptr && curr->next->curr_hole == true && curr->curr_hole == true) {
            curr->size += curr->next->size;
            next = curr->next->next;
            delete curr->next;
            curr->next = next;
        }
        curr = curr -> next;
    }
}

void MemoryManager::setAllocator(std::function<int(int, void *)> allocator){
   
    this->default_Alloc = allocator;
}

int MemoryManager::dumpMemoryMap(char *filename){
    uint16_t* holesList = static_cast<uint16_t*>(getList());
    uint16_t size = holesList[0];
    string dump = "";
    bool is_pass1 = true;
    for (int i = 1; i < size * 2 + 1; i += 2) {
        if (!is_pass1) {
            dump = dump + " - ";
        }
        string start = to_string(holesList[i]);
        string listSize = to_string(holesList[i + 1]);
        dump = dump + "[" + start + ", " + listSize + "]";
        is_pass1 = false;
    }
    delete[] holesList;
    int file = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (write(file, dump.c_str(), dump.length()) == -1){
    return -1;
    }
    else {
        close(file);
        return 0;
    }
    return 0;
}

void* MemoryManager::getList(){
    uint16_t numHoles = 0;
    Node* curr = head;
    vector<uint16_t> list = {};

    if(mem == nullptr){
        return nullptr;
    }
    while (curr != nullptr){
        if (curr->curr_hole) {
            numHoles++;
            list.push_back(curr->index);
            list.push_back(curr->size);
        }
        curr = curr->next;
    }
    holeList = new uint16_t[2*numHoles+1];
    holeList[0] = numHoles;
    for (uint16_t i = 0; i < 2 * numHoles; i++){
        holeList[i+1] = list[i];
    }
    return holeList;
}

void* MemoryManager::getBitmap(){
    size_t size = sizeInWords / 8;
    if (sizeInWords % 8 != 0){
        size++;
    }
    size_t total = size + 2;
    bitMap = new uint8_t[total];
    std::bitset<16> bitMapSize{size};
    string mapToString = bitMapSize.to_string();
    uint16_t* holeList = static_cast<uint16_t*>(getList());
    uint16_t len = holeList[0];
    string bitString = "";
    for (uint16_t i = 0; i < sizeInWords; i++){
        bitString.push_back('1');
    }
    for (uint16_t i = 1; i < len * 2; i=i+2){
        for (uint16_t j = holeList[i]; j < holeList[i] + holeList[i+1]; j++){
            bitString.at(j) = '0';
        }
    }
    int k = 2;
    uint8_t num = 0;
    for (int i = 0; i < bitString.size(); i+=8){
        for (int j = 0; j < 8; j++){
            if(i+j < bitString.size()){
                num += (bitString.at(i+j)-'0') * pow(2,j);
            }
        }
        bitMap[k] = num;
        k++;
        num = 0;
    }
    bitMap[0] = static_cast<uint8_t>(std::stoi(mapToString.substr(8,8), nullptr, 2));
    bitMap[1] = static_cast<uint8_t>(std::stoi(mapToString.substr(0,8), nullptr, 2));

    delete[] holeList;
    return bitMap;
}

unsigned MemoryManager::getMemoryLimit(){ 
    return this->sizeInWords*this->wordSize;
}
unsigned MemoryManager::getWordSize(){
    return this->wordSize;
}
void* MemoryManager::getMemoryStart(){
    return this->mem;
}