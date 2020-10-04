#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include "alloc.h"

using namespace std;

/**
 * File: main.cpp
 * Author: s3619352@student.rmit.edu.au
 * 
 * Main file that allows to call allocator code (located in alloc.h).
 * 
 * Usage: main.cpp <strategy> <input_file> <output_file>
 * Strategy List:
 * - first_fit: perform first fit allocation on <input_file>
 * - best_fit : perform best fit allocation on <input_file>
 * - worst_fit: perform worst fit allocation on <input_file>
 * 
 * On success: result of the allocation will be saved in <output_file>
 * which includes: total memory allocated using sbrk, start memory and size
 * of each node in allocMB list and freeMB list.
 * 
 * On failure: 
 * - return EINVAL (invalid argument) if the number of argument
 * is not 4.
 * - return ENOENT (no such file or directory) if <input_file> is not found.
 * 
 */

void print_usage(string argv0) {
    cout << "USAGE: " << argv0 << " <strategy> <input_file> <output_file>" << endl;
    cout << endl;
    cout << "Strategy List:" << endl
            << " - first_fit: perform first fit allocation on <input_file>" << endl
            << " - best_fit : perform best fit allocation on <input_file>" << endl
            << " - worst_fit: perform worst fit allocation on <input_file>" << endl;
    cout << endl;
    cout << "Note: data will be saved in <output_file> including the total" << endl
            << "      memory allocated using sbrk. Start memory and size of each" << endl
            << "      node in allocMB and freeMB." << endl;
}

int main(int argc, char* argv[]) {
    /* Setting up environment */
    srand((unsigned int)time(NULL)); // Define randomness
    list<MemoryNode> alloc_mb;
    list<MemoryNode> free_mb;

    /* Check valid usage */
    if (argc != 4) {
        print_usage(argv[0]);
        return EINVAL;
    }

    ifstream input_file (argv[2]);
    ofstream output_file;
    string   strategy   (argv[1]);

    if ((strategy != "first_fit") && (strategy != "best_fit") &&
        (strategy != "worst_fit")) {
        cout << ERROR::INVALID_STRAT << endl;
        print_usage(argv[0]);
        return EINVAL;
    }

    /* Allocate line by line from <input_file>*/
    Allocator *allocator = new Allocator(&alloc_mb, &free_mb, strategy); 

    int count = 0;
    string line;
    if (input_file.is_open()) {
        while (getline(input_file, line)) {
            string name;
            stringstream(line) >> name;
            allocator->alloc(name);
            count++;
            if (count > 1000) {
                /* If exceeds 1000, remove 500 (as the specs) */
                int to_remove = 500;
                while(to_remove > 0) {
                    allocator->random_dealloc();
                    to_remove--;
                }
                count = 0;
            }
        }
    } else {
        cout << ERROR::FILE_NOT_FOUND << endl;
        return ENOENT;
    }

    input_file.close();

    /* Write output into <output_file> */
    output_file.open(argv[3]);
    output_file << "Total SBRK allocation: " << allocator->total_sbrk_alloc << endl;
    list<MemoryNode>::iterator iter_alloc = alloc_mb.begin();
    list<MemoryNode>::iterator iter_free  = free_mb.begin();

    output_file << "------ALLOC LIST------" << endl;
    while (iter_alloc != alloc_mb.end()) {
        output_file<< iter_alloc->start_memory << "\t" << iter_alloc->size << endl;
        iter_alloc++;
    }
    output_file << endl;

    output_file << "------FREE LIST------" << endl;
    while (iter_free != free_mb.end()) {
        output_file<< iter_free->start_memory << "\t" << iter_free->size << endl;
        iter_free++;
    }

    /* Complete. Finish the script */
    cout << "Completed. Please check " << argv[3] << " for details." << endl;
    return 0;
}