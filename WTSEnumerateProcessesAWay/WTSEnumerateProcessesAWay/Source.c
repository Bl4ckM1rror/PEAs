#include <windows.h>
#include <strsafe.h>
#include <wtsapi32.h>
#include "noCRT.h"

#pragma comment(lib, "wtsapi32.lib")

int FindTarget(LPCWSTR szProcessName, DWORD* pdwPid) {
    BOOL found = FALSE;
    WTS_PROCESS_INFOW* pi;

    DWORD level = 1;
    DWORD count = 0;

    if (!WTSEnumerateProcessesW(WTS_CURRENT_SERVER_HANDLE, 0, level, (LPWSTR*)&pi, &count)) {
        return 0;
    }

    for (int i = 0; i < count; i++) {
        WCHAR LowerName[MAX_PATH * 2];
        DWORD dwSize = lstrlenW(pi[i].pProcessName);
        DWORD j = 0;

        if (dwSize < MAX_PATH * 2) {
            for (; j < dwSize; j++) {
                LowerName[j] = (WCHAR)tolower(pi[i].pProcessName[j]);
            }
            LowerName[j] = L'\0';
        }

        if (lstrcmpiW(szProcessName, LowerName) == 0) {
            *pdwPid = pi[i].ProcessId;
            found = TRUE;
            break;
        }
    }

    WTSFreeMemory(pi);
    return found;
}

int main()
{
    LPCWSTR szProcessName = L"notepad.exe";
    DWORD pdwPid = 0;
    if (FindTarget(szProcessName, &pdwPid)) {
        PRINTW(L"[+] FOUND: process %s with PID: %d\n", szProcessName, pdwPid);
    }
    else {
        PRINTW(L"[-] NOT FOUND process %s \n", szProcessName);
    }

    return 0;
}