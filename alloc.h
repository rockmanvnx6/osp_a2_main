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

        MemoryNode() { /* Default constructor */ }
        MemoryNode(void *mem_address, string content, double size_data) {
            start_memory = mem_address;
            data = content;
            size = size_data;
        }
};

class Allocator {
    public:
        list<MemoryNode> *alloc_mb;
        list<MemoryNode> *free_mb;
        string strategy; 

        Allocator() { /* Default constructor */ }
        
        Allocator(list<MemoryNode> *alloc, list<MemoryNode> *free, string alloc_strat) {
            strategy      = alloc_strat;
            alloc_mb      = alloc;
            free_mb       = free;
        }

        int length_alloc = 0;

        list<MemoryNode>::iterator first_fit(string data) {
            // return nullptr;
            list<MemoryNode>::iterator iter = free_mb->begin();

            while(iter != free_mb->end()) {
                if (iter->size >= data.size()) {
                    return iter;
                }
                iter++;
            }    
           
            return iter;
        }

        list<MemoryNode>::iterator best_fit(string data) {
            list<MemoryNode>::iterator iter = free_mb->begin();
            // double min_size = numeric_limits<double>::max();
            list<MemoryNode>::iterator smallest = free_mb->end();
            
            while(iter != free_mb->end()) {
                if ((iter->size >= data.size() && iter->size <= smallest->size) || 
                    (smallest == free_mb->end() && iter->size >= data.size()) ) {
                    smallest = iter;
                }
                iter++;
            }    

            if (smallest != free_mb->end()) {
                cout << "SMALLEST: " << smallest->data << endl;
            }
            
            
            return smallest;
        }

        list<MemoryNode>::iterator worst_fit(string data) {
            list<MemoryNode>::iterator iter = free_mb->begin();
            // double min_size = numeric_limits<double>::max();
            list<MemoryNode>::iterator largest = free_mb->end();
            
            while(iter != free_mb->end()) {
                if ((iter->size >= data.size() && iter->size >= largest->size) || 
                    (largest == free_mb->end() && iter->size >= data.size()) ) {
                    largest = iter;
                }
                iter++;
            }    

            if (largest != free_mb->end()) {
                cout << "LARGEST: " << largest->data << endl;
            }
            
            
            return largest;
        }

        list<MemoryNode>::iterator search_free_memory(string data, const string strategy) {
            if(strategy.compare("first_fit") == 0) {
                return first_fit(data);
            }
            
            if (strategy.compare("best_fit") == 0) {
                return best_fit(data);
            }

            if (strategy.compare("worst_fit") == 0) {
                return worst_fit(data);
            }
            cout << ERROR::INVALID_STRATEGY_ERROR << endl;
            return free_mb->end();
        }

        void *alloc(string data) {
            list<MemoryNode>::iterator memory_pointer = search_free_memory(data, strategy);
            if (memory_pointer == free_mb->end()) {
                void *request_memory = sbrk(data.size());
                if (request_memory == ERROR::SBRK_MEMORY_ERROR) { 
                    cout << ERROR::MEMORY_ERROR << endl;
                    return nullptr;
                };
                MemoryNode *new_node = new MemoryNode(request_memory, data, data.size());
                alloc_mb->push_back(*new_node);
            } else {
                /* Found a free block */
                memory_pointer->data = data;
                double remain_size = memory_pointer->size - data.size();

                if (remain_size > 0) {
                    memory_pointer->size = data.size();
                    void *new_start = (void*)((char*)(memory_pointer->start_memory) + data.size());
                    MemoryNode *remainder = new MemoryNode(new_start, "X", remain_size);
                    free_mb->insert(memory_pointer, *remainder);
               }
               alloc_mb->push_back(*memory_pointer);
               free_mb->erase(memory_pointer);
            }
            // print_alloc();
            length_alloc++;
            return nullptr;
        }

        void print_alloc() {
            list<MemoryNode>::iterator iter = alloc_mb->begin();
            cout << "----------INFO-----------" << endl;
            cout << "ALLOC: ";
            while (iter != alloc_mb->end()) {
                    cout << iter->size << "-";
                    iter++;
            }
            iter = alloc_mb->begin();
            cout << endl;
            cout << "ALLOC: ";
            while (iter != alloc_mb->end()) {
                    cout << iter->data << "-";
                    iter++;
            }
            cout << endl;
            cout << "\n"<<endl;

            iter = free_mb->begin();

            cout << "FREE: ";

            while (iter != free_mb->end()) {
                    cout << iter->size << "-";
                    iter++;
            }
            cout << endl;

            iter = free_mb->begin();
            cout << "FREE: ";
            while (iter != free_mb->end()) {
                    cout << iter->data << "-";
                    iter++;
            }
            cout << endl;


            cout << "--------------------------" << endl;
        }

        void random_dealloc() {
            list<MemoryNode>::iterator it = alloc_mb->begin();
            int random = rand() % alloc_mb->size();
            int i=0;
            while (it != alloc_mb->end()) {
                if (i == random) {
                    // cout << "REMOVING " << it->data << endl;
                    free_mb->push_back(*it);
                    alloc_mb->erase(it);
                    break;
                }
                it++;
                i++;
            }
            // print_alloc();
            return;
        }
};

