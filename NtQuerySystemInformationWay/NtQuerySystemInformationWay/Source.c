#include <windows.h>
#include <stdio.h>

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING* PUNICODE_STRING;
typedef const UNICODE_STRING* PCUNICODE_STRING;

typedef LONG KPRIORITY;

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID;

typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    BYTE Reserved1[48];
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    PVOID Reserved2;
    ULONG HandleCount;
    ULONG SessionId;
    PVOID Reserved3;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG Reserved4;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    PVOID Reserved5;
    SIZE_T QuotaPagedPoolUsage;
    PVOID Reserved6;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER Reserved7[6];
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
    SystemPerformanceInformation = 2,
    SystemTimeOfDayInformation = 3,
    SystemProcessInformation = 5,
    SystemProcessorPerformanceInformation = 8,
    SystemInterruptInformation = 23,
    SystemExceptionInformation = 33,
    SystemRegistryQuotaInformation = 37,
    SystemLookasideInformation = 45,
    SystemCodeIntegrityInformation = 103,
    SystemPolicyInformation = 134,
} SYSTEM_INFORMATION_CLASS;

typedef NTSTATUS(NTAPI* _NtQuerySystemInformation)(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID                    SystemInformation,
	ULONG                    SystemInformationLength,
	PULONG                   ReturnLength
	);

BOOL FindTarget(LPCWSTR szProcName, DWORD* pdwPid) {

    BOOL                         found = FALSE;
    _NtQuerySystemInformation    myNtQuerySystemInformation = NULL;
    ULONG                        uReturnLen1 = NULL, uReturnLen2 = NULL;
    PSYSTEM_PROCESS_INFORMATION  SystemProcInfo = NULL;
    NTSTATUS                     STATUS = NULL;
    PVOID                        pValueToFree = NULL;

    myNtQuerySystemInformation = (_NtQuerySystemInformation)GetProcAddress(GetModuleHandleA("NTDLL.DLL"), "NtQuerySystemInformation");
    if (myNtQuerySystemInformation == NULL) {
        printf("[!] GetProcAddress Failed With Error : %d\n", GetLastError());
        return FALSE;
    }

    myNtQuerySystemInformation(SystemProcessInformation, NULL, NULL, &uReturnLen1);

    SystemProcInfo = (PSYSTEM_PROCESS_INFORMATION)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)uReturnLen1);
    if (SystemProcInfo == NULL) {
        printf("[!] HeapAlloc Failed With Error : %d\n", GetLastError());
        return FALSE;
    }

    pValueToFree = SystemProcInfo;

    STATUS = myNtQuerySystemInformation(SystemProcessInformation, SystemProcInfo, uReturnLen1, &uReturnLen2);
    if (STATUS != 0x0) {
        printf("[!] NtQuerySystemInformation Failed With Error : 0x%0.8X \n", STATUS);
        return FALSE;
    }

    while (TRUE) {

        // Comparing process name to the intended target process
        if (SystemProcInfo->ImageName.Length && wcscmp(SystemProcInfo->ImageName.Buffer, szProcName) == 0) {
                *pdwPid = (DWORD)SystemProcInfo->UniqueProcessId;
                found = TRUE;
                break;
        }

        // If NextEntryOffset is 0, we reached the end of the array
        if (!SystemProcInfo->NextEntryOffset)
            break;

        // Move to the next element
        SystemProcInfo = (PSYSTEM_PROCESS_INFORMATION)((ULONG_PTR)SystemProcInfo + SystemProcInfo->NextEntryOffset);
    }

    HeapFree(GetProcessHeap(), 0, pValueToFree);

    return found;
}

int main() {

    LPCWSTR szProcessName = L"notepad.exe";
    DWORD pdwPid = NULL;
    if (FindTarget(szProcessName, &pdwPid))
        wprintf(L"[+] FOUND: process %s with PID: %d\n", szProcessName, pdwPid);
    else
        wprintf(L"[-] NOT FOUND process %s \n", szProcessName);

    return 0;
}