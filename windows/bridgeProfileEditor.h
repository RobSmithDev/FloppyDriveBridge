#pragma once
/* Bridge Profile Editor (Windows)
*
* Copyright (C) 2021-2024 Robert Smith (@RobSmithDev)
* https://amiga.robsmithdev.co.uk
*
* This file is multi-licensed under the terms of the Mozilla Public
* License Version 2.0 as published by Mozilla Corporation and the
* GNU General Public License, version 2 or later, as published by the
* Free Software Foundation.
*
* MPL2: https://www.mozilla.org/en-US/MPL/2.0/
* GPL2: https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html
*
* This file, along with currently active and supported interfaces
* are maintained from by GitHub repo at
* https://github.com/RobSmithDev/FloppyDriveBridge
*/

#include <Windows.h>
#include <vector>
#include "../floppybridge/SerialIO.h"

class BridgeConfig;

class BridgeProfileEditor {
private:
	HINSTANCE m_hInstance;
	HWND m_hwndParent;
	BridgeConfig* m_profile;
	std::vector<HBITMAP>& m_bridgeLogos;
	HCURSOR m_busyCursor, m_handPoint;
	HWND m_dialogBox;
	HDC m_hdcTemp;
	HBRUSH m_hNormalBackground;
	HBRUSH m_hSelectedBackground;
	DWORD m_hNormalBackgroundColor, m_hSelectedBackgroundColor;
	DWORD m_hNormalTextColor, m_hSelectedTextColor;
	HFONT m_boldFont;
	bool m_notDetected;

	std::vector<SerialIO::SerialPortInformation> m_portList;

	// Init dialog
	void handleInitDialog(HWND hwnd);

	void handleDrawListBox(PDRAWITEMSTRUCT item);

	void onDriverSelected();

	void handleURLClicked();

	void handleNameChange();

	void saveToProfile();

public:
	BridgeProfileEditor(HINSTANCE hInstance, HWND hwndParent, std::vector<HBITMAP>& bridgeLogos, BridgeConfig* profile);
	~BridgeProfileEditor();

	// Run the modal code, returns TRUE if OK was pressed.
	bool doModal();

	// Dialog window message handler
	INT_PTR handleDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};