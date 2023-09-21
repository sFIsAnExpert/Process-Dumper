#include "Manage.h"

using namespace Globals;

void Deconstruct() {
	WindowHandle = nullptr;
	bWindowFound = false;
	bDumpOnce = false;
	bWindowFound = false;
	cModItem = 0;
	ProcessDumper::Initiated = false;
	ProcessDumper::Failed = false;
}

void ManageGUI() {
	if (ImGui::Button("Load Process", ImVec2(125, 25))) {
		if (Tab != TAB::LOADPROCESS)
			Tab = TAB::LOADPROCESS;
		else Tab = 0;
	}
	if (bWindowFound) {
		ImGui::SameLine();
		if (ImGui::Button("Dump", ImVec2(125, 25))) {
			if (Tab != TAB::DUMP)
				Tab = TAB::DUMP;
			else Tab = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Module", ImVec2(125, 25))) {
			if (Tab != TAB::MODULE)
				Tab = TAB::MODULE;
			else Tab = 0;
		}
	}
	if (Tab == TAB::LOADPROCESS) {
		if (ImGui::InputText("Window Name", WindowName, 255, ImGuiInputTextFlags_EnterReturnsTrue)) {
			WindowHandle = FindWindowA(0, WindowName);
			if (WindowHandle)
				bWindowFound = true;
			else
				bWindowNotFound = true;
		}
		ImGui::NewLine();
		if (bWindowFound) {
			ImGui::Text("Found Window -"); ImGui::SameLine();
			ImGui::Text(WindowName);
			ImGui::Text("Check Dump Tab For Information!");
			if (ImGui::Button("Reset", ImVec2(80, 25))) {
				Deconstruct();
			}
		}
		if (bWindowNotFound && !bWindowFound)
			ImGui::Text("Window Not Found!");
	}
	if (Tab == TAB::DUMP) {
		ImGui::ListBox("Module List", &cModItem, Dump.Information.List, Dump.Information.ListSize);
		ImGui::NewLine();
		if(ImGui::Button("Reload List", ImVec2(95, 25))){
			bDumpOnce = false;
		}
	}
	if (Tab == TAB::MODULE) {
		ProcessDumper::Module mod = Dump.Information.ModuleByName(Dump.Information.ModuleByIndex(cModItem));
		ImGui::Text("Name:"); ImGui::SameLine();
		ImGui::Text(mod.Name.c_str());
		if(ImGui::Button("Unload", ImVec2(80, 25))){
			mod.Unload();
			bDumpOnce = false;
		}
	}
	if (bWindowFound && Dump.Initiated && Dump.ProcessHandle == nullptr || bWindowFound && Dump.Initiated && WindowHandle == nullptr) {
		Tab = TAB::LOADPROCESS;
		Deconstruct();
	}
	if (ProcessDumper::Failed) {
		Tab = TAB::LOADPROCESS;
		Deconstruct();
	}
	if (bWindowFound) {
		if (!bDumpOnce) {
			bDumpOnce = true;
			Dump = ProcessDumper(WindowName);
			Dump.Information.InitList();
		}
	}
	if (ImGui::Delay(iDelay)) {
		ImGui::SetSeconds(iDelay, 1);
		bDumpOnce = false;
	}
}