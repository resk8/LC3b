/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define TRUE  1
#define FALSE 0

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

#ifdef __linux__ 
    #include <cstdlib>
    inline void Exit() { exit(-1); }
#elif _WIN32
    __attribute__((noreturn))
    inline void Exit() { system("pause"); exit(-1); }
#endif