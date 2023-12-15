#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#include <wchar.h>
#include <stdbool.h>

BOOL FindTarget(LPWSTR szProcessName, DWORD* pdwPid) {

    BOOL found = FALSE;
    DWORD adwProcesses[1024 * 2], dwReturnLen1 = NULL, dwReturnLen2 = NULL, dwNmbrOfPids = NULL;
    HANDLE hProcess = NULL;
    HMODULE hModule = NULL;
    WCHAR szProc[MAX_PATH];

    // PIDs array grabbing
    if (!EnumProcesses(adwProcesses, sizeof(adwProcesses), &dwReturnLen1)) {
        wprintf(L"[!] EnumProcesses Failed With Error : %d \n", GetLastError());
        return FALSE;
    }

    //number of elements in the array
    dwNmbrOfPids = dwReturnLen1 / sizeof(DWORD);

    wprintf(L"[i] Number Of Processes Detected : %d \n", dwNmbrOfPids);

    for (int i = 0; i < dwNmbrOfPids; i++) {

        if (adwProcesses[i] != NULL) {

            // Open a process handle
            if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, adwProcesses[i])) != NULL) {

                // The module handle is needed for 'GetModuleBaseName'
                if (!EnumProcessModules(hProcess, &hModule, sizeof(HMODULE), &dwReturnLen2)) {
                    continue;
                }
                else {
                    // Get the name of the process
                    if (!GetModuleBaseName(hProcess, hModule, szProc, sizeof(szProc) / sizeof(WCHAR))) {
                        wprintf(L"[!] GetModuleBaseName Failed [ At Pid: %d ] With Error : %d \n", adwProcesses[i], GetLastError());
                    }
                    else {

                        // Convert the process name to lowercase
                        _wcslwr_s(szProc, MAX_PATH);

                        // check if process name matches the process we're looking for
                        if (wcscmp(szProc, szProcessName) == 0) {
                            *pdwPid = adwProcesses[i];
                            found = TRUE;
                            break;
                        }
                    }
                }

                CloseHandle(hProcess);
            }
        }

        // Iterate through the PIDs array
    }

    return found;
}

int main() {

    LPWSTR szProcessName = L"notepad.exe";
    DWORD pdwPid = NULL;
    if (FindTarget(szProcessName, &pdwPid))
        wprintf(L"[+] FOUND: process %s with PID: %d\n", szProcessName, pdwPid);
    else
        wprintf(L"[-] NOT FOUND process %s \n", szProcessName);

    return 0;
}
