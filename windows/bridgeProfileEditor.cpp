#pragma once

#include "bridgeProfileEditor.h"
#include "resource.h"
#include "FloppyBridge.h"


bool handleGetDriverInfo(unsigned int driverIndex, FloppyDiskBridge::BridgeDriver** driverInformation);

INT_PTR CALLBACK dialogCallback2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_INITDIALOG) SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)lParam);
	BridgeProfileEditor* dlg = (BridgeProfileEditor*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (dlg) return dlg->handleDialogProc(hwnd, msg, wParam, lParam); else return FALSE;
}

BridgeProfileEditor::BridgeProfileEditor(HINSTANCE hInstance, HWND hwndParent, std::vector<HBITMAP>& bridgeLogos, BridgeConfig* profile) :
	m_profile(profile), m_hInstance(hInstance), m_hwndParent(hwndParent), m_bridgeLogos(bridgeLogos), m_dialogBox(0) {
	m_busyCursor = (HCURSOR)LoadCursor(NULL, IDC_WAIT);
	m_handPoint = (HCURSOR)LoadCursor(NULL, IDC_HAND);
	m_hdcTemp = CreateCompatibleDC(0);

	m_hNormalBackgroundColor = GetSysColor(COLOR_WINDOW);
	m_hNormalBackground = CreateSolidBrush(m_hNormalBackgroundColor);
	m_hSelectedBackgroundColor = GetSysColor(COLOR_HIGHLIGHT);
	m_hSelectedBackground = CreateSolidBrush(m_hSelectedBackgroundColor);

	m_hNormalTextColor = GetSysColor(COLOR_WINDOWTEXT);
	m_hSelectedTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);

	SerialIO::enumSerialPorts(m_portList);

	m_notDetected = false;
	std::string port = profile->comPortToUse;
	std::wstring portW;
	quicka2w(port, portW);
	if (port.length()) {
		auto f = std::find_if(m_portList.begin(), m_portList.end(), [&portW](const SerialIO::SerialPortInformation& portsearch)->bool {
			return (portsearch.portName == portW);
			});

		// Port not found.  We'll insert it (as a dummy)
		if (f == m_portList.end()) {
			m_notDetected = true;
			SerialIO::SerialPortInformation sp;
			sp.portName = portW;
			m_portList.insert(m_portList.begin(), sp);
		}
	}
}


void BridgeProfileEditor::handleDrawListBox(PDRAWITEMSTRUCT item) {
	if (!item) return;

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

	FloppyDiskBridge::BridgeDriver* info;
	if (handleGetDriverInfo((unsigned int)item->itemData, &info)) {

		HGDIOBJ oldBmp = SelectObject(m_hdcTemp, m_bridgeLogos[item->itemData]);
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
		rec.left += bitmapSize+4; 
		if (item->rcItem.bottom - item->rcItem.top < 66) {
			rec.top += 2;
		} else rec.top += 4;

		char buffer[128];
		sprintf_s(buffer, "%s (%s)", info->name, info->manufacturer);
		rec.top += DrawTextA(item->hDC, buffer, (int)strlen(buffer), &rec, DT_LEFT | DT_NOPREFIX);
		if (item->rcItem.bottom - item->rcItem.top >= 66) {
			sprintf_s(buffer, "URL: %s", info->url);
			rec.top += DrawTextA(item->hDC, buffer, (int)strlen(buffer), &rec, DT_LEFT | DT_NOPREFIX);
			sprintf_s(buffer, "Driver by %s", info->driverAuthor);
			rec.top += DrawTextA(item->hDC, buffer, (int)strlen(buffer), &rec, DT_LEFT | DT_NOPREFIX);
		}

		if (((item->itemState & (ODS_FOCUS | ODS_SELECTED)) == (ODS_FOCUS | ODS_SELECTED))) {
			rec = item->rcItem;
			rec.left += bitmapSize;
			DrawFocusRect(item->hDC, &rec);
		}
	}
}

void BridgeProfileEditor::handleURLClicked() {
	HWND w = GetDlgItem(m_dialogBox, IDC_DRIVER);
	int lbItem = (int)SendMessage(w, CB_GETCURSEL, 0, 0);

	FloppyDiskBridge::BridgeDriver* info;
	if (handleGetDriverInfo(lbItem, &info)) {
		SetCursor(m_busyCursor);
		ShellExecuteA(m_dialogBox, "OPEN", info->url, NULL, NULL, SW_SHOW);
	}
}

void BridgeProfileEditor::handleNameChange() {
	// Block some bad characters
	HWND w = GetDlgItem(m_dialogBox, IDC_PROFILENAME);

	char buffer[256];
	GetWindowTextA(w, buffer, 255);
	std::string tmp = buffer;
	size_t pos = 0;
	bool changed = false;
	while (pos < tmp.length()) {
		if ((tmp[pos] == '|') || (tmp[pos] == ']') || (tmp[pos] == '[')) {
			tmp.erase(pos, 1);
			changed = true;
		}
		else pos++;
	}
	if (changed) {
		int index = LOWORD(SendMessage(w, EM_GETSEL, NULL, NULL))-1;
		SetWindowTextA(w, tmp.c_str());
		MessageBeep(MB_ICONEXCLAMATION);
		SendMessage(w, EM_SETSEL, (WPARAM)index, (LPARAM)index);

	}
}

void BridgeProfileEditor::saveToProfile() {
	HWND w;

	w = GetDlgItem(m_dialogBox, IDC_PROFILENAME);	
	GetWindowTextA(w, m_profile->profileName, 128);

	// Driver
	w = GetDlgItem(m_dialogBox, IDC_DRIVER);
	m_profile->bridgeIndex = (int)SendMessage(w, CB_GETCURSEL, 0, 0);

	// Com port
	w = GetDlgItem(m_dialogBox, IDC_COMPORT);
	int index = (int)SendMessage(w, CB_GETCURSEL, 0, 0);
	if (index >= 0) {
		std::string tmp;
		quickw2a(m_portList[index].portName, tmp);
		strcpy_s(m_profile->comPortToUse, 128, tmp.c_str());
	}

	// Auto-detect com port
	w = GetDlgItem(m_dialogBox, IDC_AUTODETECT);
	m_profile->autoDetectComPort = SendMessage(w, BM_GETCHECK, 0, 0) == BST_CHECKED;

	// Mode
	w = GetDlgItem(m_dialogBox, IDC_MODE);
	m_profile->bridgeMode = (CommonBridgeTemplate::BridgeMode)SendMessage(w, CB_GETCURSEL, 0, 0);	

	// Smart Speed
	w = GetDlgItem(m_dialogBox, IDC_SMART);
	m_profile->smartSpeed = SendMessage(w, BM_GETCHECK, 0, 0) == BST_CHECKED;

	// Auto-Cache
	w = GetDlgItem(m_dialogBox, IDC_AUTOCACHE);
	m_profile->autoCache = SendMessage(w, BM_GETCHECK, 0, 0) == BST_CHECKED;

	// Disk Type
	w = GetDlgItem(m_dialogBox, IDC_DISKTYPE);
	m_profile->bridgeDensity = (CommonBridgeTemplate::BridgeDensityMode)SendMessage(w, CB_GETCURSEL, 0, 0);

	// Cable select
	w = GetDlgItem(m_dialogBox, IDC_CABLE);
	m_profile->driveCable = (CommonBridgeTemplate::DriveSelection)SendMessage(w, CB_GETCURSEL, 0, 0);
}

// Dialog window message handler
INT_PTR BridgeProfileEditor::handleDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG:
		handleInitDialog(hwnd);
		return TRUE;

	case WM_SETCURSOR:
		if (((HWND)wParam == GetDlgItem(m_dialogBox, IDC_URL)) || ((HWND)wParam == GetDlgItem(m_dialogBox, IDC_URL2))) {
			SetCursor(m_handPoint);
			SetWindowLongPtr(m_dialogBox, DWLP_MSGRESULT, (LONG_PTR)TRUE);
			return TRUE;
		}
		break;

	case WM_CTLCOLORSTATIC:
		if (((HWND)lParam == GetDlgItem(m_dialogBox, IDC_URL)) || ((HWND)lParam == GetDlgItem(m_dialogBox, IDC_URL2))) {
			SetTextColor((HDC)wParam, GetSysColor(COLOR_HOTLIGHT));
			SetBkColor((HDC)wParam, GetSysColor(COLOR_3DFACE));
			SetWindowLongPtr(hwnd, DWLP_MSGRESULT, (LONG_PTR)GetSysColorBrush(COLOR_3DFACE));
			return (INT_PTR)GetSysColorBrush(COLOR_3DFACE);  // Should be TRUE but doesnt work right unless its this!?!
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_URL:
			handleURLClicked();
			return TRUE;

		case IDC_URL2:
			ShellExecuteA(m_dialogBox, "OPEN", "https://amiga.robsmithdev.co.uk", NULL, NULL, SW_SHOW);
			return TRUE;

		case IDC_DRIVER:
			if (HIWORD(wParam) == CBN_SELCHANGE) onDriverSelected();
			return TRUE;

		case IDC_MODE:
			onDriverSelected();
			return TRUE;

		case IDC_PROFILENAME:
			if (HIWORD(wParam) == EN_CHANGE) handleNameChange();
			break;

		case IDC_AUTODETECT:
			onDriverSelected();
			return TRUE;

		case IDOK:
			saveToProfile();
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

void BridgeProfileEditor::onDriverSelected() {
	HWND w = GetDlgItem(m_dialogBox, IDC_DRIVER);
	int lbItem = (int)SendMessage(w, CB_GETCURSEL, 0, 0);

	FloppyDiskBridge::BridgeDriver* info;
	if (handleGetDriverInfo(lbItem, &info)) {
		SetWindowTextA(GetDlgItem(m_dialogBox, IDC_URL), info->url);

		w = GetDlgItem(m_dialogBox, IDC_MODE);
		CommonBridgeTemplate::BridgeMode mode = (CommonBridgeTemplate::BridgeMode)SendMessage(w, CB_GETCURSEL, 0, 0);
		EnableWindow(GetDlgItem(m_dialogBox, IDC_AUTOCACHE), (info->configOptions & CONFIG_OPTIONS_AUTOCACHE) != 0);
		EnableWindow(GetDlgItem(m_dialogBox, IDC_SMART), ((info->configOptions & CONFIG_OPTIONS_SMARTSPEED) != 0) && ((mode == CommonBridgeTemplate::BridgeMode::bmFast) || (mode == CommonBridgeTemplate::BridgeMode::bmCompatible)));   // extra
		EnableWindow(GetDlgItem(m_dialogBox, IDC_AUTODETECT), (info->configOptions & CONFIG_OPTIONS_COMPORT_AUTODETECT) != 0);
		EnableWindow(GetDlgItem(m_dialogBox, IDC_COMPORT), ((info->configOptions & CONFIG_OPTIONS_COMPORT) != 0) && (SendMessage(GetDlgItem(m_dialogBox, IDC_AUTODETECT), BM_GETCHECK, 0,0)== BST_UNCHECKED));


		// Cable select
		w = GetDlgItem(m_dialogBox, IDC_CABLE);

		EnableWindow(w, (info->configOptions & (CONFIG_OPTIONS_DRIVE_AB | CONFIG_OPTIONS_DRIVE_123)) != 0);
		int i = SendMessage(w, CB_GETCURSEL, 0, 0);

		// Remove previous list
		while (SendMessage(w, CB_GETCOUNT, 0, 0) > 0)
			SendMessage(w, CB_DELETESTRING, 0, 0);

		int pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)L"IBMPC Drive as A"); SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, 0);
		pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)L"IBMPC Drive as B"); SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, 1);

		if (info->configOptions & CONFIG_OPTIONS_DRIVE_123) {
			pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)L"Shugart Drive 0"); SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, 1);
			pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)L"Shugart Drive 1"); SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, 1);
			pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)L"Shugart Drive 2"); SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, 1);
			pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)L"Shugart Drive 3"); SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, 1);
		}
		SendMessage(w, CB_SETCURSEL, (WPARAM)i, 0);
	}
}

// Init dialog
void BridgeProfileEditor::handleInitDialog(HWND hwnd) {
	m_dialogBox = hwnd;
	FloppyDiskBridge::BridgeDriver* driver;
	HWND w;
	
	// profile name
	w = GetDlgItem(hwnd, IDC_PROFILENAME);
	SendMessage(w, EM_SETLIMITTEXT, 120, 0);
	SetWindowTextA(w, m_profile->profileName);
	
	// Driver
	w = GetDlgItem(hwnd, IDC_DRIVER);
	for (unsigned int index = 0; index < MAX_NUM_DRIVERS; index++) {
		if (handleGetDriverInfo(index, &driver)) {
			LRESULT pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)driver->name);
			SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, (LPARAM)index);
		}
	}
	SendMessage(w, CB_SETCURSEL, m_profile->bridgeIndex, 0);

	w = GetDlgItem(hwnd, IDC_COMPORT);
	// Populate list of COM ports and select it
	for (unsigned int index = 0; index < m_portList.size(); index++) {
		LRESULT pos;
		if ((index == 0) && (m_notDetected)) {
			std::wstring tmp = m_portList[index].portName + L" (not detected)";
			pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)tmp.c_str());
		} else pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)m_portList[index].portName.c_str());
		SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, (LPARAM)index);
	}
	if (strlen(m_profile->comPortToUse)) {
		std::string port = m_profile->comPortToUse;
		std::wstring portW;
		quicka2w(port, portW);

		auto f = std::find_if(m_portList.begin(), m_portList.end(), [&portW](const SerialIO::SerialPortInformation& portsearch)->bool {
			return (portsearch.portName == portW);
		});
		if (f != m_portList.end()) SendMessage(w, CB_SETCURSEL, f-m_portList.begin(), 0);
	}

	// Auto-detect com port
	w = GetDlgItem(hwnd, IDC_AUTODETECT);
	SendMessage(w, BM_SETCHECK, m_profile->autoDetectComPort ? BST_CHECKED : BST_UNCHECKED, 0);

	// Mode
	w = GetDlgItem(hwnd, IDC_MODE);
	LPARAM pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)L"Normal"); SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, 0);
	pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)L"More Compatible (different rotation calculation)"); SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, 1);
	pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)L"Turbo (Very fast, may break copy protection)"); SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, 2);
	pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)L"Stalling (Accurate but will freeze the emulator)"); SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, 3);
	SendMessage(w, CB_SETCURSEL, (WPARAM)m_profile->bridgeMode, 0);

	// Smart Speed
	w = GetDlgItem(hwnd, IDC_SMART);
	SendMessage(w, BM_SETCHECK, m_profile->smartSpeed ? BST_CHECKED : BST_UNCHECKED, 0);

	// Auto-Cache
	w = GetDlgItem(hwnd, IDC_AUTOCACHE);
	SendMessage(w, BM_SETCHECK, m_profile->autoCache ? BST_CHECKED : BST_UNCHECKED, 0);

	// Disk Type
	w = GetDlgItem(hwnd, IDC_DISKTYPE);
	pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)L"Auto (automatically detect DD and HD disks)"); SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, 0);
	pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)L"DD (always detect all disks as double density)"); SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, 1);
	pos = SendMessage(w, CB_ADDSTRING, 0, (LPARAM)L"HD (always detect all disks as high density)"); SendMessage(w, CB_SETITEMDATA, (WPARAM)pos, 2);
	SendMessage(w, CB_SETCURSEL, (WPARAM)m_profile->bridgeDensity, 0);
	
	onDriverSelected();

	// Cable select
	w = GetDlgItem(m_dialogBox, IDC_CABLE);
	SendMessage(w, CB_SETCURSEL, (WPARAM)m_profile->driveCable, 0);
}




BridgeProfileEditor::~BridgeProfileEditor() {
	DeleteDC(m_hdcTemp);
	DestroyCursor(m_busyCursor);
	DestroyCursor(m_handPoint);
	DeleteObject(m_hNormalBackground);
	DeleteObject(m_hSelectedBackground);
	if (m_boldFont) DeleteObject(m_boldFont);
}

// Run the modal code, returns TRUE if OK was pressed.
bool BridgeProfileEditor::doModal() {
	return DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_PROFILEEDITOR), m_hwndParent, dialogCallback2, (LPARAM)this);
}

