// MagMaestro.cpp : main project file.

#include "stdafx.h"
#include "Form1.h"

using namespace MagMaestro;

	//[DllImport( "kernel32.dll" )]
	//static bool AttachConsole( int dwProcessId );
	//const int ATTACH_PARENT_PROCESS = -1;
	

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	AttachConsole(-1);
	Console::WriteLine("");
	Console::WriteLine("MagMaestro.cpp - main() entered");

	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Console::WriteLine("MagMaestro.cpp - (1/2) Visual Styles Enabled");
	Application::SetCompatibleTextRenderingDefault(false); 
	Console::WriteLine("MagMaestro.cpp - (2/2) Set Compatible Text Rendering Default false");

	// Create the main window and run it
	Console::WriteLine("MagMaestro.cpp - Running Form...");
	Application::Run(gcnew Form1());
	Console::WriteLine("MagMaestro.cpp - Closed Form");
	return 0;
}
