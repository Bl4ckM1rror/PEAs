#include <Windows.h>

#define PRINTW(STR, ...)                                                                      \
    if (1) {                                                                                  \
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);                                     \
        if (hConsole != INVALID_HANDLE_VALUE) {                                                \
            LPWSTR buf = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1024);          \
            if (buf != NULL) {                                                                 \
                int len = swprintf_s(buf, 1024, STR, __VA_ARGS__);                             \
                WriteConsoleW(hConsole, buf, len, NULL, NULL);                                  \
                HeapFree(GetProcessHeap(), 0, buf);                                            \
            }                                                                                 \
        }                                                                                     \
    }

#define PRINTA(STR, ...)                                                                      \
    if (1) {                                                                                  \
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);                                     \
        if (hConsole != INVALID_HANDLE_VALUE) {                                                \
            LPSTR buf = (LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1024);            \
            if (buf != NULL) {                                                                 \
                int len = sprintf_s(buf, 1024, STR, __VA_ARGS__);                              \
                WriteConsoleA(hConsole, buf, len, NULL, NULL);                                  \
                HeapFree(GetProcessHeap(), 0, buf);                                            \
            }                                                                                 \
        }                                                                                     \
    }
