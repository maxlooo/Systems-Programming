/* 
This program was run on Visual Studio Community 2017
*/
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <strsafe.h>

// create longer path than MAX_PATH
#define EXTENDEDPATH 1024

void DumpModule();
void DumpEnvStrings();
void DumpCommandLine();

// get address of current module hModule
void DumpModule() {
	HMODULE hModule = NULL;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)DumpModule,
		&hModule);
	_tprintf(TEXT("Using GetModuleHandleEx for DumpModule = 0x%x\r\n"), hModule);
}

// parse and extract environment strings
void DumpEnvStrings() {
	PTSTR pEnvBlock = GetEnvironmentStrings();
	TCHAR szName[EXTENDEDPATH];
	TCHAR szValue[EXTENDEDPATH];
	PTSTR pszCurrent = pEnvBlock;
	HRESULT hr = S_OK;
	PCTSTR pszPos = NULL;
	int currentIndex = 0;

	while (pszCurrent != NULL) {
		if (*pszCurrent != TEXT('=')) {
			pszPos = _tcschr(pszCurrent, TEXT('='));
			pszPos++;
			size_t cbNameLength = (size_t)pszPos - (size_t)pszCurrent - sizeof(TCHAR);
			hr = StringCbCopyN(szName, EXTENDEDPATH, pszCurrent, cbNameLength);
			if (FAILED(hr)) {
				printf("[%u] ERROR/r/n", currentIndex);
				break;
			}
			hr = StringCchCopyN(szValue, EXTENDEDPATH, pszPos, _tcslen(pszPos) + 1);
			if (SUCCEEDED(hr)) {
				_tprintf(TEXT("[%u] %s=%s\r\n"), currentIndex, szName, szValue);
			}
			else if (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
					_tprintf(TEXT("[%u] %s=%s...\r\n"), currentIndex, szName, szValue);
				}
				else { 
					_tprintf(
						TEXT("[%u] %s=???\r\n"), currentIndex, szName
					);
					break;
				}
		}
		else {
			_tprintf(TEXT("[%u] %s\r\n"), currentIndex, pszCurrent);
		}
		currentIndex++;
		while (*pszCurrent != TEXT('\0'))
			pszCurrent++;
		pszCurrent++;
		if (*pszCurrent == TEXT('\0'))
			break;
	};
	FreeEnvironmentStrings(pEnvBlock);
	printf("MAX_PATH = %d\r\n", MAX_PATH);
}

// extract and print command line
void DumpCommandLine() {
	PTSTR CommandLine = GetCommandLine();

	_tprintf(TEXT("Command Line = %s\r\n"), CommandLine);
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[]) {
	DumpModule();
	DumpEnvStrings();
	DumpCommandLine();
	return(0);
}
