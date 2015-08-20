#include "keyhook.hpp"
#include "keyentry.hpp"
#include <map>
#include <iostream>
#include <thread>
#include <memory>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

std::map<KeyEntry, KeyPressedCallback> g_registeredKeys;
std::function<void(bool)> g_keyboardHookedCallback;
HHOOK g_hook = nullptr;

LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode != HC_ACTION)
		return CallNextHookEx(g_hook, nCode, wParam, lParam);

	std::cout << __FUNCTION__ << std::endl;
	return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

bool isKeyRegistered(const std::string& key) {
	try {
		KeyEntry entry(key);
		for (const auto& i : g_registeredKeys)
			if (i.first == entry)
				return true;
	}
	catch (...) {
		
	}

	return false;
}

bool registerKey(const std::string& key, std::function<bool()> callback) {
	try {
		KeyEntry entry(key);

		return true;
	}
	catch (...) {
		return false;
	}
}

bool unregisterKey(const std::string& key) {
	try {
		KeyEntry entry(key);

		return true;
	}
	catch (...) {
		return false;
	}
}

bool initializeKeyhook() {
	std::thread([=](){
		if (g_hook != nullptr) {
			if (g_keyboardHookedCallback)
				g_keyboardHookedCallback(false);
			return;
		}

		g_hook = SetWindowsHookExA(WH_KEYBOARD_LL, lowLevelKeyboardProc, GetModuleHandle(0), 0);

		if (g_keyboardHookedCallback)
			g_keyboardHookedCallback(g_hook != nullptr);

		MSG msg;
		while (GetMessageA(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	}).detach();

	return true;
}

bool destroyKeyhook() {
	if (g_hook == nullptr)
		return false;

	return UnhookWindowsHookEx(g_hook) == TRUE;
}

void setKeyboardHookedCallback(std::function<void(bool)> callback) {
	g_keyboardHookedCallback = callback;
}

void unsetKeyboardHookedCallback() {
	g_keyboardHookedCallback = nullptr;
}
