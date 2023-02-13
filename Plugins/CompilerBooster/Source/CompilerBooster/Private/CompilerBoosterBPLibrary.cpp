// Corentin GUILLAUME 2020  All Rights Reserved.

#include "CompilerBoosterBPLibrary.h"
#include "Windows/MinWindows.h"
#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <sstream>
#include <string>
#include "Containers/Array.h"
#include "GenericPlatform/GenericPlatformProcess.h"


void UCompilerBoosterBPLibrary::ProcessPriority(const int P1PrioIndex, const FString P1Name, const int P2PrioIndex, const FString P2Name, const bool P1SetPrio, const bool P2SetPrio, bool &ProcessFound)
{
	PROCESSENTRY32 entrySet;
	HANDLE snapshotSet;
	HANDLE hProcessSet;
	int ProcessNum;

	entrySet.dwSize = sizeof(PROCESSENTRY32);
	snapshotSet = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	ProcessNum = 0;

	std::wstring wideusernameP1;
	for (int i = 0; i < P1Name.Len(); ++i)
		wideusernameP1 += wchar_t(P1Name[i]);
	const wchar_t* your_resultP1 = wideusernameP1.c_str();

	std::wstring wideusernameP2;
	for (int i = 0; i < P2Name.Len(); ++i)
		wideusernameP2 += wchar_t(P2Name[i]);
	const wchar_t* your_resultP2 = wideusernameP2.c_str();

	if (Process32First(snapshotSet, &entrySet) == TRUE)
	{
		while (Process32Next(snapshotSet, &entrySet) == TRUE)
		{
			if (_tcsicmp(entrySet.szExeFile, (your_resultP1)) == 0 && P1SetPrio)
			{
				hProcessSet = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entrySet.th32ProcessID);

				if (P1PrioIndex == 0) {
					SetPriorityClass(hProcessSet, BELOW_NORMAL_PRIORITY_CLASS);
				}
				if (P1PrioIndex == 1) {
					SetPriorityClass(hProcessSet, NORMAL_PRIORITY_CLASS);
				}
				if (P1PrioIndex == 2) {
					SetPriorityClass(hProcessSet, ABOVE_NORMAL_PRIORITY_CLASS);
				}
				if (P1PrioIndex == 3) {
					SetPriorityClass(hProcessSet, HIGH_PRIORITY_CLASS);
				}
				//std::cin.get(); // Make the process stay open
				CloseHandle(hProcessSet);
				ProcessNum = ProcessNum + 1;
			}
			if (_tcsicmp(entrySet.szExeFile, (your_resultP2)) == 0 && P2SetPrio)
			{
				hProcessSet = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entrySet.th32ProcessID);

				if (P2PrioIndex == 0) {
					SetPriorityClass(hProcessSet, BELOW_NORMAL_PRIORITY_CLASS);
				}
				if (P2PrioIndex == 1) {
					SetPriorityClass(hProcessSet, NORMAL_PRIORITY_CLASS);
				}
				if (P2PrioIndex == 2) {
					SetPriorityClass(hProcessSet, ABOVE_NORMAL_PRIORITY_CLASS);
				}
				if (P2PrioIndex == 3) {
					SetPriorityClass(hProcessSet, HIGH_PRIORITY_CLASS);
				}
				//std::cin.get(); // Make the process stay open
				CloseHandle(hProcessSet);
				ProcessNum = ProcessNum + 1;
			}
		}
	}
	CloseHandle(snapshotSet);
	if (ProcessNum > 0) {
		ProcessFound = true;
	}
	else {
		ProcessFound = false;
	}
}

void UCompilerBoosterBPLibrary::IsShaderCompiling(bool &P1Found, bool &P2Found, const FString P1Name, const FString P2Name, FString &P1Prio, FString &P2Prio)
{
	PROCESSENTRY32 entry;
	HANDLE snapshot;
	HANDLE hProcess;
	int P1Num;
	int P2Num;
	DWORD CurrentPrio1;
	DWORD CurrentPrio2;

	std::wstring wideusername1;
	for (int i = 0; i < P1Name.Len(); ++i)
		wideusername1 += wchar_t(P1Name[i]);
	const wchar_t* your_result1 = wideusername1.c_str();

	std::wstring wideusername2;
	for (int i = 0; i < P2Name.Len(); ++i)
		wideusername2 += wchar_t(P2Name[i]);
	const wchar_t* your_result2 = wideusername2.c_str();



	entry.dwSize = sizeof(PROCESSENTRY32);
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	P1Num = 0;
	P2Num = 0;
	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (_tcsicmp(entry.szExeFile, (your_result1)) == 0)
			{
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				CurrentPrio1 = GetPriorityClass(hProcess);
				std::ostringstream stream;
				stream << CurrentPrio1;
				std::string str = stream.str();
				FString TempString(str.c_str());
				P1Prio = TempString;
				P1Num = P1Num + 1;
				//CloseHandle(hProcess);
			}
			if (_tcsicmp(entry.szExeFile, (your_result2)) == 0)
			{
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				CurrentPrio2 = GetPriorityClass(hProcess);
				std::ostringstream stream2;
				stream2 << CurrentPrio2;
				std::string str2 = stream2.str();
				FString TempString2(str2.c_str());
				P2Prio = TempString2;
				P2Num = P2Num + 1;
				//CloseHandle(hProcess);
			}
			
		}
	}
	//CloseHandle(snapshot);
	if (P1Num > 0) {
		P1Found = true;
	}
	else {
		P1Found = false;
	}
	if (P2Num > 0) {
		P2Found = true;
	}
	else {
		P2Found = false;
	}
}
