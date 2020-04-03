/* This program was run on Visual Studio Community 2017
*/
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <strsafe.h>

#define EXTENDEDPATH 1024

void DumpModule();
void DumpEnvStrings();
void DumpCommandLine();

extern "C" const IMAGE_DOS_HEADER __ImageBase;

void DumpModule() {
	HMODULE hModule = NULL;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)DumpModule,
		&hModule);
	_tprintf(TEXT("Using GetModuleHandleEx for DumpModule = 0x%x\r\n"), hModule);
}

void DumpEnvStrings() {
	PTSTR pEnvBlock = GetEnvironmentStrings();
	TCHAR szName[MAX_PATH];
	TCHAR szValue[MAX_PATH];
	PTSTR pszCurrent = pEnvBlock;
	HRESULT hr = S_OK;
	PCTSTR pszPos = NULL;
	int currentIndex = 0;

	while (pszCurrent != NULL) {
		// Skip the meaningless strings like:
		// "=::=::\"
		if (*pszCurrent != TEXT('=')) {
			// Look for '=' separator.
			pszPos = _tcschr(pszCurrent, TEXT('='));
			// Point now to the first character of the value.
			pszPos++;
			// Copy the variable name.
			size_t cbNameLength = // Without the' ='
				(size_t)pszPos - (size_t)pszCurrent - sizeof(TCHAR);
			hr = StringCbCopyN(szName, MAX_PATH, pszCurrent, cbNameLength);
			if (FAILED(hr)) {
				printf("[%u] ERROR/r/n", currentIndex);
				break;
			}
			// Copy the variable value with the last NULL character
			// and allow truncation because this is for UI only.
			hr = StringCchCopyN(szValue, MAX_PATH, pszPos, _tcslen(pszPos) + 1);
			if (SUCCEEDED(hr)) {
				_tprintf(TEXT("[%u] %s=%s\r\n"), currentIndex, szName, szValue);
			}
			else // something wrong happened; check for truncation.
				if (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
					_tprintf(TEXT("[%u] %s=%s...\r\n"), currentIndex, szName, szValue);
				}
				else { // This should never occur.
					_tprintf(
						TEXT("[%u] %s=???\r\n"), currentIndex, szName
					);
					break;
				}
		}
		else {
			_tprintf(TEXT("[%u] %s\r\n"), currentIndex, pszCurrent);
		}
		// Next variable please.
		currentIndex++;
		// Move to the end of the string.
		while (*pszCurrent != TEXT('\0'))
			pszCurrent++;
		pszCurrent++;
		// Check if it was not the last string.
		if (*pszCurrent == TEXT('\0'))
			break;
	};
	// Don't forget to free the memory.
	FreeEnvironmentStrings(pEnvBlock);
	printf("MAX_PATH = %d\r\n", MAX_PATH);
}

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


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, 
//      or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
