#pragma once


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