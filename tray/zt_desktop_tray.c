/* This builds the tray and some assorted utility functions so that it can be included from Rust code. */

#if defined (_WIN32) || defined (_WIN64)
#define TRAY_WINAPI 1
#elif defined (__linux__) || defined (linux) || defined (__linux)
#define TRAY_APPINDICATOR 1
#elif defined (__APPLE__) || defined (__MACH__)
#define TRAY_APPKIT 1
#endif

#include "tray.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <pthread.h>
#include <pthread/qos.h>
#include <sys/qos.h>
extern void c_set_this_thread_to_background_priority()
{
	pthread_set_qos_class_self_np(QOS_CLASS_BACKGROUND, 0);
}
extern void c_set_this_thread_to_foreground_priority()
{
	pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
}
#endif

#if defined (_WIN32) || defined (_WIN64)
#define RELEASEKEY(Key) if (Key) RegCloseKey(Key);
extern int c_windows_is_dark_theme()
{
	HKEY hKey = NULL;
	DWORD Value1 = 0;
	DWORD Value2 = 0;
	DWORD size = 0;
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey)) {
		return 0;
	}
	if (RegQueryValueExA(hKey, "AppsUseLightTheme", NULL, NULL, (LPBYTE)&Value1, &size)) {
		if (RegQueryValueExA(hKey, "SystemUsesLightTheme", NULL, NULL, (LPBYTE)&Value2, &size)) {
			RELEASEKEY(hKey);
			return 0;
		}
	}
	RELEASEKEY(hKey);
	return ((Value1 | Value2) == 0) ? 1 : 0;
}
extern void c_windows_post_to_clipboard(const char *const data)
{
	if (!data)
		return;
	const size_t len = strlen(data) + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), data, len);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}
#endif
