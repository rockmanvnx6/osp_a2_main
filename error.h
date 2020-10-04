
using namespace std;

namespace ERROR {
    const string FILE_NOT_FOUND   = "Input file not found.";
    const string MEMORY_ERROR     = "MEMORRY ERROR - Insufficient memory.";
    const string INVALID_STRAT    = "Invalid name of strategy.";
    void  *SBRK_MEMORY_ERROR      = (void *) -1;
    void  *INVALID_STRATEGY_ERROR = (void *) -2;
}