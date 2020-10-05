#include <list>
#include <unordered_map>
#include <ctime>
#include "error.h"
using namespace std;

/**
 * File: alloc.h
 * Author: s3619352@student.rmit.edu.au
 * 
 * Header file contains the allocation algorithm. It will
 * allocate based by strategy. This file will be called 
 * 
 * For more information and implementation explanations. Please
 * read more at the description of each class.
 */
class MemoryNode {
    /**
     * Represent a memory node.
     * 
     * Params:
     * - start_memory: Store the start memory address. Either defined by sbrk
     * or by a node split (more at Allocator.alloc).
     * - data: store the actual content from each line.
     * - size: define the memory size of the node.
     */
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
    /**
     * Main allocator class. 
     * 
     * This class will allocate the memory to alloc_mb and free_mb lists
     * defined in the main file based on the defined strategy.
     * 
     * alloc_mb: List to store allocated memory blocks.
     * free_mb: List to store free memory blocks which are deallocated from alloc_mb.
     */
    public:
        list<MemoryNode> *alloc_mb;
        list<MemoryNode> *free_mb;
        const string EMPTY = "";
        string strategy;
        double total_sbrk_alloc = 0;

        Allocator() { /* Default constructor */ }
        
        Allocator(list<MemoryNode> *alloc, list<MemoryNode> *free, string alloc_strat) {
            strategy      = alloc_strat;
            alloc_mb      = alloc;
            free_mb       = free;
        }

        static bool compare_func(const MemoryNode &first, const MemoryNode &second) {
            return first.start_memory < second.start_memory;
        }

        list<MemoryNode>::iterator first_fit(string data) {
            /**
             * First-fit strategy implementation.
             * 
             * This function return an iterator of memory node that found by the
             * first fit - the first node that fits the data size.
             */
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
            /**
             * Best-fit strategy implementation.
             * 
             * This function return an iterator of memory node that found by the
             * best fit - the smallest node that fits the data size.
             */
            list<MemoryNode>::iterator iter = free_mb->begin();
            list<MemoryNode>::iterator smallest = free_mb->end();
            
            while(iter != free_mb->end()) {
                if ((iter->size >= data.size() && iter->size <= smallest->size) || 
                    (smallest == free_mb->end() && iter->size >= data.size()) ) {
                    smallest = iter;
                }
                iter++;
            }                
            
            return smallest;
        }

        list<MemoryNode>::iterator worst_fit(string data) {
            /**
             * Worst-fit strategy implementation.
             * 
             * This function return an iterator of memory node that found by the
             * worst fit - the largest node that fits the data size.
             */
            list<MemoryNode>::iterator iter = free_mb->begin();
            list<MemoryNode>::iterator largest = free_mb->end();
            
            while(iter != free_mb->end()) {
                if ((iter->size >= data.size() && iter->size >= largest->size) || 
                    (largest == free_mb->end() && iter->size >= data.size()) ) {
                    largest = iter;
                }
                iter++;
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

            return free_mb->end();
        }

        int alloc(string data) {
            /**
             * This function alloc a memory to alloc_mb given the data
             * in string format.
             * 
             * If search_free_memory returns a valid iterator of a free memory block 
             * from free_mb.
             * 
             * This function will then use that free_mb block to store the data, which
             * could have 3 possible outcomes:
             * 
             * 1. If there is no available block in free_mb block (could be that data size larger
             * than all of the blocks in free_mb or there is no block in free_mb). We simply
             * call sbrk() to allocate a new block.
             * 
             * 
             * 2. If the data has smaller size than the free_mb block. We divide the free_mb
             * block into 2 different block:
             *   a. One which just contains enough size to store the data. This block is
             *      simply moved to the alloc_mb list.
             *   b. One which contains the left over memory which results from free_mb block
             *      size - data size. This memory stays in the free_mb block with 
             *      start_memory = old_start_memory + data.size
             * 
             * 3. If the data has the same size as free_mb block. We simply move that block
             * to alloc_mb list. And assign data to that block.
             * 
             */
            list<MemoryNode>::iterator memory_pointer = search_free_memory(data, strategy);
            if (memory_pointer == free_mb->end()) {
                /**
                 * Case 1: No available block.
                 * 
                 * Allocate new memory node to alloc_mb using sbrk() with the data
                 * size.
                 * 
                 * If there is no memory left in the system. We will return ENOMEM 
                 * error code.
                 */
                void *request_memory = sbrk(data.size());
                if (request_memory == ERROR::SBRK_MEMORY_ERROR) { 
                    cout << ERROR::MEMORY_ERROR << endl;
                    return ENOMEM;
                };
                total_sbrk_alloc           += data.size();
                MemoryNode *new_node        = new MemoryNode(request_memory, data, data.size());
                alloc_mb->push_back(*new_node);
            } else {
                memory_pointer->data        = data;
                double remain_size          = memory_pointer->size - data.size();

                if (remain_size > 0) {
                    /**
                     * Case 2: We take the remain_size and create a new memory block
                     * in the free_mb list.
                     */
                    memory_pointer->size = data.size();
                    void *new_start = (void*)((char*)(memory_pointer->start_memory) + data.size());
                    MemoryNode *remainder = new MemoryNode(new_start, EMPTY, remain_size);
                    free_mb->insert(memory_pointer, *remainder);
               }
               /**
                * Case 2, 3: We insert the found memory to the alloc system.
                */
               alloc_mb->push_back(*memory_pointer);
               free_mb->erase(memory_pointer);
            }
            return 0;
        }

        void sort_n_merge() {
            /**
             * Sort and merge the free memory list.
             * 
             * This function also merge if the memory addresses of any 2 blocks
             * in the list are consecutive (i.e block_1_start + block_1_Size = block2_start).
             * 
             */
            free_mb->sort(Allocator::compare_func);
            list<MemoryNode>::iterator it = free_mb->begin();
            while(it != free_mb->end()) {
                list<MemoryNode>::iterator next_it = next(it);
                while (next_it != free_mb->end()) {
                    void *next_start = (void*)((char*)(it->start_memory) + (int)it->size);
                    if (next_it->start_memory == next_start) {
                        /* MERGE 2 nodes */
                        it->size += next_it->size;
                        free_mb->erase(next_it++);
                    } else {
                        break;
                    }
                }
                it++;
            }
        }

        void random_dealloc() {
            /**
             * This function randomly deallocate a memory block from alloc_mb
             * and then push it to free_mb list.
             * 
             * After deallocated, we will call sort_n_merge to sort and merge
             * consecutive memory blocks in free_mb.
             */
            list<MemoryNode>::iterator it = alloc_mb->begin();
            int random = rand() % alloc_mb->size();
            int i=0;
            while (it != alloc_mb->end()) {
                if (i == random) {
                    (it->data).clear();
                    free_mb->push_back(*it);
                    alloc_mb->erase(it);
                    break;
                }
                it++;
                i++;
            }
            sort_n_merge();
            return;
        }
};

