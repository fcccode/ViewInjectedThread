// injectview.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/*
FindInjectedThread

Loops through the threads of the process in question and exaimines the security
attributes of them

*/
void FindInjectedThread(DWORD Pid) {
	HANDLE ThreadSnap = NULL;
	THREADENTRY32 te32 = { 0 };
	te32.dwSize = sizeof(THREADENTRY32);
	HANDLE ThreadHandle;
	DWORD StartAddress;

	//Get address of NtQueryInformationThread so we can use it
	NtQueryThreadPointer NtQueryInformationThread = (NtQueryThreadPointer)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryInformationThread");

	//create a snapshot of the threads
	ThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (!Thread32First(ThreadSnap, &te32))
	{
		std::wcout << L"Error getting thread snapshot " << GetLastError() << std::endl;
		CloseHandle(ThreadSnap);     
		return;
	}

	std::wcout << L"Thread ID" << std::setw(Padding) << L"Start Address" << std::endl;
	std::wcout << L"-----------------------------------------------------------------" << std::endl;

	//loop through them and perform actions on them
	do {
		
		if (te32.th32OwnerProcessID == Pid) {
			
			//get a handle to the thread
			ThreadHandle = OpenThread(THREAD_ALL_ACCESS, NULL, te32.th32ThreadID);
			
			StartAddress = 0;

			//get start address information on thread via NtQueryInformationThread
			DWORD NtStatus = NtQueryInformationThread(ThreadHandle, ThreadQuerySetWin32StartAddress, &StartAddress, sizeof(DWORD_PTR), NULL);

			//checking
			std::wcout << te32.th32ThreadID << std::setw(Padding) << StartAddress << std::endl;
		}

	} while (Thread32Next(ThreadSnap, &te32));

	CloseHandle(ThreadSnap);

}

/*
Main function

Takes command line arguments in as the process to be inspected.
*/
int wmain(int argc, wchar_t **argv)
{
	
	if (argc == 1 || argc > 3) {
		std::wcout << L"Usage: " << argv[0] << L" PID" << std::endl;
		return 0;
	}
	
	DWORD Pid = wcstod(argv[1], _T('\0'));

	FindInjectedThread(Pid);

	return 0;

}

