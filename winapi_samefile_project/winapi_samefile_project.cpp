#pragma region MyRegionField
#include <Windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <shlobj.h>
#include <stdio.h>
#include "FileDetail.h"
#include "resource1.h"
#include <sstream>
#include <codecvt>
#include <commctrl.h>
#include <thread>
#include <Shobjidl.h> // For ITaskbarList3
#include <map>
#include <atomic>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <richedit.h>
#pragma endregion

#pragma region fromOtherClasses

#pragma comment(lib, "comctl32.lib")

extern LPWSTR ConvertDWORDToLPWSTR(DWORD value);
extern std::wstring floatToWString(float value);
extern LPWSTR wStringToLPWSTR(const std::wstring& str);
extern LPWSTR FileTimeToLPWSTR(const FILETIME& ft);
extern bool isWithinRange(int num, int target, int range);
extern int rangeOfRange(HWND hTrackBar, HWND hRadioButton2, HWND hRadioButton3);
extern void GetFilesInDirectory(std::vector<FileDetail>& out,std::wstring& directory);
extern LPWSTR forlvTabs(const wchar_t tabsName[256]);
extern std::wstring easyViewSizeThing(DWORD value);
extern bool IsValidLocalPath(const wchar_t* path);
extern void InitializeListViewColumns(HWND hListBox);
extern void CenterWindow(HWND hWnd);

#pragma endregion

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wPAram, LPARAM lParam);

#pragma region defineGlobalObjects
HWND hMainWindow;
HINSTANCE hMainInstance;
HWND hTxtInput;
HWND hButton;
HWND hButtonRepeat;
HWND hListView;
HWND hStatic;
HWND hDinamicLbl;
HWND hTrackBar;
HWND hRadioButton1;
HWND hRadioButton2;
HWND hRadioButton3;
HWND hTreeView;
HWND hButtonRefresh;
HWND hStopButton;
HWND hProgressBar;
HWND hButtonInfo;
HWND hTabControl, hStatic1, hStatic2, hStatic3, hPopup;
HINSTANCE hInst;


#define IDC_TEXTBOX 1000
#define IDC_BUTTON 1001
#define IDC_STATIC 1002
#define IDC_LISTVIEW 1003
#define IDC_LABEL 1004
#define IDC_DLABEL 1005
#define IDC_TBAR 1006
#define IDC_RBUTTON1 1007
#define IDC_RBUTTON2 1008
#define IDC_RBUTTON3 1009
#define IDC_STATICFC 1010
#define IDC_TREEVIEW 1012
#define IDC_BUTTONRPT 1013
#define IDC_BUTTONLOAD 1014
#define ID_CONTEXT_MENU_ITEM1  1015
#define ID_CONTEXT_MENU_ITEM2  1016
#define ID_CONTEXT_MENU_LITEM1  1017
#define ID_CONTEXT_MENU_LITEM2  1018
#define ID_CONTEXT_MENU_OPEN_FILE 1019
#define IDC_STOPBUTTON 1020
#define IDC_PROGRESSBAR 1021
#define IDC_BUTTONINFO 1022

#define WM_UPDATE_PROGRESS (WM_USER + 1)




//#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
using namespace std;


std::map<int, std::vector<FileDetail>> indexPurposes;
int index = 0;
int iterationOFLB = 0;
HIMAGELIST hImageList = nullptr;
int howmany = 0;
int indexForMenu2 = -1;
std::vector<int> selectedIndexes;
std::atomic<bool> isTaskRunning = false;
std::atomic<bool> stopFlag{ false };
#pragma endregion

#pragma region forUtility

void AddItemsToTreeView(HWND hTreeView) {
	TVINSERTSTRUCT tvis = { 0 };
	tvis.hInsertAfter = TVI_LAST;
	int showedFile = -1;
	int index = 0;

	for (int i = 0; i < indexPurposes.size(); ++i) {
			showedFile++;

			std::wstring buffer = L"(" + std::to_wstring(indexPurposes[i].size()) + L") " + indexPurposes[i][0].getFName();
			LPWSTR lpwstrBuffer = const_cast<LPWSTR>(buffer.c_str());

			tvis.hParent = TVI_ROOT;
			tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
			tvis.item.pszText = lpwstrBuffer;
			tvis.item.lParam = index; 
			int rootIconIndex = indexPurposes[i][0].getfIconIndex();
			tvis.item.iImage = rootIconIndex;
			tvis.item.iSelectedImage = rootIconIndex;
			HTREEITEM hRoot = TreeView_InsertItem(hTreeView, &tvis);
			index++;
		
	}
}
void AddItemsToListView(int iterationgNumber) {
	LVITEM lvI = { 0 };
	lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_IMAGE;
	ListView_SetImageList(hListView, hImageList, LVSIL_SMALL);
	for (size_t i = 0; i < indexPurposes[iterationgNumber].size(); ++i) {
		lvI.iSubItem = 0;
		lvI.cchTextMax = 40;
		lvI.state = INDEXTOSTATEIMAGEMASK(1);
		lvI.stateMask = LVIS_STATEIMAGEMASK | LVIS_SELECTED | LVIS_FOCUSED;

		int itemCount = ListView_GetItemCount(hListView);
		lvI.iItem = itemCount;
		lvI.pszText = wStringToLPWSTR(indexPurposes[iterationgNumber][i].getFName());
		int iconIndex = indexPurposes[iterationgNumber][i].getfIconIndex();
		lvI.iImage = iconIndex;

		if (ListView_InsertItem(hListView, &lvI) == -1) {
			MessageBox(NULL, L"Failed to insert item into ListView.", L"Error", MB_ICONERROR);
			return;
		}
		lvI.pszText = FileTimeToLPWSTR(indexPurposes[iterationgNumber][i].getFCreatedAt());
		ListView_SetItemText(hListView, itemCount, 1, lvI.pszText);

		lvI.pszText = FileTimeToLPWSTR(indexPurposes[iterationgNumber][i].getFLastChangedAt());
		ListView_SetItemText(hListView, itemCount, 2, lvI.pszText);

		lvI.pszText = ConvertDWORDToLPWSTR(indexPurposes[iterationgNumber][i].getFSize());
		ListView_SetItemText(hListView, itemCount, 3, lvI.pszText);

		std::wstring embracing = easyViewSizeThing(indexPurposes[iterationgNumber][i].getFSize());
		lvI.pszText = wStringToLPWSTR(embracing);
		ListView_SetItemText(hListView, itemCount, 4, lvI.pszText);

		lvI.pszText = wStringToLPWSTR(indexPurposes[iterationgNumber][i].getFAddress());
		ListView_SetItemText(hListView, itemCount, 5, lvI.pszText);
	}
}
void UpdateProgressBar(int progress) {
	PostMessage(hMainWindow, WM_UPDATE_PROGRESS, progress, 0);
}
void resetObjects()
{
	TreeView_DeleteAllItems(hTreeView);
	ListView_DeleteAllItems(hListView);
	isTaskRunning = true;
	stopFlag = false;
	if (!indexPurposes.empty()) {
		indexPurposes.clear();
	}
	index = 0;
	howmany = 0;
	if (hImageList) {
		ImageList_Destroy(hImageList);
		hImageList = NULL;
	}
	
}
bool DeleteFiles(HWND hWnd, LPCWSTR filePath)
{
	SHFILEOPSTRUCT shFileOp = { 0 };
	shFileOp.hwnd = hWnd;
	shFileOp.wFunc = FO_DELETE;
	shFileOp.pFrom = filePath;
	shFileOp.fFlags = FOF_ALLOWUNDO;
	return (SHFileOperation(&shFileOp) == 0);
}
#pragma endregion

#pragma region contextContent
void ShowContextMenuForListView(HWND hWnd, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	hListView = GetDlgItem(hWnd, IDC_LISTVIEW);
	if (WindowFromPoint(pt) == hListView)
	{
		selectedIndexes.clear();
		int selectedItem = -1;
		while ((selectedItem = ListView_GetNextItem(hListView, selectedItem, LVNI_SELECTED)) != -1)
		{
			selectedIndexes.push_back(selectedItem);
		}

		HMENU hMenu = CreatePopupMenu();
		if (selectedIndexes.empty())
		{
			AppendMenu(hMenu, MF_STRING | MF_GRAYED, 1, L"No Items Selected");
		}
		else if (selectedIndexes.size() == 1)
		{
			AppendMenu(hMenu, MF_STRING, ID_CONTEXT_MENU_LITEM1, L"Delete Selected Item");
			AppendMenu(hMenu, MF_STRING, ID_CONTEXT_MENU_OPEN_FILE, L"Open File Location");
		}
		else
		{
			wchar_t buffer[100];
			swprintf(buffer, 100, L"Delete %d Selected Items", selectedIndexes.size());
			AppendMenu(hMenu, MF_STRING, ID_CONTEXT_MENU_LITEM2, buffer);
		}
		int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, hWnd, NULL);
		if (cmd != 0)
		{
			PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(cmd, 0), 0);
		}

		DestroyMenu(hMenu);
	}
}
void ShowContextMenu(HWND hWnd, LPARAM lParam)
{
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	HMENU hMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING, ID_CONTEXT_MENU_ITEM1, (L"Delete all duplicates (leave only most bigger and older)"));

	TVHITTESTINFO hitTestInfo = { 0 };
	hitTestInfo.pt = pt;
	ScreenToClient(hTreeView, &hitTestInfo.pt);

	HTREEITEM hItem = (HTREEITEM)SendMessage(hTreeView, TVM_HITTEST, 0, (LPARAM)&hitTestInfo);

	if (hitTestInfo.flags & TVHT_ONITEM)
	{
		hItem = hitTestInfo.hItem;
		SendMessage(hTreeView, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hItem);

		TVITEM tvItem = {};
		tvItem.mask = TVIF_HANDLE | TVIF_PARAM;
		tvItem.hItem = hItem;
		TCHAR szLabel[256];

		if (TreeView_GetItem(hTreeView, &tvItem))
		{
			indexForMenu2 = (int)tvItem.lParam;
			swprintf_s(szLabel, 256, L"Selected index: %d Delete and only leave the oldest and bigger one", indexForMenu2 + 1);
		}
		else
		{
			indexForMenu2 = -1;
		}
		AppendMenu(hMenu, MF_STRING, ID_CONTEXT_MENU_ITEM2, szLabel);
	}

	int cmd = TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_RETURNCMD,
		pt.x, pt.y, 0, hWnd, NULL);

	DestroyMenu(hMenu);

	if (cmd != 0)
	{
		PostMessage(hWnd, WM_COMMAND, cmd, 0);
	}
}


#pragma endregion

#include <dwmapi.h>  // For DWM features like rounded corners
#pragma comment(lib, "Dwmapi.lib")



void CreateTabControl(HWND hwndParent) {
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_TAB_CLASSES;
	InitCommonControlsEx(&icex);

	// Create the tab control
	hTabControl = CreateWindowEx(0, WC_TABCONTROL, L"",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER,
		0, 0, 500, 500, hwndParent, (HMENU)1, hInst, NULL);

	// Create tabs
	TCITEM tie;
	tie.mask = TCIF_TEXT;

	tie.pszText = forlvTabs(L"How to start");
	TabCtrl_InsertItem(hTabControl, 0, &tie);

	tie.pszText = forlvTabs(L"How to delete");
	TabCtrl_InsertItem(hTabControl, 1, &tie);

	tie.pszText = forlvTabs(L"How its work");
	TabCtrl_InsertItem(hTabControl, 2, &tie);

	hStatic1 = CreateWindowEx(
		WS_EX_CLIENTEDGE,           // Extended styles
		TEXT("EDIT"),               // Class name
		TEXT("The program can operate in three simple ways:\r\n\r\n"
			"1. Select a folder: Click the \"Select Folder\" button to choose a directory.\r\n"
			"2. Drag and drop: Drag a folder and drop it directly into the program window.\r\n"
			"3. Manual input: Type the folder path into the box and press the right arrow button to proceed."),WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_LEFT | ES_READONLY | WS_VSCROLL, // Styles
		25, 30, 440, 400,hwndParent,NULL,hInst,NULL);

	hStatic2 = CreateWindowEx(
		WS_EX_CLIENTEDGE,           // Extended styles
		TEXT("EDIT"),               // Class name
		TEXT(
			"Right-click on an item in the folder tree:\r\n"
			"- If one item is selected, you can delete that row.\r\n"
			"- If \"Delete All\" is selected, you can delete all of them at once.\r\n"
			"-Always leaves the largest and oldest item in the tree.-\r\n\r\n"
			"Right-click on an item in the list:\r\n"
			"- Gives you the option to delete the selected item or all selected items if more than one is selected.\r\n\r\n"
			"-Program never deletes by moves to trash bin-"
		)
		, WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_LEFT | ES_READONLY | WS_VSCROLL, // Styles
		25, 30, 440, 400, hwndParent, NULL, hInst, NULL);

	hStatic3 = CreateWindowEx(
		WS_EX_CLIENTEDGE,           // Extended styles
		TEXT("EDIT"),               // Class name
		TEXT(
			"The program compares files within a folder to identify files with matching byte ranges. The comparison operates at the byte level, targeting one of the smallest units of data to ensure precise identification of similar or identical files.\r\n"
			"Any ideas for improvement are welcome! If you're interested in reviewing the code, the files have been shared.\r\n"
			"You can reach me here : \r\n"
			"https://github.com/betsunohito"
		)
		, WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_LEFT | ES_READONLY | WS_VSCROLL, // Styles
		25, 30, 440, 400, hwndParent, NULL, hInst, NULL);
	



	ShowWindow(hStatic1, SW_SHOW);
	ShowWindow(hStatic2, SW_HIDE);
	ShowWindow(hStatic3, SW_HIDE);
}

void HandleTabSelection(HWND hwndParent, int selectedTab) {
	// Hide all content first
	ShowWindow(hStatic1, SW_HIDE);
	ShowWindow(hStatic2, SW_HIDE);
	ShowWindow(hStatic3, SW_HIDE);

	// Show content based on selected tab
	switch (selectedTab) {
	case 0:
		ShowWindow(hStatic1, SW_SHOW);
		break;
	case 1:
		ShowWindow(hStatic2, SW_SHOW);
		break;
	case 2:
		ShowWindow(hStatic3, SW_SHOW);
		break;
	}
}



void TogglePopup(HWND hParent, HINSTANCE hInstance) {
	if (hPopup == NULL) {
		// Get the dimensions of the parent window
		RECT parentRect;
		GetWindowRect(hParent, &parentRect);

		int popupWidth = 500;
		int popupHeight = 500;

		// Calculate the centered position
		int xPos = parentRect.left + (parentRect.right - parentRect.left - popupWidth) / 2;
		int yPos = parentRect.top + (parentRect.bottom - parentRect.top - popupHeight) / 2;

		// Create the popup window
		hPopup = CreateWindowEx(
			WS_EX_TOPMOST,                // Always on top
			L"PopupClass",                // Class name
			L"Information",               // Title
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, // Style
			xPos, yPos,                   // Centered position
			popupWidth, popupHeight,      // Width and height
			hParent, NULL, hInstance, NULL
		);

		
	}
	if (IsWindowVisible(hPopup)) {
		ShowWindow(hPopup, SW_HIDE);
	}
	else {
		ShowWindow(hPopup, SW_SHOW);
	}
}

std::wstring GetListViewItemText(HWND hListView, int itemIndex, int subItemIndex) {
	wchar_t buffer[260];
	LVITEMW lvItem = {};
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = itemIndex;
	lvItem.iSubItem = subItemIndex;
	lvItem.pszText = buffer;
	lvItem.cchTextMax = sizeof(buffer) / sizeof(buffer[0]);

	if (SendMessageW(hListView, LVM_GETITEMTEXT, itemIndex, (LPARAM)&lvItem)) {
		return std::wstring(buffer);
	}
	return L"";
}

void AddIconsToImageList(FileDetail& item,int howmany) {
	std::wstring iconPath = item.getFAddress() + L"\\" + item.getFName();

	SHFILEINFO shfi;
	ZeroMemory(&shfi, sizeof(SHFILEINFO));
	SHGetFileInfo(wStringToLPWSTR(iconPath), 0, &shfi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON);

	if (shfi.hIcon) {
		int iconIndex = ImageList_AddIcon(hImageList, shfi.hIcon);
		item.setfIconIndex(iconIndex);
		indexPurposes[howmany].push_back(item);
		DestroyIcon(shfi.hIcon);
	}
	else {
		item.setfIconIndex(-1);
	}
}


void FindTheSameOnes(vector<FileDetail> item) {
	ITaskbarList3* pTaskbar = NULL;
	CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (void**)&pTaskbar);
	if (SUCCEEDED(hr)) {
		pTaskbar->HrInit();
		pTaskbar->SetProgressState(hMainWindow, TBPF_NORMAL);
	}
	int value = 0;
	hImageList = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);
	TreeView_SetImageList(hTreeView, hImageList, TVSIL_NORMAL);
	int range = rangeOfRange(hTrackBar, hRadioButton2, hRadioButton3);
	for (int i = 0; i < item.size(); ++i) {

		if (stopFlag) {
			break;
		}

		bool triggered = false;
		for (int j = i + 1; j < item.size(); ++j) {
			int ilkdeger = item[i].getFSize();
			int aranandeger = item[j].getFSize();
			if (isWithinRange(ilkdeger, aranandeger, range))
			{
				AddIconsToImageList(item[j], howmany);
				item.erase(item.begin() + j);
				--j;
				triggered = true;
			}
			
		}

		if (triggered)
		{
			AddIconsToImageList(item[i], howmany);
			value++;
			howmany++;
		}
		int progress = (i + 1) * 100 / item.size();
		UpdateProgressBar(progress);
		if (pTaskbar) {
			pTaskbar->SetProgressValue(hMainWindow, i + 1, item.size());
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		
	}
	if (!stopFlag) {
		AddItemsToTreeView(hTreeView);
		SetWindowText(hDinamicLbl, ConvertDWORDToLPWSTR(value));
	}
	UpdateProgressBar(0);
	if (pTaskbar) {
		pTaskbar->SetProgressState(hMainWindow, TBPF_NOPROGRESS);
		pTaskbar->Release();
		pTaskbar = NULL;
	}
	CoUninitialize(); 
}

/*_CrtDumpMemoryLeaks();*/
void startingTheProgram(std::wstring text) {
	resetObjects();
	std::thread worker([text = std::move(text)]() {

		vector<FileDetail> files;
		std::wstring modifiableDirectory = text;
		GetFilesInDirectory(files, modifiableDirectory);

		if (stopFlag) {
			files.clear();
			return;
		}

		FindTheSameOnes(files);
		files.clear();
		isTaskRunning = false;
		});
	worker.detach();
}

void open_file(HWND hWnd)
{
	if (isTaskRunning.exchange(true)) {
		// Task is already running, ignore the click
		return;
	}
	wchar_t* szDir = new wchar_t[MAX_PATH] {};
	BROWSEINFO bInfo = { }; 
	bInfo.hwndOwner = hWnd;
	bInfo.pidlRoot = NULL;
	bInfo.pszDisplayName = szDir;
	bInfo.lpszTitle = L"Select a folder:";
	bInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bInfo);
	if (pidl != NULL) {
		if (SHGetPathFromIDList(pidl, szDir)) {
			SetWindowText(hTxtInput, szDir);
			if (IsValidLocalPath(szDir))
				startingTheProgram(szDir);
			else
			isTaskRunning = false;
		}
		else {
			MessageBox(hWnd, L"Failed to get the folder path.", L"Error", MB_ICONERROR | MB_OK);
		}
	}
	else {
		isTaskRunning =false;
	}
	CoTaskMemFree(pidl);
	delete[] szDir; 

}
void whenDragDropMakeTheWork(LPWSTR path)
{
	if (isTaskRunning.exchange(true)) {
		// Task is already running, ignore the click
		return;
	}

	if (IsValidLocalPath(path))
	{
		SetWindowText(hTxtInput, path);
		startingTheProgram(path);
	}
	else
		isTaskRunning = false;
}
void DeleteTreeViewRoot(HWND hTreeView, int rootIndex) {
	HTREEITEM hRoot = TreeView_GetRoot(hTreeView);
	int currentIndex = 0;

	while (hRoot != NULL) {
		if (currentIndex == rootIndex) {
			TreeView_DeleteItem(hTreeView, hRoot);
			return;
		}
		hRoot = TreeView_GetNextSibling(hTreeView, hRoot);
		currentIndex++;
	}

	MessageBox(NULL, TEXT("Root index out of range!"), TEXT("Error"), MB_ICONERROR);
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_UPDATE_PROGRESS:
		SendMessage(hProgressBar, PBM_SETPOS, wParam, 0);
		break;

	case WM_CONTEXTMENU:
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);

		HWND hListView = GetDlgItem(hWnd, IDC_LISTVIEW);
		if (hListView != NULL && WindowFromPoint(pt) == hListView)
		{
			ShowContextMenuForListView(hWnd, lParam);
		}
		else if ((HWND)wParam == hTreeView)
		{
			ShowContextMenu(hWnd, lParam);
		}


		return 0;
	}


	case WM_NOTIFY:
	{

		LPNMHDR pNMHDR = (LPNMHDR)lParam;
		if (pNMHDR->idFrom == IDC_TREEVIEW)
		{
			if (pNMHDR->code == TVN_SELCHANGED)
			{
				ListView_DeleteAllItems(hListView);
				LPNMTREEVIEW pNMTV = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
				HTREEITEM hSelectedItem = pNMTV->itemNew.hItem;

				TVITEM tvi;
				tvi.mask = TVIF_PARAM;
				tvi.hItem = hSelectedItem;
				TreeView_GetItem(hTreeView, &tvi);
				iterationOFLB = (int)tvi.lParam;
				AddItemsToListView(iterationOFLB);


			}
		}
		if (pNMHDR->idFrom == IDC_LISTVIEW && pNMHDR->code == NM_DBLCLK) {
			LPNMITEMACTIVATE nmItem = (LPNMITEMACTIVATE)lParam;

			if (nmItem->iItem != -1) {
				HWND hListView = pNMHDR->hwndFrom;
				std::wstring fileName = GetListViewItemText(hListView, nmItem->iItem, 0);
				std::wstring fileLocation = GetListViewItemText(hListView, nmItem->iItem, 5);


				if (!fileLocation.empty() && !fileName.empty()) {
					std::wstring fullPath = fileLocation + L"\\" + fileName;
					ShellExecuteW(NULL, L"open", fullPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
				}
			}
		}


	}
	break;

	case WM_COMMAND:
	{
		switch (wParam)
		{
		case IDC_BUTTON:
		{
			open_file(hWnd);
			break;
		}
		case IDC_BUTTONRPT:
		{
			

			if (isTaskRunning.exchange(true)) {
				// Task is already running, ignore the click
				return 0;
			}
			int length = GetWindowTextLength(hTxtInput);
			if (length == 0) break;
			wchar_t* text = new wchar_t[length + 1];
			GetWindowText(hTxtInput, text, length + 1);
			if (IsValidLocalPath(text))
			{
				startingTheProgram(text);
			}
			else
			{
				isTaskRunning = false;
			}
			delete[] text;
			break;
		}
		case ID_CONTEXT_MENU_ITEM1:
		{
			std::vector<int> largestIndices;

			for (int i = 0; i < indexPurposes.size(); i++) {
				int largestIndex = 0; 
				for (int j = 1; j < indexPurposes[i].size(); j++) {
					if (indexPurposes[i][j].getFSize() > indexPurposes[i][largestIndex].getFSize()) {
						largestIndex = j;
					}
					else if (indexPurposes[i][j].getFSize() == indexPurposes[i][largestIndex].getFSize()) {
						const FILETIME& fileTimeJ = indexPurposes[i][j].getFCreatedAt();
						const FILETIME& fileTimeLargest = indexPurposes[i][largestIndex].getFCreatedAt();
						if (CompareFileTime(&fileTimeJ, &fileTimeLargest) < 0) {
							largestIndex = j;
						}
					}
				}
				largestIndices.push_back(largestIndex);
			}

			for (int i = 0; i < indexPurposes.size(); i++)
			{
				for (int j = 0; j < indexPurposes[i].size(); j++) {

					if (j == largestIndices[i])
					{
						continue;
					}
					std::wstring filePath = indexPurposes[i][j].getFAddress() +L'\\' + indexPurposes[i][j].getFName();
					std::wstring doubleNullTerminatedFilePath = filePath + L'\0' + L'\0';

					if (!DeleteFiles(hWnd, doubleNullTerminatedFilePath.c_str()))
					{
						MessageBox(hWnd, L"Failed to delete one or more files.", L"Error", MB_ICONERROR);
					}
				}
			}
			
			TreeView_DeleteAllItems(hTreeView);
			ListView_DeleteAllItems(hListView);

			break;
		}
		case ID_CONTEXT_MENU_ITEM2:
		{
			if (indexForMenu2!=-1)
			{
					int largestIndex = 0;
					for (int i = 1; i < indexPurposes[indexForMenu2].size(); i++) {
						if (indexPurposes[indexForMenu2][i].getFSize() > indexPurposes[indexForMenu2][largestIndex].getFSize()) {
							largestIndex = i;
						}
						else if (indexPurposes[indexForMenu2][i].getFSize() == indexPurposes[indexForMenu2][largestIndex].getFSize()) {
							const FILETIME& fileTimeJ = indexPurposes[indexForMenu2][i].getFCreatedAt();
							const FILETIME& fileTimeLargest = indexPurposes[indexForMenu2][largestIndex].getFCreatedAt();
							if (CompareFileTime(&fileTimeJ, &fileTimeLargest) < 0) {
								largestIndex = i;
							}
						}
					
				}

				for (int i = 0; i < indexPurposes[indexForMenu2].size(); i++) {
					if (i == largestIndex)
					{
						continue;
					}
					std::wstring filePath = indexPurposes[indexForMenu2][i].getFAddress() + L'\\' + indexPurposes[indexForMenu2][i].getFName();
					std::wstring doubleNullTerminatedFilePath = filePath + L'\0' + L'\0';

					if (!DeleteFiles(hWnd, doubleNullTerminatedFilePath.c_str()))
					{
						MessageBox(hWnd, L"Failed to delete one or more files.", L"Error", MB_ICONERROR);
					}
				}
				DeleteTreeViewRoot(hTreeView, indexForMenu2);
				ListView_DeleteAllItems(hListView);
			}
			else
			{
				MessageBox(hWnd, L"No item selected!", L"Error", MB_ICONERROR);
			}
			break;
		}
		case ID_CONTEXT_MENU_LITEM1:
		case ID_CONTEXT_MENU_LITEM2:
			if (!selectedIndexes.empty())
			{
				std::reverse(selectedIndexes.begin(), selectedIndexes.end());
				for (int selectedIndex : selectedIndexes)
				{

					std::wstring fileName = GetListViewItemText(hListView, selectedIndex, 0);

					std::wstring fileLocation = GetListViewItemText(hListView, selectedIndex, 5);

					if (!fileLocation.empty() && !fileName.empty()) {
						std::wstring fullPath = fileLocation + L"\\" + fileName;
						LPCWSTR finalText = fullPath.c_str();
						if (DeleteFiles(hWnd, finalText))
						{
							ListView_DeleteItem(hListView, selectedIndex);
						}
						else
						{
							MessageBox(hWnd, L"Failed to delete one or more files.", L"Error", MB_ICONERROR);
							ListView_DeleteAllItems(hListView);
							break;
						}
					}
				}
			}
			break;

		case ID_CONTEXT_MENU_OPEN_FILE:
		{
			if (!selectedIndexes.empty())
			{
				std::wstring fileName = GetListViewItemText(hListView, selectedIndexes[0], 0);

				std::wstring fileLocation = GetListViewItemText(hListView, selectedIndexes[0], 5);

				std::wstring fullPath = fileLocation + L"\\" + fileName;

				HINSTANCE result = ShellExecuteW(
					NULL,
					L"open",
					L"explorer.exe", // The executable
					(L"/select,\"" + fullPath + L"\"").c_str(), // Arguments
					NULL,
					SW_SHOWNORMAL
				);

				// Check for success and handle errors
				if ((int)result <= 32)
				{
					MessageBoxW(NULL, L"Failed to open the file in Explorer.", L"Error", MB_ICONERROR);
				}
			}
		}
			break;
		case IDC_STOPBUTTON:
		{
			stopFlag = true;
			isTaskRunning = false;
		}
		
		break;

		case IDC_BUTTONINFO:
		{
			TogglePopup(hWnd, hMainInstance);
			
		}
		break;


		}
	}
	break;
	case WM_CREATE:
	{
		DragAcceptFiles(hWnd, TRUE);
		hMainWindow = hWnd;

		hButtonInfo = CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON", L"Information About Program", WS_VISIBLE | WS_CHILD, 350, 630, 300, 30, hWnd, (HMENU)IDC_BUTTONINFO, hMainInstance, NULL);


		hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE, 10, 10, 200, 25, hMainWindow, NULL, GetModuleHandle(NULL), NULL);
		SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
		SendMessage(hProgressBar, PBM_SETSTEP, IDC_PROGRESSBAR, 0);

		HICON hIcons = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON,24, 24, LR_DEFAULTCOLOR);

		hButtonRefresh = CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_ICON,410, 50, 30, 30, hWnd, (HMENU)IDC_BUTTONRPT,hMainInstance, NULL);
		SendMessage(hButtonRefresh, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcons);



		wchar_t desktopPath[MAX_PATH];
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath);
		if (SUCCEEDED(hr)) {
			hTxtInput = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", desktopPath,WS_VISIBLE | WS_CHILD | ES_LEFT,20, 50, 380, 25,hWnd, (HMENU)IDC_STATIC,hMainInstance, NULL);
		}
		else {
			hTxtInput = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"Type Here", WS_VISIBLE | WS_CHILD | ES_LEFT, 20, 50, 380, 25, hWnd, (HMENU)IDC_STATIC, hMainInstance, NULL);
		}

		
		hButton = CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON", L"Select Folder", WS_VISIBLE | WS_CHILD | ES_LEFT, 300, 100, 100, 30, hWnd, (HMENU)IDC_BUTTON, hMainInstance, NULL);
		hStatic = CreateWindowEx(0, L"STATIC", L"Interconnection :", WS_VISIBLE | WS_CHILD, 125, 100, 125, 30, hWnd, (HMENU)IDC_LABEL, hMainInstance, NULL);

		hDinamicLbl = CreateWindowEx(0, L"STATIC", L"00", WS_VISIBLE | WS_CHILD, 250, 100, 30, 30, hWnd, (HMENU)IDC_DLABEL, hMainInstance, NULL);

		hListView = CreateWindowEx(0, WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER | WS_HSCROLL | WS_VSCROLL, 315, 150, 650, 450, hWnd, (HMENU)IDC_LISTVIEW, GetModuleHandle(NULL), NULL);

		hTrackBar = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 500, 90, 300, 50, hWnd, NULL, hMainInstance, NULL);

		SendMessage(hTrackBar, TBM_SETRANGE, TRUE, MAKELPARAM(0, 1000));
		SendMessage(hTrackBar, TBM_SETTICFREQ, 100, 0);
		SendMessage(hTrackBar, TBM_SETPOS, TRUE, 0);
		hStatic = CreateWindowEx(0, L"STATIC", L"0", WS_CHILD | WS_VISIBLE | SS_CENTER, 520, 55, 100, 20, hWnd, NULL, hMainInstance, NULL
		);

		InitializeListViewColumns(hListView);

		hRadioButton1 = CreateWindowEx(0, TEXT("BUTTON"), TEXT("B"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 630, 50, 50, 30, hWnd,(HMENU)IDC_RBUTTON1, hMainInstance, NULL);

		hRadioButton2 = CreateWindowEx(0, TEXT("BUTTON"), TEXT("KB"),WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,670, 50, 50, 30,hWnd,(HMENU)IDC_RBUTTON2,hMainInstance, NULL);
		hRadioButton3 = CreateWindowEx(0, TEXT("BUTTON"), TEXT("MB"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 710, 50, 50, 30, hWnd, (HMENU)IDC_RBUTTON3, hMainInstance, NULL);
		SendMessage(hRadioButton1, BM_SETCHECK, BST_CHECKED, 0);

		hTreeView = CreateWindowEx(0, WC_TREEVIEW, nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER | TVS_LINESATROOT | TVS_HASBUTTONS | WS_HSCROLL | WS_VSCROLL | TVS_HASLINES, 20, 150, 275, 450, hWnd, (HMENU)IDC_TREEVIEW, hMainInstance, nullptr);


		hStopButton = CreateWindowEx(
			0, L"BUTTON", L"Stop", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			250, 10, 50, 30, hMainWindow, (HMENU)IDC_STOPBUTTON, GetModuleHandle(NULL), NULL);

		break;
	}
	case WM_DROPFILES:
	{
		HDROP hDrop = (HDROP)wParam;
		UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
		if (fileCount > 0)
		{
			TCHAR path[MAX_PATH];
			for (UINT i = 0; i < fileCount; ++i)
			{
				DragQueryFile(hDrop, i, path, MAX_PATH);

				// Check if it's a folder
				DWORD attributes = GetFileAttributes(path);
				if (attributes != INVALID_FILE_ATTRIBUTES)
				{
					// Handle if it's a directory
					if (attributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						whenDragDropMakeTheWork(path);
						break;
					}
					// Handle if it's a shortcut
					else if (attributes & FILE_ATTRIBUTE_ARCHIVE)
					{
						IShellLink* pShellLink = NULL;
						IPersistFile* pPersistFile = NULL;
						HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pShellLink));
						if (SUCCEEDED(hr))
						{
							hr = pShellLink->QueryInterface(IID_PPV_ARGS(&pPersistFile));
							if (SUCCEEDED(hr))
							{
								hr = pPersistFile->Load(path, STGM_READ);
								if (SUCCEEDED(hr))
								{
									TCHAR targetPath[MAX_PATH];
									ZeroMemory(targetPath, sizeof(targetPath));
									hr = pShellLink->GetPath(targetPath, MAX_PATH, NULL, SLGP_SHORTPATH);
									if (SUCCEEDED(hr))
									{
										whenDragDropMakeTheWork(targetPath);
										break;

									}
								}
								pPersistFile->Release();
							}
							pShellLink->Release();
						}
					}
				}
			}
		}
		DragFinish(hDrop);
		break;
	}
	case WM_HSCROLL:
	{
	if ((HWND)lParam == hTrackBar) {
		int currentValue = 0;
		currentValue = SendMessage(hTrackBar, TBM_GETPOS, 0, 0);
		wchar_t buffer[10];
		swprintf(buffer, 10, L"%d", currentValue);
		SetWindowText(hStatic, buffer);
	}
	break;
	}
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);

	}
	return 0;


}

LRESULT CALLBACK PopupWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE:
		CreateTabControl(hwnd);
		break;

	case WM_NOTIFY: {
		LPNMHDR pnmhdr = (LPNMHDR)lParam;
		if (pnmhdr->hwndFrom == hTabControl) {
				int selectedTab = TabCtrl_GetCurSel(hTabControl);
				HandleTabSelection(hwnd, selectedTab);
		}
		break;
	}
	case WM_CLOSE:
		ShowWindow(hwnd, SW_HIDE); // Hide instead of destroying
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcPopup = { sizeof(WNDCLASSEX) };
	wcPopup.style = CS_HREDRAW | CS_VREDRAW;
	wcPopup.lpfnWndProc = PopupWndProc; // Custom procedure for the popup
	wcPopup.cbClsExtra = 0;
	wcPopup.cbWndExtra = 0;
	wcPopup.hInstance = hInstance;
	wcPopup.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcPopup.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Light gray background
	wcPopup.lpszClassName = L"PopupClass";

	if (!RegisterClassEx(&wcPopup)) {
		MessageBox(NULL, L"Failed to register PopupClass!", L"Error", MB_ICONERROR);
		return -1;
	}


	MSG msg = { 0 };
	WNDCLASS wc{ 0 };
	wc.lpfnWndProc = WindowProc;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"Mywinapi";
	if (!RegisterClass(&wc))
	{
		return 1;
	}
	hMainWindow = CreateWindowEx(
		WS_EX_APPWINDOW,
		wc.lpszClassName,
		L"Files that have the same size",
		WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_THICKFRAME | WS_VISIBLE | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 1024, 700,
		0, 0, hInstance, NULL
	);
	if (!hMainWindow)
	{
		return 1; // Exit if window creation fails
	}
	CenterWindow(hMainWindow);
	hMainInstance = wc.hInstance;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}