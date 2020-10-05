using namespace std;

namespace ERROR {
    /**
     * File: error.h
     * Author: s3619352@student.rmit.edu.au
     * 
     * Header file defines constant error code(s) and error message(s).
     * 
     * For SBRK_MEMORY_ERROR, according to the documentation, sbrk()
     * will return (void *) -1 if it fails to assign.
     * 
     * Reference: https://linux.die.net/man/2/sbrk
     */
    const string FILE_NOT_FOUND   = "Input file not found.";
    const string MEMORY_ERROR     = "MEMORY ERROR - Insufficient memory.";
    const string INVALID_STRAT    = "Invalid name of strategy.";
    void  *SBRK_MEMORY_ERROR      = (void *) -1;
}