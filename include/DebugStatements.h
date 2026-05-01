#pragma once

#ifdef _DEBUG
#define PrintDebug(...) printf(__VA_ARGS__); fflush(stdout)
#define DoDebug(...)\
    __VA_ARGS__;

#else
#define PrintDebug(...)
#define DoDebug(...)
#endif
