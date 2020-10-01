#include <list>
#include <unordered_map>
#include <ctime>
#include "error.h"
using namespace std;

class MemoryNode {
    public:
        void *start_memory;
        string data;
        double size;
        
        MemoryNode *prev;
        MemoryNode *next;

        MemoryNode() { /* Default constructor */ }
        MemoryNode(void *mem_address, string content, double size_data) {
            start_memory = mem_address;
            data = content;
            size = size_data;
        }
};

class Allocator {
    public:
        MemoryNode *alloc_mb_head;
        MemoryNode *free_mb_head;
        string strategy; 

        Allocator() { /* Default constructor */ }
        
        Allocator(string alloc_strat) {
            strategy      = alloc_strat;
        }
        MemoryNode *alloc_mb_tail;
        MemoryNode *free_mb_tail;

        int length_alloc = 0;

        MemoryNode *first_fit(string data) {
            // return nullptr;
            MemoryNode *iter = free_mb_head;

            if (free_mb_head) {
                while(iter != nullptr) {
                    if (iter->size >= data.size()) {
                        return iter;
                    }
                    iter = iter->next;
                }    
            }
           
            return nullptr;
        }

        MemoryNode *best_fit(string data) {
            MemoryNode *iter = free_mb_head;
            // double min_size = numeric_limits<double>::max();
            MemoryNode *smallest = nullptr;
            
            if (free_mb_head) {
                while(iter != nullptr) {
                    if ((smallest && iter->size >= data.size() && iter->size <= smallest->size) || 
                        (!smallest && iter->size >= data.size()) ) {
                        smallest = iter;
                    }
                    iter = iter->next;
                }    
            }

            if (smallest) {
                cout << "SMALLEST: " << smallest->data << endl;
            }
            
            
            return smallest;
        }

        void *search_free_memory(string data, const string strategy) {
            if(strategy.compare("first_fit") == 0) {
                return first_fit(data);
            }
            
            if (strategy.compare("best_fit") == 0) {
                return best_fit(data);
            }
            return ERROR::INVALID_STRATEGY_ERROR;
        }

        void *alloc(string data) {
            void *memory_pointer = search_free_memory(data, strategy);
            if (!memory_pointer) {
                void *request_memory = sbrk(data.size());
                if (request_memory == ERROR::SBRK_MEMORY_ERROR) { 
                    cout << ERROR::MEMORY_ERROR << endl;
                    return nullptr;
                };
                MemoryNode *new_node = new MemoryNode(request_memory, data, data.size());
                if (!alloc_mb_head) {
                    alloc_mb_head = new_node;
                    alloc_mb_tail = alloc_mb_head;
                } else {
                    alloc_mb_tail->next = new_node;
                    new_node->prev = alloc_mb_tail;
                    alloc_mb_tail  = alloc_mb_tail->next;
                }
            } else {
                /* Found a free block */
                MemoryNode *free_memory = (MemoryNode *) memory_pointer;
                free_memory->data = data;
                double remain_size = ((MemoryNode *) memory_pointer)->size - data.size();

                if (remain_size > 0) {
                    free_memory->size = data.size();
                    void *new_start = (void*)((char*)(free_memory->start_memory) + data.size());
                    MemoryNode *remainder = new MemoryNode(new_start, "X", remain_size);
                    remainder->prev = free_memory->prev;

                    remainder->next = free_memory->next;

                    if (remainder->next == nullptr) {
                        free_mb_tail = remainder;
                    } else {
                        (remainder->next)->prev = remainder;
                    }

                    if (remainder->prev == nullptr) {
                        free_mb_head = remainder;
                    } else {
                        (remainder->prev)->next = remainder;
                    }
                } else {
                    if (free_memory->prev) {
                        (free_memory->prev)->next = free_memory->next;
                    } else {
                        free_mb_head = free_memory->next;
                        free_mb_head->prev = nullptr;
                    }

                    if (free_memory->next) {
                        (free_memory->next)->prev = free_memory->prev;
                    } else {
                        free_mb_tail = free_memory->prev;
                        free_mb_tail->next = nullptr;
                    }
                }

                free_memory->prev = alloc_mb_tail; 
                free_memory->next = nullptr;
                
                if (alloc_mb_tail)
                    alloc_mb_tail->next = free_memory;
                
                alloc_mb_tail = free_memory;

            }
            // print_alloc();
            length_alloc++;
            return nullptr;
        }

        MemoryNode *getAllocPointer() {
            return alloc_mb_head;
        }

        MemoryNode *getFreePointer() {
            return free_mb_head;
        }

        void print_alloc() {
            MemoryNode* iter = getAllocPointer();
            cout << "----------INFO-----------" << endl;
            if (alloc_mb_head != nullptr) {
                cout << "HEAD ALLOC: " << alloc_mb_head->data << endl;
                // cout << "TEST: " << (void *)((char *)(alloc_mb_head->start_memory) + 1) << endl;
            }

            if (alloc_mb_tail != nullptr)
                cout << "TAIL ALLOC: " << alloc_mb_tail->data << endl;
            
            cout << "ALLOC: ";
            while (iter != nullptr) {
                    cout << iter->size << "-";
                    iter = iter->next; 
            }
            cout << endl;
            iter = getAllocPointer();
            cout << "ALLOC: ";
            while (iter != nullptr) {
                    cout << iter->data << "-";
                    iter = iter->next; 
            }
            cout << endl;
            cout << "\n"<<endl;

            iter = getFreePointer();

            if (free_mb_head != nullptr)
                cout << "HEAD FREE: " << free_mb_head->data << endl;

            if (free_mb_tail != nullptr)
                cout << "TAIL FREE: " << free_mb_tail->data << endl;

            cout << "FREE: ";

            while (iter != nullptr) {
                    cout << iter->size << "-";
                    iter = iter->next; 
            }
            cout << endl;

            iter = getFreePointer();
            cout << "FREE: ";
            while (iter != nullptr) {
                    cout << iter->data << "-";
                    iter = iter->next; 
            }
            cout << endl;


            cout << "--------------------------" << endl;
        }

        void random_dealloc() {
            MemoryNode *it = alloc_mb_head;
            int random = rand() % length_alloc;
            int i = 0;
            while (it != nullptr) {
                if (i == random) {
                    cout << "REMOVING " << it->data << endl;
                    // it->data = "X";

                    if (it->prev) {
                        /* DETACH FROM ALLOC LIST */
                        (it->prev)->next = it->next;
                        if (it->next) {
                            (it->next)->prev = it->prev;
                        }
                    }

                    if (it == alloc_mb_tail) {
                        alloc_mb_tail = alloc_mb_tail->prev;
                        if (alloc_mb_tail) {
                            alloc_mb_tail->next = nullptr;
                        }
                    } else if (it == alloc_mb_head) {
                        alloc_mb_head = alloc_mb_head->next;
                        if (alloc_mb_head) {
                            alloc_mb_head->prev = nullptr;
                        }
                    }

                    if(!free_mb_head) {
                        free_mb_head   = it;
                        free_mb_tail   = free_mb_head;
                        it->prev       = it->next = nullptr;
                    } else {
                        free_mb_tail->next = it;
                        it->prev = free_mb_tail;
                        it->next = nullptr;
                        free_mb_tail = it;
                    }
                    length_alloc--;
                 
                    break;
                }
                it = it->next;
                i++;
            }
            // print_alloc();
            return;
        }
};
