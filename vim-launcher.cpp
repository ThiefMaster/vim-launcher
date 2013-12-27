#include <Windows.h>
#include <Shlwapi.h>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

static void Fail(LPCSTR title) {
	LPSTR error;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&error, 0, NULL);
	MessageBox(NULL, error, title, MB_OK | MB_ICONERROR);
	LocalFree(error);
	ExitProcess(EXIT_FAILURE);
}

static LPSTR GetExeName() {
	static CHAR buf[MAX_PATH + 1];
	if (GetModuleFileName(NULL, buf, sizeof(buf)) == 0) {
		Fail("GetModuleFileName");
	}
	CHAR *pos;
	if ((pos = strrchr(buf, '\\'))) {
		memmove(buf, pos + 1, strlen(pos + 1) + 1);
	}
	return buf;
}

static LPSTR GetExeFolder() {
	static CHAR buf[MAX_PATH + 1]; 
	if (GetModuleFileName(NULL, buf, sizeof(buf)) == 0) {
		Fail("GetModuleFileName");
	}
	PathRemoveFileSpec(buf);
	return buf;
}

static bool sort_vim_folders(std::string &a, std::string &b)
{
	// "vimXY"
	return atoi(a.substr(3).c_str()) > atoi(b.substr(3).c_str());
}

static LPSTR FindProgram() {
	static CHAR result[MAX_PATH + 1];

	CHAR path[MAX_PATH + 1];
	LPSTR exefolder = GetExeFolder();
	if (PathCombine(path, exefolder, "vim??") == NULL) {
		Fail("PathCombine");
	}

	WIN32_FIND_DATA fdd;
	HANDLE find;
	if ((find = FindFirstFile(path, &fdd)) == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			return NULL;
		}
		Fail("FindFirstFile");
	}

	std::vector<std::string> candidates;
	do {
		candidates.push_back(std::string(fdd.cFileName));
	} while(FindNextFile(find, &fdd));

	if (GetLastError() != ERROR_NO_MORE_FILES) {
		Fail("FindNextFile");
	}
	FindClose(find);

	std::sort(candidates.begin(), candidates.end(), sort_vim_folders);
	if (PathCombine(result, exefolder, candidates[0].c_str()) == NULL) {
		Fail("PathCombine");
	}
	if (PathAppend(result, GetExeName()) == NULL) {
		Fail("PathAppend");
	}
	return result;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// Let windows think we started properly so it doesn't show the appstarting cursor
	MSG msg;
	PostMessage(NULL, WM_NULL, 0, 0);
	GetMessage(&msg, NULL, 0, 0);

	// Find vim
	LPSTR program = FindProgram();
	if (program == NULL) {
		MessageBox(NULL, "Could not find a vimXY folder", "Error", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}

	// Create a properly quoted command line
	PathQuoteSpaces(program);
	size_t bufsize = strlen(program) + 1 + strlen(lpCmdLine) + 1;
	LPSTR buf = new CHAR[bufsize];
	strcpy_s(buf, bufsize, program);
	if (*lpCmdLine) {
		strcat_s(buf, bufsize, " ");
		strcat_s(buf, bufsize, lpCmdLine);
	}

	// Run Vim
	STARTUPINFO sinfo = {0};
	PROCESS_INFORMATION pinfo = {0};
	if (!CreateProcess(program, buf, NULL, NULL, FALSE, 0, NULL, NULL, &sinfo, &pinfo)) {
		Fail("Could not launch Vim");
	}
	delete[] buf;

	// Wait for it to exit
	if (WaitForSingleObject(pinfo.hProcess, INFINITE) == WAIT_FAILED) {
		Fail("WaitForSingleObject");
	}
	DWORD exitcode;
	if (!GetExitCodeProcess(pinfo.hProcess, &exitcode)) {
		Fail("GetExitCodeProcess");
	}
	CloseHandle(pinfo.hProcess);
	CloseHandle(pinfo.hThread);
	return exitcode;
}