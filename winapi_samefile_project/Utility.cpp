#include <sstream>
#include <string>
#include <windows.h>
#include <strsafe.h>
#include <ShlObj.h>
#include <vector>
#include <iomanip>  
#include "FileDetail.h"




LPWSTR forlvTabs(const wchar_t tabsName[256])
{
	LPWSTR lpwstrName = new wchar_t[256];
	wcsncpy_s(lpwstrName, 256, tabsName, _TRUNCATE);
	return lpwstrName;
}

void InitializeListViewColumns(HWND hListBox) {

	LVCOLUMN lvColumn = { 0 };
	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	ListView_SetExtendedListViewStyle(hListBox, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);


	lvColumn.pszText = forlvTabs(L"Name");
	lvColumn.cx = 200;
	ListView_InsertColumn(hListBox, 0, &lvColumn);


	lvColumn.pszText = forlvTabs(L"Created At");;
	lvColumn.cx = 130;
	ListView_InsertColumn(hListBox, 1, &lvColumn);

	lvColumn.pszText = forlvTabs(L"Last Changed At");;
	lvColumn.cx = 130;
	ListView_InsertColumn(hListBox, 2, &lvColumn);

	lvColumn.pszText = forlvTabs(L"Byte");;
	lvColumn.cx = 75;
	ListView_InsertColumn(hListBox, 3, &lvColumn);

	lvColumn.pszText = forlvTabs(L"Size");
	lvColumn.cx = 75;
	ListView_InsertColumn(hListBox, 4, &lvColumn);

	lvColumn.pszText = forlvTabs(L"Adress");
	lvColumn.cx = 250;
	ListView_InsertColumn(hListBox, 5, &lvColumn);

}
void CenterWindow(HWND hWnd) {
	RECT screenRect;
	HWND desktop = GetDesktopWindow();
	GetWindowRect(desktop, &screenRect);
	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);
	int screenWidth = screenRect.right - screenRect.left;
	int screenHeight = screenRect.bottom - screenRect.top;
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;
	int xPos = (screenWidth - windowWidth) / 2;
	int yPos = (screenHeight - windowHeight) / 2;
	SetWindowPos(hWnd, NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

bool IsValidLocalPath(const wchar_t* path) {
	DWORD attributes = GetFileAttributesW(path);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

LPWSTR wStringToLPWSTR(const std::wstring& str) {
	LPWSTR buffer = new WCHAR[str.length() + 1];
	wcscpy_s(buffer, str.length() + 1, str.c_str());
	return buffer;
}
void GetFilesInDirectory(std::vector<FileDetail>& out,std::wstring& directory)
{
	WIN32_FIND_DATA file_data;
	std::wstring searchPath = directory + L"\\*";  // Construct search path
	HANDLE dir = FindFirstFile(searchPath.c_str(), &file_data);

	if (dir == INVALID_HANDLE_VALUE)
		return;  // No files found or invalid directory

	// Get the full path of the Desktop folder (for comparison)
	WCHAR desktopPath[MAX_PATH];
	bool isDesktopFolder = false;
	if (SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath) == S_OK) {
		std::wstring desktopFolderPath = desktopPath;  // Desktop folder path
		isDesktopFolder = (directory == desktopFolderPath);  // Check if current folder is Desktop
	}

	do {
		// Skip "." and ".." directories
		if (wcscmp(file_data.cFileName, L".") == 0 || wcscmp(file_data.cFileName, L"..") == 0)
			continue;

		// If we're in the Desktop folder, skip desktop.ini
		if (isDesktopFolder && wcscmp(file_data.cFileName, L"desktop.ini") == 0)
			continue;

		// If it's a directory, skip it
		if ((file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			continue;

		// Create FileDetail object and add it to the output vector
		FileDetail selectedFile(
			file_data.nFileSizeLow,
			file_data.cFileName,
			directory,
			file_data.ftCreationTime,
			file_data.ftLastWriteTime,
			0);

		out.push_back(selectedFile);

	} while (FindNextFile(dir, &file_data) != 0);
	FindClose(dir);
}

bool isWithinRange(int num, int target, int range) {
	int lowerBound = target - range;
	int upperBound = target + range;
	return (num >= lowerBound && num <= upperBound);
}
int rangeOfRange(HWND hTrackBar, HWND hRadioButton2, HWND hRadioButton3)
{
	int pos = SendMessage(hTrackBar, TBM_GETPOS, 0, 0);
	if (SendMessage(hRadioButton2, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		return pos * 1024;
	}
	else if (SendMessage(hRadioButton3, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		return pos * 1048576;
	}
	else {
		return pos;
	}

}

std::wstring floatToWString(float value) {
	std::wstringstream wss;
	wss << std::fixed << std::setprecision(2) << value;
	
	return wss.str();
}

std::wstring easyViewSizeThing(DWORD value)
{
	float floatValue = static_cast<float>(value);
	int divided = 0;
	const std::wstring units[] = {
		L"B",   // Bytes
		L"KB",  // Kilobytes
		L"MB",  // Megabytes
		L"GB",  // Gigabytes
		L"TB"   // Terabytes
	};
	const int numUnits = sizeof(units) / sizeof(units[0]);
	bool open = true;
	while (floatValue >= 1024.0f && divided < numUnits - 1) {
		floatValue /= 1024.0f;
		divided++;
	}
	std::wstring formattedValue = floatToWString(floatValue);
	return formattedValue + L" " + units[divided];
}

LPWSTR ConvertDWORDToLPWSTR(DWORD value) {
    std::wstringstream wss;
    wss << value;
    std::wstring wstr = wss.str();
    LPWSTR wideString = new wchar_t[wstr.length() + 1];
    wcscpy_s(wideString, wstr.length() + 1, wstr.c_str());
    return wideString;
}

LPWSTR FileTimeToLPWSTR(const FILETIME& ft)
{
	SYSTEMTIME stUTC, stLocal;
	WCHAR szDate[100], szTime[100];
	LPWSTR lpwstrDateTime = new WCHAR[200];

	// Convert the FILETIME to SYSTEMTIME
	FileTimeToSystemTime(&ft, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	// Get the date and time in the current locale format
	GetDateFormatW(LOCALE_USER_DEFAULT, 0, &stLocal, NULL, szDate, 100);
	GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &stLocal, NULL, szTime, 100);

	// Combine the date and time strings
	StringCchPrintfW(lpwstrDateTime, 200, L"%s %s", szDate, szTime);

	return lpwstrDateTime;
}