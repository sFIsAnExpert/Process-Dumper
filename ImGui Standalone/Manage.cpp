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
		else Tab = TAB::DEFAULT;
	}
	if (bWindowFound) {
		ImGui::SameLine();
		if (ImGui::Button("Dump", ImVec2(125, 25))) {
			if (Tab != TAB::DUMP)
				Tab = TAB::DUMP;
			else Tab = TAB::DEFAULT;
		}
		ImGui::SameLine();
		if (ImGui::Button("Module", ImVec2(125, 25))) {
			if (Tab != TAB::MODULE)
				Tab = TAB::MODULE;
			else Tab = TAB::DEFAULT;
		}
	}
	if (Tab == TAB::DEFAULT) {
		ImGui::TextColored(ImColor(44,239,245), "Process Dumper by ChasePlays");
		ImGui::TextColored(ImColor(44,239,245), "Repository -"); ImGui::SameLine();
		ImGui::TextColored(ImColor(0, 150, 200), "https://github.com/sFIsAnExpert/Process-Dumper");
		ImGui::NewLine();
		ImGui::BulletText("HOW TO USE");
		ImGui::TextColored(ImColor(219, 0, 0), "-Click Load Process");
		ImGui::NewLine();
		ImGui::TextColored(ImColor(240, 150, 70), "-Enter Window Name Or Load Process From List");
		ImGui::NewLine();
		ImGui::TextColored(ImColor(79, 162, 219), "-Profit??");
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
		ImGui::Text("Path"); ImGui::SameLine();
		ImGui::Text(mod.Path.c_str()); ImGui::SameLine();
		if (ImGui::SmallButton("Open")) {
			std::string path = mod.Path;
			path = path.substr(0, path.find_last_of("\\") + 1);
			ShellExecuteA(0, "open", path.c_str(), 0, 0, SW_SHOWDEFAULT);
		}
		ImGui::Text("Address:"); ImGui::SameLine();
		ImGui::Text(std::format("{:x}", mod.Base).c_str());
		if(ImGui::Button("Unload", ImVec2(80, 25))){
			mod.Unload();
			bDumpOnce = false;
		}
	}
	if (ProcessDumper::Failed || bWindowFound && Dump.Initiated && Dump.ProcessHandle == nullptr || bWindowFound && Dump.Initiated && WindowHandle == nullptr) {
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