#pragma once


#include <Windows.h>
#include <unordered_map>
#include <vector>

class BridgeConfig;

class BridgeProfileListEditor {
private:
	HINSTANCE m_hInstance;
	HWND m_hwndParent;
	std::unordered_map<unsigned int, BridgeConfig*>& m_profileList;
	std::vector<HBITMAP>& m_bridgeLogos;
	HCURSOR m_busyCursor, m_handPoint;
	HWND m_dialogBox = 0;
	HDC m_hdcTemp;
	HBITMAP m_patreon;
	HBRUSH m_hNormalBackground;
	HBRUSH m_hSelectedBackground;
	DWORD m_hNormalBackgroundColor, m_hSelectedBackgroundColor;
	DWORD m_hNormalTextColor, m_hSelectedTextColor;
	HFONT m_boldFont;


	// Init dialog
	void handleInitDialog(HWND hwnd);

	// Perform update check
	void doCheckForUpdates();

	// Create a new profile
	void handleCreateProfile();

	// Edit selected profile
	void handleEditSelectedProfile();

	// Delete selected profile
	void handleDeleteSelectedProfile();
	
	// URL clicked
	void handleURLClicked(bool isPatreon);

	// Listbox notifications
	void handleProfileListMessages(WPARAM wParam, LPARAM lParam);

	// OwnerDraw for listbox
	void handleDrawListBox(PDRAWITEMSTRUCT item);

	// Trigger redrawing the selected item
	void redrawSelectedItem();

	// Should check for updates?
	void setShouldCheckForUpdates(bool shouldCheck);
public:
	BridgeProfileListEditor(HINSTANCE hInstance, HWND hwndParent, std::vector<HBITMAP>& m_bridgeLogos, std::unordered_map<unsigned int, BridgeConfig*>& profileList);
	~BridgeProfileListEditor();

	// Run the modal code, returns TRUE if OK was pressed.
	bool doModal();

	// Dialog window message handler
	INT_PTR handleDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Returns TRUE if we shoudl auto-check for updates
	static bool shouldAutoCheckForUpdates();
};