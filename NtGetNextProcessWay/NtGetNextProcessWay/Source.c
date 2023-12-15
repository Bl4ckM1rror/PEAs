#include <windows.h>
#include <psapi.h>
#include <strsafe.h>
#include <shlwapi.h>
#include "noCRT.h"

#pragma comment(lib, "shlwapi.lib")

typedef NTSTATUS(NTAPI* _NtGetNextProcess)(
    HANDLE ProcessHandle,
    ACCESS_MASK DesiredAccess,
    ULONG HandleAttributes,
    ULONG Flags,
    PHANDLE NewProcessHandle
    );

BOOL FindTarget(LPWSTR szProcessName, DWORD* pdwPid) {
    BOOL found = FALSE;
    HANDLE current = NULL;
    WCHAR procName[MAX_PATH];

    _NtGetNextProcess myNtGetNextProcess = (_NtGetNextProcess)GetProcAddress(GetModuleHandleA("NTDLL.DLL"), "NtGetNextProcess");

    while (!myNtGetNextProcess(current, MAXIMUM_ALLOWED, 0, 0, &current)) {
       
        RtlSecureZeroMemory(procName, sizeof(procName));
        if (GetProcessImageFileNameW(current, procName, MAX_PATH)) {
            WCHAR LowerName[MAX_PATH];
            DWORD dwSize = lstrlenW(PathFindFileNameW(procName));
            DWORD j = 0;

            if (dwSize < MAX_PATH) {
                for (; j < dwSize; j++) {
                    LowerName[j] = (WCHAR)tolower(PathFindFileNameW(procName)[j]);
                }
                LowerName[j] = L'\0';

                if (lstrcmpiW(szProcessName, LowerName) == 0) {
                    *pdwPid = GetProcessId(current);
                    found = TRUE;
                    break;
                }
            }
        }
    }
    return found;
}

int main() {

    LPWSTR szProcessName = L"notepad.exe";
    DWORD pdwPid = 0;
    if (FindTarget(szProcessName, &pdwPid)) {
        PRINTW(L"[+] FOUND: process %s with PID: %d\n", szProcessName, pdwPid);
    }
    else {
        PRINTW(L"[-] NOT FOUND process %s \n", szProcessName);
    }

    return 0;
}
