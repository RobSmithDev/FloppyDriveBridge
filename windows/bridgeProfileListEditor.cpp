#pragma once


#include "bridgeProfileListEditor.h"
#include "bridgeProfileEditor.h"
#include "resource.h"
#include "FloppyBridge.h"
//#include 
 
// Returns a pointer to information about the project
void handleAbout(bool checkForUpdates, BridgeAbout** output);
bool handleGetDriverInfo(unsigned int driverIndex, FloppyDiskBridge::BridgeDriver** driverInformation);


INT_PTR CALLBACK dialogCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_INITDIALOG) SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)lParam);
	BridgeProfileListEditor* dlg = (BridgeProfileListEditor*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (dlg) return dlg->handleDialogProc(hwnd, msg, wParam, lParam); else return FALSE;
}

// Create a new profile
void BridgeProfileListEditor::handleCreateProfile() {
	BridgeConfig* profile = new BridgeConfig();

	// Pinch some settings from the current one
	HWND hwndList = GetDlgItem(m_dialogBox, IDC_LIST1);
	int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
	if (lbItem >= 0) {
		unsigned int profileID = (unsigned int)SendMessage(hwndList, LB_GETITEMDATA, lbItem, 0);
		auto f = m_profileList.find(profileID);
		if (f != m_profileList.end()) {
			profile->bridgeIndex = f->second->bridgeIndex;
		}
	}
	else {
		if (m_profileList.size())
			profile->bridgeIndex = m_profileList.begin()->second->bridgeIndex;
	}


	BridgeProfileEditor editor(m_hInstance, m_dialogBox, m_bridgeLogos, profile);
	if (editor.doModal()) {
		HWND hwndList = GetDlgItem(m_dialogBox, IDC_LIST1);

		int pos = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)profile->profileName);

		// Work out a new profile ID
		unsigned int profileID = 1;
		while (m_profileList.find(profileID) != m_profileList.end()) profileID++;
		m_profileList.insert(std::make_pair(profileID, profile));

		SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM)profileID);
		SendMessage(hwndList, LB_SETCURSEL, pos, 0);
	}
	else delete profile;
}

void BridgeProfileListEditor::handleEditSelectedProfile() {
	HWND hwndList = GetDlgItem(m_dialogBox, IDC_LIST1);
	int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
	if (lbItem >= 0) {
		unsigned int profileID = (unsigned int)SendMessage(hwndList, LB_GETITEMDATA, lbItem, 0);
		auto f = m_profileList.find(profileID);
		if (f != m_profileList.end()) {
			BridgeProfileEditor editor(m_hInstance, m_dialogBox, m_bridgeLogos, f->second);
			// If it updates, trigger invaldate on the listbox
			if (editor.doModal()) redrawSelectedItem();
		}
	}
}

void BridgeProfileListEditor::handleDeleteSelectedProfile() {
	HWND hwndList = GetDlgItem(m_dialogBox, IDC_LIST1);
	int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
	if (lbItem >= 0) {
		unsigned int profileID = (unsigned int)SendMessage(hwndList, LB_GETITEMDATA, lbItem, 0);
		auto f = m_profileList.find(profileID);
		if (f == m_profileList.end()) {
			// ERROR, not found. just delete it. Should never happen
			SendMessage(hwndList, LB_DELETESTRING, lbItem, 0);
		}
		else {
			char buffer[200];
			sprintf_s(buffer, "Are you sure you want to delete the profile \"%s\"?", f->second->profileName);
			if (MessageBoxA(m_dialogBox, buffer, "Delete Profile", MB_YESNO | MB_ICONQUESTION) == IDYES) {
				SendMessage(hwndList, LB_DELETESTRING, lbItem, 0);
				delete f->second;
				m_profileList.erase(f);
			}
		}
	}
}

void BridgeProfileListEditor::handleURLClicked() {
	BridgeAbout* about;

	SetCursor(m_busyCursor);
	handleAbout(false, &about);
	ShellExecuteA(m_dialogBox, "OPEN", about->url, NULL, NULL, SW_SHOW);
}

void BridgeProfileListEditor::handleProfileListMessages(WPARAM wParam, LPARAM lParam) {
	switch (HIWORD(wParam))
	{
	case LBN_SELCHANGE:
	{
		HWND hwndList = GetDlgItem(m_dialogBox, IDC_LIST1);
		int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
		EnableWindow(GetDlgItem(m_dialogBox, IDEDIT), lbItem >= 0);
		EnableWindow(GetDlgItem(m_dialogBox, IDDELETE), lbItem >= 0);
	}
	break;

	case LBN_DBLCLK:
		PostMessage(m_dialogBox, WM_COMMAND, IDEDIT, (LPARAM)GetDlgItem(m_dialogBox, IDEDIT));
		break;

	case LBN_SETFOCUS:
		redrawSelectedItem();
		break;
	case LBN_KILLFOCUS:
			redrawSelectedItem();
		break;
	}
}

void BridgeProfileListEditor::redrawSelectedItem() {
	HWND hwndList = GetDlgItem(m_dialogBox, IDC_LIST1);
	int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
	if (lbItem >= 0) {
		RECT r;
		if (SendMessage(hwndList, LB_GETITEMRECT, lbItem, (LPARAM)&r) != LB_ERR) InvalidateRect(hwndList, &r, 0);
	}
}

void BridgeProfileListEditor::handleDrawListBox(PDRAWITEMSTRUCT item) {
	if (!item) return;

	auto f = m_profileList.find(item->itemData);
	if (f == m_profileList.end()) return;

	if (item->itemState & ODS_SELECTED) {
		SetBkColor(item->hDC, m_hSelectedBackgroundColor);
		SetTextColor(item->hDC, m_hSelectedTextColor);
		FillRect(item->hDC, &item->rcItem, m_hSelectedBackground);
	}
	else {
		SetBkColor(item->hDC, m_hNormalBackgroundColor);
		SetTextColor(item->hDC, m_hNormalTextColor);
		FillRect(item->hDC, &item->rcItem, m_hNormalBackground);
	}

	HGDIOBJ oldBmp = SelectObject(m_hdcTemp, m_bridgeLogos[f->second->bridgeIndex]);

	int bitmapSize = 66;

	if (item->rcItem.bottom - item->rcItem.top < 66) {
		bitmapSize = item->rcItem.bottom - item->rcItem.top;
	}
	SetStretchBltMode(item->hDC, HALFTONE);
	SetBrushOrgEx(item->hDC, 0, 0, NULL);
	StretchBlt(item->hDC, item->rcItem.left + 1, item->rcItem.top + 1, bitmapSize - 2, bitmapSize - 2, m_hdcTemp, 0, 0, 64, 64, SRCCOPY);
	SetStretchBltMode(item->hDC, COLORONCOLOR);
	SelectObject(m_hdcTemp, oldBmp);

	RECT rec = item->rcItem;
	rec.left += bitmapSize+4; rec.top += 4;
	FloppyDiskBridge::BridgeDriver* info;
	handleGetDriverInfo(f->second->bridgeIndex, &info);

	// MAke a bold version fi it doesnt exist
	if (!m_boldFont) {
		LOGFONT logFont;
		GetObject(GetCurrentObject(item->hDC, OBJ_FONT), sizeof(logFont), (LPVOID)&logFont);
		logFont.lfWeight = FW_BOLD;
		m_boldFont = CreateFontIndirect(&logFont);
	}
	HGDIOBJ oldFont = SelectObject(item->hDC, m_boldFont);
	rec.top += DrawTextA(item->hDC, f->second->profileName, strlen(f->second->profileName), &rec, DT_LEFT | DT_NOPREFIX) + 5;
	SelectObject(item->hDC, oldFont);

	char buffer[128];
	sprintf_s(buffer, "%s (%s)", info->name, info->manufacturer);
	rec.top += DrawTextA(item->hDC, buffer, strlen(buffer), &rec, DT_LEFT | DT_NOPREFIX);
	sprintf_s(buffer, "URL: %s", info->url);
	rec.top += DrawTextA(item->hDC, buffer, strlen(buffer), &rec, DT_LEFT | DT_NOPREFIX);
	sprintf_s(buffer, "Driver by %s", info->driverAuthor);
	rec.top += DrawTextA(item->hDC, buffer, strlen(buffer), &rec, DT_LEFT | DT_NOPREFIX);

	if (((item->itemState & (ODS_FOCUS|ODS_SELECTED))== (ODS_FOCUS | ODS_SELECTED))) {
		rec = item->rcItem;
		rec.left += bitmapSize;
		DrawFocusRect(item->hDC, &rec);
	}
}

// Dialog window message handler
INT_PTR BridgeProfileListEditor::handleDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_INITDIALOG:
			handleInitDialog(hwnd);
			return TRUE;

		case WM_SETCURSOR: 
			if ((HWND)wParam == GetDlgItem(m_dialogBox, IDC_PROFILELIST_URL)) {
				SetCursor(m_handPoint);
				SetWindowLongPtr(hwnd, DWLP_MSGRESULT, (LONG)TRUE);
				return TRUE;
			}
			break;

		case WM_CTLCOLORSTATIC: 
			if ((HWND)lParam == GetDlgItem(m_dialogBox, IDC_PROFILELIST_URL)) {
				SetTextColor((HDC)wParam, GetSysColor(COLOR_HOTLIGHT));
				SetWindowLongPtr(hwnd, DWLP_MSGRESULT, (LONG_PTR)GetSysColorBrush(COLOR_BTNFACE));
				return (LONG)GetSysColorBrush(COLOR_BTNFACE);  // Should be TRUE but doesnt work right unless its this!?!
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
			case IDC_CHECKUPDATES:
				doCheckForUpdates();
				return TRUE;

			case IDCREATE: 
				handleCreateProfile();
				return TRUE;

			case IDEDIT: 
				handleEditSelectedProfile();
				return TRUE;

			case IDDELETE: 
				handleDeleteSelectedProfile();
				return TRUE;

			case IDC_PROFILELIST_URL: 
				handleURLClicked();
				return TRUE;

			case IDC_LIST1: 
				handleProfileListMessages(wParam, lParam);
				return TRUE;

			case IDOK:
				EndDialog(hwnd, TRUE);
				return TRUE;

			case IDCANCEL:
				EndDialog(hwnd, FALSE);
				return TRUE;
			}
			break;   // WM_COMMAND
	

		// Set listbox height
		case WM_MEASUREITEM:
			((PMEASUREITEMSTRUCT)lParam)->itemHeight = 66;
			return TRUE;

		// Draw profile
		case WM_DRAWITEM: 
			handleDrawListBox((PDRAWITEMSTRUCT)lParam);
			return TRUE;

		// Handle close
		case WM_SYSCOMMAND:
			switch (wParam) {
				case SC_CLOSE:
					EndDialog(hwnd, FALSE);
					return TRUE;
			}
	}

	return FALSE;
}

// Check for updates
void BridgeProfileListEditor::doCheckForUpdates() {
	BridgeAbout* about;

	SetCursor(m_busyCursor);
	handleAbout(true, &about);
	if (about->isUpdateAvailable) {
		char buffer[128];
		sprintf_s(buffer, "There is an update (V%i.%i) available.\r\n\r\nWould you like to download it?", about->updateMajorVersion, about->updateMinorVersion);
		if (MessageBoxA(m_dialogBox, buffer, "Update Available!", MB_ICONINFORMATION | MB_YESNO) == IDYES) {
			ShellExecuteA(m_dialogBox, "OPEN", about->url, NULL, NULL, SW_SHOW);
		}
	}
	else {
		MessageBoxA(m_dialogBox, "You are already using the latest version.", "No Update Available", MB_ICONINFORMATION | MB_OK);
	}
}

// Init dialog
void BridgeProfileListEditor::handleInitDialog(HWND hwnd) {
	m_dialogBox = hwnd;

	BridgeAbout* about;
	handleAbout(false, &about);
	char display[256];

	HWND w = GetDlgItem(hwnd, IDC_PROFILELIST_TITLE);
	sprintf_s(display, "%s V%i.%i %s", about->about, about->majorVersion, about->minorVersion, about->isBeta ? "beta" : "");
	SetWindowTextA(w, display);

	w = GetDlgItem(hwnd, IDC_PROFILELIST_URL);
	sprintf_s(display, "%s", about->url);
	SetWindowTextA(w, display);

	// Populate the list
	w = GetDlgItem(hwnd, IDC_LIST1);
	for (auto& f : m_profileList) {
		int pos = SendMessage(w, LB_ADDSTRING, 0, (LPARAM)f.second->profileName);
		SendMessage(w, LB_SETITEMDATA, pos, (LPARAM)f.first);
	}
	
	EnableWindow(GetDlgItem(m_dialogBox, IDEDIT), FALSE);
	EnableWindow(GetDlgItem(m_dialogBox, IDDELETE), FALSE);
}


BridgeProfileListEditor::BridgeProfileListEditor(HINSTANCE hInstance, HWND hwndParent, std::vector<HBITMAP>& bridgeLogos, std::unordered_map<unsigned int, BridgeConfig*>& profileList) :
	m_profileList(profileList), m_hInstance(hInstance), m_hwndParent(hwndParent), m_bridgeLogos(bridgeLogos), m_boldFont(0) {
	m_busyCursor = (HCURSOR)LoadCursor(NULL, IDC_WAIT);
	m_handPoint = (HCURSOR)LoadCursor(NULL, IDC_HAND);

	m_hdcTemp = CreateCompatibleDC(0);

	m_hNormalBackgroundColor = GetSysColor(COLOR_WINDOW);
	m_hNormalBackground = CreateSolidBrush(m_hNormalBackgroundColor);
	m_hSelectedBackgroundColor = GetSysColor(COLOR_HIGHLIGHT);
	m_hSelectedBackground = CreateSolidBrush(m_hSelectedBackgroundColor);

	m_hNormalTextColor = GetSysColor(COLOR_WINDOWTEXT);
	m_hSelectedTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
}

BridgeProfileListEditor::~BridgeProfileListEditor() {
	DeleteDC(m_hdcTemp);
	DestroyCursor(m_busyCursor);
	DestroyCursor(m_handPoint);
	DeleteObject(m_hNormalBackground);
	DeleteObject(m_hSelectedBackground);
	if (m_boldFont) DeleteObject(m_boldFont);
}

// Run the modal code, returns TRUE if OK was pressed.
bool BridgeProfileListEditor::doModal() {
	return DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_PROFILELISTEDITOR), m_hwndParent, dialogCallback, (LPARAM)this);
}

