#ifndef DRAWING_H
#define DRAWING_H

#include "pch.h"

class Drawing
{
private:
	static LPCSTR lpWindowName;
	static ImVec2 vWindowSize;
	static ImGuiWindowFlags WindowFlags;
	static bool bDraw;

public:
	static void Active();
	static bool isActive();
	static void Draw();
};

enum TAB {
	DEFAULT = 0,
	LOADPROCESS = 1,
	DUMP = 2,
	LIST = 3,
	MODULE = 4
};

namespace Globals {
	static inline signed int iDelay = 500;
	static inline int Tab = 0, cModItem = 0;
	static inline bool bTest = false;
	static inline bool bList = false, bWindowFound = false, bWindowNotFound = false, bDumpOnce = false, bDisconnected = false;
	static inline char WindowName[255] = "Enter Window Name...";
	static inline HWND WindowHandle = nullptr;
	static inline ProcessDumper Dump;
};

#endif
