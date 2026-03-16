#pragma once

#ifdef _DEBUG
#define PrintDebug(...) printf(__VA_ARGS__); fflush(stdout)
#else
#define PrintDebug(...) (void(0))
#endif
