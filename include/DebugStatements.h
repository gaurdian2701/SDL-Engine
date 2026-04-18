#pragma once

// #define DoDebugCode(...) \
// do{ __VA_ARGS__ } while(0);
//
// #define DoDebugStatement(...)\
// __VA_ARGS__;
//
// #define PrintDebug(...) printf(__VA_ARGS__); fflush(stdout)


#ifdef _DEBUG
#define PrintDebug(...) printf(__VA_ARGS__); fflush(stdout)

#define DoDebugCode(...) \
do{ __VA_ARGS__ } while(0);

#define DoDebugStatement(...)\
    __VA_ARGS__;

#else
#define PrintDebug(...)
#define DoDebugCode(...)
#define DoDebugStatement(...)
#endif
