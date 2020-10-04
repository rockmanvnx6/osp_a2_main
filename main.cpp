#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include "alloc.h"

using namespace std;

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));
    string line;

    list<MemoryNode> alloc_mb;
    list<MemoryNode> free_mb;

    Allocator *allocator = new Allocator(&alloc_mb, &free_mb, "best_fit");
    
    if (argc != 2) {
        cout << ERROR::NO_INPUT << endl;
        cout << "USAGE: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    ifstream file (argv[1]);
    int count = 0;
    if (file.is_open()) {
        while (getline(file, line)) {
            string name;
            stringstream(line) >> name;
            cout << "ADDING " << name << endl;
            allocator->alloc(name);
            count++;
            if (count > 1000) {
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
    }

    file.close();

    allocator->print_alloc();

    return 0;
}