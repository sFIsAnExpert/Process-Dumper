#include "ProcessDumper.h"

void ProcessDumper::Process::Kill()
{
	DWORD exitCode = 0;
	if (GetExitCodeProcess(ProcessDumper::ProcessHandle, &exitCode) != 0)
		TerminateProcess(ProcessDumper::ProcessHandle, exitCode);
	ProcessDumper::Deconstruct();
}

void ProcessDumper::Information::InitList()
{
    int i = 0;
    for (Module mod : this->ModuleList()) {
       // std::cout << mod.Name << "\n";
        const char* ptr = mod.Name.c_str();
        char* new_str = new char[std::strlen(ptr) + 1];
        std::strcpy(new_str, ptr);
        List[i] = new_str;
        i += 1;
    }
    for (int b = 0; b < 1024; b++) {
        if (List[b] == 0) {
            ListSize = b;
            break;
        }
    }
}

const char* ProcessDumper::Information::ModuleByIndex(int size)
{
    return List[size];
}

ProcessDumper::Module ProcessDumper::Information::MainModule()
{
    Module mod; char moduleName[255]; std::wstring wModName; std::string tmp;
    if (ProcessDumper::ProcessHandle != nullptr)
        K32GetModuleBaseNameA(ProcessDumper::ProcessHandle, 0, moduleName, 255);
    tmp = std::string(moduleName); wModName = std::wstring(tmp.begin(), tmp.end());
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessDumper::ProcessID);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32W modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32FirstW(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, wModName.c_str())) {
                    std::wstring wPath = std::wstring(modEntry.szExePath);
                    mod.Base = (uintptr_t)modEntry.modBaseAddr;
                    mod.Handle = modEntry.hModule;
                    mod.ID = modEntry.th32ModuleID;
                    mod.Name = tmp;
                    mod.Path = std::string(wPath.begin(), wPath.end());
                    mod.Size = modEntry.modBaseSize;
                }

            } while (Module32NextW(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return mod;
}

ProcessDumper::Module ProcessDumper::Information::ModuleByName(const char* name)
{
    for (Module mod : this->ModuleList()) {
        if (strcmp(mod.Name.c_str(), name) == 0)
            return mod;
    }
    return Module();
}

std::vector<ProcessDumper::Module> ProcessDumper::Information::ModuleList()
{
    std::vector<Module> list = {}; 
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessDumper::ProcessID);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32W modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32FirstW(hSnap, &modEntry))
        {
            do
            {
                std::wstring mName = std::wstring(modEntry.szModule);
                std::wstring pName = std::wstring(modEntry.szExePath);
                Module pModule =
                   Module(std::string(mName.begin(), mName.end()), std::string(pName.begin(), pName.end()), modEntry.hModule, (uintptr_t)modEntry.modBaseAddr, modEntry.modBaseSize, modEntry.th32ModuleID);
                list.push_back(pModule);
            } while (Module32NextW(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);

	return list;
}

void UnloadForceDLL(HMODULE hMod) {
    while (!FreeLibrary(hMod)) {
        FreeLibrary(hMod);
    }
}

void ProcessDumper::Module::Unload()
{
    if (this->Handle != nullptr) {
        LPVOID Base = GetProcAddress(GetModuleHandleA("kernel32.dll"), "FreeLibrary");
        CreateRemoteThreadEx(ProcessDumper::ProcessHandle, 0, 0, (LPTHREAD_START_ROUTINE)Base, this->Handle, 0, 0, 0);
    }
}
