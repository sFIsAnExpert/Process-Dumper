#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <iostream>
#include <TlHelp32.h>
#include <vector>
#include <Psapi.h>

struct ProcessDumper {
public:
	static inline bool Initiated = false, Failed = false;
	static inline HWND WindowHandle = nullptr;
	static inline DWORD ProcessID = 0;
	static inline HANDLE ProcessHandle = nullptr;

	struct Process {
	public:
		void Kill();
		
	};

	struct Memory {
	public:
		template<typename T> T ReadMemory(uintptr_t address) {
			T buffer;
			if (ProcessDumper::ProcessHandle != nullptr)
				ReadProcessMemory(ProcessDumper::ProcessHandle, (LPVOID)address, &buffer, sizeof(buffer), nullptr);

			return buffer;
		}
		template<typename T> T WriteMemory(uintptr_t address, int val) {
			T buffer = (T)val;
			if (ProcessDumper::ProcessHandle != nullptr)
				WriteProcessMemory(ProcessDumper::ProcessHandle, (LPVOID)address, &buffer, sizeof(buffer), nullptr);

			return buffer;
		}

	};

	struct Module {
	public:
		std::string Name = "", Path = "";
		HMODULE Handle = nullptr;
		uintptr_t Base = 0x0;
		DWORD Size = 0, ID = 0;
		void Unload();
		Module() = default;
		Module(std::string mName, std::string pName, HMODULE mH, uintptr_t bAddr, DWORD size, DWORD id) {
			this->Name = mName; this->Path = pName; this->Handle = mH; this->Base = bAddr; this->Size = size; this->ID = id;
		}
	};

	struct Information {
	public:
		void InitList();
		static inline const char* List[1024];
		static inline int ListSize = 0;
		const char* ModuleByIndex(int size);
		Module MainModule();
		Module ModuleByName(const char* name);
		std::vector<Module> ModuleList();
		Information() = default;
	};

	ProcessDumper() = default;
	ProcessDumper(const char* wName) {
		this->WindowHandle = FindWindowA(0, wName);
		if (!this->WindowHandle) {
			std::cout << "FindWindowA Failed!\n";
			Failed = true;
		}
		if (this->WindowHandle != nullptr)
			GetWindowThreadProcessId(this->WindowHandle, &this->ProcessID);
		else
			Failed = true;
		if (this->ProcessID == 0) {
			std::cout << "Failed to Gather Process ID\n";
			Failed = true;
		}
		this->ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, true, this->ProcessID);
		if (this->ProcessHandle == nullptr) {
			std::cout << "OpenProcess Failed\n";
			this->Failed = true;
		}
		else
			this->Initiated = true;
	}
	static void Deconstruct() {
		WindowHandle = nullptr;
		ProcessID = 0;
		ProcessHandle = nullptr;
	}
	Information Information;
	Process Process;
    Memory Memory;
};