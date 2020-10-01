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

class LinkedList {
    public:
        MemoryNode* start;
};