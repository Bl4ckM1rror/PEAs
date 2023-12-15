# PEAs (Process Enumeration Alternatives)

Process Enumeration alternatives that avoid the use of `CreateToolhelp32Snapshot`, `Process32First`, `Process32Next` WINAPIs to enumerate active processes on windows. 

## Purpose

This repository shows undocumented native APIs to get the list of running processes without following the classic flow explained below:

* Take a snapshot of the current list of Windows processes using `CreateToolhelp32Snapshot`
* Get the first process in the snapshot list using `Process32First`
* Iterate through the subsequent processes using `Process32Next` until the error ERROR_NO_MORE_FILES is received, which is the signal to end the process enumeration operation
