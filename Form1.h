#pragma once
#include "resource.h"
#include <string>
#include <iostream>
#include <cstring>
#include <Windows.h>
#include <setupapi.h>
#include <Winusb.h>	
#include <Xinput.h>
#define MY_DEVICE_ID  "Vid_04d8&Pid_000C"	

namespace MagMaestro {

	using namespace System;
	using namespace System::IO;
	using namespace System::Resources;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;  //Need this to support "unmanaged" code.
#ifdef UNICODE
#define	Seeifdef	Unicode
#else
#define Seeifdef	Ansi
#endif

	//Returns a HDEVINFO type for a device information set (WinUSB devices in
	//our case).  We will need the HDEVINFO as in input parameter for calling many of
	//the other SetupDixxx() functions.
	[DllImport("setupapi.dll", CharSet = CharSet::Seeifdef, EntryPoint = "SetupDiGetClassDevs", CallingConvention = CallingConvention::Winapi)]
	extern "C" HDEVINFO  SetupDiGetClassDevsUM(
		LPGUID  ClassGuid,					//Input: Supply the class GUID here. 
		PCTSTR  Enumerator,					//Input: Use NULL here, not important for our purposes
		HWND  hwndParent,					//Input: Use NULL here, not important for our purposes
		DWORD  Flags);						//Input: Flags describing what kind of filtering to use.

	//Gives us "PSP_DEVICE_INTERFACE_DATA" which contains the Interface specific GUID (different
	//from class GUID).  We need the interface GUID to get the device path.
	[DllImport("setupapi.dll", CharSet = CharSet::Seeifdef, EntryPoint = "SetupDiEnumDeviceInterfaces", CallingConvention = CallingConvention::Winapi)]
	extern "C" WINSETUPAPI BOOL WINAPI  SetupDiEnumDeviceInterfacesUM(
		HDEVINFO  DeviceInfoSet,			//Input: Give it the HDEVINFO we got from SetupDiGetClassDevs()
		PSP_DEVINFO_DATA  DeviceInfoData,	//Input (optional)
		LPGUID  InterfaceClassGuid,			//Input 
		DWORD  MemberIndex,					//Input: "Index" of the device you are interested in getting the path for.
		PSP_DEVICE_INTERFACE_DATA  DeviceInterfaceData);//Output: This function fills in an "SP_DEVICE_INTERFACE_DATA" structure.

	//SetupDiDestroyDeviceInfoList() frees up memory by destroying a DeviceInfoList
	[DllImport("setupapi.dll", CharSet = CharSet::Seeifdef, EntryPoint = "SetupDiDestroyDeviceInfoList", CallingConvention = CallingConvention::Winapi)]
	extern "C" WINSETUPAPI BOOL WINAPI  SetupDiDestroyDeviceInfoListUM(
		HDEVINFO  DeviceInfoSet);			//Input: Give it a handle to a device info list to deallocate from RAM.

	//SetupDiEnumDeviceInfo() fills in an "SP_DEVINFO_DATA" structure, which we need for SetupDiGetDeviceRegistryProperty()
	[DllImport("setupapi.dll", CharSet = CharSet::Seeifdef, EntryPoint = "SetupDiEnumDeviceInfo", CallingConvention = CallingConvention::Winapi)]
	extern "C" WINSETUPAPI BOOL WINAPI  SetupDiEnumDeviceInfoUM(
		HDEVINFO  DeviceInfoSet,
		DWORD  MemberIndex,
		PSP_DEVINFO_DATA  DeviceInfoData);

	//SetupDiGetDeviceRegistryProperty() gives us the hardware ID, which we use to check to see if it has matching VID/PID
	[DllImport("setupapi.dll", CharSet = CharSet::Seeifdef, EntryPoint = "SetupDiGetDeviceRegistryProperty", CallingConvention = CallingConvention::Winapi)]
	extern "C"	WINSETUPAPI BOOL WINAPI  SetupDiGetDeviceRegistryPropertyUM(
		HDEVINFO  DeviceInfoSet,
		PSP_DEVINFO_DATA  DeviceInfoData,
		DWORD  Property,
		PDWORD  PropertyRegDataType,
		PBYTE  PropertyBuffer,
		DWORD  PropertyBufferSize,
		PDWORD  RequiredSize);

	//SetupDiGetDeviceInterfaceDetail() gives us a device path, which is needed before CreateFile() can be used.
	[DllImport("setupapi.dll", CharSet = CharSet::Seeifdef, EntryPoint = "SetupDiGetDeviceInterfaceDetail", CallingConvention = CallingConvention::Winapi)]
	extern "C" BOOL SetupDiGetDeviceInterfaceDetailUM(
		HDEVINFO DeviceInfoSet,										//Input: Wants HDEVINFO which can be obtained from SetupDiGetClassDevs()
		PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,				//Input: Pointer to an structure which defines the device interface.  
		PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData,	//Output: Pointer to a strucutre, which will contain the device path.
		DWORD DeviceInterfaceDetailDataSize,						//Input: Number of bytes to retrieve.
		PDWORD RequiredSize,										//Output (optional): Te number of bytes needed to hold the entire struct 
		PSP_DEVINFO_DATA DeviceInfoData);							//Output

	//WinUsb_Initialize() needs to be called before the application can begin sending/receiving data with the USB device.
	[DllImport("winusb.dll", CharSet = CharSet::Seeifdef, EntryPoint = "WinUsb_Initialize", CallingConvention = CallingConvention::Winapi)]
	extern "C" BOOL WinUsb_Initialize(
		HANDLE	DeviceHandle,
		PWINUSB_INTERFACE_HANDLE InterfaceHandle);

	//WinUsb_Free() will deenumerate the USB device.
	[DllImport("winusb.dll", CharSet = CharSet::Seeifdef, EntryPoint = "WinUsb_Free", CallingConvention = CallingConvention::Winapi)]
	extern "C" BOOL WinUsb_Free(
		WINUSB_INTERFACE_HANDLE InterfaceHandle);

	//WinUsb_WritePipe() is the basic function used to write data to the USB device (sends data to OUT endpoints on the device)
	[DllImport("winusb.dll", CharSet = CharSet::Seeifdef, EntryPoint = "WinUsb_WritePipe", CallingConvention = CallingConvention::Winapi)]
	extern "C" BOOL WinUsb_WritePipe(
		WINUSB_INTERFACE_HANDLE InterfaceHandle,
		UCHAR PipeID,
		PUCHAR Buffer,
		ULONG BufferLength,
		PULONG LengthTransferred,
		LPOVERLAPPED Overlapped);	
	
	[DllImport("winusb.dll", CharSet = CharSet::Seeifdef, EntryPoint = "WinUsb_SetPipePolicy", CallingConvention = CallingConvention::Winapi)]
	extern "C" BOOL WinUsb_SetPipePolicy(
		WINUSB_INTERFACE_HANDLE InterfaceHandle,
		UCHAR                   PipeID,
		ULONG                   PolicyType,
		ULONG                   ValueLength,
		PVOID                   Value
	);

	//WinUsb_ReadPipe() is the basic function used to read data from the USB device (polls for and obtains data from
	//IN endpoints on the device)
	[DllImport("winusb.dll", CharSet = CharSet::Seeifdef, EntryPoint = "WinUsb_ReadPipe", CallingConvention = CallingConvention::Winapi)]
	extern "C" BOOL WinUsb_ReadPipe(
		WINUSB_INTERFACE_HANDLE InterfaceHandle,
		UCHAR PipeID,
		PUCHAR Buffer,
		ULONG BufferLength,
		PULONG LengthTransferred,
		LPOVERLAPPED Overlapped);

	[DllImport("winusb.dll", CharSet = CharSet::Seeifdef, EntryPoint = "WinUsb_FlushPipe", CallingConvention = CallingConvention::Winapi)]
	extern "C" BOOL WinUsb_FlushPipe(
		WINUSB_INTERFACE_HANDLE InterfaceHandle,
		UCHAR PipeID);



	//  Variables that need to have wide scope.
	HANDLE LodestoneHandle = INVALID_HANDLE_VALUE;		//First need to get the Device handle
	WINUSB_INTERFACE_HANDLE LodestoneInterfaceHandle;	//And then can call WinUsb_Initialize() to get the interface handle
														//which is needed for doing other operations with the device (like
														//reading and writing to the USB device).
//-------------------------------------------------------END CUT AND PASTE BLOCK-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------



	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>

	public ref class Form1 : public System::Windows::Forms::Form
	{
	public: static System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));


	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape46;
	public:
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape45;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape44;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape43;









	private: System::Windows::Forms::TabPage^  macroTab;
	private: System::Windows::Forms::DataGridView^  macroGrid;

	private: System::Windows::Forms::Button^  loadMacroButton;
	private: System::Windows::Forms::Button^  saveMacroButton;
	private: System::Windows::Forms::TextBox^  macroNameBox;
	private: System::Windows::Forms::CheckBox^  loopCheckBox;
	private: System::Windows::Forms::Button^  runMacroButton;
	private: System::Windows::Forms::SaveFileDialog^  saveMacroFileDialog;
	private: System::Windows::Forms::OpenFileDialog^  loadMacroFileDialog;
	private: System::Windows::Forms::DataGridViewComboBoxColumn^  Action;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Duration;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Angle;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Revolutions;
	private: System::Windows::Forms::DataGridViewCheckBoxColumn^  CCW;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Inplane;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Outplane;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Frequency;
	private: System::Windows::Forms::DataGridViewComboBoxColumn^  Direction;

	private: System::Windows::Forms::CheckBox^  precSuppCheck;
	private: Microsoft::VisualBasic::PowerPacks::RectangleShape^  VM_back;
	private: Microsoft::VisualBasic::PowerPacks::RectangleShape^  VM_fore;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  voltNeedLine;
	private: System::Windows::Forms::PictureBox^  splashLogo;
	private: Microsoft::VisualBasic::PowerPacks::RectangleShape^  rectangleShape5;
	private: System::Windows::Forms::Button^  loadConfigButton;
	private: System::Windows::Forms::Button^  saveConfigButton;
	private: System::Windows::Forms::TextBox^  configNameBox;
	private: System::Windows::Forms::SaveFileDialog^  saveConfigFileDialog;
	private: System::Windows::Forms::OpenFileDialog^  loadConfigFileDialog;
	private: System::Windows::Forms::StatusStrip^  statusStrip;
	private: System::Windows::Forms::ToolStripStatusLabel^  statusLabel;


	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape42;

	public:
		Form1(void)
		{
			Console::WriteLine("Form1.h - Began to Initialize Form");
			InitializeComponent();
			confirmBeep = gcnew System::Media::SoundPlayer(cli::safe_cast<System::IO::Stream^>(resources->GetObject("confirmBeep")));
			xboxStatusPic->Image = safe_cast<Image^>(resources->GetObject("xbox360logo gray"));
			APic->Image = safe_cast<Image^>(resources->GetObject("Abutton gray"));
			BPic->Image = safe_cast<Image^>(resources->GetObject("Bbutton gray"));
			XPic->Image = safe_cast<Image^>(resources->GetObject("Xbutton gray"));
			YPic->Image = safe_cast<Image^>(resources->GetObject("Ybutton gray"));
			RBPic->Image = safe_cast<Image^>(resources->GetObject("RBbutton"));
			LBPic->Image = safe_cast<Image^>(resources->GetObject("LBbutton"));
			RSPic->Image = safe_cast<Image^>(resources->GetObject("RSbutton"));
			LSPic->Image = safe_cast<Image^>(resources->GetObject("LSbutton"));
			RTPic->Image = safe_cast<Image^>(resources->GetObject("RT"));
			LTPic->Image = safe_cast<Image^>(resources->GetObject("LT"));
			StartPic->Image = safe_cast<Image^>(resources->GetObject("Startbutton"));
			BackPic->Image = safe_cast<Image^>(resources->GetObject("Backbutton"));
			splashLogo->Image = safe_cast<Image^>(resources->GetObject("Splash"));

			Console::WriteLine("Form1.h - Finished Initializing Form");
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  Connect_btn;
	protected:

	public:  static float              voltage = 0;
	public:  static unsigned int   voltageDisp = 0;
	public:  static unsigned int voltReadFails = 0;
	public:  static float             currentX = 0;
	public:  static float             currentY = 0;
	public:  static float             currentZ = 0;
	public:  static float              X_OeToA = 0;
	public:  static float              Y_OeToA = 0;
	public:  static float              Z_OeToA = 0;
	public:  static float                X_Max = 0;
	public:  static float                Y_Max = 0;
	public:  static float                Z_Max = 0;
	public:  static Boolean		configLoadFlag = false;
	public:  static Boolean		configSaveFlag = false;

	public:  static Decimal             precIP = 0;
	public:  static Decimal             precOP = 0;
	public:  static Decimal         precPeriod = 1;
	public:  static Boolean         precessing = 0;

	public:  static Decimal             rollIP = 0;
	public:  static Decimal             rollOP = 0;
	public:  static Decimal         rollPeriod = 1;
	public:  static long           rollCounter = 0;
	public:  static double           rollAngle = 0;
	public:  static Boolean            rolling = 0;

	public:  static Decimal				diskIP = 0;
	public:  static Decimal				diskOP = 0;
	public:  static Decimal         diskPeriod = 1;
	public:  static Decimal            hopTime = 0;
	public:  static double          hopCounter = 0;
	public:  static char			   hopping = 0;
	public:  static Boolean		       disking = 0;
	public:  static int			   diskHopping = 0;
	public:  static double          startAngle = 0;
	public:  static double            endAngle = 0;

	public:  static Decimal			      QPIP = 0;
	public:  static Decimal			      QPOP = 0;
	public:  static Decimal           QPPeriod = 1;

	public:  static Decimal				xboxIP = 0;
	public:  static Decimal				xboxOP = 0;
	public:  static Boolean			  xboxZDir = 0;
	public:  static int				 xboxState = 0; //0-Invisible controls, 1-No Controller, 2-Controller no tab, 3-Controller and tab
	public:  static Boolean		  xboxZeroLast = 0;
	public:  static unsigned int      Acounter = 0;
	public:  static unsigned int      Bcounter = 0;
	public:  static unsigned int      Xcounter = 0;
	public:  static unsigned int      Ycounter = 0;
	public:  static unsigned int Startcooldown = 0;
	public:  static unsigned int  Backcooldown = 0;
	public:  static unsigned int    RBcooldown = 0;
	public:  static unsigned int    LBcooldown = 0;
	public:  static unsigned int  TRIGcooldown = 0;

	public:  static double            outAngle = 0;
	public:  static double                 d2r = 3.14159265359 / 180;

	public:  static Boolean	 	  mainLoopFlag = false;
	public:  static bool		   isConnected = false;
	public:  static bool		   isLegacy = false;
	public:  static String^		  commandStack = "";

	public:  static Boolean			  macroing = 0;
	public:  static int			  currMacroRow = 0;
	public:  static String^	  currMacroCommand = "";
	public:  static double		 macroStartMin = 0;
	public:  static double		   macroStartS = 0;
	public:  static double		  macroStartMS = 0;
	public:  static double		   macroEndMin = 0;
	public:  static double		     macroEndS = 0;
	public:  static double		    macroEndMS = 0;
	public:  static Boolean		 macroLoadFlag = false;
	public:  static Boolean		 macroSaveFlag = false;

	public:  static double	 		   startMS = 0;
	public:  static double              startS = 0;
	public:  static double            startMin = 0;
	public:  static double              lastMS = 0;
	public:  static double             probeMS = 0;
	public:  static double			   testMS1 = 0;
	public:  static double			   testMS2 = 0;

			 //public:  static IO::MemoryStream^ confirmBeep;
	public:  static System::Media::SoundPlayer^	confirmBeep;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  SendPosButt;

	private: System::Windows::Forms::TextBox^  textBox_Z;
	private: System::Windows::Forms::TextBox^  textBox_Y;
	private: System::Windows::Forms::TextBox^  textBox_X;
	private: Microsoft::VisualBasic::PowerPacks::ShapeContainer^  shapeContainer1;
	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  statusLight;
	private: System::Windows::Forms::TabControl^  tabControl1;



	private: System::Windows::Forms::TabPage^  setupTab;



	private: System::Windows::Forms::TabPage^  constantTab;

	private: System::Windows::Forms::Label^  voltDisplay;
	private: System::Windows::Forms::Label^  Setup_X;

	private: System::Windows::Forms::TextBox^  textBoxORZ;

	private: System::Windows::Forms::TextBox^  textBoxOFZ;

	private: System::Windows::Forms::TextBox^  textBoxORY;

	private: System::Windows::Forms::TextBox^  textBoxOFY;

	private: System::Windows::Forms::TextBox^  textBoxORX;

	private: System::Windows::Forms::TextBox^  textBoxOFX;

	private: System::Windows::Forms::TextBox^  textBoxRZ;

	private: System::Windows::Forms::TextBox^  textBoxRY;



	private: System::Windows::Forms::TextBox^  textBoxRX;





	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::Label^  label30;
	private: System::Windows::Forms::Label^  labelCMFZ;

	private: System::Windows::Forms::Label^  labelCMFY;

	private: System::Windows::Forms::Label^  labelCMFX;





	private: System::Windows::Forms::TextBox^  textBoxMFZ;







	private: System::Windows::Forms::TextBox^  textBoxMFY;

	private: System::Windows::Forms::TextBox^  textBoxMFX;

	private: System::Windows::Forms::Label^  label16;

	private: System::Windows::Forms::Label^  label4;
	private: Microsoft::VisualBasic::PowerPacks::ShapeContainer^  shapeContainer2;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape6;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape5;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape4;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape3;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape2;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape1;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape7;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape10;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape9;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape8;
	private: System::Windows::Forms::TextBox^  textBoxOAZ;
	private: System::Windows::Forms::TextBox^  textBoxOAY;
	private: System::Windows::Forms::TextBox^  textBoxOAX;
	private: System::Windows::Forms::Label^  label5;

	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape11;
	private: System::Windows::Forms::Label^  label17;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::TextBox^  textBoxOPF;
	private: System::Windows::Forms::TextBox^  textBoxIPF;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::CheckBox^  checkBoxZR;
	private: System::Windows::Forms::CheckBox^  checkBoxZF;
	private: System::Windows::Forms::CheckBox^  checkBoxYR;
	private: System::Windows::Forms::CheckBox^  checkBoxYF;
	private: System::Windows::Forms::CheckBox^  checkBoxXR;
	private: System::Windows::Forms::CheckBox^  checkBoxXF;
	private: System::Windows::Forms::Label^  label21;
	private: System::Windows::Forms::Label^  label20;


	private: System::Windows::Forms::Label^  label18;

	private: Microsoft::VisualBasic::PowerPacks::RectangleShape^  Z_meter_back;


	private: Microsoft::VisualBasic::PowerPacks::RectangleShape^  Y_meter_back;

	private: Microsoft::VisualBasic::PowerPacks::RectangleShape^  X_meter_back;

	private: System::Windows::Forms::Label^  label22;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape15;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape14;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape13;
	private: System::Windows::Forms::Label^  label24;
	private: System::Windows::Forms::Label^  label25;
	private: Microsoft::VisualBasic::PowerPacks::RectangleShape^  X_meter_fore;
	private: Microsoft::VisualBasic::PowerPacks::RectangleShape^  Z_meter_fore;
	private: Microsoft::VisualBasic::PowerPacks::RectangleShape^  Y_meter_fore;



	private: System::Windows::Forms::Label^  label33;
	private: System::Windows::Forms::Label^  label32;
	private: System::Windows::Forms::Label^  label31;
	private: System::Windows::Forms::Label^  label29;
	private: System::Windows::Forms::Label^  label28;
	private: System::Windows::Forms::Label^  label27;
	private: System::Windows::Forms::Label^  label26;
	private: Microsoft::VisualBasic::PowerPacks::ShapeContainer^  shapeContainer3;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape18;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape17;

	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape16;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  ConstantVectB;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  ConstantVectA;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  ConstantVectMain;
	private: Microsoft::VisualBasic::PowerPacks::RectangleShape^  rectangleShape1;
	private: System::Windows::Forms::Button^  zeroButton;







	private: System::Windows::Forms::TabPage^  precessionTab;
	private: Microsoft::VisualBasic::PowerPacks::ShapeContainer^  shapeContainer4;




	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  ovalShape1;


	private: System::Windows::Forms::Label^  label36;
	private: System::Windows::Forms::Label^  label37;
	private: System::Windows::Forms::Label^  label38;
	private: System::Windows::Forms::Label^  label39;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape25;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape24;

	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape22;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  PrecVectB;




	private: Microsoft::VisualBasic::PowerPacks::LineShape^  PrecVectA;

	private: Microsoft::VisualBasic::PowerPacks::LineShape^  PrecVectMain;

	private: System::Windows::Forms::Label^  label45;
	private: System::Windows::Forms::Label^  label44;
	private: System::Windows::Forms::Label^  label43;
	private: System::Windows::Forms::TextBox^  precIPBox;



	private: System::Windows::Forms::Label^  label42;
	private: System::Windows::Forms::Label^  label49;
	private: System::Windows::Forms::Label^  label48;
	private: System::Windows::Forms::Label^  label47;
	private: System::Windows::Forms::Label^  label46;
	private: System::Windows::Forms::Button^  precessButton;

	private: System::Windows::Forms::TextBox^  precPeriodBox;

	private: System::Windows::Forms::TextBox^  precFreqBox;

	private: System::Windows::Forms::TextBox^  precOPBox;


	private: System::Windows::Forms::TabPage^  diskTab;
	private: System::Windows::Forms::TabPage^  rollingTab;
	private: System::Windows::Forms::TabPage^  quickPrecTab;
	private: System::Windows::Forms::RadioButton^  precCCW;



	private: System::Windows::Forms::RadioButton^  precCW;


	private: System::Windows::Forms::TextBox^  precThetaBox;

	private: System::Windows::Forms::Label^  label50;
	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  ovalShape2;
	private: System::Windows::Forms::Label^  label34;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  ConstVectThetaB;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  ConstVectThetaA;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  ConstVectThetaMain;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape21;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape19;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape20;
	private: Microsoft::VisualBasic::PowerPacks::RectangleShape^  rectangleShape2;
	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  ovalShape3;
	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  ovalShape4;
	private: System::Windows::Forms::Label^  label35;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape27;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape26;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape23;
	private: Microsoft::VisualBasic::PowerPacks::RectangleShape^  rectangleShape3;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  PrecVectThetaB;

	private: Microsoft::VisualBasic::PowerPacks::LineShape^  PrecVectThetaA;

	private: Microsoft::VisualBasic::PowerPacks::LineShape^  PrecVectThetaMain;
	private: Microsoft::VisualBasic::PowerPacks::ShapeContainer^  shapeContainer5;
	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  ovalShape7;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  rollVectA;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  rollVectB;


	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape37;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape36;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape35;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  rollVectMain;

	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  ovalShape6;

	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape33;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape32;

	private: Microsoft::VisualBasic::PowerPacks::LineShape^  rollVectThetaMain;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  rollVectThetaA;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  rollVectThetaB;




	private: System::Windows::Forms::Label^  label40;
	private: System::Windows::Forms::Label^  label41;
	private: System::Windows::Forms::Label^  label51;
	private: System::Windows::Forms::Label^  label52;
	private: System::Windows::Forms::Label^  label53;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape31;

	private: System::Windows::Forms::Button^  rollButton;
	private: System::Windows::Forms::TextBox^  rollAngleBox;


	private: System::Windows::Forms::TextBox^  rollPeriodBox;


	private: System::Windows::Forms::TextBox^  rollFreqBox;

	private: System::Windows::Forms::TextBox^  rollOPBox;

	private: System::Windows::Forms::TextBox^  rollIPBox;






	private: System::Windows::Forms::Label^  label54;
	private: System::Windows::Forms::Label^  label55;
	private: System::Windows::Forms::Label^  label56;
	private: System::Windows::Forms::Label^  label57;
	private: System::Windows::Forms::Label^  label58;
	private: System::Windows::Forms::Label^  label59;
	private: System::Windows::Forms::Label^  label60;
	private: System::Windows::Forms::Label^  label61;
	private: System::Windows::Forms::Label^  label62;


	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape12;
	private: System::Windows::Forms::RadioButton^  oneAmpRB;

	private: System::Windows::Forms::RadioButton^  twoAmpRB;





	private: System::Windows::Forms::Label^  label72;
	private: System::Windows::Forms::TextBox^  hopBox;
	private: System::Windows::Forms::Label^  label70;
	private: System::Windows::Forms::Button^  leftButton;

	private: System::Windows::Forms::TextBox^  diskThetaBox;
	private: System::Windows::Forms::TextBox^  diskPeriodBox;
	private: System::Windows::Forms::TextBox^  diskFreqBox;
	private: System::Windows::Forms::TextBox^  diskOPBox;

	private: System::Windows::Forms::TextBox^  diskIPBox;
	private: System::Windows::Forms::RadioButton^  diskCCW;
	private: System::Windows::Forms::RadioButton^  diskCW;
	private: System::Windows::Forms::Label^  label19;
	private: System::Windows::Forms::Label^  label23;
	private: System::Windows::Forms::Label^  label63;
	private: System::Windows::Forms::Label^  label64;
	private: System::Windows::Forms::Label^  label65;
	private: System::Windows::Forms::Label^  label66;
	private: System::Windows::Forms::Label^  label67;
	private: System::Windows::Forms::Label^  label68;
	private: System::Windows::Forms::Label^  label69;
	private: Microsoft::VisualBasic::PowerPacks::ShapeContainer^  shapeContainer6;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape28;
	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  ovalShape8;
	private: System::Windows::Forms::Button^  downButton;

	private: System::Windows::Forms::Button^  upButton;
	private: System::Windows::Forms::Button^  rightButton;
	private: System::Windows::Forms::TextBox^  QPThetaBox;

	private: System::Windows::Forms::Label^  label71;
	private: System::Windows::Forms::Label^  label73;
	private: System::Windows::Forms::Label^  label74;
	private: System::Windows::Forms::Label^  label75;
	private: System::Windows::Forms::Label^  label76;
	private: System::Windows::Forms::TextBox^  QPPeriodBox;

	private: System::Windows::Forms::TextBox^  QPFreqBox;

	private: System::Windows::Forms::TextBox^  QPOPBox;

	private: System::Windows::Forms::Label^  label77;
	private: System::Windows::Forms::Label^  label78;
	private: System::Windows::Forms::Label^  label79;
	private: System::Windows::Forms::TextBox^  QPIPBox;

	private: System::Windows::Forms::Label^  label80;
	private: System::Windows::Forms::Button^  QPStopButton;
	private: System::Windows::Forms::RadioButton^  QPCCWLast;

	private: System::Windows::Forms::RadioButton^  QPCWLast;

	private: System::Windows::Forms::TextBox^  QPThetaBoxLast;

	private: System::Windows::Forms::Label^  label81;
	private: System::Windows::Forms::Label^  label82;
	private: System::Windows::Forms::Label^  label83;
	private: System::Windows::Forms::Label^  label84;
	private: System::Windows::Forms::Label^  label85;
	private: System::Windows::Forms::TextBox^  QPPeriodBoxLast;

	private: System::Windows::Forms::TextBox^  QPFreqBoxLast;

	private: System::Windows::Forms::TextBox^  QPOPBoxLast;

	private: System::Windows::Forms::Label^  label86;
	private: System::Windows::Forms::Label^  label87;
	private: System::Windows::Forms::Label^  label88;
	private: System::Windows::Forms::TextBox^  QPIPBoxLast;

	private: System::Windows::Forms::Label^  label89;
	private: System::Windows::Forms::Button^  QPStartButton;
	private: System::Windows::Forms::RadioButton^  QPCCW;

	private: System::Windows::Forms::RadioButton^  QPCW;

	private: Microsoft::VisualBasic::PowerPacks::ShapeContainer^  shapeContainer7;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape29;
	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  ovalShape9;
	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  ovalShape10;
	private: System::Windows::Forms::Label^  label90;
	private: System::Windows::Forms::Label^  label91;
	private: System::Windows::Forms::CheckBox^  displayCheck;
	private: System::Windows::Forms::GroupBox^  DiskHopBox;
	private: System::Windows::Forms::GroupBox^  QP_ActiveBox;
	private: System::Windows::Forms::GroupBox^  QP_ProposedBox;
	private: System::Windows::Forms::GroupBox^  PrecessionBox;
	private: System::Windows::Forms::Button^  rollDownButton;
	private: System::Windows::Forms::Button^  rollUpButton;
	private: System::Windows::Forms::Button^  rollRightButton;
	private: System::Windows::Forms::Button^  rollLeftButton;
	private: System::Windows::Forms::TabPage^  logTab;
	private: System::Windows::Forms::RichTextBox^  logBox;
	private: System::Windows::Forms::Button^  saveFileButton;




	private: System::Windows::Forms::SaveFileDialog^  saveLogFileDialog;
	private: System::Windows::Forms::Button^  clearLogButton;
	private: System::Windows::Forms::TabPage^  xboxTab;
	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  ovalShape5;
	private: Microsoft::VisualBasic::PowerPacks::ShapeContainer^  shapeContainer8;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape30;
	private: Microsoft::VisualBasic::PowerPacks::RectangleShape^  rectangleShape6;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape34;
	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  ovalShape12;
	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  ovalShape11;
	private: System::Windows::Forms::PictureBox^  xboxStatusPic;


	private: System::Windows::Forms::Label^  label97;
	private: System::Windows::Forms::Label^  label98;
	private: System::Windows::Forms::Label^  label99;
	private: System::Windows::Forms::Label^  label100;
	private: System::Windows::Forms::Label^  label101;
	private: System::Windows::Forms::TextBox^  xboxThetaBox;

	private: System::Windows::Forms::TextBox^  xboxOPBox;

	private: System::Windows::Forms::TextBox^  xboxIPBox;


	private: System::Windows::Forms::Label^  label92;
	private: System::Windows::Forms::Label^  label93;
	private: System::Windows::Forms::Label^  label94;
	private: System::Windows::Forms::Label^  label95;
	private: System::Windows::Forms::Label^  label96;
	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  ovalShape13;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  xboxVectB;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  xboxVectA;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  xboxVectMain;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  xboxVectThetaB;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  xboxVectThetaA;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  xboxVectThetaMain;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape41;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape40;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape39;
	private: Microsoft::VisualBasic::PowerPacks::LineShape^  lineShape38;
	private: System::Windows::Forms::CheckBox^  FIBox;
	private: System::Windows::Forms::TextBox^  xboxPeriodBox;


	private: System::Windows::Forms::TextBox^  xboxFreqBox;

	private: System::Windows::Forms::Label^  label102;
	private: System::Windows::Forms::Label^  label103;
	private: System::Windows::Forms::Label^  label104;
	private: System::Windows::Forms::Label^  label105;
	private: System::Windows::Forms::PictureBox^  YPic;
	private: System::Windows::Forms::PictureBox^  APic;
	private: System::Windows::Forms::PictureBox^  BPic;
	private: System::Windows::Forms::PictureBox^  XPic;
	private: System::Windows::Forms::PictureBox^  LBPic;
	private: System::Windows::Forms::PictureBox^  RBPic;
	private: System::Windows::Forms::Label^  label106;
	private: System::Windows::Forms::Label^  label107;
	private: System::Windows::Forms::Label^  label108;
	private: System::Windows::Forms::Label^  label109;
	private: System::Windows::Forms::PictureBox^  LSPic;
	private: System::Windows::Forms::PictureBox^  RSPic;
	private: System::Windows::Forms::Label^  label110;
	private: System::Windows::Forms::Label^  label111;
	private: System::Windows::Forms::PictureBox^  RTPic;
	private: System::Windows::Forms::PictureBox^  LTPic;
	private: System::Windows::Forms::Label^  label112;
	private: System::Windows::Forms::Label^  label113;
	private: System::Windows::Forms::Label^  label114;
	private: System::Windows::Forms::Label^  label115;
	private: System::Windows::Forms::PictureBox^  StartPic;
	private: System::Windows::Forms::PictureBox^  BackPic;
	private: System::Windows::Forms::Label^  label116;
	private: System::Windows::Forms::Label^  label117;
	private: System::Windows::Forms::CheckBox^  HoldBox;
	private: System::Windows::Forms::CheckBox^  ZModeBox;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::RadioButton^  xboxCCW;
	private: System::Windows::Forms::RadioButton^  xboxCW;











	private: System::ComponentModel::IContainer^  components;



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"MMIcon")));
			this->Connect_btn = (gcnew System::Windows::Forms::Button());
			this->splashLogo = (gcnew System::Windows::Forms::PictureBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->SendPosButt = (gcnew System::Windows::Forms::Button());
			this->textBox_Z = (gcnew System::Windows::Forms::TextBox());
			this->textBox_Y = (gcnew System::Windows::Forms::TextBox());
			this->textBox_X = (gcnew System::Windows::Forms::TextBox());
			this->shapeContainer1 = (gcnew Microsoft::VisualBasic::PowerPacks::ShapeContainer());
			this->voltNeedLine = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->VM_fore = (gcnew Microsoft::VisualBasic::PowerPacks::RectangleShape());
			this->VM_back = (gcnew Microsoft::VisualBasic::PowerPacks::RectangleShape());
			this->lineShape46 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape45 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape44 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape43 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape42 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape13 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape14 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape15 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->Z_meter_fore = (gcnew Microsoft::VisualBasic::PowerPacks::RectangleShape());
			this->Y_meter_fore = (gcnew Microsoft::VisualBasic::PowerPacks::RectangleShape());
			this->X_meter_fore = (gcnew Microsoft::VisualBasic::PowerPacks::RectangleShape());
			this->Z_meter_back = (gcnew Microsoft::VisualBasic::PowerPacks::RectangleShape());
			this->Y_meter_back = (gcnew Microsoft::VisualBasic::PowerPacks::RectangleShape());
			this->X_meter_back = (gcnew Microsoft::VisualBasic::PowerPacks::RectangleShape());
			this->statusLight = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->setupTab = (gcnew System::Windows::Forms::TabPage());
			this->configNameBox = (gcnew System::Windows::Forms::TextBox());
			this->loadConfigButton = (gcnew System::Windows::Forms::Button());
			this->saveConfigButton = (gcnew System::Windows::Forms::Button());
			this->oneAmpRB = (gcnew System::Windows::Forms::RadioButton());
			this->twoAmpRB = (gcnew System::Windows::Forms::RadioButton());
			this->checkBoxZR = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxZF = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxYR = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxYF = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxXR = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxXF = (gcnew System::Windows::Forms::CheckBox());
			this->label21 = (gcnew System::Windows::Forms::Label());
			this->label20 = (gcnew System::Windows::Forms::Label());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->textBoxOPF = (gcnew System::Windows::Forms::TextBox());
			this->textBoxIPF = (gcnew System::Windows::Forms::TextBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->textBoxOAZ = (gcnew System::Windows::Forms::TextBox());
			this->textBoxOAY = (gcnew System::Windows::Forms::TextBox());
			this->textBoxOAX = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label30 = (gcnew System::Windows::Forms::Label());
			this->labelCMFZ = (gcnew System::Windows::Forms::Label());
			this->labelCMFY = (gcnew System::Windows::Forms::Label());
			this->labelCMFX = (gcnew System::Windows::Forms::Label());
			this->textBoxMFZ = (gcnew System::Windows::Forms::TextBox());
			this->textBoxMFY = (gcnew System::Windows::Forms::TextBox());
			this->textBoxMFX = (gcnew System::Windows::Forms::TextBox());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->Setup_X = (gcnew System::Windows::Forms::Label());
			this->textBoxORZ = (gcnew System::Windows::Forms::TextBox());
			this->textBoxOFZ = (gcnew System::Windows::Forms::TextBox());
			this->textBoxORY = (gcnew System::Windows::Forms::TextBox());
			this->textBoxOFY = (gcnew System::Windows::Forms::TextBox());
			this->textBoxORX = (gcnew System::Windows::Forms::TextBox());
			this->textBoxOFX = (gcnew System::Windows::Forms::TextBox());
			this->textBoxRZ = (gcnew System::Windows::Forms::TextBox());
			this->textBoxRY = (gcnew System::Windows::Forms::TextBox());
			this->textBoxRX = (gcnew System::Windows::Forms::TextBox());
			this->shapeContainer2 = (gcnew Microsoft::VisualBasic::PowerPacks::ShapeContainer());
			this->lineShape12 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape11 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape10 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape9 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape8 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape7 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape6 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape5 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape4 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape3 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape2 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape1 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->constantTab = (gcnew System::Windows::Forms::TabPage());
			this->label34 = (gcnew System::Windows::Forms::Label());
			this->label33 = (gcnew System::Windows::Forms::Label());
			this->label32 = (gcnew System::Windows::Forms::Label());
			this->label31 = (gcnew System::Windows::Forms::Label());
			this->label29 = (gcnew System::Windows::Forms::Label());
			this->label28 = (gcnew System::Windows::Forms::Label());
			this->label27 = (gcnew System::Windows::Forms::Label());
			this->label26 = (gcnew System::Windows::Forms::Label());
			this->shapeContainer3 = (gcnew Microsoft::VisualBasic::PowerPacks::ShapeContainer());
			this->ConstVectThetaB = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->ConstVectThetaA = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->ConstVectThetaMain = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape21 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape19 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape20 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->rectangleShape2 = (gcnew Microsoft::VisualBasic::PowerPacks::RectangleShape());
			this->ovalShape3 = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->ConstantVectB = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->ConstantVectA = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->ConstantVectMain = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape18 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape17 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape16 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->rectangleShape1 = (gcnew Microsoft::VisualBasic::PowerPacks::RectangleShape());
			this->precessionTab = (gcnew System::Windows::Forms::TabPage());
			this->PrecessionBox = (gcnew System::Windows::Forms::GroupBox());
			this->precCCW = (gcnew System::Windows::Forms::RadioButton());
			this->precCW = (gcnew System::Windows::Forms::RadioButton());
			this->label35 = (gcnew System::Windows::Forms::Label());
			this->precThetaBox = (gcnew System::Windows::Forms::TextBox());
			this->label50 = (gcnew System::Windows::Forms::Label());
			this->label49 = (gcnew System::Windows::Forms::Label());
			this->label48 = (gcnew System::Windows::Forms::Label());
			this->label47 = (gcnew System::Windows::Forms::Label());
			this->label46 = (gcnew System::Windows::Forms::Label());
			this->precessButton = (gcnew System::Windows::Forms::Button());
			this->precPeriodBox = (gcnew System::Windows::Forms::TextBox());
			this->precFreqBox = (gcnew System::Windows::Forms::TextBox());
			this->precOPBox = (gcnew System::Windows::Forms::TextBox());
			this->label45 = (gcnew System::Windows::Forms::Label());
			this->label44 = (gcnew System::Windows::Forms::Label());
			this->label43 = (gcnew System::Windows::Forms::Label());
			this->precIPBox = (gcnew System::Windows::Forms::TextBox());
			this->label42 = (gcnew System::Windows::Forms::Label());
			this->label36 = (gcnew System::Windows::Forms::Label());
			this->label37 = (gcnew System::Windows::Forms::Label());
			this->label38 = (gcnew System::Windows::Forms::Label());
			this->label39 = (gcnew System::Windows::Forms::Label());
			this->shapeContainer4 = (gcnew Microsoft::VisualBasic::PowerPacks::ShapeContainer());
			this->PrecVectThetaB = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->PrecVectThetaA = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->PrecVectThetaMain = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape23 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->rectangleShape3 = (gcnew Microsoft::VisualBasic::PowerPacks::RectangleShape());
			this->lineShape27 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape26 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->ovalShape4 = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->ovalShape2 = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->PrecVectMain = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape25 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape24 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape22 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->PrecVectB = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->PrecVectA = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->ovalShape1 = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->rollingTab = (gcnew System::Windows::Forms::TabPage());
			this->rollDownButton = (gcnew System::Windows::Forms::Button());
			this->rollUpButton = (gcnew System::Windows::Forms::Button());
			this->rollRightButton = (gcnew System::Windows::Forms::Button());
			this->rollLeftButton = (gcnew System::Windows::Forms::Button());
			this->rollButton = (gcnew System::Windows::Forms::Button());
			this->rollAngleBox = (gcnew System::Windows::Forms::TextBox());
			this->rollPeriodBox = (gcnew System::Windows::Forms::TextBox());
			this->rollFreqBox = (gcnew System::Windows::Forms::TextBox());
			this->rollOPBox = (gcnew System::Windows::Forms::TextBox());
			this->rollIPBox = (gcnew System::Windows::Forms::TextBox());
			this->label54 = (gcnew System::Windows::Forms::Label());
			this->label55 = (gcnew System::Windows::Forms::Label());
			this->label56 = (gcnew System::Windows::Forms::Label());
			this->label57 = (gcnew System::Windows::Forms::Label());
			this->label58 = (gcnew System::Windows::Forms::Label());
			this->label59 = (gcnew System::Windows::Forms::Label());
			this->label60 = (gcnew System::Windows::Forms::Label());
			this->label61 = (gcnew System::Windows::Forms::Label());
			this->label62 = (gcnew System::Windows::Forms::Label());
			this->label53 = (gcnew System::Windows::Forms::Label());
			this->label40 = (gcnew System::Windows::Forms::Label());
			this->label41 = (gcnew System::Windows::Forms::Label());
			this->label51 = (gcnew System::Windows::Forms::Label());
			this->label52 = (gcnew System::Windows::Forms::Label());
			this->shapeContainer5 = (gcnew Microsoft::VisualBasic::PowerPacks::ShapeContainer());
			this->lineShape31 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->rectangleShape5 = (gcnew Microsoft::VisualBasic::PowerPacks::RectangleShape());
			this->ovalShape7 = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->rollVectA = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->rollVectB = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape37 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape36 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape35 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->rollVectMain = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->ovalShape6 = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->ovalShape5 = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->lineShape33 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape32 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->rollVectThetaMain = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->rollVectThetaA = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->rollVectThetaB = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->diskTab = (gcnew System::Windows::Forms::TabPage());
			this->DiskHopBox = (gcnew System::Windows::Forms::GroupBox());
			this->diskCCW = (gcnew System::Windows::Forms::RadioButton());
			this->diskCW = (gcnew System::Windows::Forms::RadioButton());
			this->downButton = (gcnew System::Windows::Forms::Button());
			this->upButton = (gcnew System::Windows::Forms::Button());
			this->rightButton = (gcnew System::Windows::Forms::Button());
			this->label72 = (gcnew System::Windows::Forms::Label());
			this->hopBox = (gcnew System::Windows::Forms::TextBox());
			this->label70 = (gcnew System::Windows::Forms::Label());
			this->leftButton = (gcnew System::Windows::Forms::Button());
			this->diskThetaBox = (gcnew System::Windows::Forms::TextBox());
			this->diskPeriodBox = (gcnew System::Windows::Forms::TextBox());
			this->diskFreqBox = (gcnew System::Windows::Forms::TextBox());
			this->diskOPBox = (gcnew System::Windows::Forms::TextBox());
			this->diskIPBox = (gcnew System::Windows::Forms::TextBox());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->label23 = (gcnew System::Windows::Forms::Label());
			this->label63 = (gcnew System::Windows::Forms::Label());
			this->label64 = (gcnew System::Windows::Forms::Label());
			this->label65 = (gcnew System::Windows::Forms::Label());
			this->label66 = (gcnew System::Windows::Forms::Label());
			this->label67 = (gcnew System::Windows::Forms::Label());
			this->label68 = (gcnew System::Windows::Forms::Label());
			this->label69 = (gcnew System::Windows::Forms::Label());
			this->shapeContainer6 = (gcnew Microsoft::VisualBasic::PowerPacks::ShapeContainer());
			this->lineShape28 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->ovalShape8 = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->quickPrecTab = (gcnew System::Windows::Forms::TabPage());
			this->QP_ActiveBox = (gcnew System::Windows::Forms::GroupBox());
			this->QPCCWLast = (gcnew System::Windows::Forms::RadioButton());
			this->QPCWLast = (gcnew System::Windows::Forms::RadioButton());
			this->QP_ProposedBox = (gcnew System::Windows::Forms::GroupBox());
			this->QPCCW = (gcnew System::Windows::Forms::RadioButton());
			this->QPCW = (gcnew System::Windows::Forms::RadioButton());
			this->label91 = (gcnew System::Windows::Forms::Label());
			this->label90 = (gcnew System::Windows::Forms::Label());
			this->QPStopButton = (gcnew System::Windows::Forms::Button());
			this->QPThetaBoxLast = (gcnew System::Windows::Forms::TextBox());
			this->label81 = (gcnew System::Windows::Forms::Label());
			this->label82 = (gcnew System::Windows::Forms::Label());
			this->label83 = (gcnew System::Windows::Forms::Label());
			this->label84 = (gcnew System::Windows::Forms::Label());
			this->label85 = (gcnew System::Windows::Forms::Label());
			this->QPPeriodBoxLast = (gcnew System::Windows::Forms::TextBox());
			this->QPFreqBoxLast = (gcnew System::Windows::Forms::TextBox());
			this->QPOPBoxLast = (gcnew System::Windows::Forms::TextBox());
			this->label86 = (gcnew System::Windows::Forms::Label());
			this->label87 = (gcnew System::Windows::Forms::Label());
			this->label88 = (gcnew System::Windows::Forms::Label());
			this->QPIPBoxLast = (gcnew System::Windows::Forms::TextBox());
			this->label89 = (gcnew System::Windows::Forms::Label());
			this->QPStartButton = (gcnew System::Windows::Forms::Button());
			this->QPThetaBox = (gcnew System::Windows::Forms::TextBox());
			this->label71 = (gcnew System::Windows::Forms::Label());
			this->label73 = (gcnew System::Windows::Forms::Label());
			this->label74 = (gcnew System::Windows::Forms::Label());
			this->label75 = (gcnew System::Windows::Forms::Label());
			this->label76 = (gcnew System::Windows::Forms::Label());
			this->QPPeriodBox = (gcnew System::Windows::Forms::TextBox());
			this->QPFreqBox = (gcnew System::Windows::Forms::TextBox());
			this->QPOPBox = (gcnew System::Windows::Forms::TextBox());
			this->label77 = (gcnew System::Windows::Forms::Label());
			this->label78 = (gcnew System::Windows::Forms::Label());
			this->label79 = (gcnew System::Windows::Forms::Label());
			this->QPIPBox = (gcnew System::Windows::Forms::TextBox());
			this->label80 = (gcnew System::Windows::Forms::Label());
			this->shapeContainer7 = (gcnew Microsoft::VisualBasic::PowerPacks::ShapeContainer());
			this->ovalShape10 = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->lineShape29 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->ovalShape9 = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->xboxTab = (gcnew System::Windows::Forms::TabPage());
			this->precSuppCheck = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->xboxCCW = (gcnew System::Windows::Forms::RadioButton());
			this->xboxCW = (gcnew System::Windows::Forms::RadioButton());
			this->ZModeBox = (gcnew System::Windows::Forms::CheckBox());
			this->HoldBox = (gcnew System::Windows::Forms::CheckBox());
			this->xboxPeriodBox = (gcnew System::Windows::Forms::TextBox());
			this->xboxFreqBox = (gcnew System::Windows::Forms::TextBox());
			this->label102 = (gcnew System::Windows::Forms::Label());
			this->label103 = (gcnew System::Windows::Forms::Label());
			this->label104 = (gcnew System::Windows::Forms::Label());
			this->label105 = (gcnew System::Windows::Forms::Label());
			this->FIBox = (gcnew System::Windows::Forms::CheckBox());
			this->label97 = (gcnew System::Windows::Forms::Label());
			this->label98 = (gcnew System::Windows::Forms::Label());
			this->label99 = (gcnew System::Windows::Forms::Label());
			this->label100 = (gcnew System::Windows::Forms::Label());
			this->label101 = (gcnew System::Windows::Forms::Label());
			this->xboxThetaBox = (gcnew System::Windows::Forms::TextBox());
			this->xboxOPBox = (gcnew System::Windows::Forms::TextBox());
			this->xboxIPBox = (gcnew System::Windows::Forms::TextBox());
			this->label92 = (gcnew System::Windows::Forms::Label());
			this->label93 = (gcnew System::Windows::Forms::Label());
			this->label94 = (gcnew System::Windows::Forms::Label());
			this->label95 = (gcnew System::Windows::Forms::Label());
			this->label96 = (gcnew System::Windows::Forms::Label());
			this->shapeContainer8 = (gcnew Microsoft::VisualBasic::PowerPacks::ShapeContainer());
			this->ovalShape13 = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->xboxVectB = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->xboxVectA = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->xboxVectMain = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->xboxVectThetaB = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->xboxVectThetaA = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->xboxVectThetaMain = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape41 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape40 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape39 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape38 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->lineShape30 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->rectangleShape6 = (gcnew Microsoft::VisualBasic::PowerPacks::RectangleShape());
			this->lineShape34 = (gcnew Microsoft::VisualBasic::PowerPacks::LineShape());
			this->ovalShape12 = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->ovalShape11 = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->logTab = (gcnew System::Windows::Forms::TabPage());
			this->clearLogButton = (gcnew System::Windows::Forms::Button());
			this->saveFileButton = (gcnew System::Windows::Forms::Button());
			this->logBox = (gcnew System::Windows::Forms::RichTextBox());
			this->macroTab = (gcnew System::Windows::Forms::TabPage());
			this->loopCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->runMacroButton = (gcnew System::Windows::Forms::Button());
			this->macroNameBox = (gcnew System::Windows::Forms::TextBox());
			this->loadMacroButton = (gcnew System::Windows::Forms::Button());
			this->saveMacroButton = (gcnew System::Windows::Forms::Button());
			this->macroGrid = (gcnew System::Windows::Forms::DataGridView());
			this->Action = (gcnew System::Windows::Forms::DataGridViewComboBoxColumn());
			this->Duration = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Angle = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Revolutions = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->CCW = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->Inplane = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Outplane = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Frequency = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Direction = (gcnew System::Windows::Forms::DataGridViewComboBoxColumn());
			this->xboxStatusPic = (gcnew System::Windows::Forms::PictureBox());
			this->voltDisplay = (gcnew System::Windows::Forms::Label());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->label25 = (gcnew System::Windows::Forms::Label());
			this->zeroButton = (gcnew System::Windows::Forms::Button());
			this->displayCheck = (gcnew System::Windows::Forms::CheckBox());
			this->saveLogFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->YPic = (gcnew System::Windows::Forms::PictureBox());
			this->APic = (gcnew System::Windows::Forms::PictureBox());
			this->BPic = (gcnew System::Windows::Forms::PictureBox());
			this->XPic = (gcnew System::Windows::Forms::PictureBox());
			this->LBPic = (gcnew System::Windows::Forms::PictureBox());
			this->RBPic = (gcnew System::Windows::Forms::PictureBox());
			this->label106 = (gcnew System::Windows::Forms::Label());
			this->label107 = (gcnew System::Windows::Forms::Label());
			this->label108 = (gcnew System::Windows::Forms::Label());
			this->label109 = (gcnew System::Windows::Forms::Label());
			this->LSPic = (gcnew System::Windows::Forms::PictureBox());
			this->RSPic = (gcnew System::Windows::Forms::PictureBox());
			this->label110 = (gcnew System::Windows::Forms::Label());
			this->label111 = (gcnew System::Windows::Forms::Label());
			this->RTPic = (gcnew System::Windows::Forms::PictureBox());
			this->LTPic = (gcnew System::Windows::Forms::PictureBox());
			this->label112 = (gcnew System::Windows::Forms::Label());
			this->label113 = (gcnew System::Windows::Forms::Label());
			this->label114 = (gcnew System::Windows::Forms::Label());
			this->label115 = (gcnew System::Windows::Forms::Label());
			this->StartPic = (gcnew System::Windows::Forms::PictureBox());
			this->BackPic = (gcnew System::Windows::Forms::PictureBox());
			this->label116 = (gcnew System::Windows::Forms::Label());
			this->label117 = (gcnew System::Windows::Forms::Label());
			this->saveMacroFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->loadMacroFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->saveConfigFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->loadConfigFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->statusStrip = (gcnew System::Windows::Forms::StatusStrip());
			this->statusLabel = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splashLogo))->BeginInit();
			this->tabControl1->SuspendLayout();
			this->setupTab->SuspendLayout();
			this->constantTab->SuspendLayout();
			this->precessionTab->SuspendLayout();
			this->PrecessionBox->SuspendLayout();
			this->rollingTab->SuspendLayout();
			this->diskTab->SuspendLayout();
			this->DiskHopBox->SuspendLayout();
			this->quickPrecTab->SuspendLayout();
			this->QP_ActiveBox->SuspendLayout();
			this->QP_ProposedBox->SuspendLayout();
			this->xboxTab->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->logTab->SuspendLayout();
			this->macroTab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->macroGrid))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->xboxStatusPic))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->YPic))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->APic))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->BPic))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->XPic))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->LBPic))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->RBPic))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->LSPic))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->RSPic))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->RTPic))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->LTPic))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->StartPic))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->BackPic))->BeginInit();
			this->statusStrip->SuspendLayout();
			this->SuspendLayout();
			// 
			// Connect_btn
			// 
			this->Connect_btn->BackColor = System::Drawing::SystemColors::Control;
			this->Connect_btn->ForeColor = System::Drawing::SystemColors::ControlText;
			this->Connect_btn->Location = System::Drawing::Point(732, 3);
			this->Connect_btn->Name = L"Connect_btn";
			this->Connect_btn->Size = System::Drawing::Size(79, 30);
			this->Connect_btn->TabIndex = 1;
			this->Connect_btn->Text = L"Connect";
			this->Connect_btn->UseVisualStyleBackColor = false;
			this->Connect_btn->Click += gcnew System::EventHandler(this, &Form1::Connect_btn_Click);
			// 
			// splashLogo
			// 
			this->splashLogo->Enabled = false;
			this->splashLogo->InitialImage = nullptr;
			this->splashLogo->Location = System::Drawing::Point(13, 75);
			this->splashLogo->Name = L"splashLogo";
			this->splashLogo->Size = System::Drawing::Size(797, 247);
			this->splashLogo->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->splashLogo->TabIndex = 95;
			this->splashLogo->TabStop = false;
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(102, 75);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(37, 29);
			this->label1->TabIndex = 8;
			this->label1->Text = L"X:";
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(103, 107);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(36, 29);
			this->label2->TabIndex = 9;
			this->label2->Text = L"Y:";
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(104, 139);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(35, 29);
			this->label3->TabIndex = 10;
			this->label3->Text = L"Z:";
			// 
			// SendPosButt
			// 
			this->SendPosButt->AutoSize = true;
			this->SendPosButt->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->SendPosButt->Location = System::Drawing::Point(94, 217);
			this->SendPosButt->Name = L"SendPosButt";
			this->SendPosButt->Size = System::Drawing::Size(160, 39);
			this->SendPosButt->TabIndex = 11;
			this->SendPosButt->Text = L"Send";
			this->SendPosButt->UseVisualStyleBackColor = true;
			this->SendPosButt->Click += gcnew System::EventHandler(this, &Form1::SendPosButt_Click);
			// 
			// textBox_Z
			// 
			this->textBox_Z->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBox_Z->Location = System::Drawing::Point(128, 136);
			this->textBox_Z->MaxLength = 7;
			this->textBox_Z->Name = L"textBox_Z";
			this->textBox_Z->Size = System::Drawing::Size(43, 35);
			this->textBox_Z->TabIndex = 7;
			this->textBox_Z->Text = L"0";
			this->textBox_Z->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask1);
			this->textBox_Z->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBox_Z->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave0);
			// 
			// textBox_Y
			// 
			this->textBox_Y->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBox_Y->Location = System::Drawing::Point(128, 104);
			this->textBox_Y->MaxLength = 7;
			this->textBox_Y->Name = L"textBox_Y";
			this->textBox_Y->Size = System::Drawing::Size(43, 35);
			this->textBox_Y->TabIndex = 6;
			this->textBox_Y->Text = L"0";
			this->textBox_Y->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask1);
			this->textBox_Y->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBox_Y->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave0);
			// 
			// textBox_X
			// 
			this->textBox_X->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBox_X->Location = System::Drawing::Point(128, 72);
			this->textBox_X->MaxLength = 7;
			this->textBox_X->Name = L"textBox_X";
			this->textBox_X->Size = System::Drawing::Size(43, 35);
			this->textBox_X->TabIndex = 5;
			this->textBox_X->Text = L"0";
			this->textBox_X->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask1);
			this->textBox_X->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBox_X->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave0);
			// 
			// shapeContainer1
			// 
			this->shapeContainer1->AutoSize = true;
			this->shapeContainer1->Location = System::Drawing::Point(0, 0);
			this->shapeContainer1->Margin = System::Windows::Forms::Padding(0);
			this->shapeContainer1->Name = L"shapeContainer1";
			this->shapeContainer1->Shapes->AddRange(gcnew cli::array< Microsoft::VisualBasic::PowerPacks::Shape^  >(18) {
				this->voltNeedLine,
					this->VM_fore, this->VM_back, this->lineShape46, this->lineShape45, this->lineShape44, this->lineShape43, this->lineShape42,
					this->lineShape13, this->lineShape14, this->lineShape15, this->Z_meter_fore, this->Y_meter_fore, this->X_meter_fore, this->Z_meter_back,
					this->Y_meter_back, this->X_meter_back, this->statusLight
			});
			this->shapeContainer1->Size = System::Drawing::Size(821, 456);
			this->shapeContainer1->TabIndex = 13;
			this->shapeContainer1->TabStop = false;
			// 
			// voltNeedLine
			// 
			this->voltNeedLine->BorderWidth = 3;
			this->voltNeedLine->Enabled = false;
			this->voltNeedLine->Name = L"voltNeedLine";
			this->voltNeedLine->Visible = false;
			this->voltNeedLine->X1 = 431;
			this->voltNeedLine->X2 = 431;
			this->voltNeedLine->Y1 = 311;
			this->voltNeedLine->Y2 = 330;
			// 
			// VM_fore
			// 
			this->VM_fore->BorderColor = System::Drawing::Color::Transparent;
			this->VM_fore->Enabled = false;
			this->VM_fore->FillColor = System::Drawing::Color::LawnGreen;
			this->VM_fore->FillStyle = Microsoft::VisualBasic::PowerPacks::FillStyle::Solid;
			this->VM_fore->Location = System::Drawing::Point(13, 311);
			this->VM_fore->Name = L"VM_fore";
			this->VM_fore->Size = System::Drawing::Size(118, 19);
			this->VM_fore->Visible = false;
			// 
			// VM_back
			// 
			this->VM_back->BorderColor = System::Drawing::Color::Black;
			this->VM_back->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dash;
			this->VM_back->Enabled = false;
			this->VM_back->FillColor = System::Drawing::Color::White;
			this->VM_back->FillGradientColor = System::Drawing::Color::White;
			this->VM_back->FillStyle = Microsoft::VisualBasic::PowerPacks::FillStyle::Solid;
			this->VM_back->Location = System::Drawing::Point(12, 310);
			this->VM_back->Name = L"VM_back";
			this->VM_back->Size = System::Drawing::Size(795, 20);
			this->VM_back->Visible = false;
			// 
			// lineShape46
			// 
			this->lineShape46->BorderWidth = 2;
			this->lineShape46->Enabled = false;
			this->lineShape46->Name = L"lineShape46";
			this->lineShape46->Visible = false;
			this->lineShape46->X1 = 82;
			this->lineShape46->X2 = 82;
			this->lineShape46->Y1 = 340;
			this->lineShape46->Y2 = 410;
			// 
			// lineShape45
			// 
			this->lineShape45->BorderWidth = 3;
			this->lineShape45->Enabled = false;
			this->lineShape45->Name = L"lineShape45";
			this->lineShape45->Visible = false;
			this->lineShape45->X1 = 12;
			this->lineShape45->X2 = 12;
			this->lineShape45->Y1 = 340;
			this->lineShape45->Y2 = 410;
			// 
			// lineShape44
			// 
			this->lineShape44->BorderWidth = 3;
			this->lineShape44->Enabled = false;
			this->lineShape44->Name = L"lineShape44";
			this->lineShape44->Visible = false;
			this->lineShape44->X1 = 807;
			this->lineShape44->X2 = 807;
			this->lineShape44->Y1 = 340;
			this->lineShape44->Y2 = 410;
			// 
			// lineShape43
			// 
			this->lineShape43->BorderWidth = 3;
			this->lineShape43->Enabled = false;
			this->lineShape43->Name = L"lineShape43";
			this->lineShape43->Visible = false;
			this->lineShape43->X1 = 12;
			this->lineShape43->X2 = 807;
			this->lineShape43->Y1 = 410;
			this->lineShape43->Y2 = 410;
			// 
			// lineShape42
			// 
			this->lineShape42->BorderWidth = 3;
			this->lineShape42->Enabled = false;
			this->lineShape42->Name = L"lineShape42";
			this->lineShape42->Visible = false;
			this->lineShape42->X1 = 12;
			this->lineShape42->X2 = 807;
			this->lineShape42->Y1 = 340;
			this->lineShape42->Y2 = 340;
			// 
			// lineShape13
			// 
			this->lineShape13->Enabled = false;
			this->lineShape13->Name = L"lineShape13";
			this->lineShape13->Visible = false;
			this->lineShape13->X1 = 751;
			this->lineShape13->X2 = 760;
			this->lineShape13->Y1 = 168;
			this->lineShape13->Y2 = 168;
			// 
			// lineShape14
			// 
			this->lineShape14->Enabled = false;
			this->lineShape14->Name = L"lineShape14";
			this->lineShape14->Visible = false;
			this->lineShape14->X1 = 770;
			this->lineShape14->X2 = 779;
			this->lineShape14->Y1 = 168;
			this->lineShape14->Y2 = 168;
			// 
			// lineShape15
			// 
			this->lineShape15->Enabled = false;
			this->lineShape15->Name = L"lineShape15";
			this->lineShape15->Visible = false;
			this->lineShape15->X1 = 789;
			this->lineShape15->X2 = 798;
			this->lineShape15->Y1 = 168;
			this->lineShape15->Y2 = 168;
			// 
			// Z_meter_fore
			// 
			this->Z_meter_fore->BorderColor = System::Drawing::Color::Transparent;
			this->Z_meter_fore->Enabled = false;
			this->Z_meter_fore->FillColor = System::Drawing::Color::LawnGreen;
			this->Z_meter_fore->FillStyle = Microsoft::VisualBasic::PowerPacks::FillStyle::Solid;
			this->Z_meter_fore->Location = System::Drawing::Point(790, 169);
			this->Z_meter_fore->Name = L"Z_meter_fore";
			this->Z_meter_fore->Size = System::Drawing::Size(9, 0);
			this->Z_meter_fore->Visible = false;
			// 
			// Y_meter_fore
			// 
			this->Y_meter_fore->BorderColor = System::Drawing::Color::Transparent;
			this->Y_meter_fore->Enabled = false;
			this->Y_meter_fore->FillColor = System::Drawing::Color::LawnGreen;
			this->Y_meter_fore->FillStyle = Microsoft::VisualBasic::PowerPacks::FillStyle::Solid;
			this->Y_meter_fore->Location = System::Drawing::Point(771, 169);
			this->Y_meter_fore->Name = L"Y_meter_fore";
			this->Y_meter_fore->Size = System::Drawing::Size(9, 0);
			this->Y_meter_fore->Visible = false;
			// 
			// X_meter_fore
			// 
			this->X_meter_fore->BorderColor = System::Drawing::Color::Transparent;
			this->X_meter_fore->Enabled = false;
			this->X_meter_fore->FillColor = System::Drawing::Color::LawnGreen;
			this->X_meter_fore->FillStyle = Microsoft::VisualBasic::PowerPacks::FillStyle::Solid;
			this->X_meter_fore->Location = System::Drawing::Point(752, 169);
			this->X_meter_fore->Name = L"X_meter_fore";
			this->X_meter_fore->Size = System::Drawing::Size(9, 0);
			this->X_meter_fore->Visible = false;
			// 
			// Z_meter_back
			// 
			this->Z_meter_back->BorderColor = System::Drawing::Color::Black;
			this->Z_meter_back->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dash;
			this->Z_meter_back->Enabled = false;
			this->Z_meter_back->FillColor = System::Drawing::Color::White;
			this->Z_meter_back->FillGradientColor = System::Drawing::Color::White;
			this->Z_meter_back->FillStyle = Microsoft::VisualBasic::PowerPacks::FillStyle::Solid;
			this->Z_meter_back->Location = System::Drawing::Point(789, 94);
			this->Z_meter_back->Name = L"Z_meter_back";
			this->Z_meter_back->Size = System::Drawing::Size(10, 150);
			this->Z_meter_back->Visible = false;
			// 
			// Y_meter_back
			// 
			this->Y_meter_back->BorderColor = System::Drawing::Color::Black;
			this->Y_meter_back->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dash;
			this->Y_meter_back->Enabled = false;
			this->Y_meter_back->FillColor = System::Drawing::Color::White;
			this->Y_meter_back->FillGradientColor = System::Drawing::Color::White;
			this->Y_meter_back->FillStyle = Microsoft::VisualBasic::PowerPacks::FillStyle::Solid;
			this->Y_meter_back->Location = System::Drawing::Point(770, 94);
			this->Y_meter_back->Name = L"Y_meter_back";
			this->Y_meter_back->Size = System::Drawing::Size(10, 150);
			this->Y_meter_back->Visible = false;
			// 
			// X_meter_back
			// 
			this->X_meter_back->BorderColor = System::Drawing::Color::Black;
			this->X_meter_back->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dash;
			this->X_meter_back->Enabled = false;
			this->X_meter_back->FillColor = System::Drawing::Color::White;
			this->X_meter_back->FillGradientColor = System::Drawing::Color::White;
			this->X_meter_back->FillStyle = Microsoft::VisualBasic::PowerPacks::FillStyle::Solid;
			this->X_meter_back->Location = System::Drawing::Point(751, 94);
			this->X_meter_back->Name = L"X_meter_back";
			this->X_meter_back->Size = System::Drawing::Size(10, 150);
			this->X_meter_back->Visible = false;
			// 
			// statusLight
			// 
			this->statusLight->BackColor = System::Drawing::SystemColors::Control;
			this->statusLight->Enabled = false;
			this->statusLight->FillColor = System::Drawing::Color::Red;
			this->statusLight->FillGradientColor = System::Drawing::SystemColors::ActiveBorder;
			this->statusLight->FillStyle = Microsoft::VisualBasic::PowerPacks::FillStyle::Solid;
			this->statusLight->Location = System::Drawing::Point(761, 34);
			this->statusLight->Name = L"statusLight";
			this->statusLight->Size = System::Drawing::Size(25, 25);
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->setupTab);
			this->tabControl1->Controls->Add(this->constantTab);
			this->tabControl1->Controls->Add(this->precessionTab);
			this->tabControl1->Controls->Add(this->rollingTab);
			this->tabControl1->Controls->Add(this->diskTab);
			this->tabControl1->Controls->Add(this->quickPrecTab);
			this->tabControl1->Controls->Add(this->xboxTab);
			this->tabControl1->Controls->Add(this->logTab);
			this->tabControl1->Controls->Add(this->macroTab);
			this->tabControl1->Enabled = false;
			this->tabControl1->Location = System::Drawing::Point(12, 12);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(721, 290);
			this->tabControl1->TabIndex = 14;
			this->tabControl1->Visible = false;
			// 
			// setupTab
			// 
			this->setupTab->Controls->Add(this->configNameBox);
			this->setupTab->Controls->Add(this->loadConfigButton);
			this->setupTab->Controls->Add(this->saveConfigButton);
			this->setupTab->Controls->Add(this->oneAmpRB);
			this->setupTab->Controls->Add(this->twoAmpRB);
			this->setupTab->Controls->Add(this->checkBoxZR);
			this->setupTab->Controls->Add(this->checkBoxZF);
			this->setupTab->Controls->Add(this->checkBoxYR);
			this->setupTab->Controls->Add(this->checkBoxYF);
			this->setupTab->Controls->Add(this->checkBoxXR);
			this->setupTab->Controls->Add(this->checkBoxXF);
			this->setupTab->Controls->Add(this->label21);
			this->setupTab->Controls->Add(this->label20);
			this->setupTab->Controls->Add(this->label18);
			this->setupTab->Controls->Add(this->label17);
			this->setupTab->Controls->Add(this->label8);
			this->setupTab->Controls->Add(this->textBoxOPF);
			this->setupTab->Controls->Add(this->textBoxIPF);
			this->setupTab->Controls->Add(this->label7);
			this->setupTab->Controls->Add(this->label6);
			this->setupTab->Controls->Add(this->textBoxOAZ);
			this->setupTab->Controls->Add(this->textBoxOAY);
			this->setupTab->Controls->Add(this->textBoxOAX);
			this->setupTab->Controls->Add(this->label5);
			this->setupTab->Controls->Add(this->label4);
			this->setupTab->Controls->Add(this->label30);
			this->setupTab->Controls->Add(this->labelCMFZ);
			this->setupTab->Controls->Add(this->labelCMFY);
			this->setupTab->Controls->Add(this->labelCMFX);
			this->setupTab->Controls->Add(this->textBoxMFZ);
			this->setupTab->Controls->Add(this->textBoxMFY);
			this->setupTab->Controls->Add(this->textBoxMFX);
			this->setupTab->Controls->Add(this->label16);
			this->setupTab->Controls->Add(this->label15);
			this->setupTab->Controls->Add(this->label14);
			this->setupTab->Controls->Add(this->label13);
			this->setupTab->Controls->Add(this->label12);
			this->setupTab->Controls->Add(this->label11);
			this->setupTab->Controls->Add(this->label10);
			this->setupTab->Controls->Add(this->Setup_X);
			this->setupTab->Controls->Add(this->textBoxORZ);
			this->setupTab->Controls->Add(this->textBoxOFZ);
			this->setupTab->Controls->Add(this->textBoxORY);
			this->setupTab->Controls->Add(this->textBoxOFY);
			this->setupTab->Controls->Add(this->textBoxORX);
			this->setupTab->Controls->Add(this->textBoxOFX);
			this->setupTab->Controls->Add(this->textBoxRZ);
			this->setupTab->Controls->Add(this->textBoxRY);
			this->setupTab->Controls->Add(this->textBoxRX);
			this->setupTab->Controls->Add(this->shapeContainer2);
			this->setupTab->Location = System::Drawing::Point(4, 29);
			this->setupTab->Name = L"setupTab";
			this->setupTab->Padding = System::Windows::Forms::Padding(3);
			this->setupTab->Size = System::Drawing::Size(713, 257);
			this->setupTab->TabIndex = 0;
			this->setupTab->Text = L"Setup";
			this->setupTab->UseVisualStyleBackColor = true;
			this->setupTab->Enter += gcnew System::EventHandler(this, &Form1::zeroButton_Click);
			this->setupTab->Leave += gcnew System::EventHandler(this, &Form1::zeroButton_Click);
			// 
			// configNameBox
			// 
			this->configNameBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->configNameBox->Location = System::Drawing::Point(20, 188);
			this->configNameBox->Name = L"configNameBox";
			this->configNameBox->Size = System::Drawing::Size(372, 35);
			this->configNameBox->TabIndex = 67;
			// 
			// loadConfigButton
			// 
			this->loadConfigButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->loadConfigButton->Location = System::Drawing::Point(209, 220);
			this->loadConfigButton->Name = L"loadConfigButton";
			this->loadConfigButton->Size = System::Drawing::Size(183, 31);
			this->loadConfigButton->TabIndex = 66;
			this->loadConfigButton->Text = L"Load Configuration File";
			this->loadConfigButton->UseVisualStyleBackColor = true;
			this->loadConfigButton->Click += gcnew System::EventHandler(this, &Form1::loadConfigButton_Click);
			// 
			// saveConfigButton
			// 
			this->saveConfigButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->saveConfigButton->Location = System::Drawing::Point(20, 220);
			this->saveConfigButton->Name = L"saveConfigButton";
			this->saveConfigButton->Size = System::Drawing::Size(183, 31);
			this->saveConfigButton->TabIndex = 65;
			this->saveConfigButton->Text = L"Save Configuration File";
			this->saveConfigButton->UseVisualStyleBackColor = true;
			this->saveConfigButton->Click += gcnew System::EventHandler(this, &Form1::saveConfigButton_Click);
			// 
			// oneAmpRB
			// 
			this->oneAmpRB->AutoSize = true;
			this->oneAmpRB->Checked = true;
			this->oneAmpRB->Location = System::Drawing::Point(666, 10);
			this->oneAmpRB->Name = L"oneAmpRB";
			this->oneAmpRB->Size = System::Drawing::Size(58, 24);
			this->oneAmpRB->TabIndex = 64;
			this->oneAmpRB->TabStop = true;
			this->oneAmpRB->Text = L"1 A";
			this->oneAmpRB->UseVisualStyleBackColor = true;
			this->oneAmpRB->CheckedChanged += gcnew System::EventHandler(this, &Form1::testCurrentChange);
			// 
			// twoAmpRB
			// 
			this->twoAmpRB->AutoSize = true;
			this->twoAmpRB->Location = System::Drawing::Point(666, 29);
			this->twoAmpRB->Name = L"twoAmpRB";
			this->twoAmpRB->Size = System::Drawing::Size(58, 24);
			this->twoAmpRB->TabIndex = 63;
			this->twoAmpRB->Text = L"2 A";
			this->twoAmpRB->UseVisualStyleBackColor = true;
			this->twoAmpRB->CheckedChanged += gcnew System::EventHandler(this, &Form1::testCurrentChange);
			// 
			// checkBoxZR
			// 
			this->checkBoxZR->AutoSize = true;
			this->checkBoxZR->Location = System::Drawing::Point(672, 141);
			this->checkBoxZR->Name = L"checkBoxZR";
			this->checkBoxZR->Size = System::Drawing::Size(22, 21);
			this->checkBoxZR->TabIndex = 62;
			this->checkBoxZR->UseVisualStyleBackColor = true;
			this->checkBoxZR->Click += gcnew System::EventHandler(this, &Form1::oneAmpCheck);
			// 
			// checkBoxZF
			// 
			this->checkBoxZF->AutoSize = true;
			this->checkBoxZF->Location = System::Drawing::Point(621, 141);
			this->checkBoxZF->Name = L"checkBoxZF";
			this->checkBoxZF->Size = System::Drawing::Size(22, 21);
			this->checkBoxZF->TabIndex = 61;
			this->checkBoxZF->UseVisualStyleBackColor = true;
			this->checkBoxZF->Click += gcnew System::EventHandler(this, &Form1::oneAmpCheck);
			// 
			// checkBoxYR
			// 
			this->checkBoxYR->AutoSize = true;
			this->checkBoxYR->Location = System::Drawing::Point(672, 109);
			this->checkBoxYR->Name = L"checkBoxYR";
			this->checkBoxYR->Size = System::Drawing::Size(22, 21);
			this->checkBoxYR->TabIndex = 60;
			this->checkBoxYR->UseVisualStyleBackColor = true;
			this->checkBoxYR->Click += gcnew System::EventHandler(this, &Form1::oneAmpCheck);
			// 
			// checkBoxYF
			// 
			this->checkBoxYF->AutoSize = true;
			this->checkBoxYF->Location = System::Drawing::Point(621, 109);
			this->checkBoxYF->Name = L"checkBoxYF";
			this->checkBoxYF->Size = System::Drawing::Size(22, 21);
			this->checkBoxYF->TabIndex = 59;
			this->checkBoxYF->UseVisualStyleBackColor = true;
			this->checkBoxYF->Click += gcnew System::EventHandler(this, &Form1::oneAmpCheck);
			// 
			// checkBoxXR
			// 
			this->checkBoxXR->AutoSize = true;
			this->checkBoxXR->Location = System::Drawing::Point(672, 77);
			this->checkBoxXR->Name = L"checkBoxXR";
			this->checkBoxXR->Size = System::Drawing::Size(22, 21);
			this->checkBoxXR->TabIndex = 58;
			this->checkBoxXR->UseVisualStyleBackColor = true;
			this->checkBoxXR->Click += gcnew System::EventHandler(this, &Form1::oneAmpCheck);
			// 
			// checkBoxXF
			// 
			this->checkBoxXF->AutoSize = true;
			this->checkBoxXF->Location = System::Drawing::Point(621, 77);
			this->checkBoxXF->Name = L"checkBoxXF";
			this->checkBoxXF->Size = System::Drawing::Size(22, 21);
			this->checkBoxXF->TabIndex = 57;
			this->checkBoxXF->UseVisualStyleBackColor = true;
			this->checkBoxXF->Click += gcnew System::EventHandler(this, &Form1::oneAmpCheck);
			// 
			// label21
			// 
			this->label21->AutoSize = true;
			this->label21->Location = System::Drawing::Point(657, 53);
			this->label21->Name = L"label21";
			this->label21->Size = System::Drawing::Size(68, 20);
			this->label21->TabIndex = 56;
			this->label21->Text = L"Reverse";
			this->label21->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label20
			// 
			this->label20->AutoSize = true;
			this->label20->Location = System::Drawing::Point(605, 53);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(67, 20);
			this->label20->TabIndex = 55;
			this->label20->Text = L"Forward";
			this->label20->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label18
			// 
			this->label18->AutoSize = true;
			this->label18->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label18->Location = System::Drawing::Point(617, 20);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(65, 25);
			this->label18->TabIndex = 53;
			this->label18->Text = L"Send:";
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label17->Location = System::Drawing::Point(634, 227);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(36, 25);
			this->label17->TabIndex = 52;
			this->label17->Text = L"Hz";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label8->Location = System::Drawing::Point(634, 191);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(36, 25);
			this->label8->TabIndex = 51;
			this->label8->Text = L"Hz";
			// 
			// textBoxOPF
			// 
			this->textBoxOPF->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxOPF->Location = System::Drawing::Point(541, 221);
			this->textBoxOPF->Name = L"textBoxOPF";
			this->textBoxOPF->Size = System::Drawing::Size(85, 35);
			this->textBoxOPF->TabIndex = 50;
			this->textBoxOPF->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->textBoxOPF->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask3);
			this->textBoxOPF->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxOPF->Leave += gcnew System::EventHandler(this, &Form1::textBox_LeavePF);
			// 
			// textBoxIPF
			// 
			this->textBoxIPF->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxIPF->Location = System::Drawing::Point(541, 185);
			this->textBoxIPF->Name = L"textBoxIPF";
			this->textBoxIPF->Size = System::Drawing::Size(85, 35);
			this->textBoxIPF->TabIndex = 49;
			this->textBoxIPF->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->textBoxIPF->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask3);
			this->textBoxIPF->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxIPF->Leave += gcnew System::EventHandler(this, &Form1::textBox_LeavePF);
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label7->Location = System::Drawing::Point(403, 227);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(196, 25);
			this->label7->TabIndex = 48;
			this->label7->Text = L"Outplane Frequency:";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label6->Location = System::Drawing::Point(413, 191);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(180, 25);
			this->label6->TabIndex = 47;
			this->label6->Text = L"Inplane Frequency:";
			// 
			// textBoxOAZ
			// 
			this->textBoxOAZ->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxOAZ->Location = System::Drawing::Point(69, 135);
			this->textBoxOAZ->Name = L"textBoxOAZ";
			this->textBoxOAZ->Size = System::Drawing::Size(53, 35);
			this->textBoxOAZ->TabIndex = 3;
			this->textBoxOAZ->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask1);
			this->textBoxOAZ->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxOAZ->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave0Update);
			// 
			// textBoxOAY
			// 
			this->textBoxOAY->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxOAY->Location = System::Drawing::Point(69, 103);
			this->textBoxOAY->Name = L"textBoxOAY";
			this->textBoxOAY->Size = System::Drawing::Size(53, 35);
			this->textBoxOAY->TabIndex = 2;
			this->textBoxOAY->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask1);
			this->textBoxOAY->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxOAY->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave0Update);
			// 
			// textBoxOAX
			// 
			this->textBoxOAX->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxOAX->Location = System::Drawing::Point(69, 71);
			this->textBoxOAX->Name = L"textBoxOAX";
			this->textBoxOAX->Size = System::Drawing::Size(53, 35);
			this->textBoxOAX->TabIndex = 1;
			this->textBoxOAX->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask1);
			this->textBoxOAX->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxOAX->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave0Update);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(70, 20);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(79, 25);
			this->label5->TabIndex = 43;
			this->label5->Text = L"Oe to A";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(449, 53);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(103, 20);
			this->label4->TabIndex = 34;
			this->label4->Text = L"User Defined";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label30
			// 
			this->label30->AutoSize = true;
			this->label30->Location = System::Drawing::Point(526, 53);
			this->label30->Name = L"label30";
			this->label30->Size = System::Drawing::Size(84, 20);
			this->label30->TabIndex = 40;
			this->label30->Text = L"Calculated";
			this->label30->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// labelCMFZ
			// 
			this->labelCMFZ->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->labelCMFZ->Enabled = false;
			this->labelCMFZ->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelCMFZ->ImageAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->labelCMFZ->Location = System::Drawing::Point(533, 135);
			this->labelCMFZ->Name = L"labelCMFZ";
			this->labelCMFZ->Size = System::Drawing::Size(39, 26);
			this->labelCMFZ->TabIndex = 39;
			// 
			// labelCMFY
			// 
			this->labelCMFY->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->labelCMFY->Enabled = false;
			this->labelCMFY->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelCMFY->ImageAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->labelCMFY->Location = System::Drawing::Point(533, 103);
			this->labelCMFY->Name = L"labelCMFY";
			this->labelCMFY->Size = System::Drawing::Size(39, 26);
			this->labelCMFY->TabIndex = 38;
			// 
			// labelCMFX
			// 
			this->labelCMFX->BackColor = System::Drawing::Color::White;
			this->labelCMFX->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->labelCMFX->Enabled = false;
			this->labelCMFX->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelCMFX->ImageAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->labelCMFX->Location = System::Drawing::Point(533, 71);
			this->labelCMFX->Name = L"labelCMFX";
			this->labelCMFX->Size = System::Drawing::Size(39, 26);
			this->labelCMFX->TabIndex = 37;
			// 
			// textBoxMFZ
			// 
			this->textBoxMFZ->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxMFZ->Location = System::Drawing::Point(463, 135);
			this->textBoxMFZ->Name = L"textBoxMFZ";
			this->textBoxMFZ->Size = System::Drawing::Size(39, 35);
			this->textBoxMFZ->TabIndex = 17;
			this->textBoxMFZ->Text = L"100";
			this->textBoxMFZ->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->textBoxMFZ->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask3);
			this->textBoxMFZ->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxMFZ->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave1Update);
			// 
			// textBoxMFY
			// 
			this->textBoxMFY->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxMFY->Location = System::Drawing::Point(463, 103);
			this->textBoxMFY->Name = L"textBoxMFY";
			this->textBoxMFY->Size = System::Drawing::Size(39, 35);
			this->textBoxMFY->TabIndex = 16;
			this->textBoxMFY->Text = L"100";
			this->textBoxMFY->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->textBoxMFY->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask3);
			this->textBoxMFY->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxMFY->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave1Update);
			// 
			// textBoxMFX
			// 
			this->textBoxMFX->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxMFX->Location = System::Drawing::Point(463, 71);
			this->textBoxMFX->Name = L"textBoxMFX";
			this->textBoxMFX->Size = System::Drawing::Size(39, 35);
			this->textBoxMFX->TabIndex = 15;
			this->textBoxMFX->Text = L"100";
			this->textBoxMFX->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->textBoxMFX->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask3);
			this->textBoxMFX->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxMFX->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave1Update);
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label16->Location = System::Drawing::Point(468, 20);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(143, 25);
			this->label16->TabIndex = 23;
			this->label16->Text = L"Max Field (Oe)";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label15->Location = System::Drawing::Point(331, 20);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(97, 25);
			this->label15->TabIndex = 22;
			this->label15->Text = L"Offset (A)";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label14->Location = System::Drawing::Point(170, 20);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(142, 25);
			this->label14->TabIndex = 21;
			this->label14->Text = L"Resistance (Ω)";
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(366, 53);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(68, 20);
			this->label13->TabIndex = 20;
			this->label13->Text = L"Reverse";
			this->label13->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(308, 53);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(67, 20);
			this->label12->TabIndex = 19;
			this->label12->Text = L"Forward";
			this->label12->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F));
			this->label11->Location = System::Drawing::Point(11, 138);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(35, 29);
			this->label11->TabIndex = 18;
			this->label11->Text = L"Z:";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F));
			this->label10->Location = System::Drawing::Point(11, 106);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(36, 29);
			this->label10->TabIndex = 17;
			this->label10->Text = L"Y:";
			// 
			// Setup_X
			// 
			this->Setup_X->AutoSize = true;
			this->Setup_X->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F));
			this->Setup_X->Location = System::Drawing::Point(11, 74);
			this->Setup_X->Name = L"Setup_X";
			this->Setup_X->Size = System::Drawing::Size(37, 29);
			this->Setup_X->TabIndex = 16;
			this->Setup_X->Text = L"X:";
			// 
			// textBoxORZ
			// 
			this->textBoxORZ->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxORZ->Location = System::Drawing::Point(363, 135);
			this->textBoxORZ->Name = L"textBoxORZ";
			this->textBoxORZ->Size = System::Drawing::Size(53, 35);
			this->textBoxORZ->TabIndex = 12;
			this->textBoxORZ->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->textBoxORZ->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxORZ->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave0Update);
			// 
			// textBoxOFZ
			// 
			this->textBoxOFZ->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxOFZ->Location = System::Drawing::Point(304, 135);
			this->textBoxOFZ->Name = L"textBoxOFZ";
			this->textBoxOFZ->Size = System::Drawing::Size(53, 35);
			this->textBoxOFZ->TabIndex = 11;
			this->textBoxOFZ->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->textBoxOFZ->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxOFZ->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave0Update);
			// 
			// textBoxORY
			// 
			this->textBoxORY->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxORY->Location = System::Drawing::Point(363, 103);
			this->textBoxORY->Name = L"textBoxORY";
			this->textBoxORY->Size = System::Drawing::Size(53, 35);
			this->textBoxORY->TabIndex = 10;
			this->textBoxORY->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->textBoxORY->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxORY->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave0Update);
			// 
			// textBoxOFY
			// 
			this->textBoxOFY->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxOFY->Location = System::Drawing::Point(304, 103);
			this->textBoxOFY->Name = L"textBoxOFY";
			this->textBoxOFY->Size = System::Drawing::Size(53, 35);
			this->textBoxOFY->TabIndex = 9;
			this->textBoxOFY->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->textBoxOFY->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxOFY->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave0Update);
			// 
			// textBoxORX
			// 
			this->textBoxORX->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxORX->Location = System::Drawing::Point(363, 71);
			this->textBoxORX->Name = L"textBoxORX";
			this->textBoxORX->Size = System::Drawing::Size(53, 35);
			this->textBoxORX->TabIndex = 8;
			this->textBoxORX->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->textBoxORX->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxORX->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave0Update);
			// 
			// textBoxOFX
			// 
			this->textBoxOFX->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxOFX->Location = System::Drawing::Point(304, 71);
			this->textBoxOFX->Name = L"textBoxOFX";
			this->textBoxOFX->Size = System::Drawing::Size(53, 35);
			this->textBoxOFX->TabIndex = 7;
			this->textBoxOFX->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->textBoxOFX->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxOFX->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave0Update);
			// 
			// textBoxRZ
			// 
			this->textBoxRZ->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxRZ->Location = System::Drawing::Point(193, 135);
			this->textBoxRZ->Name = L"textBoxRZ";
			this->textBoxRZ->Size = System::Drawing::Size(53, 35);
			this->textBoxRZ->TabIndex = 6;
			this->textBoxRZ->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->textBoxRZ->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxRZ->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave1Update);
			// 
			// textBoxRY
			// 
			this->textBoxRY->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxRY->Location = System::Drawing::Point(193, 103);
			this->textBoxRY->Name = L"textBoxRY";
			this->textBoxRY->Size = System::Drawing::Size(53, 35);
			this->textBoxRY->TabIndex = 5;
			this->textBoxRY->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->textBoxRY->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxRY->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave1Update);
			// 
			// textBoxRX
			// 
			this->textBoxRX->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBoxRX->Location = System::Drawing::Point(193, 71);
			this->textBoxRX->Name = L"textBoxRX";
			this->textBoxRX->Size = System::Drawing::Size(53, 35);
			this->textBoxRX->TabIndex = 4;
			this->textBoxRX->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->textBoxRX->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->textBoxRX->Leave += gcnew System::EventHandler(this, &Form1::textBox_Leave1Update);
			// 
			// shapeContainer2
			// 
			this->shapeContainer2->Location = System::Drawing::Point(3, 3);
			this->shapeContainer2->Margin = System::Windows::Forms::Padding(0);
			this->shapeContainer2->Name = L"shapeContainer2";
			this->shapeContainer2->Shapes->AddRange(gcnew cli::array< Microsoft::VisualBasic::PowerPacks::Shape^  >(12) {
				this->lineShape12,
					this->lineShape11, this->lineShape10, this->lineShape9, this->lineShape8, this->lineShape7, this->lineShape6, this->lineShape5,
					this->lineShape4, this->lineShape3, this->lineShape2, this->lineShape1
			});
			this->shapeContainer2->Size = System::Drawing::Size(707, 251);
			this->shapeContainer2->TabIndex = 41;
			this->shapeContainer2->TabStop = false;
			// 
			// lineShape12
			// 
			this->lineShape12->Enabled = false;
			this->lineShape12->Name = L"lineShape12";
			this->lineShape12->X1 = 706;
			this->lineShape12->X2 = 706;
			this->lineShape12->Y1 = 0;
			this->lineShape12->Y2 = 162;
			// 
			// lineShape11
			// 
			this->lineShape11->Enabled = false;
			this->lineShape11->Name = L"lineShape11";
			this->lineShape11->X1 = 281;
			this->lineShape11->X2 = 281;
			this->lineShape11->Y1 = 0;
			this->lineShape11->Y2 = 162;
			// 
			// lineShape10
			// 
			this->lineShape10->Enabled = false;
			this->lineShape10->Name = L"lineShape10";
			this->lineShape10->X1 = 0;
			this->lineShape10->X2 = 706;
			this->lineShape10->Y1 = 0;
			this->lineShape10->Y2 = 0;
			// 
			// lineShape9
			// 
			this->lineShape9->Enabled = false;
			this->lineShape9->Name = L"lineShape9";
			this->lineShape9->X1 = 590;
			this->lineShape9->X2 = 590;
			this->lineShape9->Y1 = 0;
			this->lineShape9->Y2 = 162;
			// 
			// lineShape8
			// 
			this->lineShape8->Enabled = false;
			this->lineShape8->Name = L"lineShape8";
			this->lineShape8->X1 = 0;
			this->lineShape8->X2 = 0;
			this->lineShape8->Y1 = 0;
			this->lineShape8->Y2 = 162;
			// 
			// lineShape7
			// 
			this->lineShape7->Enabled = false;
			this->lineShape7->Name = L"lineShape7";
			this->lineShape7->X1 = 433;
			this->lineShape7->X2 = 433;
			this->lineShape7->Y1 = 0;
			this->lineShape7->Y2 = 162;
			// 
			// lineShape6
			// 
			this->lineShape6->Enabled = false;
			this->lineShape6->Name = L"lineShape6";
			this->lineShape6->X1 = 144;
			this->lineShape6->X2 = 144;
			this->lineShape6->Y1 = 0;
			this->lineShape6->Y2 = 162;
			// 
			// lineShape5
			// 
			this->lineShape5->Enabled = false;
			this->lineShape5->Name = L"lineShape5";
			this->lineShape5->X1 = 0;
			this->lineShape5->X2 = 706;
			this->lineShape5->Y1 = 64;
			this->lineShape5->Y2 = 64;
			// 
			// lineShape4
			// 
			this->lineShape4->Enabled = false;
			this->lineShape4->Name = L"lineShape4";
			this->lineShape4->X1 = 0;
			this->lineShape4->X2 = 706;
			this->lineShape4->Y1 = 162;
			this->lineShape4->Y2 = 162;
			// 
			// lineShape3
			// 
			this->lineShape3->Enabled = false;
			this->lineShape3->Name = L"lineShape3";
			this->lineShape3->X1 = 39;
			this->lineShape3->X2 = 39;
			this->lineShape3->Y1 = 0;
			this->lineShape3->Y2 = 162;
			// 
			// lineShape2
			// 
			this->lineShape2->Enabled = false;
			this->lineShape2->Name = L"lineShape2";
			this->lineShape2->X1 = 0;
			this->lineShape2->X2 = 706;
			this->lineShape2->Y1 = 129;
			this->lineShape2->Y2 = 129;
			// 
			// lineShape1
			// 
			this->lineShape1->Enabled = false;
			this->lineShape1->Name = L"lineShape1";
			this->lineShape1->X1 = 0;
			this->lineShape1->X2 = 706;
			this->lineShape1->Y1 = 97;
			this->lineShape1->Y2 = 97;
			// 
			// constantTab
			// 
			this->constantTab->Controls->Add(this->label34);
			this->constantTab->Controls->Add(this->label33);
			this->constantTab->Controls->Add(this->label32);
			this->constantTab->Controls->Add(this->label31);
			this->constantTab->Controls->Add(this->label29);
			this->constantTab->Controls->Add(this->label28);
			this->constantTab->Controls->Add(this->label27);
			this->constantTab->Controls->Add(this->label26);
			this->constantTab->Controls->Add(this->SendPosButt);
			this->constantTab->Controls->Add(this->textBox_X);
			this->constantTab->Controls->Add(this->textBox_Y);
			this->constantTab->Controls->Add(this->label3);
			this->constantTab->Controls->Add(this->textBox_Z);
			this->constantTab->Controls->Add(this->label2);
			this->constantTab->Controls->Add(this->label1);
			this->constantTab->Controls->Add(this->shapeContainer3);
			this->constantTab->Location = System::Drawing::Point(4, 29);
			this->constantTab->Name = L"constantTab";
			this->constantTab->Padding = System::Windows::Forms::Padding(3);
			this->constantTab->Size = System::Drawing::Size(713, 257);
			this->constantTab->TabIndex = 1;
			this->constantTab->Text = L"Constant";
			this->constantTab->UseVisualStyleBackColor = true;
			// 
			// label34
			// 
			this->label34->AutoSize = true;
			this->label34->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label34->Location = System::Drawing::Point(381, 17);
			this->label34->Name = L"label34";
			this->label34->Size = System::Drawing::Size(23, 25);
			this->label34->TabIndex = 20;
			this->label34->Text = L"θ";
			// 
			// label33
			// 
			this->label33->AutoSize = true;
			this->label33->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label33->Location = System::Drawing::Point(575, 238);
			this->label33->Name = L"label33";
			this->label33->Size = System::Drawing::Size(32, 25);
			this->label33->TabIndex = 19;
			this->label33->Text = L"-Y";
			// 
			// label32
			// 
			this->label32->AutoSize = true;
			this->label32->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label32->Location = System::Drawing::Point(575, 17);
			this->label32->Name = L"label32";
			this->label32->Size = System::Drawing::Size(37, 25);
			this->label32->TabIndex = 18;
			this->label32->Text = L"+Y";
			// 
			// label31
			// 
			this->label31->AutoSize = true;
			this->label31->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label31->Location = System::Drawing::Point(460, 127);
			this->label31->Name = L"label31";
			this->label31->Size = System::Drawing::Size(33, 25);
			this->label31->TabIndex = 17;
			this->label31->Text = L"-X";
			// 
			// label29
			// 
			this->label29->AutoSize = true;
			this->label29->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label29->Location = System::Drawing::Point(686, 127);
			this->label29->Name = L"label29";
			this->label29->Size = System::Drawing::Size(38, 25);
			this->label29->TabIndex = 16;
			this->label29->Text = L"+X";
			// 
			// label28
			// 
			this->label28->AutoSize = true;
			this->label28->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label28->Location = System::Drawing::Point(172, 139);
			this->label28->Name = L"label28";
			this->label28->Size = System::Drawing::Size(46, 29);
			this->label28->TabIndex = 14;
			this->label28->Text = L"Oe";
			// 
			// label27
			// 
			this->label27->AutoSize = true;
			this->label27->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label27->Location = System::Drawing::Point(170, 107);
			this->label27->Name = L"label27";
			this->label27->Size = System::Drawing::Size(46, 29);
			this->label27->TabIndex = 13;
			this->label27->Text = L"Oe";
			// 
			// label26
			// 
			this->label26->AutoSize = true;
			this->label26->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label26->Location = System::Drawing::Point(170, 75);
			this->label26->Name = L"label26";
			this->label26->Size = System::Drawing::Size(46, 29);
			this->label26->TabIndex = 12;
			this->label26->Text = L"Oe";
			// 
			// shapeContainer3
			// 
			this->shapeContainer3->Location = System::Drawing::Point(3, 3);
			this->shapeContainer3->Margin = System::Windows::Forms::Padding(0);
			this->shapeContainer3->Name = L"shapeContainer3";
			this->shapeContainer3->Shapes->AddRange(gcnew cli::array< Microsoft::VisualBasic::PowerPacks::Shape^  >(15) {
				this->ConstVectThetaB,
					this->ConstVectThetaA, this->ConstVectThetaMain, this->lineShape21, this->lineShape19, this->lineShape20, this->rectangleShape2,
					this->ovalShape3, this->ConstantVectB, this->ConstantVectA, this->ConstantVectMain, this->lineShape18, this->lineShape17, this->lineShape16,
					this->rectangleShape1
			});
			this->shapeContainer3->Size = System::Drawing::Size(707, 251);
			this->shapeContainer3->TabIndex = 15;
			this->shapeContainer3->TabStop = false;
			// 
			// ConstVectThetaB
			// 
			this->ConstVectThetaB->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->ConstVectThetaB->BorderWidth = 3;
			this->ConstVectThetaB->Enabled = false;
			this->ConstVectThetaB->Name = L"ConstVectThetaB";
			this->ConstVectThetaB->Visible = false;
			this->ConstVectThetaB->X1 = 385;
			this->ConstVectThetaB->X2 = 435;
			this->ConstVectThetaB->Y1 = 253;
			this->ConstVectThetaB->Y2 = 253;
			// 
			// ConstVectThetaA
			// 
			this->ConstVectThetaA->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->ConstVectThetaA->BorderWidth = 3;
			this->ConstVectThetaA->Enabled = false;
			this->ConstVectThetaA->Name = L"ConstVectThetaA";
			this->ConstVectThetaA->Visible = false;
			this->ConstVectThetaA->X1 = 385;
			this->ConstVectThetaA->X2 = 435;
			this->ConstVectThetaA->Y1 = 246;
			this->ConstVectThetaA->Y2 = 246;
			// 
			// ConstVectThetaMain
			// 
			this->ConstVectThetaMain->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->ConstVectThetaMain->BorderWidth = 3;
			this->ConstVectThetaMain->Enabled = false;
			this->ConstVectThetaMain->Name = L"ConstVectThetaMain";
			this->ConstVectThetaMain->Visible = false;
			this->ConstVectThetaMain->X1 = 385;
			this->ConstVectThetaMain->X2 = 435;
			this->ConstVectThetaMain->Y1 = 239;
			this->ConstVectThetaMain->Y2 = 239;
			// 
			// lineShape21
			// 
			this->lineShape21->Enabled = false;
			this->lineShape21->Name = L"lineShape21";
			this->lineShape21->X1 = 337;
			this->lineShape21->X2 = 437;
			this->lineShape21->Y1 = 132;
			this->lineShape21->Y2 = 132;
			// 
			// lineShape19
			// 
			this->lineShape19->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dot;
			this->lineShape19->BorderWidth = 5;
			this->lineShape19->Enabled = false;
			this->lineShape19->Name = L"lineShape19";
			this->lineShape19->X1 = 320;
			this->lineShape19->X2 = 320;
			this->lineShape19->Y1 = 0;
			this->lineShape19->Y2 = 264;
			// 
			// lineShape20
			// 
			this->lineShape20->BorderWidth = 2;
			this->lineShape20->Enabled = false;
			this->lineShape20->Name = L"lineShape20";
			this->lineShape20->X1 = 336;
			this->lineShape20->X2 = 336;
			this->lineShape20->Y1 = 32;
			this->lineShape20->Y2 = 232;
			// 
			// rectangleShape2
			// 
			this->rectangleShape2->BackColor = System::Drawing::Color::White;
			this->rectangleShape2->BackStyle = Microsoft::VisualBasic::PowerPacks::BackStyle::Opaque;
			this->rectangleShape2->BorderColor = System::Drawing::Color::Transparent;
			this->rectangleShape2->Enabled = false;
			this->rectangleShape2->Location = System::Drawing::Point(236, 30);
			this->rectangleShape2->Name = L"rectangleShape2";
			this->rectangleShape2->Size = System::Drawing::Size(100, 205);
			// 
			// ovalShape3
			// 
			this->ovalShape3->BorderWidth = 2;
			this->ovalShape3->Enabled = false;
			this->ovalShape3->Location = System::Drawing::Point(237, 32);
			this->ovalShape3->Name = L"ovalShape3";
			this->ovalShape3->Size = System::Drawing::Size(200, 200);
			// 
			// ConstantVectB
			// 
			this->ConstantVectB->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->ConstantVectB->BorderWidth = 3;
			this->ConstantVectB->Enabled = false;
			this->ConstantVectB->Name = L"ConstantVectB";
			this->ConstantVectB->Visible = false;
			this->ConstantVectB->X1 = 645;
			this->ConstantVectB->X2 = 695;
			this->ConstantVectB->Y1 = 254;
			this->ConstantVectB->Y2 = 254;
			// 
			// ConstantVectA
			// 
			this->ConstantVectA->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->ConstantVectA->BorderWidth = 3;
			this->ConstantVectA->Enabled = false;
			this->ConstantVectA->Name = L"ConstantVectA";
			this->ConstantVectA->Visible = false;
			this->ConstantVectA->X1 = 645;
			this->ConstantVectA->X2 = 695;
			this->ConstantVectA->Y1 = 247;
			this->ConstantVectA->Y2 = 247;
			// 
			// ConstantVectMain
			// 
			this->ConstantVectMain->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->ConstantVectMain->BorderWidth = 3;
			this->ConstantVectMain->Enabled = false;
			this->ConstantVectMain->Name = L"ConstantVectMain";
			this->ConstantVectMain->Visible = false;
			this->ConstantVectMain->X1 = 645;
			this->ConstantVectMain->X2 = 695;
			this->ConstantVectMain->Y1 = 240;
			this->ConstantVectMain->Y2 = 240;
			// 
			// lineShape18
			// 
			this->lineShape18->Enabled = false;
			this->lineShape18->Name = L"lineShape18";
			this->lineShape18->X1 = 482;
			this->lineShape18->X2 = 682;
			this->lineShape18->Y1 = 132;
			this->lineShape18->Y2 = 132;
			// 
			// lineShape17
			// 
			this->lineShape17->Enabled = false;
			this->lineShape17->Name = L"lineShape17";
			this->lineShape17->X1 = 582;
			this->lineShape17->X2 = 582;
			this->lineShape17->Y1 = 32;
			this->lineShape17->Y2 = 232;
			// 
			// lineShape16
			// 
			this->lineShape16->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dot;
			this->lineShape16->BorderWidth = 5;
			this->lineShape16->Enabled = false;
			this->lineShape16->Name = L"lineShape16";
			this->lineShape16->X1 = 450;
			this->lineShape16->X2 = 450;
			this->lineShape16->Y1 = 0;
			this->lineShape16->Y2 = 264;
			// 
			// rectangleShape1
			// 
			this->rectangleShape1->BorderWidth = 2;
			this->rectangleShape1->Enabled = false;
			this->rectangleShape1->Location = System::Drawing::Point(482, 32);
			this->rectangleShape1->Name = L"rectangleShape1";
			this->rectangleShape1->Size = System::Drawing::Size(200, 200);
			// 
			// precessionTab
			// 
			this->precessionTab->Controls->Add(this->PrecessionBox);
			this->precessionTab->Controls->Add(this->label35);
			this->precessionTab->Controls->Add(this->precThetaBox);
			this->precessionTab->Controls->Add(this->label50);
			this->precessionTab->Controls->Add(this->label49);
			this->precessionTab->Controls->Add(this->label48);
			this->precessionTab->Controls->Add(this->label47);
			this->precessionTab->Controls->Add(this->label46);
			this->precessionTab->Controls->Add(this->precessButton);
			this->precessionTab->Controls->Add(this->precPeriodBox);
			this->precessionTab->Controls->Add(this->precFreqBox);
			this->precessionTab->Controls->Add(this->precOPBox);
			this->precessionTab->Controls->Add(this->label45);
			this->precessionTab->Controls->Add(this->label44);
			this->precessionTab->Controls->Add(this->label43);
			this->precessionTab->Controls->Add(this->precIPBox);
			this->precessionTab->Controls->Add(this->label42);
			this->precessionTab->Controls->Add(this->label36);
			this->precessionTab->Controls->Add(this->label37);
			this->precessionTab->Controls->Add(this->label38);
			this->precessionTab->Controls->Add(this->label39);
			this->precessionTab->Controls->Add(this->shapeContainer4);
			this->precessionTab->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->precessionTab->Location = System::Drawing::Point(4, 29);
			this->precessionTab->Name = L"precessionTab";
			this->precessionTab->Size = System::Drawing::Size(713, 257);
			this->precessionTab->TabIndex = 2;
			this->precessionTab->Text = L"Precession";
			this->precessionTab->UseVisualStyleBackColor = true;
			// 
			// PrecessionBox
			// 
			this->PrecessionBox->Controls->Add(this->precCCW);
			this->PrecessionBox->Controls->Add(this->precCW);
			this->PrecessionBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->PrecessionBox->Location = System::Drawing::Point(215, 32);
			this->PrecessionBox->Name = L"PrecessionBox";
			this->PrecessionBox->Size = System::Drawing::Size(96, 62);
			this->PrecessionBox->TabIndex = 44;
			this->PrecessionBox->TabStop = false;
			// 
			// precCCW
			// 
			this->precCCW->AutoSize = true;
			this->precCCW->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->precCCW->Location = System::Drawing::Point(14, 33);
			this->precCCW->Name = L"precCCW";
			this->precCCW->Size = System::Drawing::Size(98, 33);
			this->precCCW->TabIndex = 40;
			this->precCCW->Text = L"CCW";
			this->precCCW->UseVisualStyleBackColor = true;
			// 
			// precCW
			// 
			this->precCW->AutoSize = true;
			this->precCW->Checked = true;
			this->precCW->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->precCW->Location = System::Drawing::Point(14, 10);
			this->precCW->Name = L"precCW";
			this->precCW->Size = System::Drawing::Size(80, 33);
			this->precCW->TabIndex = 39;
			this->precCW->TabStop = true;
			this->precCW->Text = L"CW";
			this->precCW->UseVisualStyleBackColor = true;
			// 
			// label35
			// 
			this->label35->AutoSize = true;
			this->label35->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label35->Location = System::Drawing::Point(381, 17);
			this->label35->Name = L"label35";
			this->label35->Size = System::Drawing::Size(23, 25);
			this->label35->TabIndex = 43;
			this->label35->Text = L"θ";
			// 
			// precThetaBox
			// 
			this->precThetaBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->precThetaBox->Location = System::Drawing::Point(115, 101);
			this->precThetaBox->MaxLength = 7;
			this->precThetaBox->Name = L"precThetaBox";
			this->precThetaBox->Size = System::Drawing::Size(59, 35);
			this->precThetaBox->TabIndex = 42;
			this->precThetaBox->Text = L"0";
			this->precThetaBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask1);
			this->precThetaBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->precThetaBox->Leave += gcnew System::EventHandler(this, &Form1::setPlaneAndTheta);
			// 
			// label50
			// 
			this->label50->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label50->AutoSize = true;
			this->label50->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label50->Location = System::Drawing::Point(57, 104);
			this->label50->Name = L"label50";
			this->label50->Size = System::Drawing::Size(84, 29);
			this->label50->TabIndex = 41;
			this->label50->Text = L"Theta:";
			// 
			// label49
			// 
			this->label49->AutoSize = true;
			this->label49->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label49->Location = System::Drawing::Point(177, 175);
			this->label49->Name = L"label49";
			this->label49->Size = System::Drawing::Size(25, 29);
			this->label49->TabIndex = 38;
			this->label49->Text = L"s";
			// 
			// label48
			// 
			this->label48->AutoSize = true;
			this->label48->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label48->Location = System::Drawing::Point(177, 143);
			this->label48->Name = L"label48";
			this->label48->Size = System::Drawing::Size(41, 29);
			this->label48->TabIndex = 37;
			this->label48->Text = L"Hz";
			// 
			// label47
			// 
			this->label47->AutoSize = true;
			this->label47->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label47->Location = System::Drawing::Point(158, 72);
			this->label47->Name = L"label47";
			this->label47->Size = System::Drawing::Size(46, 29);
			this->label47->TabIndex = 36;
			this->label47->Text = L"Oe";
			// 
			// label46
			// 
			this->label46->AutoSize = true;
			this->label46->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label46->Location = System::Drawing::Point(158, 38);
			this->label46->Name = L"label46";
			this->label46->Size = System::Drawing::Size(46, 29);
			this->label46->TabIndex = 35;
			this->label46->Text = L"Oe";
			// 
			// precessButton
			// 
			this->precessButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->precessButton->Location = System::Drawing::Point(94, 217);
			this->precessButton->Name = L"precessButton";
			this->precessButton->Size = System::Drawing::Size(160, 35);
			this->precessButton->TabIndex = 34;
			this->precessButton->Text = L"Send";
			this->precessButton->UseVisualStyleBackColor = true;
			this->precessButton->Click += gcnew System::EventHandler(this, &Form1::precessButton_Click);
			// 
			// precPeriodBox
			// 
			this->precPeriodBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->precPeriodBox->Location = System::Drawing::Point(114, 172);
			this->precPeriodBox->MaxLength = 12;
			this->precPeriodBox->Name = L"precPeriodBox";
			this->precPeriodBox->Size = System::Drawing::Size(60, 35);
			this->precPeriodBox->TabIndex = 33;
			this->precPeriodBox->Text = L"1";
			this->precPeriodBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->precPeriodBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->precPeriodBox->Leave += gcnew System::EventHandler(this, &Form1::setPeriodAndFrequency);
			// 
			// precFreqBox
			// 
			this->precFreqBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->precFreqBox->Location = System::Drawing::Point(114, 140);
			this->precFreqBox->MaxLength = 12;
			this->precFreqBox->Name = L"precFreqBox";
			this->precFreqBox->Size = System::Drawing::Size(60, 35);
			this->precFreqBox->TabIndex = 32;
			this->precFreqBox->Text = L"1";
			this->precFreqBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->precFreqBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->precFreqBox->Leave += gcnew System::EventHandler(this, &Form1::setPeriodAndFrequency);
			// 
			// precOPBox
			// 
			this->precOPBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->precOPBox->Location = System::Drawing::Point(115, 69);
			this->precOPBox->MaxLength = 7;
			this->precOPBox->Name = L"precOPBox";
			this->precOPBox->Size = System::Drawing::Size(43, 35);
			this->precOPBox->TabIndex = 31;
			this->precOPBox->Text = L"0";
			this->precOPBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask4);
			this->precOPBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->precOPBox->Leave += gcnew System::EventHandler(this, &Form1::setPlaneAndTheta);
			// 
			// label45
			// 
			this->label45->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label45->AutoSize = true;
			this->label45->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label45->Location = System::Drawing::Point(51, 175);
			this->label45->Name = L"label45";
			this->label45->Size = System::Drawing::Size(93, 29);
			this->label45->TabIndex = 30;
			this->label45->Text = L"Period:";
			// 
			// label44
			// 
			this->label44->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label44->AutoSize = true;
			this->label44->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label44->Location = System::Drawing::Point(21, 143);
			this->label44->Name = L"label44";
			this->label44->Size = System::Drawing::Size(139, 29);
			this->label44->TabIndex = 29;
			this->label44->Text = L"Frequency:";
			// 
			// label43
			// 
			this->label43->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label43->AutoSize = true;
			this->label43->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label43->Location = System::Drawing::Point(32, 72);
			this->label43->Name = L"label43";
			this->label43->Size = System::Drawing::Size(122, 29);
			this->label43->TabIndex = 28;
			this->label43->Text = L"Outplane:";
			// 
			// precIPBox
			// 
			this->precIPBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->precIPBox->Location = System::Drawing::Point(115, 35);
			this->precIPBox->MaxLength = 7;
			this->precIPBox->Name = L"precIPBox";
			this->precIPBox->Size = System::Drawing::Size(43, 35);
			this->precIPBox->TabIndex = 27;
			this->precIPBox->Text = L"0";
			this->precIPBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->precIPBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->precIPBox->Leave += gcnew System::EventHandler(this, &Form1::setPlaneAndTheta);
			// 
			// label42
			// 
			this->label42->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label42->AutoSize = true;
			this->label42->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label42->Location = System::Drawing::Point(46, 38);
			this->label42->Name = L"label42";
			this->label42->Size = System::Drawing::Size(102, 29);
			this->label42->TabIndex = 26;
			this->label42->Text = L"Inplane:";
			// 
			// label36
			// 
			this->label36->AutoSize = true;
			this->label36->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label36->Location = System::Drawing::Point(575, 238);
			this->label36->Name = L"label36";
			this->label36->Size = System::Drawing::Size(32, 25);
			this->label36->TabIndex = 23;
			this->label36->Text = L"-Y";
			// 
			// label37
			// 
			this->label37->AutoSize = true;
			this->label37->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label37->Location = System::Drawing::Point(575, 17);
			this->label37->Name = L"label37";
			this->label37->Size = System::Drawing::Size(37, 25);
			this->label37->TabIndex = 22;
			this->label37->Text = L"+Y";
			// 
			// label38
			// 
			this->label38->AutoSize = true;
			this->label38->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label38->Location = System::Drawing::Point(460, 127);
			this->label38->Name = L"label38";
			this->label38->Size = System::Drawing::Size(33, 25);
			this->label38->TabIndex = 21;
			this->label38->Text = L"-X";
			// 
			// label39
			// 
			this->label39->AutoSize = true;
			this->label39->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label39->Location = System::Drawing::Point(686, 127);
			this->label39->Name = L"label39";
			this->label39->Size = System::Drawing::Size(38, 25);
			this->label39->TabIndex = 20;
			this->label39->Text = L"+X";
			// 
			// shapeContainer4
			// 
			this->shapeContainer4->Location = System::Drawing::Point(0, 0);
			this->shapeContainer4->Margin = System::Windows::Forms::Padding(0);
			this->shapeContainer4->Name = L"shapeContainer4";
			this->shapeContainer4->Shapes->AddRange(gcnew cli::array< Microsoft::VisualBasic::PowerPacks::Shape^  >(16) {
				this->PrecVectThetaB,
					this->PrecVectThetaA, this->PrecVectThetaMain, this->lineShape23, this->rectangleShape3, this->lineShape27, this->lineShape26,
					this->ovalShape4, this->ovalShape2, this->PrecVectMain, this->lineShape25, this->lineShape24, this->lineShape22, this->PrecVectB,
					this->PrecVectA, this->ovalShape1
			});
			this->shapeContainer4->Size = System::Drawing::Size(713, 257);
			this->shapeContainer4->TabIndex = 0;
			this->shapeContainer4->TabStop = false;
			// 
			// PrecVectThetaB
			// 
			this->PrecVectThetaB->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->PrecVectThetaB->BorderWidth = 3;
			this->PrecVectThetaB->Enabled = false;
			this->PrecVectThetaB->Name = L"PrecVectThetaB";
			this->PrecVectThetaB->Visible = false;
			this->PrecVectThetaB->X1 = 387;
			this->PrecVectThetaB->X2 = 437;
			this->PrecVectThetaB->Y1 = 248;
			this->PrecVectThetaB->Y2 = 248;
			// 
			// PrecVectThetaA
			// 
			this->PrecVectThetaA->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->PrecVectThetaA->BorderWidth = 3;
			this->PrecVectThetaA->Enabled = false;
			this->PrecVectThetaA->Name = L"PrecVectThetaA";
			this->PrecVectThetaA->Visible = false;
			this->PrecVectThetaA->X1 = 387;
			this->PrecVectThetaA->X2 = 437;
			this->PrecVectThetaA->Y1 = 241;
			this->PrecVectThetaA->Y2 = 241;
			// 
			// PrecVectThetaMain
			// 
			this->PrecVectThetaMain->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->PrecVectThetaMain->BorderWidth = 3;
			this->PrecVectThetaMain->Enabled = false;
			this->PrecVectThetaMain->Name = L"PrecVectThetaMain";
			this->PrecVectThetaMain->Visible = false;
			this->PrecVectThetaMain->X1 = 387;
			this->PrecVectThetaMain->X2 = 437;
			this->PrecVectThetaMain->Y1 = 234;
			this->PrecVectThetaMain->Y2 = 234;
			// 
			// lineShape23
			// 
			this->lineShape23->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dot;
			this->lineShape23->BorderWidth = 5;
			this->lineShape23->Enabled = false;
			this->lineShape23->Name = L"lineShape23";
			this->lineShape23->X1 = 323;
			this->lineShape23->X2 = 323;
			this->lineShape23->Y1 = 3;
			this->lineShape23->Y2 = 260;
			// 
			// rectangleShape3
			// 
			this->rectangleShape3->BackColor = System::Drawing::Color::White;
			this->rectangleShape3->BackStyle = Microsoft::VisualBasic::PowerPacks::BackStyle::Opaque;
			this->rectangleShape3->BorderColor = System::Drawing::Color::Transparent;
			this->rectangleShape3->Enabled = false;
			this->rectangleShape3->Location = System::Drawing::Point(238, 33);
			this->rectangleShape3->Name = L"rectangleShape3";
			this->rectangleShape3->Size = System::Drawing::Size(100, 205);
			// 
			// lineShape27
			// 
			this->lineShape27->BorderWidth = 2;
			this->lineShape27->Enabled = false;
			this->lineShape27->Name = L"lineShape27";
			this->lineShape27->X1 = 339;
			this->lineShape27->X2 = 339;
			this->lineShape27->Y1 = 35;
			this->lineShape27->Y2 = 235;
			// 
			// lineShape26
			// 
			this->lineShape26->Enabled = false;
			this->lineShape26->Name = L"lineShape26";
			this->lineShape26->X1 = 340;
			this->lineShape26->X2 = 440;
			this->lineShape26->Y1 = 135;
			this->lineShape26->Y2 = 135;
			// 
			// ovalShape4
			// 
			this->ovalShape4->BorderWidth = 2;
			this->ovalShape4->Enabled = false;
			this->ovalShape4->Location = System::Drawing::Point(240, 35);
			this->ovalShape4->Name = L"ovalShape4";
			this->ovalShape4->Size = System::Drawing::Size(200, 200);
			// 
			// ovalShape2
			// 
			this->ovalShape2->Location = System::Drawing::Point(181, 106);
			this->ovalShape2->Name = L"ovalShape2";
			this->ovalShape2->Size = System::Drawing::Size(6, 6);
			// 
			// PrecVectMain
			// 
			this->PrecVectMain->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->PrecVectMain->BorderWidth = 3;
			this->PrecVectMain->Enabled = false;
			this->PrecVectMain->Name = L"PrecVectMain";
			this->PrecVectMain->Visible = false;
			this->PrecVectMain->X1 = 648;
			this->PrecVectMain->X2 = 698;
			this->PrecVectMain->Y1 = 233;
			this->PrecVectMain->Y2 = 233;
			// 
			// lineShape25
			// 
			this->lineShape25->Enabled = false;
			this->lineShape25->Name = L"lineShape25";
			this->lineShape25->X1 = 585;
			this->lineShape25->X2 = 585;
			this->lineShape25->Y1 = 235;
			this->lineShape25->Y2 = 35;
			// 
			// lineShape24
			// 
			this->lineShape24->Enabled = false;
			this->lineShape24->Name = L"lineShape24";
			this->lineShape24->X1 = 485;
			this->lineShape24->X2 = 685;
			this->lineShape24->Y1 = 135;
			this->lineShape24->Y2 = 135;
			// 
			// lineShape22
			// 
			this->lineShape22->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dot;
			this->lineShape22->BorderWidth = 5;
			this->lineShape22->Enabled = false;
			this->lineShape22->Name = L"lineShape22";
			this->lineShape22->X1 = 453;
			this->lineShape22->X2 = 453;
			this->lineShape22->Y1 = 3;
			this->lineShape22->Y2 = 260;
			// 
			// PrecVectB
			// 
			this->PrecVectB->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->PrecVectB->BorderWidth = 3;
			this->PrecVectB->Enabled = false;
			this->PrecVectB->Name = L"PrecVectB";
			this->PrecVectB->Visible = false;
			this->PrecVectB->X1 = 648;
			this->PrecVectB->X2 = 698;
			this->PrecVectB->Y1 = 247;
			this->PrecVectB->Y2 = 247;
			// 
			// PrecVectA
			// 
			this->PrecVectA->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->PrecVectA->BorderWidth = 3;
			this->PrecVectA->Enabled = false;
			this->PrecVectA->Name = L"PrecVectA";
			this->PrecVectA->Visible = false;
			this->PrecVectA->X1 = 648;
			this->PrecVectA->X2 = 698;
			this->PrecVectA->Y1 = 240;
			this->PrecVectA->Y2 = 240;
			// 
			// ovalShape1
			// 
			this->ovalShape1->BorderWidth = 2;
			this->ovalShape1->Enabled = false;
			this->ovalShape1->Location = System::Drawing::Point(485, 35);
			this->ovalShape1->Name = L"ovalShape1";
			this->ovalShape1->Size = System::Drawing::Size(200, 200);
			// 
			// rollingTab
			// 
			this->rollingTab->Controls->Add(this->rollDownButton);
			this->rollingTab->Controls->Add(this->rollUpButton);
			this->rollingTab->Controls->Add(this->rollRightButton);
			this->rollingTab->Controls->Add(this->rollLeftButton);
			this->rollingTab->Controls->Add(this->rollButton);
			this->rollingTab->Controls->Add(this->rollAngleBox);
			this->rollingTab->Controls->Add(this->rollPeriodBox);
			this->rollingTab->Controls->Add(this->rollFreqBox);
			this->rollingTab->Controls->Add(this->rollOPBox);
			this->rollingTab->Controls->Add(this->rollIPBox);
			this->rollingTab->Controls->Add(this->label54);
			this->rollingTab->Controls->Add(this->label55);
			this->rollingTab->Controls->Add(this->label56);
			this->rollingTab->Controls->Add(this->label57);
			this->rollingTab->Controls->Add(this->label58);
			this->rollingTab->Controls->Add(this->label59);
			this->rollingTab->Controls->Add(this->label60);
			this->rollingTab->Controls->Add(this->label61);
			this->rollingTab->Controls->Add(this->label62);
			this->rollingTab->Controls->Add(this->label53);
			this->rollingTab->Controls->Add(this->label40);
			this->rollingTab->Controls->Add(this->label41);
			this->rollingTab->Controls->Add(this->label51);
			this->rollingTab->Controls->Add(this->label52);
			this->rollingTab->Controls->Add(this->shapeContainer5);
			this->rollingTab->Location = System::Drawing::Point(4, 29);
			this->rollingTab->Name = L"rollingTab";
			this->rollingTab->Size = System::Drawing::Size(713, 257);
			this->rollingTab->TabIndex = 4;
			this->rollingTab->Text = L"Rolling";
			this->rollingTab->UseVisualStyleBackColor = true;
			// 
			// rollDownButton
			// 
			this->rollDownButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rollDownButton->Location = System::Drawing::Point(232, 129);
			this->rollDownButton->Name = L"rollDownButton";
			this->rollDownButton->Size = System::Drawing::Size(50, 26);
			this->rollDownButton->TabIndex = 90;
			this->rollDownButton->Text = L"Down";
			this->rollDownButton->UseVisualStyleBackColor = true;
			this->rollDownButton->Click += gcnew System::EventHandler(this, &Form1::rollDirectionClick);
			// 
			// rollUpButton
			// 
			this->rollUpButton->BackColor = System::Drawing::Color::Transparent;
			this->rollUpButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rollUpButton->Location = System::Drawing::Point(232, 77);
			this->rollUpButton->Name = L"rollUpButton";
			this->rollUpButton->Size = System::Drawing::Size(50, 26);
			this->rollUpButton->TabIndex = 89;
			this->rollUpButton->Text = L"Up";
			this->rollUpButton->UseVisualStyleBackColor = false;
			this->rollUpButton->Click += gcnew System::EventHandler(this, &Form1::rollDirectionClick);
			// 
			// rollRightButton
			// 
			this->rollRightButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rollRightButton->Location = System::Drawing::Point(257, 103);
			this->rollRightButton->Name = L"rollRightButton";
			this->rollRightButton->Size = System::Drawing::Size(50, 26);
			this->rollRightButton->TabIndex = 88;
			this->rollRightButton->Text = L"Right";
			this->rollRightButton->UseVisualStyleBackColor = true;
			this->rollRightButton->Click += gcnew System::EventHandler(this, &Form1::rollDirectionClick);
			// 
			// rollLeftButton
			// 
			this->rollLeftButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rollLeftButton->Location = System::Drawing::Point(208, 103);
			this->rollLeftButton->Name = L"rollLeftButton";
			this->rollLeftButton->Size = System::Drawing::Size(50, 26);
			this->rollLeftButton->TabIndex = 87;
			this->rollLeftButton->Text = L"Left";
			this->rollLeftButton->UseVisualStyleBackColor = true;
			this->rollLeftButton->Click += gcnew System::EventHandler(this, &Form1::rollDirectionClick);
			// 
			// rollButton
			// 
			this->rollButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rollButton->Location = System::Drawing::Point(94, 217);
			this->rollButton->Name = L"rollButton";
			this->rollButton->Size = System::Drawing::Size(160, 35);
			this->rollButton->TabIndex = 61;
			this->rollButton->Text = L"Send";
			this->rollButton->UseVisualStyleBackColor = true;
			this->rollButton->Click += gcnew System::EventHandler(this, &Form1::rollButton_Click);
			// 
			// rollAngleBox
			// 
			this->rollAngleBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rollAngleBox->Location = System::Drawing::Point(115, 101);
			this->rollAngleBox->MaxLength = 7;
			this->rollAngleBox->Name = L"rollAngleBox";
			this->rollAngleBox->Size = System::Drawing::Size(59, 35);
			this->rollAngleBox->TabIndex = 60;
			this->rollAngleBox->Text = L"0";
			this->rollAngleBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask1);
			this->rollAngleBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->rollAngleBox->Leave += gcnew System::EventHandler(this, &Form1::rollInfoChange);
			// 
			// rollPeriodBox
			// 
			this->rollPeriodBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rollPeriodBox->Location = System::Drawing::Point(114, 172);
			this->rollPeriodBox->MaxLength = 12;
			this->rollPeriodBox->Name = L"rollPeriodBox";
			this->rollPeriodBox->Size = System::Drawing::Size(60, 35);
			this->rollPeriodBox->TabIndex = 59;
			this->rollPeriodBox->Text = L"1";
			this->rollPeriodBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->rollPeriodBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->rollPeriodBox->Leave += gcnew System::EventHandler(this, &Form1::setPeriodAndFrequency);
			// 
			// rollFreqBox
			// 
			this->rollFreqBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rollFreqBox->Location = System::Drawing::Point(114, 140);
			this->rollFreqBox->MaxLength = 12;
			this->rollFreqBox->Name = L"rollFreqBox";
			this->rollFreqBox->Size = System::Drawing::Size(60, 35);
			this->rollFreqBox->TabIndex = 58;
			this->rollFreqBox->Text = L"1";
			this->rollFreqBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->rollFreqBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->rollFreqBox->Leave += gcnew System::EventHandler(this, &Form1::setPeriodAndFrequency);
			// 
			// rollOPBox
			// 
			this->rollOPBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rollOPBox->Location = System::Drawing::Point(115, 69);
			this->rollOPBox->MaxLength = 7;
			this->rollOPBox->Name = L"rollOPBox";
			this->rollOPBox->Size = System::Drawing::Size(43, 35);
			this->rollOPBox->TabIndex = 57;
			this->rollOPBox->Text = L"0";
			this->rollOPBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask4);
			this->rollOPBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->rollOPBox->Leave += gcnew System::EventHandler(this, &Form1::rollInfoChange);
			// 
			// rollIPBox
			// 
			this->rollIPBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rollIPBox->Location = System::Drawing::Point(115, 35);
			this->rollIPBox->MaxLength = 7;
			this->rollIPBox->Name = L"rollIPBox";
			this->rollIPBox->Size = System::Drawing::Size(43, 35);
			this->rollIPBox->TabIndex = 56;
			this->rollIPBox->Text = L"0";
			this->rollIPBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->rollIPBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->rollIPBox->Leave += gcnew System::EventHandler(this, &Form1::rollInfoChange);
			// 
			// label54
			// 
			this->label54->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label54->AutoSize = true;
			this->label54->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label54->Location = System::Drawing::Point(23, 104);
			this->label54->Name = L"label54";
			this->label54->Size = System::Drawing::Size(136, 29);
			this->label54->TabIndex = 53;
			this->label54->Text = L"Roll Angle:";
			// 
			// label55
			// 
			this->label55->AutoSize = true;
			this->label55->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label55->Location = System::Drawing::Point(177, 175);
			this->label55->Name = L"label55";
			this->label55->Size = System::Drawing::Size(25, 29);
			this->label55->TabIndex = 52;
			this->label55->Text = L"s";
			// 
			// label56
			// 
			this->label56->AutoSize = true;
			this->label56->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label56->Location = System::Drawing::Point(177, 143);
			this->label56->Name = L"label56";
			this->label56->Size = System::Drawing::Size(41, 29);
			this->label56->TabIndex = 51;
			this->label56->Text = L"Hz";
			// 
			// label57
			// 
			this->label57->AutoSize = true;
			this->label57->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label57->Location = System::Drawing::Point(158, 72);
			this->label57->Name = L"label57";
			this->label57->Size = System::Drawing::Size(46, 29);
			this->label57->TabIndex = 50;
			this->label57->Text = L"Oe";
			// 
			// label58
			// 
			this->label58->AutoSize = true;
			this->label58->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label58->Location = System::Drawing::Point(158, 38);
			this->label58->Name = L"label58";
			this->label58->Size = System::Drawing::Size(46, 29);
			this->label58->TabIndex = 49;
			this->label58->Text = L"Oe";
			// 
			// label59
			// 
			this->label59->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label59->AutoSize = true;
			this->label59->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label59->Location = System::Drawing::Point(51, 175);
			this->label59->Name = L"label59";
			this->label59->Size = System::Drawing::Size(93, 29);
			this->label59->TabIndex = 48;
			this->label59->Text = L"Period:";
			// 
			// label60
			// 
			this->label60->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label60->AutoSize = true;
			this->label60->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label60->Location = System::Drawing::Point(21, 143);
			this->label60->Name = L"label60";
			this->label60->Size = System::Drawing::Size(139, 29);
			this->label60->TabIndex = 47;
			this->label60->Text = L"Frequency:";
			// 
			// label61
			// 
			this->label61->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label61->AutoSize = true;
			this->label61->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label61->Location = System::Drawing::Point(32, 72);
			this->label61->Name = L"label61";
			this->label61->Size = System::Drawing::Size(122, 29);
			this->label61->TabIndex = 46;
			this->label61->Text = L"Outplane:";
			// 
			// label62
			// 
			this->label62->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label62->AutoSize = true;
			this->label62->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label62->Location = System::Drawing::Point(46, 38);
			this->label62->Name = L"label62";
			this->label62->Size = System::Drawing::Size(102, 29);
			this->label62->TabIndex = 45;
			this->label62->Text = L"Inplane:";
			// 
			// label53
			// 
			this->label53->AutoSize = true;
			this->label53->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label53->Location = System::Drawing::Point(381, 17);
			this->label53->Name = L"label53";
			this->label53->Size = System::Drawing::Size(23, 25);
			this->label53->TabIndex = 44;
			this->label53->Text = L"θ";
			// 
			// label40
			// 
			this->label40->AutoSize = true;
			this->label40->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label40->Location = System::Drawing::Point(575, 238);
			this->label40->Name = L"label40";
			this->label40->Size = System::Drawing::Size(32, 25);
			this->label40->TabIndex = 27;
			this->label40->Text = L"-Y";
			// 
			// label41
			// 
			this->label41->AutoSize = true;
			this->label41->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label41->Location = System::Drawing::Point(575, 17);
			this->label41->Name = L"label41";
			this->label41->Size = System::Drawing::Size(37, 25);
			this->label41->TabIndex = 26;
			this->label41->Text = L"+Y";
			// 
			// label51
			// 
			this->label51->AutoSize = true;
			this->label51->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label51->Location = System::Drawing::Point(460, 127);
			this->label51->Name = L"label51";
			this->label51->Size = System::Drawing::Size(33, 25);
			this->label51->TabIndex = 25;
			this->label51->Text = L"-X";
			// 
			// label52
			// 
			this->label52->AutoSize = true;
			this->label52->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label52->Location = System::Drawing::Point(686, 127);
			this->label52->Name = L"label52";
			this->label52->Size = System::Drawing::Size(38, 25);
			this->label52->TabIndex = 24;
			this->label52->Text = L"+X";
			// 
			// shapeContainer5
			// 
			this->shapeContainer5->Location = System::Drawing::Point(0, 0);
			this->shapeContainer5->Margin = System::Windows::Forms::Padding(0);
			this->shapeContainer5->Name = L"shapeContainer5";
			this->shapeContainer5->Shapes->AddRange(gcnew cli::array< Microsoft::VisualBasic::PowerPacks::Shape^  >(16) {
				this->lineShape31,
					this->rectangleShape5, this->ovalShape7, this->rollVectA, this->rollVectB, this->lineShape37, this->lineShape36, this->lineShape35,
					this->rollVectMain, this->ovalShape6, this->ovalShape5, this->lineShape33, this->lineShape32, this->rollVectThetaMain, this->rollVectThetaA,
					this->rollVectThetaB
			});
			this->shapeContainer5->Size = System::Drawing::Size(713, 257);
			this->shapeContainer5->TabIndex = 0;
			this->shapeContainer5->TabStop = false;
			// 
			// lineShape31
			// 
			this->lineShape31->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dot;
			this->lineShape31->BorderWidth = 5;
			this->lineShape31->Enabled = false;
			this->lineShape31->Name = L"lineShape31";
			this->lineShape31->X1 = 323;
			this->lineShape31->X2 = 323;
			this->lineShape31->Y1 = 3;
			this->lineShape31->Y2 = 260;
			// 
			// rectangleShape5
			// 
			this->rectangleShape5->BackColor = System::Drawing::Color::White;
			this->rectangleShape5->BackStyle = Microsoft::VisualBasic::PowerPacks::BackStyle::Opaque;
			this->rectangleShape5->BorderColor = System::Drawing::Color::Transparent;
			this->rectangleShape5->Enabled = false;
			this->rectangleShape5->Location = System::Drawing::Point(238, 34);
			this->rectangleShape5->Name = L"rectangleShape5";
			this->rectangleShape5->Size = System::Drawing::Size(100, 205);
			// 
			// ovalShape7
			// 
			this->ovalShape7->BorderWidth = 2;
			this->ovalShape7->Enabled = false;
			this->ovalShape7->Location = System::Drawing::Point(485, 35);
			this->ovalShape7->Name = L"ovalShape7";
			this->ovalShape7->Size = System::Drawing::Size(200, 200);
			// 
			// rollVectA
			// 
			this->rollVectA->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->rollVectA->BorderWidth = 3;
			this->rollVectA->Enabled = false;
			this->rollVectA->Name = L"rollVectA";
			this->rollVectA->Visible = false;
			this->rollVectA->X1 = 648;
			this->rollVectA->X2 = 698;
			this->rollVectA->Y1 = 240;
			this->rollVectA->Y2 = 240;
			// 
			// rollVectB
			// 
			this->rollVectB->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->rollVectB->BorderWidth = 3;
			this->rollVectB->Enabled = false;
			this->rollVectB->Name = L"rollVectB";
			this->rollVectB->Visible = false;
			this->rollVectB->X1 = 648;
			this->rollVectB->X2 = 698;
			this->rollVectB->Y1 = 247;
			this->rollVectB->Y2 = 247;
			// 
			// lineShape37
			// 
			this->lineShape37->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dot;
			this->lineShape37->BorderWidth = 5;
			this->lineShape37->Enabled = false;
			this->lineShape37->Name = L"lineShape37";
			this->lineShape37->X1 = 453;
			this->lineShape37->X2 = 453;
			this->lineShape37->Y1 = 3;
			this->lineShape37->Y2 = 260;
			// 
			// lineShape36
			// 
			this->lineShape36->Enabled = false;
			this->lineShape36->Name = L"lineShape36";
			this->lineShape36->X1 = 485;
			this->lineShape36->X2 = 685;
			this->lineShape36->Y1 = 135;
			this->lineShape36->Y2 = 135;
			// 
			// lineShape35
			// 
			this->lineShape35->Enabled = false;
			this->lineShape35->Name = L"lineShape35";
			this->lineShape35->X1 = 585;
			this->lineShape35->X2 = 585;
			this->lineShape35->Y1 = 235;
			this->lineShape35->Y2 = 35;
			// 
			// rollVectMain
			// 
			this->rollVectMain->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->rollVectMain->BorderWidth = 3;
			this->rollVectMain->Enabled = false;
			this->rollVectMain->Name = L"rollVectMain";
			this->rollVectMain->Visible = false;
			this->rollVectMain->X1 = 648;
			this->rollVectMain->X2 = 698;
			this->rollVectMain->Y1 = 233;
			this->rollVectMain->Y2 = 233;
			// 
			// ovalShape6
			// 
			this->ovalShape6->Location = System::Drawing::Point(181, 105);
			this->ovalShape6->Name = L"ovalShape6";
			this->ovalShape6->Size = System::Drawing::Size(6, 6);
			// 
			// ovalShape5
			// 
			this->ovalShape5->BorderWidth = 2;
			this->ovalShape5->Enabled = false;
			this->ovalShape5->Location = System::Drawing::Point(240, 35);
			this->ovalShape5->Name = L"ovalShape5";
			this->ovalShape5->Size = System::Drawing::Size(200, 200);
			// 
			// lineShape33
			// 
			this->lineShape33->Enabled = false;
			this->lineShape33->Name = L"lineShape33";
			this->lineShape33->X1 = 340;
			this->lineShape33->X2 = 440;
			this->lineShape33->Y1 = 135;
			this->lineShape33->Y2 = 135;
			// 
			// lineShape32
			// 
			this->lineShape32->BorderWidth = 2;
			this->lineShape32->Enabled = false;
			this->lineShape32->Name = L"lineShape32";
			this->lineShape32->X1 = 339;
			this->lineShape32->X2 = 339;
			this->lineShape32->Y1 = 35;
			this->lineShape32->Y2 = 235;
			// 
			// rollVectThetaMain
			// 
			this->rollVectThetaMain->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->rollVectThetaMain->BorderWidth = 3;
			this->rollVectThetaMain->Enabled = false;
			this->rollVectThetaMain->Name = L"rollVectThetaMain";
			this->rollVectThetaMain->Visible = false;
			this->rollVectThetaMain->X1 = 387;
			this->rollVectThetaMain->X2 = 437;
			this->rollVectThetaMain->Y1 = 234;
			this->rollVectThetaMain->Y2 = 234;
			// 
			// rollVectThetaA
			// 
			this->rollVectThetaA->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->rollVectThetaA->BorderWidth = 3;
			this->rollVectThetaA->Enabled = false;
			this->rollVectThetaA->Name = L"rollVectThetaA";
			this->rollVectThetaA->Visible = false;
			this->rollVectThetaA->X1 = 387;
			this->rollVectThetaA->X2 = 437;
			this->rollVectThetaA->Y1 = 241;
			this->rollVectThetaA->Y2 = 241;
			// 
			// rollVectThetaB
			// 
			this->rollVectThetaB->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->rollVectThetaB->BorderWidth = 3;
			this->rollVectThetaB->Enabled = false;
			this->rollVectThetaB->Name = L"rollVectThetaB";
			this->rollVectThetaB->Visible = false;
			this->rollVectThetaB->X1 = 387;
			this->rollVectThetaB->X2 = 437;
			this->rollVectThetaB->Y1 = 248;
			this->rollVectThetaB->Y2 = 248;
			// 
			// diskTab
			// 
			this->diskTab->Controls->Add(this->DiskHopBox);
			this->diskTab->Controls->Add(this->downButton);
			this->diskTab->Controls->Add(this->upButton);
			this->diskTab->Controls->Add(this->rightButton);
			this->diskTab->Controls->Add(this->label72);
			this->diskTab->Controls->Add(this->hopBox);
			this->diskTab->Controls->Add(this->label70);
			this->diskTab->Controls->Add(this->leftButton);
			this->diskTab->Controls->Add(this->diskThetaBox);
			this->diskTab->Controls->Add(this->diskPeriodBox);
			this->diskTab->Controls->Add(this->diskFreqBox);
			this->diskTab->Controls->Add(this->diskOPBox);
			this->diskTab->Controls->Add(this->diskIPBox);
			this->diskTab->Controls->Add(this->label19);
			this->diskTab->Controls->Add(this->label23);
			this->diskTab->Controls->Add(this->label63);
			this->diskTab->Controls->Add(this->label64);
			this->diskTab->Controls->Add(this->label65);
			this->diskTab->Controls->Add(this->label66);
			this->diskTab->Controls->Add(this->label67);
			this->diskTab->Controls->Add(this->label68);
			this->diskTab->Controls->Add(this->label69);
			this->diskTab->Controls->Add(this->shapeContainer6);
			this->diskTab->Location = System::Drawing::Point(4, 29);
			this->diskTab->Name = L"diskTab";
			this->diskTab->Size = System::Drawing::Size(713, 257);
			this->diskTab->TabIndex = 3;
			this->diskTab->Text = L"Disk Hopper";
			this->diskTab->UseVisualStyleBackColor = true;
			// 
			// DiskHopBox
			// 
			this->DiskHopBox->Controls->Add(this->diskCCW);
			this->DiskHopBox->Controls->Add(this->diskCW);
			this->DiskHopBox->Location = System::Drawing::Point(215, 32);
			this->DiskHopBox->Name = L"DiskHopBox";
			this->DiskHopBox->Size = System::Drawing::Size(96, 62);
			this->DiskHopBox->TabIndex = 87;
			this->DiskHopBox->TabStop = false;
			// 
			// diskCCW
			// 
			this->diskCCW->AutoSize = true;
			this->diskCCW->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->diskCCW->Location = System::Drawing::Point(14, 33);
			this->diskCCW->Name = L"diskCCW";
			this->diskCCW->Size = System::Drawing::Size(98, 33);
			this->diskCCW->TabIndex = 72;
			this->diskCCW->Text = L"CCW";
			this->diskCCW->UseVisualStyleBackColor = true;
			// 
			// diskCW
			// 
			this->diskCW->AutoSize = true;
			this->diskCW->Checked = true;
			this->diskCW->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->diskCW->Location = System::Drawing::Point(14, 10);
			this->diskCW->Name = L"diskCW";
			this->diskCW->Size = System::Drawing::Size(80, 33);
			this->diskCW->TabIndex = 71;
			this->diskCW->TabStop = true;
			this->diskCW->Text = L"CW";
			this->diskCW->UseVisualStyleBackColor = true;
			// 
			// downButton
			// 
			this->downButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->downButton->Location = System::Drawing::Point(142, 233);
			this->downButton->Name = L"downButton";
			this->downButton->Size = System::Drawing::Size(50, 26);
			this->downButton->TabIndex = 86;
			this->downButton->Text = L"Down";
			this->downButton->UseVisualStyleBackColor = true;
			this->downButton->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::directionButtonClick);
			// 
			// upButton
			// 
			this->upButton->BackColor = System::Drawing::Color::Transparent;
			this->upButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->upButton->Location = System::Drawing::Point(142, 205);
			this->upButton->Name = L"upButton";
			this->upButton->Size = System::Drawing::Size(50, 26);
			this->upButton->TabIndex = 85;
			this->upButton->Text = L"Up";
			this->upButton->UseVisualStyleBackColor = false;
			this->upButton->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::directionButtonClick);
			// 
			// rightButton
			// 
			this->rightButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rightButton->Location = System::Drawing::Point(193, 220);
			this->rightButton->Name = L"rightButton";
			this->rightButton->Size = System::Drawing::Size(50, 26);
			this->rightButton->TabIndex = 84;
			this->rightButton->Text = L"Right";
			this->rightButton->UseVisualStyleBackColor = true;
			this->rightButton->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::directionButtonClick);
			// 
			// label72
			// 
			this->label72->AutoSize = true;
			this->label72->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label72->Location = System::Drawing::Point(315, 175);
			this->label72->Name = L"label72";
			this->label72->Size = System::Drawing::Size(25, 29);
			this->label72->TabIndex = 83;
			this->label72->Text = L"s";
			// 
			// hopBox
			// 
			this->hopBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->hopBox->Location = System::Drawing::Point(253, 172);
			this->hopBox->MaxLength = 12;
			this->hopBox->Name = L"hopBox";
			this->hopBox->Size = System::Drawing::Size(60, 35);
			this->hopBox->TabIndex = 81;
			this->hopBox->Text = L"0";
			this->hopBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->hopBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->hopBox->Leave += gcnew System::EventHandler(this, &Form1::hopBox_Leave);
			// 
			// label70
			// 
			this->label70->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label70->AutoSize = true;
			this->label70->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label70->Location = System::Drawing::Point(207, 175);
			this->label70->Name = L"label70";
			this->label70->Size = System::Drawing::Size(66, 29);
			this->label70->TabIndex = 80;
			this->label70->Text = L"Hop:";
			// 
			// leftButton
			// 
			this->leftButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->leftButton->Location = System::Drawing::Point(91, 220);
			this->leftButton->Name = L"leftButton";
			this->leftButton->Size = System::Drawing::Size(50, 26);
			this->leftButton->TabIndex = 78;
			this->leftButton->Text = L"Left";
			this->leftButton->UseVisualStyleBackColor = true;
			this->leftButton->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::directionButtonClick);
			// 
			// diskThetaBox
			// 
			this->diskThetaBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->diskThetaBox->Location = System::Drawing::Point(115, 101);
			this->diskThetaBox->MaxLength = 7;
			this->diskThetaBox->Name = L"diskThetaBox";
			this->diskThetaBox->Size = System::Drawing::Size(59, 35);
			this->diskThetaBox->TabIndex = 77;
			this->diskThetaBox->Text = L"0";
			this->diskThetaBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask1);
			this->diskThetaBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->diskThetaBox->Leave += gcnew System::EventHandler(this, &Form1::setPlaneAndTheta);
			// 
			// diskPeriodBox
			// 
			this->diskPeriodBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->diskPeriodBox->Location = System::Drawing::Point(114, 172);
			this->diskPeriodBox->MaxLength = 12;
			this->diskPeriodBox->Name = L"diskPeriodBox";
			this->diskPeriodBox->Size = System::Drawing::Size(60, 35);
			this->diskPeriodBox->TabIndex = 76;
			this->diskPeriodBox->Text = L"1";
			this->diskPeriodBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->diskPeriodBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->diskPeriodBox->Leave += gcnew System::EventHandler(this, &Form1::setPeriodAndFrequency);
			// 
			// diskFreqBox
			// 
			this->diskFreqBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->diskFreqBox->Location = System::Drawing::Point(114, 140);
			this->diskFreqBox->MaxLength = 12;
			this->diskFreqBox->Name = L"diskFreqBox";
			this->diskFreqBox->Size = System::Drawing::Size(60, 35);
			this->diskFreqBox->TabIndex = 75;
			this->diskFreqBox->Text = L"1";
			this->diskFreqBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->diskFreqBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->diskFreqBox->Leave += gcnew System::EventHandler(this, &Form1::setPeriodAndFrequency);
			// 
			// diskOPBox
			// 
			this->diskOPBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->diskOPBox->Location = System::Drawing::Point(115, 69);
			this->diskOPBox->MaxLength = 7;
			this->diskOPBox->Name = L"diskOPBox";
			this->diskOPBox->Size = System::Drawing::Size(43, 35);
			this->diskOPBox->TabIndex = 74;
			this->diskOPBox->Text = L"0";
			this->diskOPBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->diskOPBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->diskOPBox->Leave += gcnew System::EventHandler(this, &Form1::setPlaneAndTheta);
			// 
			// diskIPBox
			// 
			this->diskIPBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->diskIPBox->Location = System::Drawing::Point(115, 35);
			this->diskIPBox->MaxLength = 7;
			this->diskIPBox->Name = L"diskIPBox";
			this->diskIPBox->Size = System::Drawing::Size(43, 35);
			this->diskIPBox->TabIndex = 73;
			this->diskIPBox->Text = L"0";
			this->diskIPBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->diskIPBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->diskIPBox->Leave += gcnew System::EventHandler(this, &Form1::setPlaneAndTheta);
			// 
			// label19
			// 
			this->label19->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label19->AutoSize = true;
			this->label19->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label19->Location = System::Drawing::Point(57, 104);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(84, 29);
			this->label19->TabIndex = 70;
			this->label19->Text = L"Theta:";
			// 
			// label23
			// 
			this->label23->AutoSize = true;
			this->label23->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label23->Location = System::Drawing::Point(177, 175);
			this->label23->Name = L"label23";
			this->label23->Size = System::Drawing::Size(25, 29);
			this->label23->TabIndex = 69;
			this->label23->Text = L"s";
			// 
			// label63
			// 
			this->label63->AutoSize = true;
			this->label63->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label63->Location = System::Drawing::Point(177, 143);
			this->label63->Name = L"label63";
			this->label63->Size = System::Drawing::Size(41, 29);
			this->label63->TabIndex = 68;
			this->label63->Text = L"Hz";
			// 
			// label64
			// 
			this->label64->AutoSize = true;
			this->label64->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label64->Location = System::Drawing::Point(158, 72);
			this->label64->Name = L"label64";
			this->label64->Size = System::Drawing::Size(46, 29);
			this->label64->TabIndex = 67;
			this->label64->Text = L"Oe";
			// 
			// label65
			// 
			this->label65->AutoSize = true;
			this->label65->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label65->Location = System::Drawing::Point(158, 38);
			this->label65->Name = L"label65";
			this->label65->Size = System::Drawing::Size(46, 29);
			this->label65->TabIndex = 66;
			this->label65->Text = L"Oe";
			// 
			// label66
			// 
			this->label66->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label66->AutoSize = true;
			this->label66->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label66->Location = System::Drawing::Point(51, 175);
			this->label66->Name = L"label66";
			this->label66->Size = System::Drawing::Size(93, 29);
			this->label66->TabIndex = 65;
			this->label66->Text = L"Period:";
			// 
			// label67
			// 
			this->label67->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label67->AutoSize = true;
			this->label67->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label67->Location = System::Drawing::Point(21, 143);
			this->label67->Name = L"label67";
			this->label67->Size = System::Drawing::Size(139, 29);
			this->label67->TabIndex = 64;
			this->label67->Text = L"Frequency:";
			// 
			// label68
			// 
			this->label68->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label68->AutoSize = true;
			this->label68->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label68->Location = System::Drawing::Point(32, 72);
			this->label68->Name = L"label68";
			this->label68->Size = System::Drawing::Size(122, 29);
			this->label68->TabIndex = 63;
			this->label68->Text = L"Outplane:";
			// 
			// label69
			// 
			this->label69->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label69->AutoSize = true;
			this->label69->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label69->Location = System::Drawing::Point(46, 38);
			this->label69->Name = L"label69";
			this->label69->Size = System::Drawing::Size(102, 29);
			this->label69->TabIndex = 62;
			this->label69->Text = L"Inplane:";
			// 
			// shapeContainer6
			// 
			this->shapeContainer6->Location = System::Drawing::Point(0, 0);
			this->shapeContainer6->Margin = System::Windows::Forms::Padding(0);
			this->shapeContainer6->Name = L"shapeContainer6";
			this->shapeContainer6->Shapes->AddRange(gcnew cli::array< Microsoft::VisualBasic::PowerPacks::Shape^  >(2) {
				this->lineShape28,
					this->ovalShape8
			});
			this->shapeContainer6->Size = System::Drawing::Size(713, 257);
			this->shapeContainer6->TabIndex = 79;
			this->shapeContainer6->TabStop = false;
			// 
			// lineShape28
			// 
			this->lineShape28->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dot;
			this->lineShape28->BorderWidth = 5;
			this->lineShape28->Enabled = false;
			this->lineShape28->Name = L"lineShape28";
			this->lineShape28->X1 = 336;
			this->lineShape28->X2 = 336;
			this->lineShape28->Y1 = 4;
			this->lineShape28->Y2 = 261;
			// 
			// ovalShape8
			// 
			this->ovalShape8->Location = System::Drawing::Point(181, 105);
			this->ovalShape8->Name = L"ovalShape8";
			this->ovalShape8->Size = System::Drawing::Size(6, 6);
			// 
			// quickPrecTab
			// 
			this->quickPrecTab->Controls->Add(this->QP_ActiveBox);
			this->quickPrecTab->Controls->Add(this->QP_ProposedBox);
			this->quickPrecTab->Controls->Add(this->label91);
			this->quickPrecTab->Controls->Add(this->label90);
			this->quickPrecTab->Controls->Add(this->QPStopButton);
			this->quickPrecTab->Controls->Add(this->QPThetaBoxLast);
			this->quickPrecTab->Controls->Add(this->label81);
			this->quickPrecTab->Controls->Add(this->label82);
			this->quickPrecTab->Controls->Add(this->label83);
			this->quickPrecTab->Controls->Add(this->label84);
			this->quickPrecTab->Controls->Add(this->label85);
			this->quickPrecTab->Controls->Add(this->QPPeriodBoxLast);
			this->quickPrecTab->Controls->Add(this->QPFreqBoxLast);
			this->quickPrecTab->Controls->Add(this->QPOPBoxLast);
			this->quickPrecTab->Controls->Add(this->label86);
			this->quickPrecTab->Controls->Add(this->label87);
			this->quickPrecTab->Controls->Add(this->label88);
			this->quickPrecTab->Controls->Add(this->QPIPBoxLast);
			this->quickPrecTab->Controls->Add(this->label89);
			this->quickPrecTab->Controls->Add(this->QPStartButton);
			this->quickPrecTab->Controls->Add(this->QPThetaBox);
			this->quickPrecTab->Controls->Add(this->label71);
			this->quickPrecTab->Controls->Add(this->label73);
			this->quickPrecTab->Controls->Add(this->label74);
			this->quickPrecTab->Controls->Add(this->label75);
			this->quickPrecTab->Controls->Add(this->label76);
			this->quickPrecTab->Controls->Add(this->QPPeriodBox);
			this->quickPrecTab->Controls->Add(this->QPFreqBox);
			this->quickPrecTab->Controls->Add(this->QPOPBox);
			this->quickPrecTab->Controls->Add(this->label77);
			this->quickPrecTab->Controls->Add(this->label78);
			this->quickPrecTab->Controls->Add(this->label79);
			this->quickPrecTab->Controls->Add(this->QPIPBox);
			this->quickPrecTab->Controls->Add(this->label80);
			this->quickPrecTab->Controls->Add(this->shapeContainer7);
			this->quickPrecTab->Location = System::Drawing::Point(4, 29);
			this->quickPrecTab->Name = L"quickPrecTab";
			this->quickPrecTab->Size = System::Drawing::Size(713, 257);
			this->quickPrecTab->TabIndex = 5;
			this->quickPrecTab->Text = L"Quick Precession";
			this->quickPrecTab->UseVisualStyleBackColor = true;
			this->quickPrecTab->Leave += gcnew System::EventHandler(this, &Form1::QPStopButton_Click);
			// 
			// QP_ActiveBox
			// 
			this->QP_ActiveBox->Controls->Add(this->QPCCWLast);
			this->QP_ActiveBox->Controls->Add(this->QPCWLast);
			this->QP_ActiveBox->Location = System::Drawing::Point(588, 32);
			this->QP_ActiveBox->Name = L"QP_ActiveBox";
			this->QP_ActiveBox->Size = System::Drawing::Size(96, 62);
			this->QP_ActiveBox->TabIndex = 81;
			this->QP_ActiveBox->TabStop = false;
			// 
			// QPCCWLast
			// 
			this->QPCCWLast->AutoSize = true;
			this->QPCCWLast->Enabled = false;
			this->QPCCWLast->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPCCWLast->Location = System::Drawing::Point(14, 33);
			this->QPCCWLast->Name = L"QPCCWLast";
			this->QPCCWLast->Size = System::Drawing::Size(98, 33);
			this->QPCCWLast->TabIndex = 76;
			this->QPCCWLast->Text = L"CCW";
			this->QPCCWLast->UseVisualStyleBackColor = true;
			// 
			// QPCWLast
			// 
			this->QPCWLast->AutoSize = true;
			this->QPCWLast->Enabled = false;
			this->QPCWLast->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPCWLast->Location = System::Drawing::Point(14, 10);
			this->QPCWLast->Name = L"QPCWLast";
			this->QPCWLast->Size = System::Drawing::Size(80, 33);
			this->QPCWLast->TabIndex = 75;
			this->QPCWLast->TabStop = true;
			this->QPCWLast->Text = L"CW";
			this->QPCWLast->UseVisualStyleBackColor = true;
			// 
			// QP_ProposedBox
			// 
			this->QP_ProposedBox->Controls->Add(this->QPCCW);
			this->QP_ProposedBox->Controls->Add(this->QPCW);
			this->QP_ProposedBox->Location = System::Drawing::Point(215, 32);
			this->QP_ProposedBox->Name = L"QP_ProposedBox";
			this->QP_ProposedBox->Size = System::Drawing::Size(96, 62);
			this->QP_ProposedBox->TabIndex = 80;
			this->QP_ProposedBox->TabStop = false;
			// 
			// QPCCW
			// 
			this->QPCCW->AutoSize = true;
			this->QPCCW->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPCCW->Location = System::Drawing::Point(14, 33);
			this->QPCCW->Name = L"QPCCW";
			this->QPCCW->Size = System::Drawing::Size(98, 33);
			this->QPCCW->TabIndex = 59;
			this->QPCCW->Text = L"CCW";
			this->QPCCW->UseVisualStyleBackColor = true;
			// 
			// QPCW
			// 
			this->QPCW->AutoSize = true;
			this->QPCW->Checked = true;
			this->QPCW->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPCW->Location = System::Drawing::Point(14, 10);
			this->QPCW->Name = L"QPCW";
			this->QPCW->Size = System::Drawing::Size(80, 33);
			this->QPCW->TabIndex = 58;
			this->QPCW->TabStop = true;
			this->QPCW->Text = L"CW";
			this->QPCW->UseVisualStyleBackColor = true;
			// 
			// label91
			// 
			this->label91->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label91->AutoSize = true;
			this->label91->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label91->Location = System::Drawing::Point(124, 9);
			this->label91->Name = L"label91";
			this->label91->Size = System::Drawing::Size(157, 29);
			this->label91->TabIndex = 79;
			this->label91->Text = L"PROPOSED";
			// 
			// label90
			// 
			this->label90->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label90->AutoSize = true;
			this->label90->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label90->Location = System::Drawing::Point(498, 9);
			this->label90->Name = L"label90";
			this->label90->Size = System::Drawing::Size(104, 29);
			this->label90->TabIndex = 78;
			this->label90->Text = L"ACTIVE";
			// 
			// QPStopButton
			// 
			this->QPStopButton->Enabled = false;
			this->QPStopButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPStopButton->Location = System::Drawing::Point(467, 217);
			this->QPStopButton->Name = L"QPStopButton";
			this->QPStopButton->Size = System::Drawing::Size(160, 35);
			this->QPStopButton->TabIndex = 77;
			this->QPStopButton->Text = L"Stop";
			this->QPStopButton->UseVisualStyleBackColor = true;
			this->QPStopButton->Click += gcnew System::EventHandler(this, &Form1::QPStopButton_Click);
			// 
			// QPThetaBoxLast
			// 
			this->QPThetaBoxLast->Enabled = false;
			this->QPThetaBoxLast->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPThetaBoxLast->Location = System::Drawing::Point(481, 101);
			this->QPThetaBoxLast->MaxLength = 7;
			this->QPThetaBoxLast->Name = L"QPThetaBoxLast";
			this->QPThetaBoxLast->Size = System::Drawing::Size(59, 35);
			this->QPThetaBoxLast->TabIndex = 74;
			this->QPThetaBoxLast->Text = L"0";
			// 
			// label81
			// 
			this->label81->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label81->AutoSize = true;
			this->label81->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label81->Location = System::Drawing::Point(423, 104);
			this->label81->Name = L"label81";
			this->label81->Size = System::Drawing::Size(84, 29);
			this->label81->TabIndex = 73;
			this->label81->Text = L"Theta:";
			// 
			// label82
			// 
			this->label82->AutoSize = true;
			this->label82->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label82->Location = System::Drawing::Point(543, 175);
			this->label82->Name = L"label82";
			this->label82->Size = System::Drawing::Size(25, 29);
			this->label82->TabIndex = 72;
			this->label82->Text = L"s";
			// 
			// label83
			// 
			this->label83->AutoSize = true;
			this->label83->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label83->Location = System::Drawing::Point(543, 143);
			this->label83->Name = L"label83";
			this->label83->Size = System::Drawing::Size(41, 29);
			this->label83->TabIndex = 71;
			this->label83->Text = L"Hz";
			// 
			// label84
			// 
			this->label84->AutoSize = true;
			this->label84->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label84->Location = System::Drawing::Point(524, 72);
			this->label84->Name = L"label84";
			this->label84->Size = System::Drawing::Size(46, 29);
			this->label84->TabIndex = 70;
			this->label84->Text = L"Oe";
			// 
			// label85
			// 
			this->label85->AutoSize = true;
			this->label85->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label85->Location = System::Drawing::Point(524, 38);
			this->label85->Name = L"label85";
			this->label85->Size = System::Drawing::Size(46, 29);
			this->label85->TabIndex = 69;
			this->label85->Text = L"Oe";
			// 
			// QPPeriodBoxLast
			// 
			this->QPPeriodBoxLast->Enabled = false;
			this->QPPeriodBoxLast->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPPeriodBoxLast->Location = System::Drawing::Point(480, 172);
			this->QPPeriodBoxLast->MaxLength = 12;
			this->QPPeriodBoxLast->Name = L"QPPeriodBoxLast";
			this->QPPeriodBoxLast->Size = System::Drawing::Size(60, 35);
			this->QPPeriodBoxLast->TabIndex = 68;
			this->QPPeriodBoxLast->Text = L"0";
			// 
			// QPFreqBoxLast
			// 
			this->QPFreqBoxLast->Enabled = false;
			this->QPFreqBoxLast->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPFreqBoxLast->Location = System::Drawing::Point(480, 140);
			this->QPFreqBoxLast->MaxLength = 12;
			this->QPFreqBoxLast->Name = L"QPFreqBoxLast";
			this->QPFreqBoxLast->Size = System::Drawing::Size(60, 35);
			this->QPFreqBoxLast->TabIndex = 67;
			this->QPFreqBoxLast->Text = L"0";
			// 
			// QPOPBoxLast
			// 
			this->QPOPBoxLast->Enabled = false;
			this->QPOPBoxLast->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPOPBoxLast->Location = System::Drawing::Point(481, 69);
			this->QPOPBoxLast->MaxLength = 7;
			this->QPOPBoxLast->Name = L"QPOPBoxLast";
			this->QPOPBoxLast->Size = System::Drawing::Size(43, 35);
			this->QPOPBoxLast->TabIndex = 66;
			this->QPOPBoxLast->Text = L"0";
			// 
			// label86
			// 
			this->label86->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label86->AutoSize = true;
			this->label86->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label86->Location = System::Drawing::Point(417, 175);
			this->label86->Name = L"label86";
			this->label86->Size = System::Drawing::Size(93, 29);
			this->label86->TabIndex = 65;
			this->label86->Text = L"Period:";
			// 
			// label87
			// 
			this->label87->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label87->AutoSize = true;
			this->label87->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label87->Location = System::Drawing::Point(387, 143);
			this->label87->Name = L"label87";
			this->label87->Size = System::Drawing::Size(139, 29);
			this->label87->TabIndex = 64;
			this->label87->Text = L"Frequency:";
			// 
			// label88
			// 
			this->label88->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label88->AutoSize = true;
			this->label88->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label88->Location = System::Drawing::Point(398, 72);
			this->label88->Name = L"label88";
			this->label88->Size = System::Drawing::Size(122, 29);
			this->label88->TabIndex = 63;
			this->label88->Text = L"Outplane:";
			// 
			// QPIPBoxLast
			// 
			this->QPIPBoxLast->Enabled = false;
			this->QPIPBoxLast->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPIPBoxLast->Location = System::Drawing::Point(481, 35);
			this->QPIPBoxLast->MaxLength = 7;
			this->QPIPBoxLast->Name = L"QPIPBoxLast";
			this->QPIPBoxLast->Size = System::Drawing::Size(43, 35);
			this->QPIPBoxLast->TabIndex = 62;
			this->QPIPBoxLast->Text = L"0";
			// 
			// label89
			// 
			this->label89->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label89->AutoSize = true;
			this->label89->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label89->Location = System::Drawing::Point(412, 38);
			this->label89->Name = L"label89";
			this->label89->Size = System::Drawing::Size(102, 29);
			this->label89->TabIndex = 61;
			this->label89->Text = L"Inplane:";
			// 
			// QPStartButton
			// 
			this->QPStartButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPStartButton->Location = System::Drawing::Point(94, 217);
			this->QPStartButton->Name = L"QPStartButton";
			this->QPStartButton->Size = System::Drawing::Size(160, 35);
			this->QPStartButton->TabIndex = 60;
			this->QPStartButton->Text = L"Send";
			this->QPStartButton->UseVisualStyleBackColor = true;
			this->QPStartButton->Click += gcnew System::EventHandler(this, &Form1::QPStartButton_Click);
			// 
			// QPThetaBox
			// 
			this->QPThetaBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPThetaBox->Location = System::Drawing::Point(115, 101);
			this->QPThetaBox->MaxLength = 7;
			this->QPThetaBox->Name = L"QPThetaBox";
			this->QPThetaBox->Size = System::Drawing::Size(59, 35);
			this->QPThetaBox->TabIndex = 56;
			this->QPThetaBox->Text = L"0";
			this->QPThetaBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask1);
			this->QPThetaBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->QPThetaBox->Leave += gcnew System::EventHandler(this, &Form1::setPlaneAndTheta);
			// 
			// label71
			// 
			this->label71->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label71->AutoSize = true;
			this->label71->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label71->Location = System::Drawing::Point(57, 104);
			this->label71->Name = L"label71";
			this->label71->Size = System::Drawing::Size(84, 29);
			this->label71->TabIndex = 55;
			this->label71->Text = L"Theta:";
			// 
			// label73
			// 
			this->label73->AutoSize = true;
			this->label73->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label73->Location = System::Drawing::Point(177, 175);
			this->label73->Name = L"label73";
			this->label73->Size = System::Drawing::Size(25, 29);
			this->label73->TabIndex = 54;
			this->label73->Text = L"s";
			// 
			// label74
			// 
			this->label74->AutoSize = true;
			this->label74->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label74->Location = System::Drawing::Point(177, 143);
			this->label74->Name = L"label74";
			this->label74->Size = System::Drawing::Size(41, 29);
			this->label74->TabIndex = 53;
			this->label74->Text = L"Hz";
			// 
			// label75
			// 
			this->label75->AutoSize = true;
			this->label75->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label75->Location = System::Drawing::Point(158, 72);
			this->label75->Name = L"label75";
			this->label75->Size = System::Drawing::Size(46, 29);
			this->label75->TabIndex = 52;
			this->label75->Text = L"Oe";
			// 
			// label76
			// 
			this->label76->AutoSize = true;
			this->label76->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label76->Location = System::Drawing::Point(158, 38);
			this->label76->Name = L"label76";
			this->label76->Size = System::Drawing::Size(46, 29);
			this->label76->TabIndex = 51;
			this->label76->Text = L"Oe";
			// 
			// QPPeriodBox
			// 
			this->QPPeriodBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPPeriodBox->Location = System::Drawing::Point(114, 172);
			this->QPPeriodBox->MaxLength = 12;
			this->QPPeriodBox->Name = L"QPPeriodBox";
			this->QPPeriodBox->Size = System::Drawing::Size(60, 35);
			this->QPPeriodBox->TabIndex = 50;
			this->QPPeriodBox->Text = L"1";
			this->QPPeriodBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->QPPeriodBox->Leave += gcnew System::EventHandler(this, &Form1::setPeriodAndFrequency);
			// 
			// QPFreqBox
			// 
			this->QPFreqBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPFreqBox->Location = System::Drawing::Point(114, 140);
			this->QPFreqBox->MaxLength = 12;
			this->QPFreqBox->Name = L"QPFreqBox";
			this->QPFreqBox->Size = System::Drawing::Size(60, 35);
			this->QPFreqBox->TabIndex = 49;
			this->QPFreqBox->Text = L"1";
			this->QPFreqBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->QPFreqBox->Leave += gcnew System::EventHandler(this, &Form1::setPeriodAndFrequency);
			// 
			// QPOPBox
			// 
			this->QPOPBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPOPBox->Location = System::Drawing::Point(115, 69);
			this->QPOPBox->MaxLength = 7;
			this->QPOPBox->Name = L"QPOPBox";
			this->QPOPBox->Size = System::Drawing::Size(43, 35);
			this->QPOPBox->TabIndex = 48;
			this->QPOPBox->Text = L"0";
			this->QPOPBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask4);
			this->QPOPBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->QPOPBox->Leave += gcnew System::EventHandler(this, &Form1::setPlaneAndTheta);
			// 
			// label77
			// 
			this->label77->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label77->AutoSize = true;
			this->label77->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label77->Location = System::Drawing::Point(51, 175);
			this->label77->Name = L"label77";
			this->label77->Size = System::Drawing::Size(93, 29);
			this->label77->TabIndex = 47;
			this->label77->Text = L"Period:";
			// 
			// label78
			// 
			this->label78->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label78->AutoSize = true;
			this->label78->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label78->Location = System::Drawing::Point(21, 143);
			this->label78->Name = L"label78";
			this->label78->Size = System::Drawing::Size(139, 29);
			this->label78->TabIndex = 46;
			this->label78->Text = L"Frequency:";
			// 
			// label79
			// 
			this->label79->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label79->AutoSize = true;
			this->label79->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label79->Location = System::Drawing::Point(32, 72);
			this->label79->Name = L"label79";
			this->label79->Size = System::Drawing::Size(122, 29);
			this->label79->TabIndex = 45;
			this->label79->Text = L"Outplane:";
			// 
			// QPIPBox
			// 
			this->QPIPBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->QPIPBox->Location = System::Drawing::Point(115, 35);
			this->QPIPBox->MaxLength = 7;
			this->QPIPBox->Name = L"QPIPBox";
			this->QPIPBox->Size = System::Drawing::Size(43, 35);
			this->QPIPBox->TabIndex = 44;
			this->QPIPBox->Text = L"0";
			this->QPIPBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->QPIPBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->QPIPBox->Leave += gcnew System::EventHandler(this, &Form1::setPlaneAndTheta);
			// 
			// label80
			// 
			this->label80->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label80->AutoSize = true;
			this->label80->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label80->Location = System::Drawing::Point(46, 38);
			this->label80->Name = L"label80";
			this->label80->Size = System::Drawing::Size(102, 29);
			this->label80->TabIndex = 43;
			this->label80->Text = L"Inplane:";
			// 
			// shapeContainer7
			// 
			this->shapeContainer7->Location = System::Drawing::Point(0, 0);
			this->shapeContainer7->Margin = System::Windows::Forms::Padding(0);
			this->shapeContainer7->Name = L"shapeContainer7";
			this->shapeContainer7->Shapes->AddRange(gcnew cli::array< Microsoft::VisualBasic::PowerPacks::Shape^  >(3) {
				this->ovalShape10,
					this->lineShape29, this->ovalShape9
			});
			this->shapeContainer7->Size = System::Drawing::Size(713, 257);
			this->shapeContainer7->TabIndex = 57;
			this->shapeContainer7->TabStop = false;
			// 
			// ovalShape10
			// 
			this->ovalShape10->Location = System::Drawing::Point(547, 105);
			this->ovalShape10->Name = L"ovalShape10";
			this->ovalShape10->Size = System::Drawing::Size(6, 6);
			// 
			// lineShape29
			// 
			this->lineShape29->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dot;
			this->lineShape29->BorderWidth = 5;
			this->lineShape29->Enabled = false;
			this->lineShape29->Name = L"lineShape29";
			this->lineShape29->X1 = 356;
			this->lineShape29->X2 = 356;
			this->lineShape29->Y1 = 3;
			this->lineShape29->Y2 = 260;
			// 
			// ovalShape9
			// 
			this->ovalShape9->Location = System::Drawing::Point(181, 105);
			this->ovalShape9->Name = L"ovalShape9";
			this->ovalShape9->Size = System::Drawing::Size(6, 6);
			// 
			// xboxTab
			// 
			this->xboxTab->Controls->Add(this->precSuppCheck);
			this->xboxTab->Controls->Add(this->groupBox1);
			this->xboxTab->Controls->Add(this->ZModeBox);
			this->xboxTab->Controls->Add(this->HoldBox);
			this->xboxTab->Controls->Add(this->xboxPeriodBox);
			this->xboxTab->Controls->Add(this->xboxFreqBox);
			this->xboxTab->Controls->Add(this->label102);
			this->xboxTab->Controls->Add(this->label103);
			this->xboxTab->Controls->Add(this->label104);
			this->xboxTab->Controls->Add(this->label105);
			this->xboxTab->Controls->Add(this->FIBox);
			this->xboxTab->Controls->Add(this->label97);
			this->xboxTab->Controls->Add(this->label98);
			this->xboxTab->Controls->Add(this->label99);
			this->xboxTab->Controls->Add(this->label100);
			this->xboxTab->Controls->Add(this->label101);
			this->xboxTab->Controls->Add(this->xboxThetaBox);
			this->xboxTab->Controls->Add(this->xboxOPBox);
			this->xboxTab->Controls->Add(this->xboxIPBox);
			this->xboxTab->Controls->Add(this->label92);
			this->xboxTab->Controls->Add(this->label93);
			this->xboxTab->Controls->Add(this->label94);
			this->xboxTab->Controls->Add(this->label95);
			this->xboxTab->Controls->Add(this->label96);
			this->xboxTab->Controls->Add(this->shapeContainer8);
			this->xboxTab->Location = System::Drawing::Point(4, 29);
			this->xboxTab->Name = L"xboxTab";
			this->xboxTab->Size = System::Drawing::Size(713, 257);
			this->xboxTab->TabIndex = 7;
			this->xboxTab->Text = L"Xbox";
			this->xboxTab->UseVisualStyleBackColor = true;
			// 
			// precSuppCheck
			// 
			this->precSuppCheck->Checked = true;
			this->precSuppCheck->CheckState = System::Windows::Forms::CheckState::Checked;
			this->precSuppCheck->Location = System::Drawing::Point(215, 169);
			this->precSuppCheck->Name = L"precSuppCheck";
			this->precSuppCheck->Size = System::Drawing::Size(90, 30);
			this->precSuppCheck->TabIndex = 69;
			this->precSuppCheck->Text = L"Precession Suppression";
			this->precSuppCheck->UseVisualStyleBackColor = true;
			this->precSuppCheck->Click += gcnew System::EventHandler(this, &Form1::precSuppCheckChange);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->xboxCCW);
			this->groupBox1->Controls->Add(this->xboxCW);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(215, 32);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(96, 62);
			this->groupBox1->TabIndex = 68;
			this->groupBox1->TabStop = false;
			// 
			// xboxCCW
			// 
			this->xboxCCW->AutoSize = true;
			this->xboxCCW->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->xboxCCW->Location = System::Drawing::Point(14, 33);
			this->xboxCCW->Name = L"xboxCCW";
			this->xboxCCW->Size = System::Drawing::Size(98, 33);
			this->xboxCCW->TabIndex = 40;
			this->xboxCCW->Text = L"CCW";
			this->xboxCCW->UseVisualStyleBackColor = true;
			// 
			// xboxCW
			// 
			this->xboxCW->AutoSize = true;
			this->xboxCW->Checked = true;
			this->xboxCW->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->xboxCW->Location = System::Drawing::Point(14, 10);
			this->xboxCW->Name = L"xboxCW";
			this->xboxCW->Size = System::Drawing::Size(80, 33);
			this->xboxCW->TabIndex = 39;
			this->xboxCW->TabStop = true;
			this->xboxCW->Text = L"CW";
			this->xboxCW->UseVisualStyleBackColor = true;
			// 
			// ZModeBox
			// 
			this->ZModeBox->AutoSize = true;
			this->ZModeBox->Location = System::Drawing::Point(215, 146);
			this->ZModeBox->Name = L"ZModeBox";
			this->ZModeBox->Size = System::Drawing::Size(114, 24);
			this->ZModeBox->TabIndex = 67;
			this->ZModeBox->Text = L"Constant Z";
			this->ZModeBox->UseVisualStyleBackColor = true;
			// 
			// HoldBox
			// 
			this->HoldBox->AutoSize = true;
			this->HoldBox->Location = System::Drawing::Point(215, 123);
			this->HoldBox->Name = L"HoldBox";
			this->HoldBox->Size = System::Drawing::Size(68, 24);
			this->HoldBox->TabIndex = 66;
			this->HoldBox->Text = L"Hold";
			this->HoldBox->UseVisualStyleBackColor = true;
			// 
			// xboxPeriodBox
			// 
			this->xboxPeriodBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->xboxPeriodBox->Location = System::Drawing::Point(114, 172);
			this->xboxPeriodBox->MaxLength = 12;
			this->xboxPeriodBox->Name = L"xboxPeriodBox";
			this->xboxPeriodBox->Size = System::Drawing::Size(60, 35);
			this->xboxPeriodBox->TabIndex = 65;
			this->xboxPeriodBox->Text = L"1";
			this->xboxPeriodBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->xboxPeriodBox->Leave += gcnew System::EventHandler(this, &Form1::setPeriodAndFrequency);
			// 
			// xboxFreqBox
			// 
			this->xboxFreqBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->xboxFreqBox->Location = System::Drawing::Point(114, 140);
			this->xboxFreqBox->MaxLength = 12;
			this->xboxFreqBox->Name = L"xboxFreqBox";
			this->xboxFreqBox->Size = System::Drawing::Size(60, 35);
			this->xboxFreqBox->TabIndex = 64;
			this->xboxFreqBox->Text = L"1";
			this->xboxFreqBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->xboxFreqBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->xboxFreqBox->Leave += gcnew System::EventHandler(this, &Form1::setPeriodAndFrequency);
			// 
			// label102
			// 
			this->label102->AutoSize = true;
			this->label102->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label102->Location = System::Drawing::Point(177, 175);
			this->label102->Name = L"label102";
			this->label102->Size = System::Drawing::Size(25, 29);
			this->label102->TabIndex = 63;
			this->label102->Text = L"s";
			// 
			// label103
			// 
			this->label103->AutoSize = true;
			this->label103->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label103->Location = System::Drawing::Point(177, 143);
			this->label103->Name = L"label103";
			this->label103->Size = System::Drawing::Size(41, 29);
			this->label103->TabIndex = 62;
			this->label103->Text = L"Hz";
			// 
			// label104
			// 
			this->label104->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label104->AutoSize = true;
			this->label104->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label104->Location = System::Drawing::Point(51, 175);
			this->label104->Name = L"label104";
			this->label104->Size = System::Drawing::Size(93, 29);
			this->label104->TabIndex = 61;
			this->label104->Text = L"Period:";
			// 
			// label105
			// 
			this->label105->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label105->AutoSize = true;
			this->label105->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label105->Location = System::Drawing::Point(21, 143);
			this->label105->Name = L"label105";
			this->label105->Size = System::Drawing::Size(139, 29);
			this->label105->TabIndex = 60;
			this->label105->Text = L"Frequency:";
			// 
			// FIBox
			// 
			this->FIBox->AutoSize = true;
			this->FIBox->Location = System::Drawing::Point(215, 100);
			this->FIBox->Name = L"FIBox";
			this->FIBox->Size = System::Drawing::Size(112, 24);
			this->FIBox->TabIndex = 58;
			this->FIBox->Text = L"Fix Inplane";
			this->FIBox->UseVisualStyleBackColor = true;
			// 
			// label97
			// 
			this->label97->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label97->AutoSize = true;
			this->label97->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label97->Location = System::Drawing::Point(57, 104);
			this->label97->Name = L"label97";
			this->label97->Size = System::Drawing::Size(84, 29);
			this->label97->TabIndex = 56;
			this->label97->Text = L"Theta:";
			// 
			// label98
			// 
			this->label98->AutoSize = true;
			this->label98->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label98->Location = System::Drawing::Point(158, 72);
			this->label98->Name = L"label98";
			this->label98->Size = System::Drawing::Size(46, 29);
			this->label98->TabIndex = 55;
			this->label98->Text = L"Oe";
			// 
			// label99
			// 
			this->label99->AutoSize = true;
			this->label99->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label99->Location = System::Drawing::Point(158, 38);
			this->label99->Name = L"label99";
			this->label99->Size = System::Drawing::Size(46, 29);
			this->label99->TabIndex = 54;
			this->label99->Text = L"Oe";
			// 
			// label100
			// 
			this->label100->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label100->AutoSize = true;
			this->label100->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label100->Location = System::Drawing::Point(32, 72);
			this->label100->Name = L"label100";
			this->label100->Size = System::Drawing::Size(122, 29);
			this->label100->TabIndex = 53;
			this->label100->Text = L"Outplane:";
			// 
			// label101
			// 
			this->label101->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label101->AutoSize = true;
			this->label101->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label101->Location = System::Drawing::Point(46, 38);
			this->label101->Name = L"label101";
			this->label101->Size = System::Drawing::Size(102, 29);
			this->label101->TabIndex = 52;
			this->label101->Text = L"Inplane:";
			// 
			// xboxThetaBox
			// 
			this->xboxThetaBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->xboxThetaBox->Location = System::Drawing::Point(115, 101);
			this->xboxThetaBox->MaxLength = 7;
			this->xboxThetaBox->Name = L"xboxThetaBox";
			this->xboxThetaBox->Size = System::Drawing::Size(59, 35);
			this->xboxThetaBox->TabIndex = 51;
			this->xboxThetaBox->Text = L"0";
			this->xboxThetaBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask1);
			this->xboxThetaBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->xboxThetaBox->Leave += gcnew System::EventHandler(this, &Form1::setPlaneAndTheta);
			// 
			// xboxOPBox
			// 
			this->xboxOPBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->xboxOPBox->Location = System::Drawing::Point(115, 69);
			this->xboxOPBox->MaxLength = 7;
			this->xboxOPBox->Name = L"xboxOPBox";
			this->xboxOPBox->Size = System::Drawing::Size(43, 35);
			this->xboxOPBox->TabIndex = 50;
			this->xboxOPBox->Text = L"0";
			this->xboxOPBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask4);
			this->xboxOPBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->xboxOPBox->Leave += gcnew System::EventHandler(this, &Form1::setPlaneAndTheta);
			// 
			// xboxIPBox
			// 
			this->xboxIPBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->xboxIPBox->Location = System::Drawing::Point(115, 35);
			this->xboxIPBox->MaxLength = 7;
			this->xboxIPBox->Name = L"xboxIPBox";
			this->xboxIPBox->Size = System::Drawing::Size(43, 35);
			this->xboxIPBox->TabIndex = 49;
			this->xboxIPBox->Text = L"0";
			this->xboxIPBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox_Mask2);
			this->xboxIPBox->DoubleClick += gcnew System::EventHandler(this, &Form1::textBox_DoubleClick);
			this->xboxIPBox->Leave += gcnew System::EventHandler(this, &Form1::setPlaneAndTheta);
			// 
			// label92
			// 
			this->label92->AutoSize = true;
			this->label92->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label92->Location = System::Drawing::Point(381, 17);
			this->label92->Name = L"label92";
			this->label92->Size = System::Drawing::Size(23, 25);
			this->label92->TabIndex = 48;
			this->label92->Text = L"θ";
			// 
			// label93
			// 
			this->label93->AutoSize = true;
			this->label93->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label93->Location = System::Drawing::Point(575, 238);
			this->label93->Name = L"label93";
			this->label93->Size = System::Drawing::Size(32, 25);
			this->label93->TabIndex = 47;
			this->label93->Text = L"-Y";
			// 
			// label94
			// 
			this->label94->AutoSize = true;
			this->label94->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label94->Location = System::Drawing::Point(575, 17);
			this->label94->Name = L"label94";
			this->label94->Size = System::Drawing::Size(37, 25);
			this->label94->TabIndex = 46;
			this->label94->Text = L"+Y";
			// 
			// label95
			// 
			this->label95->AutoSize = true;
			this->label95->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label95->Location = System::Drawing::Point(460, 127);
			this->label95->Name = L"label95";
			this->label95->Size = System::Drawing::Size(33, 25);
			this->label95->TabIndex = 45;
			this->label95->Text = L"-X";
			// 
			// label96
			// 
			this->label96->AutoSize = true;
			this->label96->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label96->Location = System::Drawing::Point(686, 127);
			this->label96->Name = L"label96";
			this->label96->Size = System::Drawing::Size(38, 25);
			this->label96->TabIndex = 44;
			this->label96->Text = L"+X";
			// 
			// shapeContainer8
			// 
			this->shapeContainer8->Location = System::Drawing::Point(0, 0);
			this->shapeContainer8->Margin = System::Windows::Forms::Padding(0);
			this->shapeContainer8->Name = L"shapeContainer8";
			this->shapeContainer8->Shapes->AddRange(gcnew cli::array< Microsoft::VisualBasic::PowerPacks::Shape^  >(16) {
				this->ovalShape13,
					this->xboxVectB, this->xboxVectA, this->xboxVectMain, this->xboxVectThetaB, this->xboxVectThetaA, this->xboxVectThetaMain, this->lineShape41,
					this->lineShape40, this->lineShape39, this->lineShape38, this->lineShape30, this->rectangleShape6, this->lineShape34, this->ovalShape12,
					this->ovalShape11
			});
			this->shapeContainer8->Size = System::Drawing::Size(713, 257);
			this->shapeContainer8->TabIndex = 0;
			this->shapeContainer8->TabStop = false;
			// 
			// ovalShape13
			// 
			this->ovalShape13->Enabled = false;
			this->ovalShape13->Location = System::Drawing::Point(181, 106);
			this->ovalShape13->Name = L"ovalShape13";
			this->ovalShape13->Size = System::Drawing::Size(6, 6);
			// 
			// xboxVectB
			// 
			this->xboxVectB->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->xboxVectB->BorderWidth = 3;
			this->xboxVectB->Enabled = false;
			this->xboxVectB->Name = L"xboxVectB";
			this->xboxVectB->Visible = false;
			this->xboxVectB->X1 = 644;
			this->xboxVectB->X2 = 694;
			this->xboxVectB->Y1 = 246;
			this->xboxVectB->Y2 = 246;
			// 
			// xboxVectA
			// 
			this->xboxVectA->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->xboxVectA->BorderWidth = 3;
			this->xboxVectA->Enabled = false;
			this->xboxVectA->Name = L"xboxVectA";
			this->xboxVectA->Visible = false;
			this->xboxVectA->X1 = 644;
			this->xboxVectA->X2 = 694;
			this->xboxVectA->Y1 = 239;
			this->xboxVectA->Y2 = 239;
			// 
			// xboxVectMain
			// 
			this->xboxVectMain->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->xboxVectMain->BorderWidth = 3;
			this->xboxVectMain->Enabled = false;
			this->xboxVectMain->Name = L"xboxVectMain";
			this->xboxVectMain->Visible = false;
			this->xboxVectMain->X1 = 644;
			this->xboxVectMain->X2 = 694;
			this->xboxVectMain->Y1 = 232;
			this->xboxVectMain->Y2 = 232;
			// 
			// xboxVectThetaB
			// 
			this->xboxVectThetaB->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->xboxVectThetaB->BorderWidth = 3;
			this->xboxVectThetaB->Enabled = false;
			this->xboxVectThetaB->Name = L"xboxVectThetaB";
			this->xboxVectThetaB->Visible = false;
			this->xboxVectThetaB->X1 = 383;
			this->xboxVectThetaB->X2 = 433;
			this->xboxVectThetaB->Y1 = 247;
			this->xboxVectThetaB->Y2 = 247;
			// 
			// xboxVectThetaA
			// 
			this->xboxVectThetaA->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->xboxVectThetaA->BorderWidth = 3;
			this->xboxVectThetaA->Enabled = false;
			this->xboxVectThetaA->Name = L"xboxVectThetaA";
			this->xboxVectThetaA->Visible = false;
			this->xboxVectThetaA->X1 = 383;
			this->xboxVectThetaA->X2 = 433;
			this->xboxVectThetaA->Y1 = 240;
			this->xboxVectThetaA->Y2 = 240;
			// 
			// xboxVectThetaMain
			// 
			this->xboxVectThetaMain->BorderColor = System::Drawing::SystemColors::HotTrack;
			this->xboxVectThetaMain->BorderWidth = 3;
			this->xboxVectThetaMain->Enabled = false;
			this->xboxVectThetaMain->Name = L"xboxVectThetaMain";
			this->xboxVectThetaMain->Visible = false;
			this->xboxVectThetaMain->X1 = 383;
			this->xboxVectThetaMain->X2 = 433;
			this->xboxVectThetaMain->Y1 = 233;
			this->xboxVectThetaMain->Y2 = 233;
			// 
			// lineShape41
			// 
			this->lineShape41->BorderWidth = 2;
			this->lineShape41->Enabled = false;
			this->lineShape41->Name = L"lineShape41";
			this->lineShape41->X1 = 339;
			this->lineShape41->X2 = 339;
			this->lineShape41->Y1 = 35;
			this->lineShape41->Y2 = 235;
			// 
			// lineShape40
			// 
			this->lineShape40->Enabled = false;
			this->lineShape40->Name = L"lineShape40";
			this->lineShape40->X1 = 340;
			this->lineShape40->X2 = 440;
			this->lineShape40->Y1 = 135;
			this->lineShape40->Y2 = 135;
			// 
			// lineShape39
			// 
			this->lineShape39->Enabled = false;
			this->lineShape39->Name = L"lineShape39";
			this->lineShape39->X1 = 585;
			this->lineShape39->X2 = 585;
			this->lineShape39->Y1 = 235;
			this->lineShape39->Y2 = 35;
			// 
			// lineShape38
			// 
			this->lineShape38->Enabled = false;
			this->lineShape38->Name = L"lineShape38";
			this->lineShape38->X1 = 485;
			this->lineShape38->X2 = 685;
			this->lineShape38->Y1 = 135;
			this->lineShape38->Y2 = 135;
			// 
			// lineShape30
			// 
			this->lineShape30->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dot;
			this->lineShape30->BorderWidth = 5;
			this->lineShape30->Enabled = false;
			this->lineShape30->Name = L"lineShape30";
			this->lineShape30->X1 = 323;
			this->lineShape30->X2 = 323;
			this->lineShape30->Y1 = 4;
			this->lineShape30->Y2 = 261;
			// 
			// rectangleShape6
			// 
			this->rectangleShape6->BackColor = System::Drawing::Color::White;
			this->rectangleShape6->BackStyle = Microsoft::VisualBasic::PowerPacks::BackStyle::Opaque;
			this->rectangleShape6->BorderColor = System::Drawing::Color::Transparent;
			this->rectangleShape6->Enabled = false;
			this->rectangleShape6->Location = System::Drawing::Point(238, 34);
			this->rectangleShape6->Name = L"rectangleShape6";
			this->rectangleShape6->Size = System::Drawing::Size(100, 205);
			// 
			// lineShape34
			// 
			this->lineShape34->BorderStyle = System::Drawing::Drawing2D::DashStyle::Dot;
			this->lineShape34->BorderWidth = 5;
			this->lineShape34->Enabled = false;
			this->lineShape34->Name = L"lineShape34";
			this->lineShape34->X1 = 453;
			this->lineShape34->X2 = 453;
			this->lineShape34->Y1 = 3;
			this->lineShape34->Y2 = 260;
			// 
			// ovalShape12
			// 
			this->ovalShape12->BorderWidth = 2;
			this->ovalShape12->Enabled = false;
			this->ovalShape12->Location = System::Drawing::Point(240, 35);
			this->ovalShape12->Name = L"ovalShape12";
			this->ovalShape12->Size = System::Drawing::Size(200, 200);
			// 
			// ovalShape11
			// 
			this->ovalShape11->BorderWidth = 2;
			this->ovalShape11->Enabled = false;
			this->ovalShape11->Location = System::Drawing::Point(485, 35);
			this->ovalShape11->Name = L"ovalShape11";
			this->ovalShape11->Size = System::Drawing::Size(200, 200);
			// 
			// logTab
			// 
			this->logTab->Controls->Add(this->clearLogButton);
			this->logTab->Controls->Add(this->saveFileButton);
			this->logTab->Controls->Add(this->logBox);
			this->logTab->Location = System::Drawing::Point(4, 29);
			this->logTab->Name = L"logTab";
			this->logTab->Size = System::Drawing::Size(713, 257);
			this->logTab->TabIndex = 6;
			this->logTab->Text = L"Log";
			this->logTab->UseVisualStyleBackColor = true;
			// 
			// clearLogButton
			// 
			this->clearLogButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->clearLogButton->Location = System::Drawing::Point(539, 3);
			this->clearLogButton->Name = L"clearLogButton";
			this->clearLogButton->Size = System::Drawing::Size(171, 31);
			this->clearLogButton->TabIndex = 2;
			this->clearLogButton->Text = L"Clear Log File";
			this->clearLogButton->UseVisualStyleBackColor = true;
			this->clearLogButton->Click += gcnew System::EventHandler(this, &Form1::clearLogButton_Click);
			// 
			// saveFileButton
			// 
			this->saveFileButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->saveFileButton->Location = System::Drawing::Point(539, 230);
			this->saveFileButton->Name = L"saveFileButton";
			this->saveFileButton->Size = System::Drawing::Size(171, 31);
			this->saveFileButton->TabIndex = 1;
			this->saveFileButton->Text = L"Save Log File";
			this->saveFileButton->UseVisualStyleBackColor = true;
			this->saveFileButton->Click += gcnew System::EventHandler(this, &Form1::saveFileButton_Click);
			// 
			// logBox
			// 
			this->logBox->Font = (gcnew System::Drawing::Font(L"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->logBox->Location = System::Drawing::Point(3, 3);
			this->logBox->Name = L"logBox";
			this->logBox->ReadOnly = true;
			this->logBox->Size = System::Drawing::Size(530, 258);
			this->logBox->TabIndex = 0;
			this->logBox->Text = L"";
			// 
			// macroTab
			// 
			this->macroTab->Controls->Add(this->loopCheckBox);
			this->macroTab->Controls->Add(this->runMacroButton);
			this->macroTab->Controls->Add(this->macroNameBox);
			this->macroTab->Controls->Add(this->loadMacroButton);
			this->macroTab->Controls->Add(this->saveMacroButton);
			this->macroTab->Controls->Add(this->macroGrid);
			this->macroTab->Location = System::Drawing::Point(4, 29);
			this->macroTab->Name = L"macroTab";
			this->macroTab->Size = System::Drawing::Size(713, 257);
			this->macroTab->TabIndex = 8;
			this->macroTab->Text = L"Macros";
			this->macroTab->UseVisualStyleBackColor = true;
			// 
			// loopCheckBox
			// 
			this->loopCheckBox->AutoSize = true;
			this->loopCheckBox->Location = System::Drawing::Point(644, 207);
			this->loopCheckBox->Name = L"loopCheckBox";
			this->loopCheckBox->Size = System::Drawing::Size(71, 24);
			this->loopCheckBox->TabIndex = 6;
			this->loopCheckBox->Text = L"Loop";
			this->loopCheckBox->UseVisualStyleBackColor = true;
			// 
			// runMacroButton
			// 
			this->runMacroButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->runMacroButton->Location = System::Drawing::Point(621, 230);
			this->runMacroButton->Name = L"runMacroButton";
			this->runMacroButton->Size = System::Drawing::Size(89, 31);
			this->runMacroButton->TabIndex = 5;
			this->runMacroButton->Text = L"Run";
			this->runMacroButton->UseVisualStyleBackColor = true;
			this->runMacroButton->Click += gcnew System::EventHandler(this, &Form1::runMacroButton_Click);
			// 
			// macroNameBox
			// 
			this->macroNameBox->Location = System::Drawing::Point(621, 6);
			this->macroNameBox->Name = L"macroNameBox";
			this->macroNameBox->Size = System::Drawing::Size(89, 26);
			this->macroNameBox->TabIndex = 4;
			// 
			// loadMacroButton
			// 
			this->loadMacroButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->loadMacroButton->Location = System::Drawing::Point(621, 69);
			this->loadMacroButton->Name = L"loadMacroButton";
			this->loadMacroButton->Size = System::Drawing::Size(89, 31);
			this->loadMacroButton->TabIndex = 3;
			this->loadMacroButton->Text = L"Load";
			this->loadMacroButton->UseVisualStyleBackColor = true;
			this->loadMacroButton->Click += gcnew System::EventHandler(this, &Form1::loadMacroButton_Click);
			// 
			// saveMacroButton
			// 
			this->saveMacroButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->saveMacroButton->Location = System::Drawing::Point(621, 32);
			this->saveMacroButton->Name = L"saveMacroButton";
			this->saveMacroButton->Size = System::Drawing::Size(89, 31);
			this->saveMacroButton->TabIndex = 2;
			this->saveMacroButton->Text = L"Save";
			this->saveMacroButton->UseVisualStyleBackColor = true;
			this->saveMacroButton->Click += gcnew System::EventHandler(this, &Form1::saveMacroButton_Click);
			// 
			// macroGrid
			// 
			this->macroGrid->CellBorderStyle = System::Windows::Forms::DataGridViewCellBorderStyle::Sunken;
			this->macroGrid->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->macroGrid->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(9) {
				this->Action, this->Duration,
					this->Angle, this->Revolutions, this->CCW, this->Inplane, this->Outplane, this->Frequency, this->Direction
			});
			this->macroGrid->Location = System::Drawing::Point(5, 5);
			this->macroGrid->Name = L"macroGrid";
			this->macroGrid->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->macroGrid->Size = System::Drawing::Size(610, 256);
			this->macroGrid->TabIndex = 0;
			this->macroGrid->CellValidated += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::macroGrid_CellLeave);
			this->macroGrid->CellValueChanged += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::macroGrid_CellValueChanged);
			this->macroGrid->EditingControlShowing += gcnew System::Windows::Forms::DataGridViewEditingControlShowingEventHandler(this, &Form1::macroGrid_EditingControlShowing);
			// 
			// Action
			// 
			this->Action->HeaderText = L"Action";
			this->Action->Items->AddRange(gcnew cli::array< System::Object^  >(4) { L"Precess", L"Roll", L"Wait", L"Disk Hop" });
			this->Action->MaxDropDownItems = 2;
			this->Action->Name = L"Action";
			this->Action->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->Action->Width = 80;
			// 
			// Duration
			// 
			this->Duration->HeaderText = L"Duration";
			this->Duration->Name = L"Duration";
			this->Duration->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->Duration->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
			this->Duration->Width = 60;
			// 
			// Angle
			// 
			this->Angle->HeaderText = L"Angle";
			this->Angle->Name = L"Angle";
			this->Angle->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->Angle->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
			this->Angle->Width = 50;
			// 
			// Revolutions
			// 
			this->Revolutions->HeaderText = L"Revolutions";
			this->Revolutions->Name = L"Revolutions";
			this->Revolutions->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->Revolutions->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
			this->Revolutions->Width = 70;
			// 
			// CCW
			// 
			this->CCW->HeaderText = L"CCW";
			this->CCW->Name = L"CCW";
			this->CCW->Width = 35;
			// 
			// Inplane
			// 
			this->Inplane->HeaderText = L"Inplane";
			this->Inplane->Name = L"Inplane";
			this->Inplane->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->Inplane->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
			this->Inplane->Width = 60;
			// 
			// Outplane
			// 
			this->Outplane->HeaderText = L"Outplane";
			this->Outplane->Name = L"Outplane";
			this->Outplane->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->Outplane->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
			this->Outplane->Width = 60;
			// 
			// Frequency
			// 
			this->Frequency->HeaderText = L"Frequency";
			this->Frequency->Name = L"Frequency";
			this->Frequency->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->Frequency->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
			this->Frequency->Width = 70;
			// 
			// Direction
			// 
			this->Direction->HeaderText = L"Direction";
			this->Direction->Items->AddRange(gcnew cli::array< System::Object^  >(4) { L"Up", L"Down", L"Left", L"Right" });
			this->Direction->Name = L"Direction";
			this->Direction->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->Direction->Width = 65;
			// 
			// xboxStatusPic
			// 
			this->xboxStatusPic->Location = System::Drawing::Point(22, 350);
			this->xboxStatusPic->Name = L"xboxStatusPic";
			this->xboxStatusPic->Size = System::Drawing::Size(54, 50);
			this->xboxStatusPic->TabIndex = 57;
			this->xboxStatusPic->TabStop = false;
			this->xboxStatusPic->Visible = false;
			// 
			// voltDisplay
			// 
			this->voltDisplay->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->voltDisplay->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F));
			this->voltDisplay->Location = System::Drawing::Point(740, 282);
			this->voltDisplay->Name = L"voltDisplay";
			this->voltDisplay->Size = System::Drawing::Size(67, 20);
			this->voltDisplay->TabIndex = 15;
			this->voltDisplay->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->voltDisplay->Visible = false;
			// 
			// label22
			// 
			this->label22->AutoSize = true;
			this->label22->Location = System::Drawing::Point(749, 73);
			this->label22->Name = L"label22";
			this->label22->Size = System::Drawing::Size(20, 20);
			this->label22->TabIndex = 20;
			this->label22->Text = L"X";
			this->label22->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->label22->Visible = false;
			// 
			// label24
			// 
			this->label24->AutoSize = true;
			this->label24->Location = System::Drawing::Point(768, 73);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(20, 20);
			this->label24->TabIndex = 21;
			this->label24->Text = L"Y";
			this->label24->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->label24->Visible = false;
			// 
			// label25
			// 
			this->label25->AutoSize = true;
			this->label25->Location = System::Drawing::Point(787, 73);
			this->label25->Name = L"label25";
			this->label25->Size = System::Drawing::Size(19, 20);
			this->label25->TabIndex = 22;
			this->label25->Text = L"Z";
			this->label25->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->label25->Visible = false;
			// 
			// zeroButton
			// 
			this->zeroButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->zeroButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->zeroButton->Location = System::Drawing::Point(749, 251);
			this->zeroButton->Name = L"zeroButton";
			this->zeroButton->Size = System::Drawing::Size(52, 23);
			this->zeroButton->TabIndex = 64;
			this->zeroButton->Text = L"ZERO";
			this->zeroButton->UseVisualStyleBackColor = true;
			this->zeroButton->Visible = false;
			this->zeroButton->Click += gcnew System::EventHandler(this, &Form1::zeroButton_Click);
			// 
			// displayCheck
			// 
			this->displayCheck->AutoSize = true;
			this->displayCheck->Checked = true;
			this->displayCheck->CheckState = System::Windows::Forms::CheckState::Checked;
			this->displayCheck->Location = System::Drawing::Point(730, 415);
			this->displayCheck->Name = L"displayCheck";
			this->displayCheck->Size = System::Drawing::Size(118, 24);
			this->displayCheck->TabIndex = 65;
			this->displayCheck->Text = L"Live Display";
			this->displayCheck->UseVisualStyleBackColor = true;
			this->displayCheck->Visible = false;
			// 
			// saveLogFileDialog
			// 
			this->saveLogFileDialog->DefaultExt = L"txt";
			this->saveLogFileDialog->Filter = L"Text files|*.txt|All files|*.*";
			this->saveLogFileDialog->Title = L"Save Log File";
			this->saveLogFileDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::saveLogFileDialog_FileOk);
			// 
			// YPic
			// 
			this->YPic->Location = System::Drawing::Point(100, 350);
			this->YPic->Name = L"YPic";
			this->YPic->Size = System::Drawing::Size(25, 25);
			this->YPic->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->YPic->TabIndex = 66;
			this->YPic->TabStop = false;
			this->YPic->Visible = false;
			// 
			// APic
			// 
			this->APic->Location = System::Drawing::Point(100, 375);
			this->APic->Name = L"APic";
			this->APic->Size = System::Drawing::Size(25, 25);
			this->APic->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->APic->TabIndex = 67;
			this->APic->TabStop = false;
			this->APic->Visible = false;
			// 
			// BPic
			// 
			this->BPic->Location = System::Drawing::Point(245, 350);
			this->BPic->Name = L"BPic";
			this->BPic->Size = System::Drawing::Size(25, 25);
			this->BPic->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->BPic->TabIndex = 68;
			this->BPic->TabStop = false;
			this->BPic->Visible = false;
			// 
			// XPic
			// 
			this->XPic->Location = System::Drawing::Point(245, 375);
			this->XPic->Name = L"XPic";
			this->XPic->Size = System::Drawing::Size(25, 25);
			this->XPic->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->XPic->TabIndex = 69;
			this->XPic->TabStop = false;
			this->XPic->Visible = false;
			// 
			// LBPic
			// 
			this->LBPic->Location = System::Drawing::Point(472, 350);
			this->LBPic->Name = L"LBPic";
			this->LBPic->Size = System::Drawing::Size(34, 15);
			this->LBPic->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->LBPic->TabIndex = 71;
			this->LBPic->TabStop = false;
			this->LBPic->Visible = false;
			// 
			// RBPic
			// 
			this->RBPic->Location = System::Drawing::Point(573, 350);
			this->RBPic->Name = L"RBPic";
			this->RBPic->Size = System::Drawing::Size(34, 15);
			this->RBPic->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->RBPic->TabIndex = 70;
			this->RBPic->TabStop = false;
			this->RBPic->Visible = false;
			// 
			// label106
			// 
			this->label106->AutoSize = true;
			this->label106->Location = System::Drawing::Point(125, 356);
			this->label106->Name = L"label106";
			this->label106->Size = System::Drawing::Size(140, 20);
			this->label106->TabIndex = 72;
			this->label106->Text = L"Increase Outplane";
			this->label106->Visible = false;
			// 
			// label107
			// 
			this->label107->AutoSize = true;
			this->label107->Location = System::Drawing::Point(125, 382);
			this->label107->Name = L"label107";
			this->label107->Size = System::Drawing::Size(147, 20);
			this->label107->TabIndex = 73;
			this->label107->Text = L"Decrease Outplane";
			this->label107->Visible = false;
			// 
			// label108
			// 
			this->label108->AutoSize = true;
			this->label108->Location = System::Drawing::Point(270, 382);
			this->label108->Name = L"label108";
			this->label108->Size = System::Drawing::Size(135, 20);
			this->label108->TabIndex = 75;
			this->label108->Text = L"Decrease Inplane";
			this->label108->Visible = false;
			// 
			// label109
			// 
			this->label109->AutoSize = true;
			this->label109->Location = System::Drawing::Point(270, 356);
			this->label109->Name = L"label109";
			this->label109->Size = System::Drawing::Size(128, 20);
			this->label109->TabIndex = 74;
			this->label109->Text = L"Increase Inplane";
			this->label109->Visible = false;
			// 
			// LSPic
			// 
			this->LSPic->Location = System::Drawing::Point(380, 375);
			this->LSPic->Name = L"LSPic";
			this->LSPic->Size = System::Drawing::Size(25, 25);
			this->LSPic->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->LSPic->TabIndex = 77;
			this->LSPic->TabStop = false;
			this->LSPic->Visible = false;
			// 
			// RSPic
			// 
			this->RSPic->Location = System::Drawing::Point(380, 350);
			this->RSPic->Name = L"RSPic";
			this->RSPic->Size = System::Drawing::Size(25, 25);
			this->RSPic->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->RSPic->TabIndex = 76;
			this->RSPic->TabStop = false;
			this->RSPic->Visible = false;
			// 
			// label110
			// 
			this->label110->AutoSize = true;
			this->label110->Location = System::Drawing::Point(405, 382);
			this->label110->Name = L"label110";
			this->label110->Size = System::Drawing::Size(35, 20);
			this->label110->TabIndex = 79;
			this->label110->Text = L"N/A";
			this->label110->Visible = false;
			// 
			// label111
			// 
			this->label111->AutoSize = true;
			this->label111->Location = System::Drawing::Point(405, 356);
			this->label111->Name = L"label111";
			this->label111->Size = System::Drawing::Size(36, 20);
			this->label111->TabIndex = 78;
			this->label111->Text = L"Roll";
			this->label111->Visible = false;
			// 
			// RTPic
			// 
			this->RTPic->Location = System::Drawing::Point(573, 366);
			this->RTPic->Name = L"RTPic";
			this->RTPic->Size = System::Drawing::Size(34, 34);
			this->RTPic->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->RTPic->TabIndex = 80;
			this->RTPic->TabStop = false;
			this->RTPic->Visible = false;
			// 
			// LTPic
			// 
			this->LTPic->Location = System::Drawing::Point(472, 366);
			this->LTPic->Name = L"LTPic";
			this->LTPic->Size = System::Drawing::Size(34, 34);
			this->LTPic->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->LTPic->TabIndex = 81;
			this->LTPic->TabStop = false;
			this->LTPic->Visible = false;
			// 
			// label112
			// 
			this->label112->AutoSize = true;
			this->label112->Location = System::Drawing::Point(506, 351);
			this->label112->Name = L"label112";
			this->label112->Size = System::Drawing::Size(63, 20);
			this->label112->TabIndex = 82;
			this->label112->Text = L"Z Mode";
			this->label112->Visible = false;
			// 
			// label113
			// 
			this->label113->AutoSize = true;
			this->label113->Location = System::Drawing::Point(607, 351);
			this->label113->Name = L"label113";
			this->label113->Size = System::Drawing::Size(42, 20);
			this->label113->TabIndex = 83;
			this->label113->Text = L"Hold";
			this->label113->Visible = false;
			// 
			// label114
			// 
			this->label114->AutoSize = true;
			this->label114->Location = System::Drawing::Point(506, 376);
			this->label114->Name = L"label114";
			this->label114->Size = System::Drawing::Size(24, 20);
			this->label114->TabIndex = 84;
			this->label114->Text = L"-Z";
			this->label114->Visible = false;
			// 
			// label115
			// 
			this->label115->AutoSize = true;
			this->label115->Location = System::Drawing::Point(607, 376);
			this->label115->Name = L"label115";
			this->label115->Size = System::Drawing::Size(28, 20);
			this->label115->TabIndex = 85;
			this->label115->Text = L"+Z";
			this->label115->Visible = false;
			// 
			// StartPic
			// 
			this->StartPic->Location = System::Drawing::Point(674, 350);
			this->StartPic->Name = L"StartPic";
			this->StartPic->Size = System::Drawing::Size(36, 20);
			this->StartPic->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->StartPic->TabIndex = 86;
			this->StartPic->TabStop = false;
			this->StartPic->Visible = false;
			// 
			// BackPic
			// 
			this->BackPic->Location = System::Drawing::Point(674, 379);
			this->BackPic->Name = L"BackPic";
			this->BackPic->Size = System::Drawing::Size(36, 20);
			this->BackPic->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->BackPic->TabIndex = 87;
			this->BackPic->TabStop = false;
			this->BackPic->Visible = false;
			// 
			// label116
			// 
			this->label116->AutoSize = true;
			this->label116->Location = System::Drawing::Point(710, 354);
			this->label116->Name = L"label116";
			this->label116->Size = System::Drawing::Size(76, 20);
			this->label116->TabIndex = 88;
			this->label116->Text = L"Xbox Tab";
			this->label116->Visible = false;
			// 
			// label117
			// 
			this->label117->AutoSize = true;
			this->label117->Location = System::Drawing::Point(710, 383);
			this->label117->Name = L"label117";
			this->label117->Size = System::Drawing::Size(86, 20);
			this->label117->TabIndex = 89;
			this->label117->Text = L"Fix Inplane";
			this->label117->Visible = false;
			// 
			// saveMacroFileDialog
			// 
			this->saveMacroFileDialog->DefaultExt = L"bin";
			this->saveMacroFileDialog->Filter = L"Macro files|*.bin|All files|*.*";
			this->saveMacroFileDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::saveMacroFileDialog_FileOk);
			// 
			// loadMacroFileDialog
			// 
			this->loadMacroFileDialog->DefaultExt = L"bin";
			this->loadMacroFileDialog->Filter = L"Macro files|*.bin|All files|*.*";
			this->loadMacroFileDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::loadMacroFileDialog_FileOk);
			// 
			// saveConfigFileDialog
			// 
			this->saveConfigFileDialog->DefaultExt = L"bin";
			this->saveConfigFileDialog->Filter = L"Configuration files|*.cfg|All files|*.*";
			this->saveConfigFileDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::saveConfigFileDialog_FileOk);
			// 
			// loadConfigFileDialog
			// 
			this->loadConfigFileDialog->DefaultExt = L"bin";
			this->loadConfigFileDialog->Filter = L"Configuration files|*.cfg|All files|*.*";
			this->loadConfigFileDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::loadConfigFileDialog_FileOk);
			// 
			// statusStrip
			// 
			this->statusStrip->ImageScalingSize = System::Drawing::Size(24, 24);
			this->statusStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->statusLabel });
			this->statusStrip->Location = System::Drawing::Point(0, 426);
			this->statusStrip->Name = L"statusStrip";
			this->statusStrip->Size = System::Drawing::Size(821, 30);
			this->statusStrip->TabIndex = 96;
			// 
			// statusLabel
			// 
			this->statusLabel->Name = L"statusLabel";
			this->statusLabel->Size = System::Drawing::Size(207, 25);
			this->statusLabel->Text = L"Lodestone Disconnected";
			// 
			// Form1
			// 
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->CancelButton = this->zeroButton;
			this->ClientSize = System::Drawing::Size(821, 456);
			this->Controls->Add(this->statusStrip);
			this->Controls->Add(this->displayCheck);
			this->Controls->Add(this->label117);
			this->Controls->Add(this->label116);
			this->Controls->Add(this->BackPic);
			this->Controls->Add(this->StartPic);
			this->Controls->Add(this->label115);
			this->Controls->Add(this->label114);
			this->Controls->Add(this->label113);
			this->Controls->Add(this->label112);
			this->Controls->Add(this->LTPic);
			this->Controls->Add(this->RTPic);
			this->Controls->Add(this->label110);
			this->Controls->Add(this->label111);
			this->Controls->Add(this->LSPic);
			this->Controls->Add(this->RSPic);
			this->Controls->Add(this->label108);
			this->Controls->Add(this->label109);
			this->Controls->Add(this->label107);
			this->Controls->Add(this->label106);
			this->Controls->Add(this->LBPic);
			this->Controls->Add(this->RBPic);
			this->Controls->Add(this->XPic);
			this->Controls->Add(this->BPic);
			this->Controls->Add(this->APic);
			this->Controls->Add(this->YPic);
			this->Controls->Add(this->xboxStatusPic);
			this->Controls->Add(this->zeroButton);
			this->Controls->Add(this->label25);
			this->Controls->Add(this->label24);
			this->Controls->Add(this->label22);
			this->Controls->Add(this->voltDisplay);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->Connect_btn);
			this->Controls->Add(this->splashLogo);
			this->Controls->Add(this->shapeContainer1);
			this->MaximumSize = System::Drawing::Size(1000, 1000);
			this->MinimumSize = System::Drawing::Size(828, 385);
			this->Name = L"Form1";
			this->Text = L"MagMaestro v1.2";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &Form1::Form1_FormClosed);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splashLogo))->EndInit();
			this->tabControl1->ResumeLayout(false);
			this->setupTab->ResumeLayout(false);
			this->setupTab->PerformLayout();
			this->constantTab->ResumeLayout(false);
			this->constantTab->PerformLayout();
			this->precessionTab->ResumeLayout(false);
			this->precessionTab->PerformLayout();
			this->PrecessionBox->ResumeLayout(false);
			this->PrecessionBox->PerformLayout();
			this->rollingTab->ResumeLayout(false);
			this->rollingTab->PerformLayout();
			this->diskTab->ResumeLayout(false);
			this->diskTab->PerformLayout();
			this->DiskHopBox->ResumeLayout(false);
			this->DiskHopBox->PerformLayout();
			this->quickPrecTab->ResumeLayout(false);
			this->quickPrecTab->PerformLayout();
			this->QP_ActiveBox->ResumeLayout(false);
			this->QP_ActiveBox->PerformLayout();
			this->QP_ProposedBox->ResumeLayout(false);
			this->QP_ProposedBox->PerformLayout();
			this->xboxTab->ResumeLayout(false);
			this->xboxTab->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->logTab->ResumeLayout(false);
			this->macroTab->ResumeLayout(false);
			this->macroTab->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->macroGrid))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->xboxStatusPic))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->YPic))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->APic))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->BPic))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->XPic))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->LBPic))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->RBPic))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->LSPic))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->RSPic))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->RTPic))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->LTPic))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->StartPic))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->BackPic))->EndInit();
			this->statusStrip->ResumeLayout(false);
			this->statusStrip->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Connect_btn_Click(System::Object^  sender, System::EventArgs^  e) {
		if (mainLoopFlag) {
			WinUsb_Free(LodestoneInterfaceHandle);
			isConnected = false;
			mainLoopFlag = false;
			tabControl1->Enabled = false;
			statusLight->FillColor = System::Drawing::Color::Red;
			Connect_btn->Text = "Connect";
			statusLabel->Text = "Lodestone Disconnected";
			voltDisplay->Text = "";
			checkBoxXF->Checked = false;
			checkBoxXR->Checked = false;
			checkBoxYF->Checked = false;
			checkBoxYR->Checked = false;
			checkBoxZF->Checked = false;
			checkBoxZR->Checked = false;
			tabControl1->Visible = false;
			lineShape13->Visible = false;
			lineShape14->Visible = false;
			lineShape15->Visible = false;
			X_meter_back->Visible = false;
			Y_meter_back->Visible = false;
			Z_meter_back->Visible = false;
			voltDisplay->Visible = false;
			zeroButton->Visible = false;
			label22->Visible = false;
			label24->Visible = false;
			label25->Visible = false;
			displayCheck->Visible = false;
			VM_back->Visible = false;
			VM_fore->Visible = false;
			voltNeedLine->Visible = false;
			splashLogo->Visible = true;
			return;
		}
		GUID InterfaceClassGuidLegacy = { 0x761ED34A, 0xCCFA, 0x416B, 0x94, 0xBB, 0x33, 0x48, 0x6D, 0xB1, 0xF5, 0xD5 };
		GUID InterfaceClassGuid = { 0x7DDCC5E8, 0xFADE, 0x4D11, 0x96, 0x90, 0x9F, 0x86, 0xAA, 0xC6, 0x1D, 0xCA };

		HDEVINFO DeviceInfoTable = INVALID_HANDLE_VALUE;
		PSP_DEVICE_INTERFACE_DATA InterfaceDataStructure = new SP_DEVICE_INTERFACE_DATA;
		PSP_DEVICE_INTERFACE_DETAIL_DATA DetailedInterfaceDataStructure = new SP_DEVICE_INTERFACE_DETAIL_DATA;
		SP_DEVINFO_DATA DevInfoData;

		DWORD InterfaceIndex = 0;
		DWORD StatusLastError = 0;
		DWORD dwRegType;
		DWORD dwRegSize;
		DWORD StructureSize = 0;
		PBYTE PropertyValueBuffer;
		bool MatchFound = false;
		DWORD ErrorStatus;
		BOOL BoolStatus = FALSE;

		Console::WriteLine("Form1.h - Clicked Button");

		String^ DeviceIDToFind = MY_DEVICE_ID;
		//First populate a list of plugged in devices (by specifying "DIGCF_PRESENT"), which are of the specified class GUID. 
		DeviceInfoTable = SetupDiGetClassDevsUM(&InterfaceClassGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

		//Now look through the list we just populated.  We are trying to see if any of them match our device. 
		while (true)
		{
			InterfaceDataStructure->cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
			if (SetupDiEnumDeviceInterfacesUM(DeviceInfoTable, NULL, &InterfaceClassGuid, InterfaceIndex, InterfaceDataStructure))
			{
				ErrorStatus = GetLastError();
				if (ERROR_NO_MORE_ITEMS == ErrorStatus)	//Did we reach the end of the list of matching devices in the DeviceInfoTable?
				{	//Cound not find the device.  Must not have been attached.
					SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
					return;
				}
			}
			else 
			{
				DeviceInfoTable = SetupDiGetClassDevsUM(&InterfaceClassGuidLegacy, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
				if (SetupDiEnumDeviceInterfacesUM(DeviceInfoTable, NULL, &InterfaceClassGuidLegacy, InterfaceIndex, InterfaceDataStructure))
				{
					isLegacy = true;
					ErrorStatus = GetLastError();
					if (ERROR_NO_MORE_ITEMS == ErrorStatus)	//Did we reach the end of the list of matching devices in the DeviceInfoTable?
					{	//Cound not find the device.  Must not have been attached.
						SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
						return;
					}
				}
				else	//Else some other kind of unknown error ocurred...
				{
					ErrorStatus = GetLastError();
					SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
					return;
				}
			}


			//Now retrieve the hardware ID from the registry.  The hardware ID contains the VID and PID, which we will then 
			//check to see if it is the correct device or not.

			//Initialize an appropriate SP_DEVINFO_DATA structure.  We need this structure for SetupDiGetDeviceRegistryProperty().
			DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
			SetupDiEnumDeviceInfoUM(DeviceInfoTable, InterfaceIndex, &DevInfoData);

			//First query for the size of the hardware ID, so we can know how big a buffer to allocate for the data.
			SetupDiGetDeviceRegistryPropertyUM(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, &dwRegType, NULL, 0, &dwRegSize);

			//Allocate a buffer for the hardware ID.
			PropertyValueBuffer = (BYTE *)malloc(dwRegSize);
			if (PropertyValueBuffer == NULL)	//if null, error, couldn't allocate enough memory
			{	//Can't really recover from this situation, just exit instead.
				SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
				return;
			}

			//Retrieve the hardware IDs for the current device we are looking at.  PropertyValueBuffer gets filled with a 
			//REG_MULTI_SZ (array of null terminated strings).  To find a device, we only care about the very first string in the
			//buffer, which will be the "device ID".  The device ID is a string which contains the VID and PID, in the example 
			//format "Vid_04d8&Pid_003f".
			SetupDiGetDeviceRegistryPropertyUM(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, &dwRegType, PropertyValueBuffer, dwRegSize, NULL);

			//Now check if the first string in the hardware ID matches the device ID of my USB device.
#ifdef UNICODE
			String^ DeviceIDFromRegistry = gcnew String((wchar_t *)PropertyValueBuffer);
#else
			String^ DeviceIDFromRegistry = gcnew String((char *)PropertyValueBuffer);
#endif

			free(PropertyValueBuffer);		//No longer need the PropertyValueBuffer, free the memory to prevent potential memory leaks

			//Convert both strings to lower case.  This makes the code more robust/portable accross OS Versions
			DeviceIDFromRegistry = DeviceIDFromRegistry->ToLowerInvariant();
			DeviceIDToFind = DeviceIDToFind->ToLowerInvariant();
			//Now check if the hardware ID we are looking at contains the correct VID/PID
			MatchFound = DeviceIDFromRegistry->Contains(DeviceIDToFind);
			if (MatchFound == true)
			{
				//Device must have been found.  Open WinUSB interface handle now.  In order to do this, we will need the actual device path first.
				//We can get the path by calling SetupDiGetDeviceInterfaceDetail(), however, we have to call this function twice:  The first
				//time to get the size of the required structure/buffer to hold the detailed interface data, then a second time to actually 
				//get the structure (after we have allocated enough memory for the structure.)
				DetailedInterfaceDataStructure->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
				//First call populates "StructureSize" with the correct value
				SetupDiGetDeviceInterfaceDetailUM(DeviceInfoTable, InterfaceDataStructure, NULL, NULL, &StructureSize, NULL);
				DetailedInterfaceDataStructure = (PSP_DEVICE_INTERFACE_DETAIL_DATA)(malloc(StructureSize));		//Allocate enough memory
				if (DetailedInterfaceDataStructure == NULL)	//if null, error, couldn't allocate enough memory
				{	//Can't really recover from this situation, just exit instead.
					SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
					return;
				}
				DetailedInterfaceDataStructure->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
				//Now call SetupDiGetDeviceInterfaceDetail() a second time to receive the goods.  
				SetupDiGetDeviceInterfaceDetailUM(DeviceInfoTable, InterfaceDataStructure, DetailedInterfaceDataStructure, StructureSize, NULL, NULL);

				//We now have the proper device path, and we can finally open a device handle to the device.
				//WinUSB requires the device handle to be opened with the FILE_FLAG_OVERLAPPED attribute.
				if (LodestoneHandle == INVALID_HANDLE_VALUE)
					LodestoneHandle = CreateFile((DetailedInterfaceDataStructure->DevicePath), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);

				BoolStatus = WinUsb_Initialize(LodestoneHandle, &LodestoneInterfaceHandle);
				if (BoolStatus == TRUE) //Successfully Connected
				{
					bool flag = false;
					ULONG timeout = 100; // ms
					BOOL VALUE = FALSE;
					BOOL VALUE2 = TRUE;
					flag = WinUsb_SetPipePolicy(LodestoneInterfaceHandle, 0x84, IGNORE_SHORT_PACKETS, sizeof(BOOL), &VALUE2);
					if (!flag) throw;
					WinUsb_SetPipePolicy(LodestoneInterfaceHandle, 0x84, AUTO_FLUSH, sizeof(BOOL), &VALUE2);
					if (!flag) throw;
					WinUsb_SetPipePolicy(LodestoneInterfaceHandle, 0x84, AUTO_CLEAR_STALL, sizeof(BOOL), &VALUE2);
					WinUsb_SetPipePolicy(LodestoneInterfaceHandle, 0x84, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout);
					if (!flag) throw;
					isConnected = true;
					tabControl1->Enabled = true;
					//progressBar1->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
					statusLight->FillColor = System::Drawing::Color::LawnGreen;
					//Connect_btn->Enabled = false;
					Connect_btn->Text = "Disconnect";
					getConfig();
					getConfig();
					if (isLegacy)
						statusLabel->Text = "Lodestone Connected  USB Protocal: Legacy";
					else
						statusLabel->Text = "Lodestone Connected  USB Protocal: Microsoft Descriptor";
					tabControl1->Visible = true;
					mainLoopFlag = true;
					lineShape13->Visible = true;
					lineShape14->Visible = true;
					lineShape15->Visible = true;
					X_meter_back->Visible = true;
					Y_meter_back->Visible = true;
					Z_meter_back->Visible = true;
					voltDisplay->Visible = true;
					zeroButton->Visible = true;
					label22->Visible = true;
					label24->Visible = true;
					label25->Visible = true;
					displayCheck->Visible = true;
					VM_back->Visible = true;
					VM_fore->Visible = true;
					voltNeedLine->Visible = true;
					splashLogo->Visible = false;
				}

				SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
				while (mainLoopFlag)
					mainLoop();
				return;
			}

			InterfaceIndex++;
			//Keep looping until we either find a device with matching VID and PID, or until we run out of items.

		}//end of while(true)	

	}
	private: System::Void SendPosButt_Click(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: SendPosButt clicked.
		// SCOPE: Constant Tab.
		// DESCRIPTION: Gets called when the user sends a constant field on the constant tab.
		zeroButton_Click(zeroButton, e);
		float X = (float)(Convert::ToDouble(textBox_X->Text));
		float Y = (float)(Convert::ToDouble(textBox_Y->Text));
		float Z = (float)(Convert::ToDouble(textBox_Z->Text));
		SendPosition(X, Y, Z);
	}
	private: System::Void SendPosition(Single X, Single Y, Single Z) {
		// SCOPE: Used by all tabs.
		// DESCRIPTION: Sends a position packet to Lodestone.
		BOOL XferSuccess = 0;
		ULONG BytesWritten = 0;
		String^ logMessage;
		unsigned char OutputPacketBuffer[12];

		if ((X < 0) && (X < (-1)*X_Max))	X = (-1)*X_Max;
		if ((X > 0) && (X > X_Max))	X = X_Max;
		if ((Y < 0) && (Y < (-1)*Y_Max))	Y = (-1)*Y_Max;
		if ((Y > 0) && (Y > Y_Max))	Y = Y_Max;
		if ((Z < 0) && (Z < (-1)*Z_Max))	Z = (-1)*Z_Max;
		if ((Z > 0) && (Z > Z_Max))	Z = Z_Max;

		currentX = X;
		currentY = Y;
		currentZ = Z;

		if (X_OeToA == 0)	X = 0;
		else               X /= X_OeToA;
		if (Y_OeToA == 0)	Y = 0;
		else               Y /= Y_OeToA;
		if (Z_OeToA == 0)	Z = 0;
		else               Z /= Z_OeToA;

		OutputPacketBuffer[0] = BitConverter::GetBytes(X)[0];
		OutputPacketBuffer[1] = BitConverter::GetBytes(X)[1];
		OutputPacketBuffer[2] = BitConverter::GetBytes(X)[2];
		OutputPacketBuffer[3] = BitConverter::GetBytes(X)[3];
		OutputPacketBuffer[4] = BitConverter::GetBytes(Y)[0];
		OutputPacketBuffer[5] = BitConverter::GetBytes(Y)[1];
		OutputPacketBuffer[6] = BitConverter::GetBytes(Y)[2];
		OutputPacketBuffer[7] = BitConverter::GetBytes(Y)[3];
		OutputPacketBuffer[8] = BitConverter::GetBytes(Z)[0];
		OutputPacketBuffer[9] = BitConverter::GetBytes(Z)[1];
		OutputPacketBuffer[10] = BitConverter::GetBytes(Z)[2];
		OutputPacketBuffer[11] = BitConverter::GetBytes(Z)[3];

		XferSuccess = WinUsb_WritePipe(LodestoneInterfaceHandle, 0x01, &OutputPacketBuffer[0], 12, &BytesWritten, NULL);
		logMessage = currentTime();
		//logMessage += " --- X: " + float2String(currentX) + " Oe , Y: " + float2String(currentY) + " Oe , Z: " + float2String(currentZ) + " Oe\n";
		logMessage += " --- X: " + float2String(currentX) + " Oe , Y: " + float2String(currentY) + " Oe , Z: " + float2String(currentZ) + " Oe (" + (probeMS).ToString() + ")\n";
		logBox->AppendText(logMessage);
	}
	private: System::Void sendConfig() {
		// SCOPE: Setup Tab.
		// DESCRIPTION: Sends a configuration packet to Lodestone based on current Setup tab settings.
		BOOL XferSuccess1 = 0;
		BOOL XferSuccess2 = 0;
		ULONG BytesWritten1 = 0;
		ULONG BytesWritten2 = 0;
		unsigned char OutputPacketBuffer[56];

		X_Max = Convert::ToSingle(textBoxMFX->Text);
		Y_Max = Convert::ToSingle(textBoxMFY->Text);
		Z_Max = Convert::ToSingle(textBoxMFZ->Text);
		X_OeToA = Convert::ToSingle(textBoxOAX->Text);
		Y_OeToA = Convert::ToSingle(textBoxOAY->Text);
		Z_OeToA = Convert::ToSingle(textBoxOAZ->Text);

		unsigned int InplaneField = Convert::ToUInt32(textBoxIPF->Text);
		unsigned int OutplaneField = Convert::ToUInt32(textBoxOPF->Text);
		float RX = Convert::ToSingle(textBoxRX->Text);
		float RY = Convert::ToSingle(textBoxRY->Text);
		float RZ = Convert::ToSingle(textBoxRZ->Text);
		float OffsetFX = Convert::ToSingle(textBoxOFX->Text);
		float OffsetRX = Convert::ToSingle(textBoxORX->Text);
		float OffsetFY = Convert::ToSingle(textBoxOFY->Text);
		float OffsetRY = Convert::ToSingle(textBoxORY->Text);
		float OffsetFZ = Convert::ToSingle(textBoxOFZ->Text);
		float OffsetRZ = Convert::ToSingle(textBoxORZ->Text);
		float X_OeToA = Convert::ToSingle(textBoxOAX->Text);
		float Y_OeToA = Convert::ToSingle(textBoxOAY->Text);
		float Z_OeToA = Convert::ToSingle(textBoxOAZ->Text);
		unsigned char a = BitConverter::GetBytes(OffsetRX)[0];
		unsigned char b = BitConverter::GetBytes(OffsetRX)[1];
		unsigned char c = BitConverter::GetBytes(OffsetRX)[2];
		unsigned char d = BitConverter::GetBytes(OffsetRX)[3];


		OutputPacketBuffer[0] = BitConverter::GetBytes(Convert::ToUInt32(textBoxIPF->Text))[0];
		OutputPacketBuffer[1] = BitConverter::GetBytes(Convert::ToUInt32(textBoxIPF->Text))[1];
		OutputPacketBuffer[2] = BitConverter::GetBytes(Convert::ToUInt32(textBoxIPF->Text))[2];
		OutputPacketBuffer[3] = BitConverter::GetBytes(Convert::ToUInt32(textBoxIPF->Text))[3];
		OutputPacketBuffer[4] = BitConverter::GetBytes(Convert::ToUInt32(textBoxOPF->Text))[0];
		OutputPacketBuffer[5] = BitConverter::GetBytes(Convert::ToUInt32(textBoxOPF->Text))[1];
		OutputPacketBuffer[6] = BitConverter::GetBytes(Convert::ToUInt32(textBoxOPF->Text))[2];
		OutputPacketBuffer[7] = BitConverter::GetBytes(Convert::ToUInt32(textBoxOPF->Text))[3];
		OutputPacketBuffer[8] = BitConverter::GetBytes(Convert::ToSingle(textBoxRX->Text))[0];
		OutputPacketBuffer[9] = BitConverter::GetBytes(Convert::ToSingle(textBoxRX->Text))[1];
		OutputPacketBuffer[10] = BitConverter::GetBytes(Convert::ToSingle(textBoxRX->Text))[2];
		OutputPacketBuffer[11] = BitConverter::GetBytes(Convert::ToSingle(textBoxRX->Text))[3];
		OutputPacketBuffer[12] = BitConverter::GetBytes(Convert::ToSingle(textBoxRY->Text))[0];
		OutputPacketBuffer[13] = BitConverter::GetBytes(Convert::ToSingle(textBoxRY->Text))[1];
		OutputPacketBuffer[14] = BitConverter::GetBytes(Convert::ToSingle(textBoxRY->Text))[2];
		OutputPacketBuffer[15] = BitConverter::GetBytes(Convert::ToSingle(textBoxRY->Text))[3];
		OutputPacketBuffer[16] = BitConverter::GetBytes(Convert::ToSingle(textBoxRZ->Text))[0];
		OutputPacketBuffer[17] = BitConverter::GetBytes(Convert::ToSingle(textBoxRZ->Text))[1];
		OutputPacketBuffer[18] = BitConverter::GetBytes(Convert::ToSingle(textBoxRZ->Text))[2];
		OutputPacketBuffer[19] = BitConverter::GetBytes(Convert::ToSingle(textBoxRZ->Text))[3];
		OutputPacketBuffer[20] = BitConverter::GetBytes(Convert::ToSingle(textBoxOFX->Text))[0];
		OutputPacketBuffer[21] = BitConverter::GetBytes(Convert::ToSingle(textBoxOFX->Text))[1];
		OutputPacketBuffer[22] = BitConverter::GetBytes(Convert::ToSingle(textBoxOFX->Text))[2];
		OutputPacketBuffer[23] = BitConverter::GetBytes(Convert::ToSingle(textBoxOFX->Text))[3];
		OutputPacketBuffer[24] = BitConverter::GetBytes(Convert::ToSingle(textBoxORX->Text))[0];
		OutputPacketBuffer[25] = BitConverter::GetBytes(Convert::ToSingle(textBoxORX->Text))[1];
		OutputPacketBuffer[26] = BitConverter::GetBytes(Convert::ToSingle(textBoxORX->Text))[2];
		OutputPacketBuffer[27] = BitConverter::GetBytes(Convert::ToSingle(textBoxORX->Text))[3];
		OutputPacketBuffer[28] = BitConverter::GetBytes(Convert::ToSingle(textBoxOFY->Text))[0];
		OutputPacketBuffer[29] = BitConverter::GetBytes(Convert::ToSingle(textBoxOFY->Text))[1];
		OutputPacketBuffer[30] = BitConverter::GetBytes(Convert::ToSingle(textBoxOFY->Text))[2];
		OutputPacketBuffer[31] = BitConverter::GetBytes(Convert::ToSingle(textBoxOFY->Text))[3];
		OutputPacketBuffer[32] = BitConverter::GetBytes(Convert::ToSingle(textBoxORY->Text))[0];
		OutputPacketBuffer[33] = BitConverter::GetBytes(Convert::ToSingle(textBoxORY->Text))[1];
		OutputPacketBuffer[34] = BitConverter::GetBytes(Convert::ToSingle(textBoxORY->Text))[2];
		OutputPacketBuffer[35] = BitConverter::GetBytes(Convert::ToSingle(textBoxORY->Text))[3];
		OutputPacketBuffer[36] = BitConverter::GetBytes(Convert::ToSingle(textBoxOFZ->Text))[0];
		OutputPacketBuffer[37] = BitConverter::GetBytes(Convert::ToSingle(textBoxOFZ->Text))[1];
		OutputPacketBuffer[38] = BitConverter::GetBytes(Convert::ToSingle(textBoxOFZ->Text))[2];
		OutputPacketBuffer[39] = BitConverter::GetBytes(Convert::ToSingle(textBoxOFZ->Text))[3];
		OutputPacketBuffer[40] = BitConverter::GetBytes(Convert::ToSingle(textBoxORZ->Text))[0];
		OutputPacketBuffer[41] = BitConverter::GetBytes(Convert::ToSingle(textBoxORZ->Text))[1];
		OutputPacketBuffer[42] = BitConverter::GetBytes(Convert::ToSingle(textBoxORZ->Text))[2];
		OutputPacketBuffer[43] = BitConverter::GetBytes(Convert::ToSingle(textBoxORZ->Text))[3];
		OutputPacketBuffer[44] = BitConverter::GetBytes(Convert::ToSingle(textBoxOAX->Text))[0];
		OutputPacketBuffer[45] = BitConverter::GetBytes(Convert::ToSingle(textBoxOAX->Text))[1];
		OutputPacketBuffer[46] = BitConverter::GetBytes(Convert::ToSingle(textBoxOAX->Text))[2];
		OutputPacketBuffer[47] = BitConverter::GetBytes(Convert::ToSingle(textBoxOAX->Text))[3];
		OutputPacketBuffer[48] = BitConverter::GetBytes(Convert::ToSingle(textBoxOAY->Text))[0];
		OutputPacketBuffer[49] = BitConverter::GetBytes(Convert::ToSingle(textBoxOAY->Text))[1];
		OutputPacketBuffer[50] = BitConverter::GetBytes(Convert::ToSingle(textBoxOAY->Text))[2];
		OutputPacketBuffer[51] = BitConverter::GetBytes(Convert::ToSingle(textBoxOAY->Text))[3];
		OutputPacketBuffer[52] = BitConverter::GetBytes(Convert::ToSingle(textBoxOAZ->Text))[0];
		OutputPacketBuffer[53] = BitConverter::GetBytes(Convert::ToSingle(textBoxOAZ->Text))[1];
		OutputPacketBuffer[54] = BitConverter::GetBytes(Convert::ToSingle(textBoxOAZ->Text))[2];
		OutputPacketBuffer[55] = BitConverter::GetBytes(Convert::ToSingle(textBoxOAZ->Text))[3];

		for (int i = 0; i < 56; i++)
		{
			unsigned char a = OutputPacketBuffer[i];
		}

		unsigned char controlMessage = 2;

		if(!WinUsb_WritePipe(LodestoneInterfaceHandle, 0x03, &controlMessage, 1, &BytesWritten1, NULL)) throw;
		if(!WinUsb_WritePipe(LodestoneInterfaceHandle, 0x05, &OutputPacketBuffer[0], 56, &BytesWritten2, NULL)) throw;

		//XferSuccess1 = WinUsb_WritePipe(LodestoneInterfaceHandle, 0x03, &controlMessage, 1, &BytesWritten1, NULL);
		//XferSuccess2 = WinUsb_WritePipe(LodestoneInterfaceHandle, 0x05, &OutputPacketBuffer[0], 56, &BytesWritten2, NULL);
		getConfig();
	}
	private: System::Void getConfig() {
		// SCOPE: Setup Tab.
		// DESCRIPTION: Requests and retrieves the current configuration of Lodestone and displays it on the Setup tab.
		ULONG BytesRead = 0;
		ULONG BytesWritten = 0;
		unsigned char OutputPacketBuffer[1];
		unsigned char InputPacketBuffer[56];
		BOOL isInTransferSucess = false;
		BOOL isOutTransferSuccess = false;

		OutputPacketBuffer[0] = 1;
		WinUsb_FlushPipe(LodestoneInterfaceHandle, 0x03);
		WinUsb_FlushPipe(LodestoneInterfaceHandle, 0x84);
		isOutTransferSuccess = WinUsb_WritePipe(LodestoneInterfaceHandle, 0x03, &OutputPacketBuffer[0], 1, &BytesWritten, NULL);
		isInTransferSucess = WinUsb_ReadPipe(LodestoneInterfaceHandle, 0x84, &InputPacketBuffer[0], 56, &BytesRead, NULL);

		if (!isInTransferSucess)	return;

		array<unsigned char, 1>^byteArray = gcnew array<unsigned char, 1>(56);

		for (int i = 0; i < 56; i++)
			byteArray[i] = InputPacketBuffer[i];

		textBoxIPF->Text = BitConverter::ToUInt32(byteArray, 0).ToString();
		textBoxOPF->Text = BitConverter::ToUInt32(byteArray, 4).ToString();
		textBoxRX->Text = BitConverter::ToSingle(byteArray, 8).ToString();
		textBoxRY->Text = BitConverter::ToSingle(byteArray, 12).ToString();
		textBoxRZ->Text = BitConverter::ToSingle(byteArray, 16).ToString();
		textBoxOFX->Text = BitConverter::ToSingle(byteArray, 20).ToString();
		textBoxORX->Text = BitConverter::ToSingle(byteArray, 24).ToString();
		textBoxOFY->Text = BitConverter::ToSingle(byteArray, 28).ToString();
		textBoxORY->Text = BitConverter::ToSingle(byteArray, 32).ToString();
		textBoxOFZ->Text = BitConverter::ToSingle(byteArray, 36).ToString();
		textBoxORZ->Text = BitConverter::ToSingle(byteArray, 40).ToString();
		textBoxOAX->Text = BitConverter::ToSingle(byteArray, 44).ToString();
		textBoxOAY->Text = BitConverter::ToSingle(byteArray, 48).ToString();
		textBoxOAZ->Text = BitConverter::ToSingle(byteArray, 52).ToString();


		X_Max = Convert::ToSingle(textBoxMFX->Text);
		Y_Max = Convert::ToSingle(textBoxMFY->Text);
		Z_Max = Convert::ToSingle(textBoxMFZ->Text);
		X_OeToA = Convert::ToSingle(textBoxOAX->Text);
		Y_OeToA = Convert::ToSingle(textBoxOAY->Text);
		Z_OeToA = Convert::ToSingle(textBoxOAZ->Text);

	}
	private: System::Void SendQP() {
		// SCOPE: QuickPrecession tab.
		// DESCRIPTION: Sends a QuickPrecession packet.
		BOOL XferSuccess1 = 0;
		BOOL XferSuccess2 = 0;
		BOOL XferSuccess3 = 0;
		BOOL XferSuccess4 = 0;
		ULONG BytesWritten1 = 0;
		ULONG BytesWritten2 = 0;
		unsigned char OutputPacketBuffer[13];

		Single  inPlane = Convert::ToSingle(QPIPBox->Text);
		Single outPlane = Convert::ToSingle(QPOPBox->Text);
		Single   period = Convert::ToSingle(QPPeriodBox->Text);

		Single inPlaneCurrent = inPlane / X_OeToA;
		Single outPlaneCurrent = outPlane / Z_OeToA;

		OutputPacketBuffer[0] = BitConverter::GetBytes(inPlaneCurrent)[0];
		OutputPacketBuffer[1] = BitConverter::GetBytes(inPlaneCurrent)[1];
		OutputPacketBuffer[2] = BitConverter::GetBytes(inPlaneCurrent)[2];
		OutputPacketBuffer[3] = BitConverter::GetBytes(inPlaneCurrent)[3];
		OutputPacketBuffer[4] = BitConverter::GetBytes(outPlaneCurrent)[0];
		OutputPacketBuffer[5] = BitConverter::GetBytes(outPlaneCurrent)[1];
		OutputPacketBuffer[6] = BitConverter::GetBytes(outPlaneCurrent)[2];
		OutputPacketBuffer[7] = BitConverter::GetBytes(outPlaneCurrent)[3];
		OutputPacketBuffer[8] = BitConverter::GetBytes(period)[0];
		OutputPacketBuffer[9] = BitConverter::GetBytes(period)[1];
		OutputPacketBuffer[10] = BitConverter::GetBytes(period)[2];
		OutputPacketBuffer[11] = BitConverter::GetBytes(period)[3];
		if (QPCW->Checked)	OutputPacketBuffer[12] = 1;
		else				OutputPacketBuffer[12] = 0;

		unsigned char controlMessage = 3;

		XferSuccess1 = WinUsb_WritePipe(LodestoneInterfaceHandle, 0x03, &controlMessage, 1, &BytesWritten1, NULL);
		XferSuccess2 = WinUsb_WritePipe(LodestoneInterfaceHandle, 0x05, &OutputPacketBuffer[0], 13, &BytesWritten2, NULL);
		if ((XferSuccess1 == 0) || (XferSuccess2 == 0))		return;

		QPIPBoxLast->Text = QPIPBox->Text;
		QPOPBoxLast->Text = QPOPBox->Text;
		QPThetaBoxLast->Text = QPThetaBox->Text;
		QPFreqBoxLast->Text = QPFreqBox->Text;
		QPPeriodBoxLast->Text = QPPeriodBox->Text;
		QPCWLast->Checked = QPCW->Checked;
		QPCCWLast->Checked = QPCCW->Checked;
		QPStopButton->Enabled = true;
	}
	private: System::Void StopQP() {
		// SCOPE: QuickPrecession tab.
		// DESCRIPTION: Sends a StopQP message to Lodestone.
		BOOL XferSuccess = 0;
		ULONG BytesWritten = 0;
		unsigned char controlMessage = 4;
		XferSuccess = WinUsb_WritePipe(LodestoneInterfaceHandle, 0x03, &controlMessage, 1, &BytesWritten, NULL);
		SendPosition(0, 0, 0);

		QPIPBoxLast->Text = "0";
		QPOPBoxLast->Text = "0";
		QPThetaBoxLast->Text = "0";
		QPFreqBoxLast->Text = "0";
		QPPeriodBoxLast->Text = "0";
		QPCCWLast->Checked = false;
		QPCWLast->Checked = false;
		QPStopButton->Enabled = false;
	}
	private: System::Void textBox_Mask1(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user types a charecter into a textbox.
		// SCOPE: All tabs.
		// DESCRIPTION: When a textbox can accept POSITIVE OR NEGATIVES DECIMALS.
		TextBox^ textBox = safe_cast<TextBox^>(sender);
		String^ numbers("0123456789");
		int length = textBox->Text->Length;
		System::Object^ durp = textBox;
		if (length == 0)	return;
		int cursorLocation = textBox->SelectionStart;
		if (cursorLocation == 0)	return;
		String^ oldString = textBox->Text->Substring(0, cursorLocation - 1) + textBox->Text->Substring(cursorLocation, length - cursorLocation);
		String^ lastChar = textBox->Text->Substring(cursorLocation - 1, 1);
		if (!(numbers->Contains(lastChar)))
		{
			if ((cursorLocation == 1) && (lastChar == "-"))
				return;
			if ((lastChar == ".") && !(oldString->Contains(".")))
				return;
			textBox->Text = oldString;
			textBox->SelectionStart = cursorLocation - 1;
		}
	}
	private: System::Void textBox_Mask2(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user types a charecter into a textbox.
		// SCOPE: All tabs.
		// DESCRIPTION: When a textbox can accept POSITIVES DECIMALS.
		TextBox^ textBox = safe_cast<TextBox^>(sender);
		String^ numbers("0123456789");
		int length = textBox->Text->Length;
		System::Object^ durp = textBox;
		if (length == 0)	return;
		int cursorLocation = textBox->SelectionStart;
		if (cursorLocation == 0)	return;
		String^ oldString = textBox->Text->Substring(0, cursorLocation - 1) + textBox->Text->Substring(cursorLocation, length - cursorLocation);
		String^ lastChar = textBox->Text->Substring(cursorLocation - 1, 1);
		if (!(numbers->Contains(lastChar)))
		{
			if ((lastChar == ".") && !(oldString->Contains(".")))
				return;
			textBox->Text = oldString;
			textBox->SelectionStart = cursorLocation - 1;
		}
	}
	private: System::Void textBox_Mask3(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user types a charecter into a textbox.
		// SCOPE: All tabs.
		// DESCRIPTION: When a textbox can accept POSITIVES INTEGERS.
		TextBox^ textBox = safe_cast<TextBox^>(sender);
		String^ numbers("0123456789");
		int length = textBox->Text->Length;
		System::Object^ durp = textBox;
		if (length == 0)	return;
		int cursorLocation = textBox->SelectionStart;
		if (cursorLocation == 0)	return;
		String^ oldString = textBox->Text->Substring(0, cursorLocation - 1) + textBox->Text->Substring(cursorLocation, length - cursorLocation);
		String^ lastChar = textBox->Text->Substring(cursorLocation - 1, 1);
		if (!(numbers->Contains(lastChar)))
		{
			textBox->Text = oldString;
			textBox->SelectionStart = cursorLocation - 1;
		}
	}
	private: System::Void textBox_Mask4(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user types a charecter into a textbox.
		// SCOPE: All tabs.
		// DESCRIPTION: When a textbox can accept POSITIVE OR NEGATIVE INTEGERS.
		TextBox^ textBox = safe_cast<TextBox^>(sender);
		String^ numbers("0123456789");
		int length = textBox->Text->Length;
		System::Object^ durp = textBox;
		if (length == 0)	return;
		int cursorLocation = textBox->SelectionStart;
		if (cursorLocation == 0)	return;
		String^ oldString = textBox->Text->Substring(0, cursorLocation - 1) + textBox->Text->Substring(cursorLocation, length - cursorLocation);
		String^ lastChar = textBox->Text->Substring(cursorLocation - 1, 1);
		if (!(numbers->Contains(lastChar)))
		{
			if ((cursorLocation == 1) && (lastChar == "-"))
				return;
			textBox->Text = oldString;
			textBox->SelectionStart = cursorLocation - 1;
		}
	}
	private: System::Void textBox_Leave0(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user leaves a textbox.
		// SCOPE: All tabs.
		// DESCRIPTION: If there is no valid text in the textbox, place a 0 in said textbox.
		TextBox^ textBox = safe_cast<TextBox^>(sender);
		if (!(textBox->Text->Contains("1") ||
			textBox->Text->Contains("2") ||
			textBox->Text->Contains("3") ||
			textBox->Text->Contains("4") ||
			textBox->Text->Contains("5") ||
			textBox->Text->Contains("6") ||
			textBox->Text->Contains("7") ||
			textBox->Text->Contains("8") ||
			textBox->Text->Contains("9")))
			textBox->Text = "0";
	}
	private: System::Void textBox_Leave0Update(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user leaves a textbox.
		// SCOPE: Setup tab.
		// DESCRIPTION: If there is no valid text in the textbox, place a 0 in said textbox and update configuration settings.
		TextBox^ textBox = safe_cast<TextBox^>(sender);
		if (!(textBox->Text->Contains("1") ||
			textBox->Text->Contains("2") ||
			textBox->Text->Contains("3") ||
			textBox->Text->Contains("4") ||
			textBox->Text->Contains("5") ||
			textBox->Text->Contains("6") ||
			textBox->Text->Contains("7") ||
			textBox->Text->Contains("8") ||
			textBox->Text->Contains("9")))
			textBox->Text = "0";
		if ((textBox == textBoxOAX) || (textBox == textBoxOAY) || (textBox == textBoxOAZ)) {
			X_OeToA = Convert::ToSingle(textBoxOAX->Text);
			Y_OeToA = Convert::ToSingle(textBoxOAY->Text);
			Z_OeToA = Convert::ToSingle(textBoxOAZ->Text);
		}
		zeroButton_Click(zeroButton, e);
		sendConfig();
	}
	private: System::Void textBox_Leave1(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user leaves a textbox.
		// SCOPE: All tabs.
		// DESCRIPTION: If there is no valid text in the textbox, place a 1 in said textbox.
		TextBox^ textBox = safe_cast<TextBox^>(sender);
		if (!(textBox->Text->Contains("1") ||
			textBox->Text->Contains("2") ||
			textBox->Text->Contains("3") ||
			textBox->Text->Contains("4") ||
			textBox->Text->Contains("5") ||
			textBox->Text->Contains("6") ||
			textBox->Text->Contains("7") ||
			textBox->Text->Contains("8") ||
			textBox->Text->Contains("9")))
			textBox->Text = "1";
	}
	private: System::Void textBox_Leave1Update(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user leaves a textbox.
		// SCOPE: Setup tab.
		// DESCRIPTION: If there is no valid text in the textbox, place a 0 in said textbox and update configuration settings.
		TextBox^ textBox = safe_cast<TextBox^>(sender);
		if (!(textBox->Text->Contains("1") ||
			textBox->Text->Contains("2") ||
			textBox->Text->Contains("3") ||
			textBox->Text->Contains("4") ||
			textBox->Text->Contains("5") ||
			textBox->Text->Contains("6") ||
			textBox->Text->Contains("7") ||
			textBox->Text->Contains("8") ||
			textBox->Text->Contains("9")))
			textBox->Text = "1";
		if ((textBox == textBoxMFX) || (textBox == textBoxMFY) || (textBox == textBoxMFZ)) {
			X_Max = Convert::ToSingle(textBoxMFX->Text);
			Y_Max = Convert::ToSingle(textBoxMFY->Text);
			Z_Max = Convert::ToSingle(textBoxMFZ->Text);
			return;
		}
		zeroButton_Click(zeroButton, e);
		sendConfig();
	}
	private: System::Void textBox_LeavePF(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user leaves either the IPF or the OPF textbox.
		// SCOPE: Setup Tab.
		// DESCRIPTION: Set limits of the Inplane and Outplane PWM frequency.
		TextBox^ textBox = safe_cast<TextBox^>(sender);

		if (!(textBox->Text->Contains("1") ||
			textBox->Text->Contains("2") ||
			textBox->Text->Contains("3") ||
			textBox->Text->Contains("4") ||
			textBox->Text->Contains("5") ||
			textBox->Text->Contains("6") ||
			textBox->Text->Contains("7") ||
			textBox->Text->Contains("8") ||
			textBox->Text->Contains("9")))
			textBox->Text = "1";

		//Absolute limits of firmware: 0.3Hz to 40,000Hz (min uC period of 1000)
		//Set limits: 1000Hz to 40,000Hz
		if (Convert::ToDouble(textBoxIPF->Text) < 1000)		textBoxIPF->Text = (1000).ToString();
		if (Convert::ToDouble(textBoxOPF->Text) < 1000)		textBoxOPF->Text = (1000).ToString();
		if (Convert::ToDouble(textBoxIPF->Text) > 40000)	textBoxIPF->Text = (40000).ToString();
		if (Convert::ToDouble(textBoxOPF->Text) > 40000)	textBoxOPF->Text = (40000).ToString();

		zeroButton_Click(zeroButton, e);
		sendConfig();
	}
	private: System::Void textBox_DoubleClick(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user double clicks a textbox.
		// SCOPE: All tabs.
		// DESCRIPTION: Highlights all of the contents of the textbox so it can be easily cleared.
		TextBox^ textBox = safe_cast<TextBox^>(sender);
		textBox->SelectionStart = 0;
		textBox->SelectionLength = textBox->Text->Length;
	}
	private: System::Void enterButton_Click(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user presses ENTER.
		// SCOPE: Form Background.
		// DESCRIPTION: Does whatever ENTER should do on the current tab.
		if (setupTab->Visible) {
			zeroButton->Focus();
		}
		else if (constantTab->Visible) {
			SendPosButt->Focus();
			SendPosButt_Click(SendPosButt, e);
		}
		else if (precessionTab->Visible) {
			precessButton->Focus();
			precessing = false;
			precessButton_Click(precessButton, e);
		}
		else if (rollingTab->Visible) {
			rollButton->Focus();
			rolling = false;
			rollButton_Click(rollButton, e);
		}
		else if (quickPrecTab->Visible) {
			QPStartButton->Focus();
			QPStartButton_Click(QPStartButton, e);
		}
	}
	private: System::Void oneAmpCheck(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user clicks a checkbox to send constant current.
		// SCOPE: Setup tab.
		// DESCRIPTION: Sends a one amp (or two amp depending on the 1A/2A radio button) on the appropriate channel in the appropriate direction.
		CheckBox^ checkBox = safe_cast<CheckBox^>(sender);
		bool checked = checkBox->Checked;
		zeroButton_Click(sender, e);
		checkBoxXF->Checked = false;
		checkBoxXR->Checked = false;
		checkBoxYF->Checked = false;
		checkBoxYR->Checked = false;
		checkBoxZF->Checked = false;
		checkBoxZR->Checked = false;
		if (!checked) {
			SendPosition(0, 0, 0);
			return;
		}
		checkBox->Checked = true;
		if (oneAmpRB->Checked) {
			if (checkBox == checkBoxXF)		SendPosition(X_OeToA, 0, 0);
			else if (checkBox == checkBoxXR)		SendPosition((-1)*X_OeToA, 0, 0);
			else if (checkBox == checkBoxYF)		SendPosition(0, Y_OeToA, 0);
			else if (checkBox == checkBoxYR)		SendPosition(0, (-1)*Y_OeToA, 0);
			else if (checkBox == checkBoxZF)		SendPosition(0, 0, Z_OeToA);
			else if (checkBox == checkBoxZR)		SendPosition(0, 0, (-1)*Z_OeToA);
		}
		else {
			if (checkBox == checkBoxXF)		SendPosition(2 * X_OeToA, 0, 0);
			else if (checkBox == checkBoxXR)		SendPosition((-2)*X_OeToA, 0, 0);
			else if (checkBox == checkBoxYF)		SendPosition(0, 2 * Y_OeToA, 0);
			else if (checkBox == checkBoxYR)		SendPosition(0, (-2)*Y_OeToA, 0);
			else if (checkBox == checkBoxZF)		SendPosition(0, 0, 2 * Z_OeToA);
			else if (checkBox == checkBoxZR)		SendPosition(0, 0, (-2)*Z_OeToA);
		}
		return;
	}
	private: System::Void doubleMeterFunction(Microsoft::VisualBasic::PowerPacks::RectangleShape^ background, Microsoft::VisualBasic::PowerPacks::RectangleShape^ foreground, float magnitude) {
		// SCOPE: Form background (when Lodestone is connected).
		// DESCRIPTION: Shows active output magnitude and direction via bar graph.  Scales to user defined maximum field on setup tab.  Meter goes from green to red when the magnitude reaches 99.9% of max.
		unsigned int top = background->Top;
		unsigned int bottom = background->Bottom;
		unsigned int length = background->Height / 2;
		unsigned int middle = background->Top + length;

		if (magnitude > 1)	magnitude = 1;
		if (magnitude < -1)	magnitude = -1;

		if (magnitude == 0)
			foreground->Visible = false;
		else if (magnitude < 0) {
			magnitude *= -1;
			if (magnitude > 0.999)
				foreground->FillColor = System::Drawing::Color::Red;
			else
				foreground->FillColor = System::Drawing::Color::LawnGreen;
			foreground->Visible = true;
			foreground->Top = middle;
			foreground->Height = float2Int(magnitude*length + 1);
		}
		else {
			if (magnitude > 0.999)
				foreground->FillColor = System::Drawing::Color::Red;
			else
				foreground->FillColor = System::Drawing::Color::LawnGreen;
			foreground->Visible = true;
			foreground->Top = middle - float2Int(magnitude*length);
			foreground->Height = (int)(magnitude*length - 1);
		}
	}
	private: System::Void testCurrentChange(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user clicks changes the 1A/2A radio button.
		// SCOPE: Setup tab.
		// DESCRIPTION: In the event that constant current is being sent out on a channel, this function will make sure the current changes when the 1A/2A radio button selection changes.
		if (checkBoxXF->Checked)		oneAmpCheck(checkBoxXF, e);
		if (checkBoxXR->Checked)		oneAmpCheck(checkBoxXR, e);
		if (checkBoxYF->Checked)		oneAmpCheck(checkBoxYF, e);
		if (checkBoxYR->Checked)		oneAmpCheck(checkBoxYR, e);
		if (checkBoxZF->Checked)		oneAmpCheck(checkBoxZF, e);
		if (checkBoxZR->Checked)		oneAmpCheck(checkBoxZR, e);
	}
	private: System::Void calculateMaxFields(System::Void) {
		// SCOPE: Setup tab.
		// DESCRIPTION: Uses the voltage and configuration settings to determine the maximum field on each channel.
		float X_MaxCalculated, Y_MaxCalculated, Z_MaxCalculated, RX, RY, RZ;
		try {
			RX = Convert::ToSingle(textBoxRX->Text);
			RY = Convert::ToSingle(textBoxRY->Text);
			RZ = Convert::ToSingle(textBoxRZ->Text);
			if (RX > 0)	X_MaxCalculated = (int)((voltage / RX)*X_OeToA);
			if (RY > 0)	Y_MaxCalculated = (int)((voltage / RY)*Y_OeToA);
			if (RZ > 0)	Z_MaxCalculated = (int)((voltage / RZ)*Z_OeToA);
		}
		catch (FormatException ^e) {
			return;
		}

		labelCMFX->Text = X_MaxCalculated.ToString();
		labelCMFY->Text = Y_MaxCalculated.ToString();
		labelCMFZ->Text = Z_MaxCalculated.ToString();
	}
	private: System::Void calculateNeededVoltage() {
		// SCOPE: Setup tab.
		// DESCRIPTION: Displays the necessary voltage needed to achieve user defined max fields.
		float MFX, MFY, MFZ, RX, RY, RZ, maxVoltage;
		try {
			RX = Convert::ToSingle(textBoxRX->Text);
			RY = Convert::ToSingle(textBoxRY->Text);
			RZ = Convert::ToSingle(textBoxRZ->Text);
			MFX = Convert::ToSingle(textBoxMFX->Text);
			MFY = Convert::ToSingle(textBoxMFY->Text);
			MFZ = Convert::ToSingle(textBoxMFZ->Text);
		}
		catch (FormatException ^e) {
			return;
		}

		if (X_OeToA > 0)										maxVoltage = (MFX / X_OeToA)*RX;
		if (Y_OeToA > 0)	if (maxVoltage < (MFY / Y_OeToA)*RY)	maxVoltage = (MFY / Y_OeToA)*RY;
		if (Z_OeToA > 0)	if (maxVoltage < (MFZ / Z_OeToA)*RZ)	maxVoltage = (MFZ / Z_OeToA)*RZ;
		if (maxVoltage >= 20) {
			voltNeedLine->BorderColor = System::Drawing::Color::Red;
			voltNeedLine->X1 = VM_back->Location.X + VM_back->Width;
			voltNeedLine->X2 = VM_back->Location.X + VM_back->Width;
		}
		else {
			voltNeedLine->BorderColor = System::Drawing::SystemColors::ControlText;
			voltNeedLine->X1 = VM_back->Location.X + VM_back->Width*(maxVoltage / 20);
			voltNeedLine->X2 = VM_back->Location.X + VM_back->Width*(maxVoltage / 20);
		}
	}
	private: System::Void vectorDisplay(unsigned int radius, unsigned int middleX, unsigned int middleY,
		Microsoft::VisualBasic::PowerPacks::LineShape^ mainLine,
		Microsoft::VisualBasic::PowerPacks::LineShape^ lineA,
		Microsoft::VisualBasic::PowerPacks::LineShape^ lineB,
		int radiusRestricted, float X, float Y, float XMax, float YMax) {
		// SCOPE: All tabs.
		// DESCRIPTION: Creates a vector display to visualize the output.  Can be suppressed with the displayCheck checkbox.

					// RADIUS RESTRICTED VARIABLE:
					// 0 - No vector magnitude restriction
					// 1 - Vector magnitude restricted to circle
					// 2 - Vector magnitude forced to circle

		float magnitude = 0;
		double angle = 0;
		double d2r = 3.14159265359 / 180;

		if (radiusRestricted != 2) {
			if (X > XMax)		X = XMax;
			if (X < XMax*(-1))	X = XMax * (-1);
			if (Y > YMax)        Y = YMax;
			if (Y < YMax*(-1))   Y = YMax * (-1);
		}

		float limit = XMax;
		if (YMax < XMax)		limit = YMax;

		if (radiusRestricted == 0)
			magnitude = sqrt((X / XMax)*(X / XMax) + (Y / YMax)*(Y / YMax))*radius;
		if (radiusRestricted == 1)
			magnitude = sqrt(X*X + Y * Y)*radius / limit;
		else if ((radiusRestricted == 2) && ((X > 0.01*XMax) || (X < -0.01*XMax) || (Y > 0.01*YMax) || (Y < -0.1*YMax)))
			magnitude = (float)radius;

		if (radiusRestricted && (magnitude > radius))		magnitude = (float)radius;

		if (magnitude < 1) {
			mainLine->Visible = false;
			lineA->Visible = false;
			lineB->Visible = false;
			return;
		}
		mainLine->Visible = true;
		lineA->Visible = true;
		lineB->Visible = true;

		if (X == 0 && Y > 0)  angle = 90;
		else if (X == 0 && Y < 0)  angle = 270;
		else             		  angle = atan(Y / X) / d2r;
		if (X < 0)				  angle += 180;
		if (angle < 0)             angle += 360;
		mainLine->X1 = middleX;
		mainLine->Y1 = middleY;
		mainLine->X2 = middleX + float2Int(magnitude*cos(d2r*angle));
		mainLine->Y2 = middleY - float2Int(magnitude*sin(d2r*angle));
		lineA->X1 = mainLine->X2;
		lineB->X1 = mainLine->X2;
		lineA->Y1 = mainLine->Y2;
		lineB->Y1 = mainLine->Y2;
		lineA->X2 = mainLine->X2 + float2Int(0.25*magnitude*cos(d2r*(angle - 15 + 180)));
		lineA->Y2 = mainLine->Y2 - float2Int(0.25*magnitude*sin(d2r*(angle - 15 + 180)));
		lineB->X2 = mainLine->X2 + float2Int(0.25*magnitude*cos(d2r*(angle + 15 + 180)));
		lineB->Y2 = mainLine->Y2 - float2Int(0.25*magnitude*sin(d2r*(angle + 15 + 180)));
	}
	private: int          float2Int(double input) {
		// SCOPE: All tabs.
		// DESCRIPTION: Converts a double precision float to an integer with proper rounding.
		int temp = (int)input;
		double returnValue = 0;

		if (input > 0) {
			if ((double)temp - input > 0.5)	return temp;
			else                            return temp - 1;
		}
		else {
			if (input - (double)temp < 0.5)	return temp;
			else                            return temp - 1;
		}
	}
	private: System::String^ float2String(float input) {
		// SCOPE: All tabs.
		// DESCRIPTION: Converts a float to a String formatted for the log file.
		String^ output;
		int precision = 0;

		if ((input < 0.001) && (input > -0.001))	return "   0    ";

		output = input.ToString();
		if (output->Contains(".")) {
			precision = output->Length - output->IndexOf(".") + 1;
			while (precision < 5) {
				output += " ";
				precision++;
			}
			while (precision > 5) {
				output = output->Substring(0, output->Length - 1);
				precision--;
			}
		}
		else output += "    ";
		if (input > 0) {
			output = " " + output;
			if (input < 100)	output = " " + output;
			if (input < 10)		output = " " + output;
		}
		else {
			if (input > -100)	output = " " + output;
			if (input > -10)	output = " " + output;
		}
		return output;
	}
	private: System::Void zeroButton_Click(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user clicks the zeroButton.
		// SCOPE: Form background (when Lodestone is connected).
		// DESCRIPTION: Stops all routines and macros.  Also sends a zero position message to Lodestone.  (HARD ZERO)
		if (QPStopButton->Enabled)  StopQP();

		SendPosition(0, 0, 0);

		checkBoxXF->Checked = false;
		checkBoxXR->Checked = false;
		checkBoxYF->Checked = false;
		checkBoxYR->Checked = false;
		checkBoxZF->Checked = false;
		checkBoxZR->Checked = false;

		upButton->BackColor = System::Drawing::Color::Transparent;
		leftButton->BackColor = System::Drawing::Color::Transparent;
		downButton->BackColor = System::Drawing::Color::Transparent;
		rightButton->BackColor = System::Drawing::Color::Transparent;

		precessing = false;
		precessButton->Text = "Send";
		rolling = false;
		rollButton->Text = "Send";
		diskHopping = 0;

		//Xbox reset
		ZModeBox->Checked = false;
		FIBox->Checked = false;
		HoldBox->Checked = false;

		//Macro reset
		macroing = false;
		runMacroButton->Text = "Run";
		macroGrid->Enabled = true;
	}
	private: System::Void zeroButton_Click_Macro(System::Object^ sender, System::EventArgs^ e) {
		// SCOPE: Form background (when Lodestone is connected).
		// DESCRIPTION: Stops all routines but not macros.  (SOFT ZERO)
		if (QPStopButton->Enabled)  StopQP();

		//SendPosition(0, 0, 0);

		checkBoxXF->Checked = false;
		checkBoxXR->Checked = false;
		checkBoxYF->Checked = false;
		checkBoxYR->Checked = false;
		checkBoxZF->Checked = false;
		checkBoxZR->Checked = false;

		upButton->BackColor = System::Drawing::Color::Transparent;
		leftButton->BackColor = System::Drawing::Color::Transparent;
		downButton->BackColor = System::Drawing::Color::Transparent;
		rightButton->BackColor = System::Drawing::Color::Transparent;

		precessing = false;
		precessButton->Text = "Send";
		rolling = false;
		rollButton->Text = "Send";
		diskHopping = 0;

		//Xbox reset
		ZModeBox->Checked = false;
		FIBox->Checked = false;
		HoldBox->Checked = false;

		//Macro reset
		//macroing = false;
		//runMacroButton->Text = "Run";
		//macroGrid->Enabled = true;
	}
	private: System::Void setPlaneAndTheta(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user leaves an inplane, outplane, or theta box.
		// SCOPE: Precession, QuickPrecession, diskHop, and Xbox tabs.
		// DESCRIPTION: If the modified box is invalid, it calculates the correct value from the other two boxes.
		//              If the modified box is valid, it calculates the lowest priority box with the values from the other two boxes.
		TextBox^ IPBox;
		TextBox^ OPBox;
		TextBox^ thetaBox;
		Decimal IP;
		Decimal OP;
		float IPMax1;
		float IPMax2;
		float OPMax;

		if (macroing && (currMacroCommand == "Precess")) {
			IPBox = precIPBox;
			OPBox = precOPBox;
			thetaBox = precThetaBox;
			IP = precIP;
			OP = precOP;
			IPMax1 = X_Max;
			IPMax2 = Y_Max;
			OPMax = Z_Max;
		}
		else if (macroing && (currMacroCommand == "Disk Hop")) {
			IPBox = diskIPBox;
			OPBox = diskOPBox;
			thetaBox = diskThetaBox;
			IP = diskIP;
			OP = diskOP;
			IPMax1 = X_Max;
			IPMax2 = Y_Max;
			OPMax = Z_Max;
		}
		else if (precessionTab->Visible) {
			IPBox = precIPBox;
			OPBox = precOPBox;
			thetaBox = precThetaBox;
			IP = precIP;
			OP = precOP;
			IPMax1 = X_Max;
			IPMax2 = Y_Max;
			OPMax = Z_Max;
		}
		else if (diskTab->Visible) {
			IPBox = diskIPBox;
			OPBox = diskOPBox;
			thetaBox = diskThetaBox;
			IP = diskIP;
			OP = diskOP;
			IPMax1 = X_Max;
			IPMax2 = Y_Max;
			OPMax = Z_Max;
		}
		else if (quickPrecTab->Visible) {
			IPBox = QPIPBox;
			OPBox = QPOPBox;
			thetaBox = QPThetaBox;
			IP = QPIP;
			OP = QPOP;
			IPMax1 = X_Max;
			IPMax2 = Y_Max;
			OPMax = Z_Max;
		}
		else if (xboxTab->Visible) {
			IPBox = xboxIPBox;
			OPBox = xboxOPBox;
			thetaBox = xboxThetaBox;
			IP = xboxIP;
			OP = xboxOP;
			IPMax1 = X_Max;
			IPMax2 = Y_Max;
			OPMax = Z_Max;
		}
		else  return;

		TextBox^ calcBox;
		TextBox^ textBox = safe_cast<TextBox^>(sender);
		if (!checkValidText(textBox->Text))
			calcBox = textBox;
		else {
			if (textBox == thetaBox)	calcBox = OPBox;
			else                       calcBox = thetaBox;

			double input = Convert::ToDouble(textBox->Text);
			if (textBox == OPBox) {
				if (input > OPMax)		OPBox->Text = OPMax.ToString();
				if (input*(-1) > OPMax)		OPBox->Text = (OPMax*(-1)).ToString();
				OP = Convert::ToDecimal(OPBox->Text);
			}
			if (textBox == IPBox) {
				double IP_Max = IPMax1;
				if (IPMax2 < IPMax1)			IP_Max = IPMax2;
				if (input > IP_Max)	   IPBox->Text = IP_Max.ToString();
				if (input < 0)			   IPBox->Text = "0";
				IP = Convert::ToDecimal(IPBox->Text);
			}
			if (textBox == thetaBox) {
				if (input > 90)				thetaBox->Text = "90";
				if (input < -90)			thetaBox->Text = "-90";
			}
		}

		Decimal r2d = Convert::ToDecimal(180 / 3.14159265359);
		double  d2r = 3.14159265359 / 180;
		int inPlane;
		int outPlane;
		Decimal theta;


		if (calcBox == OPBox) {
			inPlane = Convert::ToInt16(IPBox->Text);
			theta = Convert::ToDecimal(thetaBox->Text);

			if ((Convert::ToDouble(theta) <= 0.09) && (Convert::ToDouble(theta)*(-1) <= 0.09)) {
				OPBox->Text = "0";
				OP = 0;
			}
			else if (Convert::ToDouble(theta) >= 89.91) {
				thetaBox->Text = "90";
				if (!checkValidText(OPBox->Text)) {
					OPBox->Text = "0";
					OP = 0;
				}
				else {
					IPBox->Text = "0";
					IP = 0;
				}
			}
			else if (Convert::ToDouble(theta)*(-1) >= 89.91) {
				thetaBox->Text = "90";
				if (!checkValidText(OPBox->Text)) {
					OPBox->Text = "0";
					OP = 0;
				}
				else {
					IPBox->Text = "0";
					IP = 0;
				}
				return;
			}
			else {

				Decimal tanTheta = Convert::ToDecimal(tan(d2r*Convert::ToDouble(thetaBox->Text)));
				OP = Decimal::Multiply(inPlane, tanTheta);

				if (Convert::ToDouble(OP) > Z_Max)
					OP = Convert::ToDecimal(Z_Max);
				if (Convert::ToDouble(OP)*(-1) > Z_Max)
					OP = Convert::ToDecimal(Z_Max)*(-1);

				outPlane = (int)Decimal::Round(OP, 0);
				OPBox->Text = outPlane.ToString();

				if (OP != Decimal::Multiply(inPlane, tanTheta)) {
					IPBox->Text = "";
					setPlaneAndTheta(IPBox, e);
				}
			}
		}
		else if (calcBox == IPBox) {
			outPlane = Convert::ToInt16(OPBox->Text);
			theta = Convert::ToDecimal(thetaBox->Text);

			if ((Convert::ToDouble(theta) <= 0.09) && (Convert::ToDouble(theta)*(-1) <= 0.09)) {
				IPBox->Text = "0";
				IP = 0;
			}
			else if (Convert::ToDouble(theta) >= 89.91) {
				thetaBox->Text = "90";
				IPBox->Text = "0";
				IP = 0;
			}
			else if (Convert::ToDouble(theta)*(-1) >= 89.91) {
				thetaBox->Text = "-90";
				IPBox->Text = "0";
				IP = 0;
			}
			else {
				Decimal tanTheta = Convert::ToDecimal(tan(d2r*Convert::ToDouble(thetaBox->Text)));
				IP = Decimal::Divide(outPlane, tanTheta);

				double IP_Max = X_Max;
				if (X_Max > Y_Max)		IP_Max = Y_Max;
				if (Convert::ToDouble(IP) > IP_Max)
					IP = Convert::ToDecimal(IP_Max);
				if (Convert::ToDouble(IP)*(-1) > IP_Max)
					IP = Convert::ToDecimal(IP_Max)*(-1);

				inPlane = (int)Decimal::Round(IP, 0);
				IPBox->Text = inPlane.ToString();
			}
		}
		else {
			inPlane = Convert::ToInt16(IPBox->Text);
			outPlane = Convert::ToInt16(OPBox->Text);

			if (outPlane == 0)
				thetaBox->Text = "0";
			else if ((inPlane == 0) && (outPlane > 0))
				thetaBox->Text = "90";
			else if ((inPlane == 0) && (outPlane < 0))
				thetaBox->Text = "-90";
			else if ((inPlane == 0) && (outPlane < 0.09) && (outPlane*(-1) < 0.09))
				thetaBox->Text = "0";
			else {
				theta = Decimal::Multiply(r2d, Convert::ToDecimal(atan((double)outPlane / (double)inPlane)));
				theta = Decimal::Round(Convert::ToDecimal(theta), 1);
				thetaBox->Text = theta.ToString();
			}
		}
		if (macroing && (currMacroCommand == "Precess")) {
			precIP = IP;
			precOP = OP;
		}
		else if (macroing && (currMacroCommand == "Disk Hop")) {
			diskIP = IP;
			diskOP = OP;
		}
		else if (precessionTab->Visible) {
			precIP = IP;
			precOP = OP;
		}
		else if (rollingTab->Visible) {
			rollIP = IP;
			rollOP = OP;
		}
		else if (diskTab->Visible) {
			diskIP = IP;
			diskOP = OP;
		}
		else if (quickPrecTab->Visible) {
			QPIP = IP;
			QPOP = OP;
		}
		else if (xboxTab->Visible) {
			xboxIP = IP;
			xboxOP = OP;
		}
	}
	private: System::Void setPeriodAndFrequency(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user leaves a frequency or period box.
		// SCOPE: Precession, QuickPrecession, Rolling, DiskHop, and Xbox tabs.
		// DESCRIPTION: If the modified box is invalid, it calculates the modified box from the other one.
		//              If the modified box is valid, it calculates the other box from the modified one.
		TextBox^ periodBox;
		TextBox^ freqBox;
		Button^ button;
		Decimal period;
		if (macroing && (currMacroCommand == "Precess")) {
			periodBox = precPeriodBox;
			freqBox = precFreqBox;
			button = precessButton;
		}
		else if (macroing && (currMacroCommand == "Roll")) {
			periodBox = rollPeriodBox;
			freqBox = rollFreqBox;
			button = rollButton;
		}
		else if (macroing && (currMacroCommand == "Disk Hop")) {
			periodBox = diskPeriodBox;
			freqBox = diskFreqBox;
		}
		else if (precessionTab->Visible) {
			periodBox = precPeriodBox;
			freqBox = precFreqBox;
			button = precessButton;
		}
		else if (rollingTab->Visible) {
			periodBox = rollPeriodBox;
			freqBox = rollFreqBox;
			button = rollButton;
		}
		else if (diskTab->Visible) {
			periodBox = diskPeriodBox;
			freqBox = diskFreqBox;
			//button = diskButton;
		}
		else if (quickPrecTab->Visible) {
			periodBox = QPPeriodBox;
			freqBox = QPFreqBox;
		}
		else if (xboxTab->Visible) {
			periodBox = xboxPeriodBox;
			freqBox = xboxFreqBox;
		}
		else return;
		Boolean orderFlag = false;		//Period Box first (true), Frequency Box first (false)
		TextBox^ textBox = safe_cast<TextBox^>(sender);
		if (textBox == periodBox)
			if (!checkValidText1(textBox->Text))	orderFlag = true;
		if (textBox == freqBox)
			if (checkValidText1(textBox->Text))		orderFlag = true;

		if (orderFlag) {
			periodBox->Text = Decimal::Round(Decimal::Divide(1, Convert::ToDecimal(freqBox->Text)), 4).ToString();
		}
		else {
			freqBox->Text = Decimal::Round(Decimal::Divide(1, Convert::ToDecimal(periodBox->Text)), 2).ToString();
		}
		period = Convert::ToDecimal(periodBox->Text);
		if (macroing && (currMacroCommand == "Precess"))	precPeriod = period;
		else if (macroing && (currMacroCommand == "Roll"))		rollPeriod = period;
		else if (macroing && (currMacroCommand == "Disk Hop"))  diskPeriod = period;
		else if (precessionTab->Visible)						precPeriod = period;
		else if (rollingTab->Visible)							rollPeriod = period;
		else if (diskTab->Visible)								diskPeriod = period;
		else if (quickPrecTab->Visible)							QPPeriod = period;
	}
	private: System::Boolean checkValidText(System::String^ input) {
		// SCOPE: All tabs.
		// DESCRIPTION: Returns a true if the box contains a number.
		if (System::String::IsNullOrEmpty(input))		return false;
		if (input->Contains("1") ||
			input->Contains("2") ||
			input->Contains("3") ||
			input->Contains("4") ||
			input->Contains("5") ||
			input->Contains("6") ||
			input->Contains("7") ||
			input->Contains("8") ||
			input->Contains("9") ||
			input->Contains("0"))
			return true;
		return false;
	}
	private: System::Boolean checkValidText1(System::String^ input) {
		// SCOPE: All tabs.
		// DESCRIPTION: Returns a true if the box contains a NONZERO number.
		if (System::String::IsNullOrEmpty(input))		return false;
		if (input->Contains("1") ||
			input->Contains("2") ||
			input->Contains("3") ||
			input->Contains("4") ||
			input->Contains("5") ||
			input->Contains("6") ||
			input->Contains("7") ||
			input->Contains("8") ||
			input->Contains("9"))
			return true;
		return false;
	}
	private: System::Void precession(Decimal inPlane, Decimal outPlane, Decimal period, System::Windows::Forms::RadioButton^ CWbutton) {
		// SCOPE: All tabs.
		// DESCRIPTION: Precess on the XY plane with an applied field in the Z direction.
		double position = updateTime(Convert::ToDouble(period));

		if (position > 1) {
			position = position - 1;
			newStartTime(Convert::ToDouble(period));
		}

		if (CWbutton->Checked) position = 1 - position;

		outAngle = position * 360;
		position = position * 6.28318530718;
		Single X = (Single)(cos(position)*Convert::ToDouble(inPlane));
		Single Y = (Single)(sin(position)*Convert::ToDouble(inPlane));
		Single Z = Convert::ToSingle(outPlane);
		SendPosition(X, Y, Z);
	}
	private: System::Void roll(Decimal inPlane, Decimal outPlane, Decimal period) {
		// SCOPE: All tabs.
		// DESCRIPTION: Roll in the direction defined by the rollAngle box, with an applied field in the outplane (orthagonal to the defined plane)
		//																(Or maybe not orthagonal, I'm not that sciency... you know what I mean.)
		double position = updateTime(Convert::ToDouble(period));

		if (position > 1) {
			position = position - 1;
			newStartTime(Convert::ToDouble(period));
		}

		position = position * 6.28318530718;
		Single X = (Single)((-1)*cos(position)*cos(rollAngle*d2r)*Convert::ToDouble(inPlane));
		Single Y = (Single)((-1)*cos(position)*sin(rollAngle*d2r)*Convert::ToDouble(inPlane));
		Single Z = (Single)(sin(position)*Convert::ToDouble(outPlane));
		SendPosition(X, Y, Z);
	}
	private: System::Void diskHop(Decimal inPlane, Decimal outPlane, Decimal period, Decimal hopTime, System::Windows::Forms::RadioButton^ CWbutton) {
		// SCOPE: All tabs.
		// DESCRIPTION: Rotate 180 degrees in the XY plane, 1/2 delay, flip Z direction, 1/2 delay, repeat.
		SYSTEMTIME currTime;
		double currMin;
		double currS;
		double currMS;
		double position;
		Decimal halfHopTime;
		Decimal fullHopTime;
		double tempAngle;
		/*-----------------------------------------//
		//		diskHopping Variable				//
		//  1: rotate 180 degrees					//
		//  2: delay half of hop time and switch Z	//
		//  3: delay half of hop time				//
		//-----------------------------------------*/
		switch (diskHopping) {
		case 1:
			position = updateTime(Convert::ToDouble(period));
			GetLocalTime(&currTime);
			currMin = currTime.wMinute;
			currS = currTime.wSecond;
			currMS = currTime.wMilliseconds;
			if (CWbutton->Checked)	position = (-1)*position;
			position = position + startAngle / 360;
			while (position >= 1)		position = position - 1;
			while (position < 0)		position = position + 1;
			outAngle = position * 360;
			position = position * 6.28318530718;
			if (((startAngle == 0) && (!CWbutton->Checked) && (outAngle > 180))
				|| ((startAngle == 0) && (CWbutton->Checked) && (outAngle < 180) && (outAngle > 0))
				|| ((startAngle == 180) && (!CWbutton->Checked) && (outAngle < 180) && (outAngle > 0))
				|| ((startAngle == 180) && (CWbutton->Checked) && (outAngle > 180))
				|| ((startAngle == 90) && (!CWbutton->Checked) && !((outAngle >= 90) && (outAngle <= 270)))
				|| ((startAngle == 90) && (CWbutton->Checked) && ((outAngle > 90) && (outAngle < 270)))
				|| ((startAngle == 270) && (!CWbutton->Checked) && ((outAngle > 90) && (outAngle < 270)))
				|| ((startAngle == 270) && (CWbutton->Checked) && !((outAngle >= 90) && (outAngle <= 270)))) {
				//If it satisfies exit condition then exit, else send message
				position = (endAngle / 360)*6.28318530718;
				Single X = (Single)(cos(position)*Convert::ToDouble(inPlane));
				Single Y = (Single)(sin(position)*Convert::ToDouble(inPlane));
				Single Z = Convert::ToSingle(outPlane);
				if (currentZ < 0)	Z = (-1)*Z;
				SendPosition(X, Y, Z);
				diskHopping = 2;
				return;
			}
			else {
				Single X = (Single)(cos(position)*Convert::ToDouble(inPlane));
				Single Y = (Single)(sin(position)*Convert::ToDouble(inPlane));
				Single Z = Convert::ToSingle(outPlane);
				if (currentZ < 0)	Z = (-1)*Z;
				SendPosition(X, Y, Z);
			}
			break;
		case 2:
			halfHopTime = Decimal::Divide((Decimal::Divide(hopTime, 2)), period);
			position = updateTime(Convert::ToDouble(period));
			if (position > 0.5 + Convert::ToDouble(halfHopTime)) {
				SendPosition(currentX, currentY, (-1)*currentZ);
				diskHopping = 3;
				return;
			}
			break;
		case 3:
			fullHopTime = Decimal::Divide(hopTime, period);
			position = updateTime(Convert::ToDouble(period));
			if (position > 0.5 + Convert::ToDouble(fullHopTime)) {
				tempAngle = endAngle;
				endAngle = startAngle;
				startAngle = tempAngle;
				newStartTime(Convert::ToDouble(Decimal::Add(Decimal::Divide(period, 2), hopTime)));
				diskHopping = 1;
				return;
			}
		default:
			break;
		}

	}
	private: System::Void precessButton_Click(System::Object^  sender, System::EventArgs^  e) {
		// Event: When the user clicks the precessButton.
		// SCOPE: Precession Tab.
		// DESCRIPTION: Do a soft zero, then initiate Precession subroutine.
		SYSTEMTIME currTime;
		if (!precessing) {
			zeroButton_Click_Macro(sender, e);
			GetLocalTime(&currTime);
			startMS = currTime.wMilliseconds;
			startS = currTime.wSecond;
			startMin = currTime.wMinute;
			if ((precPeriod == 0) || ((precIP == 0) && (precOP == 0)))  return;
			precessing = true;
			precessButton->Text = "Stop";
			//rolling = false;
			//rollButton->Text = "Send";
		}
		else {
			zeroButton_Click(zeroButton, e);
		}
	}
	private: System::Void rollButton_Click(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user clicks the rollButton.
		// SCOPE: Rolling Tab.
		// DESCRIPTION: Do a soft zero, then initiate Rolling subroutine.
		SYSTEMTIME currTime;
		if (!rolling) {
			zeroButton_Click_Macro(sender, e);
			GetLocalTime(&currTime);
			startMS = currTime.wMilliseconds;
			startS = currTime.wSecond;
			startMin = currTime.wMinute;
			if ((rollPeriod == 0) || ((rollIP == 0) && (rollOP == 0)))	return;
			rolling = true;
			rollButton->Text = "Stop";
			//precessing = false;
			//precessButton->Text = "Send";
		}
		else {
			zeroButton_Click(zeroButton, e);
		}
	}
	private: System::Void hopBox_Leave(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When user leaves the hopBox textbox.
		// SCOPE: DiskHop Tab.
		// DESCRIPTION: Load hopTime textbox value into hopTime variable.
		TextBox^ textBox = safe_cast<TextBox^>(sender);
		if (!(textBox->Text->Contains("1") ||
			textBox->Text->Contains("2") ||
			textBox->Text->Contains("3") ||
			textBox->Text->Contains("4") ||
			textBox->Text->Contains("5") ||
			textBox->Text->Contains("6") ||
			textBox->Text->Contains("7") ||
			textBox->Text->Contains("8") ||
			textBox->Text->Contains("9")))
			textBox->Text = "0";

		hopTime = Convert::ToDecimal(hopBox->Text);
		if (Convert::ToDouble(hopTime) < 0.001)		hopTime = Convert::ToDecimal(0.001);
	}
	private: System::Void directionButtonClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		// EVENT: When the user clicks either Up, Down, Left or Right DiskHop buttons.
		// SCOPE: DiskHop Tab.
		// DESCRIPTION: Begins the diskHopping routine in the selected direction.
		SYSTEMTIME currTime;
		Button^ thisButton = safe_cast<Button^>(sender);
		if ((diskIP == 0) || (diskOP == 0) || (diskPeriod == 0))		return;
		if (thisButton->BackColor == System::Drawing::Color::LawnGreen) {
			thisButton->BackColor = System::Drawing::Color::Transparent;
			zeroButton_Click(sender, e);
			return;
		}
		else {
			zeroButton_Click_Macro(sender, e);
			thisButton->BackColor = System::Drawing::Color::LawnGreen;
			if (thisButton->Text == "Up") { startAngle = 270;	 endAngle = 90; }
			if (thisButton->Text == "Left") { startAngle = 0;	 endAngle = 180; }
			if (thisButton->Text == "Down") { startAngle = 90;    endAngle = 270; }
			if (thisButton->Text == "Right") { startAngle = 180;   endAngle = 0; }
			GetLocalTime(&currTime);
			startMin = currTime.wMinute;
			startS = currTime.wSecond;
			startMS = currTime.wMilliseconds;
			diskHopping = 1;
			currentZ = Decimal::ToSingle(diskOP);
		}
	}
	private: System::Void rollDirectionClick(System::Object^ sender, System::EventArgs^  e) {
		// EVENT: When the user clicks either Up, Down, Left or Right Rolling buttons.
		// SCOPE: Rolling Tab.
		// DESCRIPTION: Sets the roll angle to the selected direction.
		Button^ thisButton = safe_cast<Button^>(sender);
		if (thisButton->Text == "Up") {
			rollAngleBox->Text = "90";
			rollAngle = 90;
		}
		else if (thisButton->Text == "Down") {
			rollAngleBox->Text = "270";
			rollAngle = 270;
		}
		else if (thisButton->Text == "Left") {
			rollAngleBox->Text = "180";
			rollAngle = 180;
		}
		else if (thisButton->Text == "Right") {
			rollAngleBox->Text = "0";
			rollAngle = 0;
		}
	}
	private: System::Void rollInfoChange(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user leaves the Inplane, Outplane, or Roll angle boxes
		// SCOPE: Rolling Tab.
		// DESCRIPTION: Updates the rollIP, rollOP, and rollAngle variables.  Also checks to see if any fields have exceeded max.
		Decimal maxIP = Convert::ToDecimal(X_Max);
		Decimal maxOP = Convert::ToDecimal(Z_Max);

		TextBox^ textBox = safe_cast<TextBox^>(sender);
		if (!(textBox->Text->Contains("1") ||
			textBox->Text->Contains("2") ||
			textBox->Text->Contains("3") ||
			textBox->Text->Contains("4") ||
			textBox->Text->Contains("5") ||
			textBox->Text->Contains("6") ||
			textBox->Text->Contains("7") ||
			textBox->Text->Contains("8") ||
			textBox->Text->Contains("9")))
			textBox->Text = "0";

		if (Convert::ToDecimal(Y_Max) < maxIP)	maxIP = Convert::ToDecimal(Y_Max);

		rollIP = Convert::ToDecimal(rollIPBox->Text);
		if (rollIP > maxIP) { rollIP = maxIP;		rollIPBox->Text = maxIP.ToString(); }
		rollOP = Convert::ToDecimal(rollOPBox->Text);
		if (rollOP > maxOP) { rollOP = maxOP;		rollOPBox->Text = maxOP.ToString(); }
		if (rollOP*(-1) > maxOP) { rollOP = maxOP * (-1);	rollOPBox->Text = (maxOP*(-1)).ToString(); }
		rollAngle = Convert::ToDouble(rollAngleBox->Text);
	}
	private: System::Void QPStartButton_Click(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user clicks the QPStartButton.
		// SCOPE: QuickPrecession Tab.
		// DESCRIPTION: Hard zero, then start the SendQP routine.
		zeroButton_Click(sender, e);
		SendQP();
	}
	private: System::Void QPStopButton_Click(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user clicks the QPStopButton.
		// SCOPE: QuickPrecession Tab.
		// DESCRIPTION: Start the StopQP routine.
		StopQP();
	}
	private: System::Void mainLoop() {
		// SCOPE: Form
		// DESCRIPTION: The main loop that handles updating displays, measureing time, and managing active routines.
		SYSTEMTIME currTime;

		Application::DoEvents();

		GetLocalTime(&currTime);
		testMS2 = currTime.wMilliseconds;
		double mainLoopMS = currTime.wMilliseconds;
		unsigned char OutputPacketBuffer[1];
		unsigned char InputPacketBuffer[4];
		BOOL XferSuccess = 0;
		ULONG BytesWritten = 0;
		ULONG BytesRead = 0;
		OutputPacketBuffer[0] = 1;

		if (!QPStopButton->Enabled && mainLoopFlag) {
			XferSuccess = WinUsb_ReadPipe(LodestoneInterfaceHandle, 0x82, &InputPacketBuffer[0], 4, &BytesRead, NULL);
			array<unsigned char>^byteArray = { InputPacketBuffer[0], InputPacketBuffer[1], InputPacketBuffer[2], InputPacketBuffer[3] };
			voltage = BitConverter::ToSingle(byteArray, 0);
			voltageDisp++;
			if (voltageDisp >= 100) {
				voltageDisp = 0;
				if (voltage.ToString()->Length > 5)
					voltDisplay->Text = voltage.ToString()->Substring(0, 5) + " V";
				else
					voltDisplay->Text = voltage.ToString() + " V";
				VM_fore->Width = VM_back->Width*(voltage / 20);
			}

			if (XferSuccess)	voltReadFails = 0;
			else				voltReadFails++;
			if (voltReadFails > 100)
			{
				mainLoopFlag = false;
				tabControl1->Enabled = false;
				statusLight->FillColor = System::Drawing::Color::Red;
				Connect_btn->Text = "Connect";
				statusLabel->Text = "Lodestone Disconnected";
				isConnected = false;
				voltDisplay->Text = "";
				checkBoxXF->Checked = false;
				checkBoxXR->Checked = false;
				checkBoxYF->Checked = false;
				checkBoxYR->Checked = false;
				checkBoxZF->Checked = false;
				checkBoxZR->Checked = false;
				tabControl1->Visible = false;
				lineShape13->Visible = false;
				lineShape14->Visible = false;
				lineShape15->Visible = false;
				X_meter_back->Visible = false;
				Y_meter_back->Visible = false;
				Z_meter_back->Visible = false;
				voltDisplay->Visible = false;
				zeroButton->Visible = false;
				label22->Visible = false;
				label24->Visible = false;
				label25->Visible = false;
				displayCheck->Visible = false;
				VM_back->Visible = false;
				VM_fore->Visible = false;
				voltNeedLine->Visible = false;
				splashLogo->Visible = true;
			}
		}

		// VOLTAGE DISPLAY
		if (setupTab->Visible) {
			if (voltage == 0) {
				labelCMFX->Text = "-----";
				labelCMFY->Text = "-----";
				labelCMFZ->Text = "-----";
			}
			else
				calculateMaxFields();
			calculateNeededVoltage();
		}

		// FIELD METERS
		if (!QPStopButton->Enabled) {
			doubleMeterFunction(X_meter_back, X_meter_fore, currentX / X_Max);
			doubleMeterFunction(Y_meter_back, Y_meter_fore, currentY / Y_Max);
			doubleMeterFunction(Z_meter_back, Z_meter_fore, currentZ / Z_Max);
		}
		else {
			meterBusy(X_meter_back, X_meter_fore);
			meterBusy(Y_meter_back, Y_meter_fore);
			meterBusy(Z_meter_back, Z_meter_fore);
		}

		// VECTOR DISPLAY
		if (constantTab->Visible) {
			if (displayCheck->Checked) {
				vectorDisplay(100, 582, 132, ConstantVectMain, ConstantVectA, ConstantVectB, 0, currentX, currentY, X_Max, Y_Max);
				vectorDisplay(100, 337, 132, ConstVectThetaMain, ConstVectThetaA, ConstVectThetaB, 2, sqrt(currentX*currentX + currentY * currentY), currentZ, 1, 1);
			}
			else {
				vectorDisplay(100, 582, 132, ConstantVectMain, ConstantVectA, ConstantVectB, 0, 0, 0, X_Max, Y_Max);
				vectorDisplay(100, 337, 132, ConstVectThetaMain, ConstVectThetaA, ConstVectThetaB, 2, 0, 0, 1, 1);
			}
		}
		else if (precessionTab->Visible) {
			if (displayCheck->Checked) {
				vectorDisplay(100, 585, 135, PrecVectMain, PrecVectA, PrecVectB, 1, currentX, currentY, X_Max, Y_Max);
				vectorDisplay(100, 340, 135, PrecVectThetaMain, PrecVectThetaA, PrecVectThetaB, 2, sqrt(currentX*currentX + currentY * currentY), currentZ, 1, 1);
			}
			else {
				vectorDisplay(100, 585, 135, PrecVectMain, PrecVectA, PrecVectB, 1, 0, 0, X_Max, Y_Max);
				vectorDisplay(100, 340, 135, PrecVectThetaMain, PrecVectThetaA, PrecVectThetaB, 2, 0, 0, 1, 1);
			}
		}
		else if (rollingTab->Visible) {
			if (displayCheck->Checked) {
				vectorDisplay(100, 585, 135, rollVectMain, rollVectA, rollVectB, 2, Convert::ToSingle(cos(rollAngle*d2r)), Convert::ToSingle(sin(rollAngle*d2r)), 1, 1);
				vectorDisplay(100, 340, 135, rollVectThetaMain, rollVectThetaA, rollVectThetaB, 2, sqrt(currentX*currentX + currentY * currentY), currentZ, 1, 1);
			}
			else {
				vectorDisplay(100, 585, 135, rollVectMain, rollVectA, rollVectB, 2, 0, 0, 1, 1);
				vectorDisplay(100, 340, 135, rollVectThetaMain, rollVectThetaA, rollVectThetaB, 2, 0, 0, 1, 1);
			}
		}
		else if (xboxTab->Visible) {
			if (displayCheck->Checked) {
				vectorDisplay(100, 585, 135, xboxVectMain, xboxVectA, xboxVectB, FIBox->Checked, currentX, currentY, X_Max, Y_Max);
				vectorDisplay(100, 340, 135, xboxVectThetaMain, xboxVectThetaA, xboxVectThetaB, 2, sqrt(currentX*currentX + currentY * currentY), currentZ, 1, 1);
			}
			else {
				vectorDisplay(100, 585, 135, xboxVectMain, xboxVectA, xboxVectB, 2, 0, 0, 1, 1);
				vectorDisplay(100, 340, 135, xboxVectThetaMain, xboxVectThetaA, xboxVectThetaB, 2, 0, 0, 1, 1);
			}
		}
		if (diskHopping != 0)		diskHop(diskIP, diskOP, diskPeriod, hopTime, diskCW);
		if (precessing)				precession(precIP, precOP, precPeriod, precCW);
		if (rolling)				roll(rollIP, rollOP, rollPeriod);

		//XBOX CONTROL
		xboxState = xboxCheckState(xboxState);
		if (xboxState == 2)			checkForStartButton();
		else if (xboxState == 3)	xboxControls();

		//MACROS
		if (macroing)				isMacroCommandOver();
		if (macroSaveFlag)			saveMacro();
		if (macroLoadFlag)			loadMacro();

		//CONFIGURATIONS
		if (configSaveFlag)			saveConfig();
		if (configLoadFlag)			loadConfig();

		GetLocalTime(&currTime);
		while (currTime.wMilliseconds == mainLoopMS) { GetLocalTime(&currTime); }
	}
	private: System::Void meterBusy(Microsoft::VisualBasic::PowerPacks::RectangleShape^ background, Microsoft::VisualBasic::PowerPacks::RectangleShape^ foreground) {
		// SCOPE: Form background (When Lodestone is connected).
		// DESCRIPTION: Changes meter color to Yellow to indicate Lodestone is busy with a QuickPrecession routine.
		foreground->Visible = true;
		foreground->FillColor = System::Drawing::Color::Yellow;
		foreground->Top = background->Top + 1;
		foreground->Left = background->Left + 1;
		foreground->Height = background->Height - 1;
		foreground->Width = background->Width - 1;
		return;
	}
	private: System::String^ currentTime() {
		// SCOPE: Log Tab.
		// DESCRIPTION: Converts current time to a string formatted for the Log file.
		SYSTEMTIME currTime;
		GetLocalTime(&currTime);
		Boolean hourPadFlag = false;
		Boolean minPadFlag = false;
		Boolean secPadFlag = false;
		String^ output = "";

		if (currTime.wHour > 12)			currTime.wHour -= 12;
		if (currTime.wHour < 10)			output += " ";
		output += currTime.wHour.ToString();
		output += ":";
		if (currTime.wMinute < 10)			output += "0";
		output += currTime.wMinute.ToString();
		output += ":";
		if (currTime.wSecond < 10)			output += "0";
		output += currTime.wSecond.ToString();
		output += ".";
		output += currTime.wMilliseconds.ToString();
		if (currTime.wMilliseconds < 100)	output += " ";
		if (currTime.wMilliseconds < 10)	output += " ";

		return output;
	}
	private: double       updateTime(double period) {
		// SCOPE: All tabs.
		// DESCRIPTION: Based on the start time, current time, and period, this function will output a number from 0 to 1 corresponding to its position in the period.
		SYSTEMTIME currTime;
		GetLocalTime(&currTime);
		double currMin = currTime.wMinute;
		double currS = currTime.wSecond;
		double currMS = currTime.wMilliseconds;
		double minDiff = currMin - startMin;
		if (minDiff < 0)	minDiff = minDiff + 60;
		double secDiff = currS - startS;
		if (secDiff < 0) { secDiff = secDiff + 60;	minDiff = minDiff - 1; }
		double mSecDiff = currMS - startMS;
		if (mSecDiff < 0) { mSecDiff = mSecDiff + 1000;	secDiff = secDiff - 1;		if (secDiff < 0) { secDiff = secDiff + 60;		minDiff = minDiff - 1; } }
		double timeDiff = 60 * minDiff + secDiff + mSecDiff / 1000;

		testMS1 = currMS;

		probeMS = currMS - lastMS;
		if (probeMS < 0)	probeMS += 1000;
		lastMS = currMS;

		return (timeDiff / period);
	}
	private: System::Void newStartTime(double period) {
		// SCOPE: All tabs.
		// DESCRIPTION: Increments the start time by the period passed to this function (seconds).
		int secTrunct;
		int minTrunct;
		minTrunct = Convert::ToInt16(floor(period / 60));
		secTrunct = Convert::ToInt16(floor(period)) - minTrunct * 60;
		startMin += minTrunct;
		startS += secTrunct;
		startMS += 1000 * Convert::ToDouble(period - secTrunct - minTrunct * 60);
		if (startMS > 1000) { startMS = startMS - 1000;	startS = startS + 1; }
		if (startS > 60) { startS = startS - 60;	startMin = startMin + 1; }
		if (startMin > 60)		 startMin = startMin - 60;
		return;
	}
	private: System::Void saveFileButton_Click(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user clicks the button to save the output log.
		// SCOPE: Log Tab.
		// DESCRIPTION: This function shows the save dialog for the log.
		saveLogFileDialog->ShowDialog();
	}
	private: System::Void saveLogFileDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
		// EVENT: When the user enters a valid name to save the log file
		// SCOPE: Log Tab.
		// DESCRIPTION: This function saves the log to a file.
		String^ fileName = saveLogFileDialog->FileName;
		array<String^>^ logArray = gcnew array<String^>(logBox->Lines->Length);

		for (int i = 0; i < logBox->Lines->Length; i++) {
			if (logBox->Lines[i] != "") {
				logArray[i] = logBox->Lines[i]->ToString();
			}
		}

		System::IO::File::WriteAllLines(fileName, logArray);

	}
	private: System::Void clearLogButton_Click(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user clicks the button to clear the log.
		// SCOPE: Log Tab.
		// DESCRIPTION: This function clears the log.
		logBox->Clear();
	}
	private: System::Void Form1_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
		// EVENT: When the user closes the form.
		// SCOPE: Form.
		// DESCRIPTION: Exits the main loop and frees Lodestone.
		mainLoopFlag = false;
		if (isConnected)
			BOOL BoolStatus = WinUsb_Free(LodestoneInterfaceHandle);
	}
	private: System::Int16 xboxCheckState(int currentState) {
		// SCOPE: All tabs.
		// DESCRIPTION: Checks to see if an Xbox controller is connected, and if the xbox tab is selected.  The function adjusts the control display accordingly.
		DWORD result;
		XINPUT_STATE state;
		int newState;

		//First Call
		if (currentState == 0) {
			label106->Visible = true;
			label107->Visible = true;
			label108->Visible = true;
			label109->Visible = true;
			label110->Visible = true;
			label111->Visible = true;
			label112->Visible = true;
			label113->Visible = true;
			label114->Visible = true;
			label115->Visible = true;
			label116->Visible = true;
			label117->Visible = true;
			lineShape42->Visible = true;
			lineShape43->Visible = true;
			lineShape44->Visible = true;
			lineShape45->Visible = true;
			lineShape46->Visible = true;
			xboxStatusPic->Visible = true;
			APic->Visible = true;
			BPic->Visible = true;
			XPic->Visible = true;
			YPic->Visible = true;
			RBPic->Visible = true;
			LBPic->Visible = true;
			RSPic->Visible = true;
			LSPic->Visible = true;
			RTPic->Visible = true;
			LTPic->Visible = true;
			StartPic->Visible = true;
			BackPic->Visible = true;
		}

		ZeroMemory(&state, sizeof(XINPUT_STATE));
		result = XInputGetState(0, &state);
		if (result == ERROR_SUCCESS) {
			if (xboxTab->Visible)		newState = 3;
			else                       newState = 2;
		}
		else                         newState = 1;

		if (newState == currentState)	return newState;
		switch (newState) {

			// No Xbox controller, Xbox tab not selected
		case 1:
			xboxStatusPic->Image = safe_cast<Image^>(resources->GetObject("xbox360logo gray"));
			APic->Image = safe_cast<Image^>(resources->GetObject("Abutton gray"));
			BPic->Image = safe_cast<Image^>(resources->GetObject("Bbutton gray"));
			XPic->Image = safe_cast<Image^>(resources->GetObject("Xbutton gray"));
			YPic->Image = safe_cast<Image^>(resources->GetObject("Ybutton gray"));
			RBPic->Image = safe_cast<Image^>(resources->GetObject("RBbutton"));
			LBPic->Image = safe_cast<Image^>(resources->GetObject("LBbutton"));
			RSPic->Image = safe_cast<Image^>(resources->GetObject("RSbutton"));
			LSPic->Image = safe_cast<Image^>(resources->GetObject("LSbutton"));
			RTPic->Image = safe_cast<Image^>(resources->GetObject("RT"));
			LTPic->Image = safe_cast<Image^>(resources->GetObject("LT"));
			StartPic->Image = safe_cast<Image^>(resources->GetObject("Startbutton"));
			BackPic->Image = safe_cast<Image^>(resources->GetObject("Backbutton"));
			label106->Enabled = false;
			label107->Enabled = false;
			label108->Enabled = false;
			label109->Enabled = false;
			label110->Enabled = false;
			label111->Enabled = false;
			label112->Enabled = false;
			label113->Enabled = false;
			label114->Enabled = false;
			label115->Enabled = false;
			label116->Enabled = false;
			label117->Enabled = false;
			lineShape42->BorderColor = System::Drawing::Color::Black;
			lineShape43->BorderColor = System::Drawing::Color::Black;
			lineShape44->BorderColor = System::Drawing::Color::Black;
			lineShape45->BorderColor = System::Drawing::Color::Black;
			lineShape46->BorderColor = System::Drawing::Color::Black;
			break;

			// Xbox controller present, Xbox tab not selected
		case 2:
			xboxStatusPic->Image = safe_cast<Image^>(resources->GetObject("xbox360logo"));
			APic->Image = safe_cast<Image^>(resources->GetObject("Abutton gray"));
			BPic->Image = safe_cast<Image^>(resources->GetObject("Bbutton gray"));
			XPic->Image = safe_cast<Image^>(resources->GetObject("Xbutton gray"));
			YPic->Image = safe_cast<Image^>(resources->GetObject("Ybutton gray"));
			RBPic->Image = safe_cast<Image^>(resources->GetObject("RBbutton"));
			LBPic->Image = safe_cast<Image^>(resources->GetObject("LBbutton"));
			RSPic->Image = safe_cast<Image^>(resources->GetObject("RSbutton"));
			LSPic->Image = safe_cast<Image^>(resources->GetObject("LSbutton"));
			RTPic->Image = safe_cast<Image^>(resources->GetObject("RT"));
			LTPic->Image = safe_cast<Image^>(resources->GetObject("LT"));
			StartPic->Image = safe_cast<Image^>(resources->GetObject("Startbutton"));
			BackPic->Image = safe_cast<Image^>(resources->GetObject("Backbutton"));
			label106->Enabled = false;
			label107->Enabled = false;
			label108->Enabled = false;
			label109->Enabled = false;
			label110->Enabled = false;
			label111->Enabled = false;
			label112->Enabled = false;
			label113->Enabled = false;
			label114->Enabled = false;
			label115->Enabled = false;
			label116->Enabled = true;
			label117->Enabled = false;
			label116->Text = "Xbox Tab";
			lineShape42->BorderColor = System::Drawing::Color::LimeGreen;
			lineShape43->BorderColor = System::Drawing::Color::LimeGreen;
			lineShape44->BorderColor = System::Drawing::Color::LimeGreen;
			lineShape45->BorderColor = System::Drawing::Color::LimeGreen;
			lineShape46->BorderColor = System::Drawing::Color::LimeGreen;
			break;

			// Xbox controller present, Xbox tab selected
		case 3:
			xboxStatusPic->Image = safe_cast<Image^>(resources->GetObject("xbox360logo"));
			APic->Image = safe_cast<Image^>(resources->GetObject("Abutton"));
			BPic->Image = safe_cast<Image^>(resources->GetObject("Bbutton"));
			XPic->Image = safe_cast<Image^>(resources->GetObject("Xbutton"));
			YPic->Image = safe_cast<Image^>(resources->GetObject("Ybutton"));
			RBPic->Image = safe_cast<Image^>(resources->GetObject("RBbutton"));
			LBPic->Image = safe_cast<Image^>(resources->GetObject("LBbutton"));
			RSPic->Image = safe_cast<Image^>(resources->GetObject("RSbutton"));
			LSPic->Image = safe_cast<Image^>(resources->GetObject("LSbutton"));
			RTPic->Image = safe_cast<Image^>(resources->GetObject("RT"));
			LTPic->Image = safe_cast<Image^>(resources->GetObject("LT"));
			StartPic->Image = safe_cast<Image^>(resources->GetObject("Startbutton"));
			BackPic->Image = safe_cast<Image^>(resources->GetObject("Backbutton"));
			label106->Enabled = true;
			label107->Enabled = true;
			label108->Enabled = true;
			label109->Enabled = true;
			label110->Enabled = true;
			label111->Enabled = true;
			label112->Enabled = true;
			label113->Enabled = true;
			label114->Enabled = true;
			label115->Enabled = true;
			label116->Enabled = true;
			label117->Enabled = true;
			label116->Text = "Zero";
			lineShape42->BorderColor = System::Drawing::Color::LimeGreen;
			lineShape43->BorderColor = System::Drawing::Color::LimeGreen;
			lineShape44->BorderColor = System::Drawing::Color::LimeGreen;
			lineShape45->BorderColor = System::Drawing::Color::LimeGreen;
			lineShape46->BorderColor = System::Drawing::Color::LimeGreen;
			break;
		}

		return newState;
	}
	private: System::Void checkForStartButton() {
		// SCOPE: All tabs.
		// DESCRIPTION: Checks to see if the user has pressed the start button to get to the xbox tab.
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		XInputGetState(0, &state);
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START)		tabControl1->SelectTab(6);
		return;
	}
	private: System::Void xboxControls() {
		// SCOPE: Xbox tab.
		// DESCRIPTION: Checks to see if any buttons on the xbox controller have been pressed, and controls the system accordingly.
		XINPUT_STATE state;
		System::EventArgs^ e;	//For calling events
		unsigned int buttonThreshold = 100;
		unsigned int bumperCooldown = 50;
		unsigned int triggerCooldown = 100;
		float L_Dead = (float) 0.25;
		float R_Dead = (float) 0.25;
		unsigned int TrigThresh = 200;
		float X = 0;
		float Y = 0;
		float Z = 0;
		double r2d = 180 / 3.14159265359;

		ZeroMemory(&state, sizeof(XINPUT_STATE));
		XInputGetState(0, &state);

		//GET JOYSTICK POSITIONS
		float normLX = ((float)state.Gamepad.sThumbLX) / 32767;
		float normLY = ((float)state.Gamepad.sThumbLY) / 32767;
		float normRX = ((float)state.Gamepad.sThumbRX) / 32767;
		float normRY = ((float)state.Gamepad.sThumbRY) / 32767;
		if (normLX < -1)	normLX = -1;
		if (normLY < -1)	normLY = -1;
		if (normRX < -1)	normRX = -1;
		if (normRY < -1)	normRY = -1;
		float LVectMag = sqrt(normLX*normLX + normLY * normLY);
		float RVectMag = sqrt(normRX*normRX + normRY * normRY);
		if (LVectMag < L_Dead) { LVectMag = 0;	normLX = 0; normLY = 0; }
		if (RVectMag < R_Dead) { RVectMag = 0;	normRX = 0; normRY = 0; }
		if (((LVectMag > 1) || (FIBox->Checked)) && (LVectMag != 0)) { normLX = normLX / LVectMag;  normLY = normLY / LVectMag;  LVectMag = 1; }
		if (((RVectMag > 1) || (FIBox->Checked)) && (RVectMag != 0)) { normRX = normRX / RVectMag;  normRY = normRY / RVectMag;  RVectMag = 1; }
		float Langle = (float)(atan(normLY / normLX)*r2d);
		if (normLX < 0)	Langle = Langle + 180;
		if (Langle < 0) Langle = Langle + 360;
		float Rangle = (float)(atan(normRY / normRX)*r2d);

		// D-PAD UP
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
			LVectMag = 1;
			normLX = 0;
			normLY = 1;
			Langle = 90;
		}

		// D-PAD LEFT
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
			LVectMag = 1;
			normLX = -1;
			normLY = 0;
			Langle = 180;
		}

		// D-PAD RIGHT
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
			LVectMag = 1;
			normLX = 1;
			normLY = 0;
			Langle = 0;
		}

		// D-PAD DOWN
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
			LVectMag = 1;
			normLX = 0;
			normLY = -1;
			Langle = 270;
		}

		// RB BUTTON (Hold)
		if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) && (RBcooldown == 0)) {
			HoldBox->Checked = !HoldBox->Checked;
			RBcooldown = bumperCooldown;
		}
		else if ((RBcooldown != 0) && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER))
			RBcooldown = 0;

		// LB BUTTON (Z Mode Toggle)
		if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) && (LBcooldown == 0)) {
			ZModeBox->Checked = !ZModeBox->Checked;
			if (ZModeBox->Checked) {
				label114->Text = "Toggle Z";
				label115->Text = "Toggle Z";
			}
			else {
				label114->Text = "-Z";
				label115->Text = "+Z";
			}
			LBcooldown = bumperCooldown;
		}
		else if ((LBcooldown != 0) && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER))
			LBcooldown = 0;

		// TRIGGERS (Z axis)
		if (ZModeBox->Checked) {
			if (((state.Gamepad.bRightTrigger > TrigThresh) || (state.Gamepad.bLeftTrigger)) && (TRIGcooldown == 0)) {
				if (precessing || rolling || diskHopping || QPStopButton->Enabled)		zeroButton_Click(zeroButton, e);
				xboxZDir = !xboxZDir;
				TRIGcooldown = 1;
			}
			else if ((TRIGcooldown != 0) && !((state.Gamepad.bRightTrigger > TrigThresh) || (state.Gamepad.bLeftTrigger)))
				TRIGcooldown = 0;
			if (xboxZDir)		Z = Convert::ToSingle(xboxOP);
			else               Z = Convert::ToSingle(xboxOP)*(-1);
		}
		else {
			if (state.Gamepad.bRightTrigger > TrigThresh) {
				if (precessing || rolling || diskHopping || QPStopButton->Enabled)		zeroButton_Click(zeroButton, e);
				Z = Convert::ToSingle(xboxOP);
			}
			if (state.Gamepad.bLeftTrigger > TrigThresh) {
				if (precessing || rolling || diskHopping || QPStopButton->Enabled)		zeroButton_Click(zeroButton, e);
				Z = Convert::ToSingle(xboxOP)*(-1);
			}
		}

		// START BUTTON (Zero)
		if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) && (Startcooldown == 0)) {
			Startcooldown = 1;
			zeroButton_Click(zeroButton, e);
		}
		else if (!(state.Gamepad.wButtons & XINPUT_GAMEPAD_START) && (Startcooldown != 0))
			Startcooldown = 0;

		// BACK BUTTON (Fix Inplane)
		if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) && (Backcooldown == 0)) {
			Backcooldown = 1;
			FIBox->Checked = !FIBox->Checked;
		}
		else if (!(state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) && (Backcooldown != 0))
			Backcooldown = 0;

		// Y BUTTON (Increase Outplane Field)
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) {
			Ycounter = Ycounter + 1;
			if (Ycounter >= buttonThreshold) {
				xboxOPBox->Text = (Convert::ToInt16(xboxOPBox->Text) + 1).ToString();
				setPlaneAndTheta(xboxOPBox, e);
				Ycounter = 0;
			}
		}
		else  Ycounter = 0;

		// A BUTTON (Decrease Outplane Field)
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			Acounter = Acounter + 1;
			if (Acounter >= buttonThreshold) {
				xboxOPBox->Text = (Convert::ToInt16(xboxOPBox->Text) - 1).ToString();
				setPlaneAndTheta(xboxOPBox, e);
				Acounter = 0;
			}
		}
		else  Acounter = 0;

		// B BUTTON (Increase Inplane Field)
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
			Bcounter = Bcounter + 1;
			if (Bcounter >= buttonThreshold) {
				xboxIPBox->Text = (Convert::ToInt16(xboxIPBox->Text) + 1).ToString();
				setPlaneAndTheta(xboxIPBox, e);
				Bcounter = 0;
			}
		}
		else  Bcounter = 0;

		// X BUTTON (Decrease Inplane Field)
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
			Xcounter = Xcounter + 1;
			if (Xcounter >= buttonThreshold) {
				xboxIPBox->Text = (Convert::ToInt16(xboxIPBox->Text) - 1).ToString();
				setPlaneAndTheta(xboxIPBox, e);
				Xcounter = 0;
			}
		}
		else  Xcounter = 0;

		// RIGHT STICK CLICK (Roll)
		if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) && (LVectMag != 0)) {
			rollIPBox->Text = xboxIPBox->Text;
			rollOPBox->Text = xboxOPBox->Text;
			rollAngleBox->Text = Decimal::Round(Convert::ToDecimal(Langle), 1).ToString();
			rollFreqBox->Text = xboxFreqBox->Text;
			rollPeriodBox->Text = xboxPeriodBox->Text;
			tabControl1->SelectTab(3);
			rollIP = Convert::ToDecimal(rollIPBox->Text);
			rollOP = Convert::ToDecimal(rollOPBox->Text);
			rollPeriod = Convert::ToDecimal(rollPeriodBox->Text);
			rollAngle = Convert::ToDouble(rollAngleBox->Text);
			rollButton_Click(rollButton, e);
			xboxState = xboxCheckState(xboxState);
			return;
		}

		// LEFT STICK CLICK (Precess)
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB && !precSuppCheck->Checked) {
			precIPBox->Text = xboxIPBox->Text;
			precOPBox->Text = xboxOPBox->Text;
			precThetaBox->Text = xboxThetaBox->Text;
			precFreqBox->Text = xboxFreqBox->Text;
			precPeriodBox->Text = xboxPeriodBox->Text;
			precCW->Checked = xboxCW->Checked;
			precCCW->Checked = xboxCCW->Checked;
			tabControl1->SelectTab(2);
			precIP = Convert::ToDecimal(precIPBox->Text);
			precOP = Convert::ToDecimal(precOPBox->Text);
			precPeriod = Convert::ToDecimal(precPeriodBox->Text);
			precessButton_Click(precessButton, e);
			xboxState = xboxCheckState(xboxState);
			return;
		}

		// Send Position
		if (((normLX != 0) || (normLY != 0)) && (!HoldBox->Checked)) {
			if (precessing || rolling || diskHopping || QPStopButton->Enabled)		zeroButton_Click(zeroButton, e);
			X = normLX * (Convert::ToSingle(xboxIP));
			Y = normLY * (Convert::ToSingle(xboxIP));
			SendPosition(X, Y, Z);
			xboxZeroLast = false;
		}
		else if (!HoldBox->Checked && (!xboxZeroLast || Z != currentZ) && !(precessing || rolling || diskHopping || QPStopButton->Enabled)) {
			SendPosition(0, 0, Z);
			xboxZeroLast = true;
		}
		else if (HoldBox->Checked && (Z != currentZ)) {
			SendPosition(currentX, currentY, Z);
		}
		return;
	}

	private: System::Void macroGrid_CellLeave(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
		// EVENT: When the user leaves a cell in the macro grid.
		// SCOPE: Macro tab.
		// DESCRIPTION: Properly initilizes a row if a new row type is selected.  Otherwise, it conditions cell input.
		int currRow = e->RowIndex;
		int currCol = e->ColumnIndex;
		DataGridViewComboBoxCell^ contents = dynamic_cast<DataGridViewComboBoxCell^>(macroGrid->Rows[currRow]->Cells[0]);
		String^ rowType = Convert::ToString(contents->EditedFormattedValue);

		switch (currCol) {
		case 1:
			if (rowType == "Wait") {
				if (!checkValidText1(dynamic_cast<String^>(macroGrid->Rows[currRow]->Cells[currCol]->Value)))
					macroGrid->Rows[currRow]->Cells[currCol]->Value = "1";
			}
			else if (rowType == "Disk Hop") {
				if (!checkValidText(dynamic_cast<String^>(macroGrid->Rows[currRow]->Cells[currCol]->Value)))
					macroGrid->Rows[currRow]->Cells[currCol]->Value = "0";
			}
			break;

		case 2:
			if ((rowType == "Precess") || (rowType == "Roll")) {
				if (!checkValidText(dynamic_cast<String^>(macroGrid->Rows[currRow]->Cells[currCol]->Value)))
					macroGrid->Rows[currRow]->Cells[currCol]->Value = "0";
			}
			break;

		case 3:
			if ((rowType == "Precess") || (rowType == "Roll") || (rowType == "Disk Hop")) {
				if (!checkValidText1(dynamic_cast<String^>(macroGrid->Rows[currRow]->Cells[currCol]->Value)))
					macroGrid->Rows[currRow]->Cells[currCol]->Value = "1";
			}
			break;

		case 5:
			if ((rowType == "Precess") || (rowType == "Roll") || (rowType == "Disk Hop")) {
				if (!checkValidText(dynamic_cast<String^>(macroGrid->Rows[currRow]->Cells[currCol]->Value)))
					macroGrid->Rows[currRow]->Cells[currCol]->Value = "0";
			}
			break;

		case 6:
			if ((rowType == "Precess") || (rowType == "Roll") || (rowType == "Disk Hop")) {
				if (!checkValidText(dynamic_cast<String^>(macroGrid->Rows[currRow]->Cells[currCol]->Value)))
					macroGrid->Rows[currRow]->Cells[currCol]->Value = "0";
			}
			break;

		case 7:
			if ((rowType == "Precess") || (rowType == "Roll") || (rowType == "Disk Hop")) {
				if (!checkValidText1(dynamic_cast<String^>(macroGrid->Rows[currRow]->Cells[currCol]->Value)))
					macroGrid->Rows[currRow]->Cells[currCol]->Value = "1";
			}
			break;

		default:
			break;
		}
	}
	private: System::Void runMacroButton_Click(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user clicks runMacroButton.
		// SCOPE: Macro Tab.
		// DESCRIPTION: Starts/Stops current macro.
		if (!macroing) {
			validateMacroRows();
			if (macroGrid->Rows->Count == 1)	return;
			macroing = 1;
			runMacroButton->Text = "Stop";
			macroGrid->ClearSelection();
			macroGrid->Enabled = false;
			DataGridViewComboBoxCell^ contents = dynamic_cast<DataGridViewComboBoxCell^>(macroGrid->Rows[0]->Cells[0]);
			currMacroCommand = Convert::ToString(contents->EditedFormattedValue);
			currMacroRow = 0;

			SYSTEMTIME currTime;
			GetLocalTime(&currTime);
			macroStartMin = currTime.wMinute;
			macroStartS = currTime.wSecond;
			macroStartMS = currTime.wMilliseconds;

			startMacroTask();
		}
		else {
			zeroButton_Click(zeroButton, e);
		}
	}
	private: System::Void validateMacroRows() {
		// SCOPE: Macro Tab.
		// DESCRIPTION: Removes any empty rows.
		int numberOfRows = macroGrid->Rows->Count - 1;
		DataGridViewComboBoxCell^ contents;
		String^ rowType = "";
		for (int i = 0; i < numberOfRows; i++) {
			DataGridViewComboBoxCell^ contents = dynamic_cast<DataGridViewComboBoxCell^>(macroGrid->Rows[i]->Cells[0]);
			if (Convert::ToString(contents->EditedFormattedValue) == "") {
				macroGrid->Rows->RemoveAt(i);
				i--;
				numberOfRows--;
			}
		}
		return;
	}
	private: System::Boolean isMacroCommandOver() {
		// SCOPE: All tabs.
		// DESCRIPTION: Checks to see if current macro row is complete, and if so, it will call the startMacroTask routine for the new task.
		SYSTEMTIME currTime;
		GetLocalTime(&currTime);

		System::EventArgs^ e;

		if (((currTime.wMinute == macroEndMin) &&
			(currTime.wSecond == macroEndS) &&
			(currTime.wMilliseconds >= macroEndMS)) ||
			(currTime.wMinute == (macroEndMin + 1)) ||
			((currTime.wMinute == macroEndMin) && (currTime.wSecond > macroEndS))) {

			zeroButton_Click_Macro(zeroButton, e);

			macroStartMin = macroEndMin;
			macroStartS = macroEndS;
			macroStartMS = macroEndMS;

			if (currMacroRow == (macroGrid->Rows->Count - 2)) {
				if (loopCheckBox->Checked) {
					currMacroRow = 0;
				}
				else {
					zeroButton_Click(zeroButton, e);
					return true;
				}
			}
			else currMacroRow++;

			DataGridViewComboBoxCell^ contents = dynamic_cast<DataGridViewComboBoxCell^>(macroGrid->Rows[currMacroRow]->Cells[0]);
			currMacroCommand = Convert::ToString(contents->EditedFormattedValue);
			startMacroTask();
			return true;
		}
		else return false;
	}
	private: System::Void startMacroTask() {
		// SCOPE: All tabs.
		// DESCRIPTION: Starts a macro command.
		String^ rowDuration = "";
		String^ rowAngle = "";
		String^ rowRevolutions = "";
		Boolean rowCCW = false;
		String^ rowInplane = "";
		String^ rowOutplane = "";
		String^ rowFrequency = "";
		String^ direction = "";
		if (!macroGrid->Rows[currMacroRow]->Cells[1]->ReadOnly)
			rowDuration = macroGrid->Rows[currMacroRow]->Cells[1]->Value->ToString();
		if (!macroGrid->Rows[currMacroRow]->Cells[2]->ReadOnly)
			rowAngle = macroGrid->Rows[currMacroRow]->Cells[2]->Value->ToString();
		if (!macroGrid->Rows[currMacroRow]->Cells[3]->ReadOnly)
			rowRevolutions = macroGrid->Rows[currMacroRow]->Cells[3]->Value->ToString();
		if (!macroGrid->Rows[currMacroRow]->Cells[4]->ReadOnly)
			rowCCW = Convert::ToBoolean(macroGrid->Rows[currMacroRow]->Cells[4]->Value);
		if (!macroGrid->Rows[currMacroRow]->Cells[5]->ReadOnly)
			rowInplane = macroGrid->Rows[currMacroRow]->Cells[5]->Value->ToString();
		if (!macroGrid->Rows[currMacroRow]->Cells[6]->ReadOnly)
			rowOutplane = macroGrid->Rows[currMacroRow]->Cells[6]->Value->ToString();
		if (!macroGrid->Rows[currMacroRow]->Cells[7]->ReadOnly)
			rowFrequency = macroGrid->Rows[currMacroRow]->Cells[7]->Value->ToString();
		if (!macroGrid->Rows[currMacroRow]->Cells[8]->ReadOnly) {
			DataGridViewComboBoxCell^ contents = dynamic_cast<DataGridViewComboBoxCell^>(macroGrid->Rows[currMacroRow]->Cells[8]);
			direction = Convert::ToString(contents->EditedFormattedValue);
		}

		if (currMacroCommand == "Wait") {
			Single macroX = Convert::ToSingle(rowInplane)*((Single)(cos(Convert::ToSingle(rowAngle)*d2r)));
			Single macroY = Convert::ToSingle(rowInplane)*((Single)(sin(Convert::ToSingle(rowAngle)*d2r)));
			Single macroZ = Convert::ToSingle(rowOutplane);
			SendPosition(macroX, macroY, macroZ);
			macroEndMin = macroStartMin;
			macroEndS = macroStartS;
			macroEndMS = macroStartMS + 1000 * Convert::ToDouble(rowDuration);
			while (macroEndMS >= 1000) { macroEndMS = macroEndMS - 1000;		macroEndS = macroEndS + 1; }
			while (macroEndS >= 60) { macroEndS = macroEndS - 60;			macroEndMin = macroEndMin + 1; }
			while (macroEndMin >= 60) { macroEndMin = macroEndMin - 60; }
		}
		else if (currMacroCommand == "Precess") {
			EventArgs^ e;
			precIPBox->Text = rowInplane;
			setPlaneAndTheta(precIPBox, e);
			precOPBox->Text = rowOutplane;
			setPlaneAndTheta(precOPBox, e);
			precFreqBox->Text = rowFrequency;
			setPeriodAndFrequency(precFreqBox, e);

			precCW->Checked = !rowCCW;
			precCCW->Checked = rowCCW;

			macroEndMin = macroStartMin;
			macroEndS = macroStartS;
			macroEndMS = macroStartMS + 1000 * Convert::ToDouble(rowRevolutions) / Convert::ToDouble(rowFrequency);
			while (macroEndMS >= 1000) { macroEndMS = macroEndMS - 1000;		macroEndS = macroEndS + 1; }
			while (macroEndS >= 60) { macroEndS = macroEndS - 60;			macroEndMin = macroEndMin + 1; }
			while (macroEndMin >= 60) { macroEndMin = macroEndMin - 60; }

			precessButton_Click(precessButton, e);
			if (!rowCCW)		setStartAngle(Convert::ToDouble(rowAngle), Convert::ToDouble(precPeriod));
			if (rowCCW)			setStartAngle(Convert::ToDouble(rowAngle)*(-1), Convert::ToDouble(precPeriod));
		}
		else if (currMacroCommand == "Roll") {
			EventArgs^ e;
			rollIPBox->Text = rowInplane;
			rollInfoChange(rollIPBox, e);
			rollOPBox->Text = rowOutplane;
			rollInfoChange(rollOPBox, e);
			rollAngleBox->Text = rowAngle;
			rollInfoChange(rollAngleBox, e);
			setPlaneAndTheta(precOPBox, e);
			rollFreqBox->Text = rowFrequency;
			setPeriodAndFrequency(rollFreqBox, e);

			macroEndMin = macroStartMin;
			macroEndS = macroStartS;
			macroEndMS = macroStartMS + 1000 * Convert::ToDouble(rowRevolutions) / Convert::ToDouble(rowFrequency);
			while (macroEndMS >= 1000) { macroEndMS = macroEndMS - 1000;		macroEndS = macroEndS + 1; }
			while (macroEndS >= 60) { macroEndS = macroEndS - 60;			macroEndMin = macroEndMin + 1; }
			while (macroEndMin >= 60) { macroEndMin = macroEndMin - 60; }

			rollButton_Click(rollButton, e);
		}
		else if (currMacroCommand == "Disk Hop") {
			EventArgs^ e;
			MouseEventArgs^ ee;
			diskIPBox->Text = rowInplane;
			setPlaneAndTheta(diskIPBox, e);
			diskOPBox->Text = rowOutplane;
			setPlaneAndTheta(diskOPBox, e);
			diskFreqBox->Text = rowFrequency;
			setPeriodAndFrequency(diskFreqBox, e);
			hopBox->Text = rowDuration;
			hopBox_Leave(hopBox, e);


			diskCW->Checked = !rowCCW;
			diskCCW->Checked = rowCCW;

			macroEndMin = macroStartMin;
			macroEndS = macroStartS;
			macroEndMS = macroStartMS + 1000 * Convert::ToDouble(rowRevolutions)*(Convert::ToDouble(rowDuration) + 1 / (2 * Convert::ToDouble(rowFrequency)));
			while (macroEndMS >= 1000) { macroEndMS = macroEndMS - 1000;		macroEndS = macroEndS + 1; }
			while (macroEndS >= 60) { macroEndS = macroEndS - 60;			macroEndMin = macroEndMin + 1; }
			while (macroEndMin >= 60) { macroEndMin = macroEndMin - 60; }

			if (direction == "Up") {
				directionButtonClick(upButton, ee);
			}
			else if (direction == "Down") {
				directionButtonClick(downButton, ee);
			}
			else if (direction == "Left") {
				directionButtonClick(leftButton, ee);
			}
			else if (direction == "Right") {
				directionButtonClick(rightButton, ee);
			}
		}

	}
	private: System::Void macroGrid_KeyPress(System::Object^  sender, KeyPressEventArgs^  e) {
		// SCOPE: Macro Tab.
		// DESCRIPTION: Will not allow invalid charecters to be typed into the current macro cell.
		DataGridViewTextBoxEditingControl^ thisTextBox;
		int currRow = macroGrid->CurrentCell->RowIndex;
		int currCol = macroGrid->CurrentCell->ColumnIndex;

		DataGridViewComboBoxCell^ contents = dynamic_cast<DataGridViewComboBoxCell^>(macroGrid->Rows[currRow]->Cells[0]);
		String^ rowType = Convert::ToString(contents->EditedFormattedValue);

		if (rowType == "")							return;
		if (macroGrid->CurrentCell->ReadOnly)						return;
		if ((currCol == 0) || (currCol == 4) || (currCol == 8))		return;

		thisTextBox = safe_cast<DataGridViewTextBoxEditingControl^>(sender);

		String^ validCharSet1 = "0123456789.\b";
		String^ validCharSet2 = "0123456789.-\b";
		String^ validCharSet3 = "0123456789\b";
		String^ validCharSet4 = "0123456789-\b";
		String^ inputChar = e->KeyChar.ToString();
		String^ boxContents = thisTextBox->Text;

		if (((rowType == "Precess") &&
			((currCol == 3) ||
			(currCol == 7))) ||
				((rowType == "Roll") &&
			((currCol == 3) ||
					(currCol == 7))) ||
				((rowType == "Wait") &&
			(currCol == 1)) ||
					((rowType == "Disk Hop") &&
			((currCol == 1) ||
						(currCol == 7)))) {
			if (!(validCharSet1->Contains(inputChar)))
				e->KeyChar = '\0';
			if (boxContents->Contains(".") && (e->KeyChar.Equals('.')) && (thisTextBox->SelectionStart != 0))
				e->KeyChar = '\0';
		}
		else
			if (((rowType == "Precess") &&
				(currCol == 2)) ||
				((rowType == "Roll") &&
				(currCol == 2)) ||
					((rowType == "Wait") &&
				(currCol == 2))) {
				if (!(validCharSet2->Contains(inputChar)))
					e->KeyChar = '\0';
				if (boxContents->Contains(".") && (e->KeyChar.Equals('.')))
					e->KeyChar = '\0';
				if ((boxContents->Contains("-") || (boxContents->Length != 0)) && (e->KeyChar.Equals('-') && (thisTextBox->SelectionStart != 0)))
					e->KeyChar = '\0';
			}
			else
				if (((rowType == "Precess") &&
					(currCol == 5)) ||
					((rowType == "Roll") &&
					(currCol == 5)) ||
						((rowType == "Wait") &&
					(currCol == 5)) ||
							((rowType == "Disk Hop") &&
					((currCol == 3) ||
								(currCol == 5)))) {
					if (!(validCharSet3->Contains(inputChar)))
						e->KeyChar = '\0';
				}
				else
					if (((rowType == "Precess") &&
						(currCol == 6)) ||
						((rowType == "Roll") &&
						(currCol == 6)) ||
							((rowType == "Wait") &&
						(currCol == 6)) ||
								((rowType == "Disk Hop") &&
						(currCol == 6))) {
						if (!(validCharSet4->Contains(inputChar)))
							e->KeyChar = '\0';
						if ((boxContents->Contains("-") || (boxContents->Length != 0)) && (e->KeyChar.Equals('-') && (thisTextBox->SelectionStart != 0)))
							e->KeyChar = '\0';
					}
	}
	private: System::Void macroGrid_EditingControlShowing(System::Object^  sender, System::Windows::Forms::DataGridViewEditingControlShowingEventArgs^  e) {
		// EVENT: When the user presses a key in a macro grid cell.
		// SCOPE: Macro Tab.
		// DESCRIPTION: calls the macroGrid_keyPress function.
		e->Control->KeyPress += gcnew KeyPressEventHandler(this, &Form1::macroGrid_KeyPress);
	}
	private: System::Void setStartAngle(double startAngle, double period) {
		// SCOPE: All Tabs.
		// DESCRIPTION: This function sets the start angle for a macro precession command.
		double tempMin = startMin;
		double tempS = startS;
		double tempMS = startMS;

		while (startAngle <= -360)	startAngle = startAngle + 360;
		while (startAngle > 0)		startAngle = startAngle - 360;

		tempMS = tempMS + period * 1000 * (startAngle / 360);
		while (tempMS < 0) { tempMS = tempMS + 1000;	tempS = tempS - 1; }
		while (tempS < 0) { tempS = tempS + 60;		tempMin = tempMin - 1; }

		startMin = tempMin;
		startS = tempS;
		startMS = tempMS;

		return;
	}
	private: System::Void saveMacro() {
		// SCOPE: Macro Tab.
		// DESCRIPTION: Saves the macro to a file.
		String^ file = saveMacroFileDialog->FileName;
		BinaryWriter^ bw = gcnew BinaryWriter(File::Open(file, FileMode::Create));
		bw->Write(macroNameBox->Text);
		bw->Write(macroGrid->ColumnCount);
		bw->Write(macroGrid->RowCount);
		for each (DataGridViewRow^ macroRow in macroGrid->Rows)
		{
			for (int i = 0; i < macroGrid->ColumnCount; i++) {
				Object^ val = macroRow->Cells[i]->Value;
				if (val == nullptr) {
					bw->Write(false);
					bw->Write(false);
				}
				else {
					bw->Write(true);
					bw->Write(val->ToString());
				}
			}
		}
		macroSaveFlag = false;
	}
	private: System::Void loadMacro() {
		// SCOPE: Macro Tab.
		// DESCRIPTION: Loads a macro from a file.
		System::Windows::Forms::DataGridViewCellEventArgs^  e;
		macroGrid->Rows->Clear();
		macroGrid->CurrentCell = macroGrid->Rows[0]->Cells[1];
		String^ file = loadMacroFileDialog->FileName;
		BinaryReader^ br = gcnew BinaryReader(File::Open(file, FileMode::Open));
		macroNameBox->Text = br->ReadString();
		int cols = br->ReadInt32();
		int rows = br->ReadInt32();
		for (int i = 0; i < rows - 1; i++) {
			macroGrid->Rows->Add();
			for (int j = 0; j < cols; j++) {
				if (br->ReadBoolean()) {
					macroGrid->Rows[i]->Cells[j]->Value = br->ReadString();
					if (j == 0) {
						e = gcnew DataGridViewCellEventArgs(0, i);
						macroGrid_CellLeave(macroGrid, e);
					}
					else if (j == cols - 1)
						macroGrid->CurrentCell = macroGrid->Rows[i]->Cells[1];
				}
				else br->ReadBoolean();
			}
		}
		macroGrid->ClearSelection();
		macroLoadFlag = false;
	}
	private: System::Void saveMacroButton_Click(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user clicks the button to save the macro.
		// SCOPE: Macro Tab.
		// DESCRIPTION: Shows the dialog to save the macro to a file.
		saveMacroFileDialog->FileName = macroNameBox->Text;
		saveMacroFileDialog->ShowDialog();
	}
	private: System::Void loadMacroButton_Click(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user clicks the button to load a macro.
		// SCOPE: Macro Tab.
		// DESCRIPTION: Shows the dialog to load a macro from a file.
		loadMacroFileDialog->ShowDialog();
	}
	private: System::Void saveMacroFileDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
		// EVENT: When the user enters a valid name to save the macro file.
		// SCOPE: Macro Tab.
		// DESCRIPTION: Sets flag to save the macro.
		macroSaveFlag = true;
	}
	private: System::Void loadMacroFileDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
		// EVENT: When the user enters a valid name to load a macro file.
		// SCOPE: Macro Tab.
		// DESCRIPTION: Sets flag to load a macro.
		macroLoadFlag = true;
	}
	private: System::Void precSuppCheckChange(System::Object^ sender, System::EventArgs^ e) {
		if (precSuppCheck->Checked)		label110->Text = "N/A";
		else                           label110->Text = "Precess";
	}
	private: System::Void displayVoltage() {//Microsoft::VisualBasic::PowerPacks::RectangleShape^ foreground){
		int watch = VM_back->Width;
		int watchB;
		VM_fore->Width = watch / 2 - 1;
		VM_fore->Height = VM_back->Height - 1;
		watchB = VM_fore->Width;
		watchB++;
		//VM_fore->Width = watchB;
	}
	private: System::Void saveConfig() {
		// SCOPE: Setup Tab.
		// DESCRIPTION: Saves the configuration to a file.
		String^ file = saveConfigFileDialog->FileName;
		BinaryWriter^ bw = gcnew BinaryWriter(File::Open(file, FileMode::Create));
		bw->Write(configNameBox->Text);
		bw->Write(textBoxIPF->Text);
		bw->Write(textBoxOPF->Text);
		bw->Write(textBoxRX->Text);
		bw->Write(textBoxRY->Text);
		bw->Write(textBoxRZ->Text);
		bw->Write(textBoxOFX->Text);
		bw->Write(textBoxORX->Text);
		bw->Write(textBoxOFY->Text);
		bw->Write(textBoxORY->Text);
		bw->Write(textBoxOFZ->Text);
		bw->Write(textBoxORZ->Text);
		bw->Write(textBoxOAX->Text);
		bw->Write(textBoxOAY->Text);
		bw->Write(textBoxOAZ->Text);
		configSaveFlag = false;
	}
	private: System::Void loadConfig() {
		// DESCRIPTION: Loads a configuration from a file.
		String^ file = loadConfigFileDialog->FileName;
		BinaryReader^ br = gcnew BinaryReader(File::Open(file, FileMode::Open));
		configNameBox->Text = br->ReadString();
		textBoxIPF->Text = br->ReadString();
		textBoxOPF->Text = br->ReadString();
		textBoxRX->Text = br->ReadString();
		textBoxRY->Text = br->ReadString();
		textBoxRZ->Text = br->ReadString();
		textBoxOFX->Text = br->ReadString();
		textBoxORX->Text = br->ReadString();
		textBoxOFY->Text = br->ReadString();
		textBoxORY->Text = br->ReadString();
		textBoxOFZ->Text = br->ReadString();
		textBoxORZ->Text = br->ReadString();
		textBoxOAX->Text = br->ReadString();
		textBoxOAY->Text = br->ReadString();
		textBoxOAZ->Text = br->ReadString();
		sendConfig();
		configLoadFlag = false;
	}
	private: System::Void saveConfigButton_Click(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user clicks the button to save the config file.
		// SCOPE: Setup Tab.
		// DESCRIPTION: Shows the dialog to save the configuration to a file.
		saveConfigFileDialog->FileName = configNameBox->Text;
		saveConfigFileDialog->ShowDialog();
	}
	private: System::Void loadConfigButton_Click(System::Object^  sender, System::EventArgs^  e) {
		// EVENT: When the user clicks the button to load a config file.
		// SCOPE: Setup Tab.
		// DESCRIPTION: Shows the dialog to load a configuration from a file.
		loadConfigFileDialog->ShowDialog();
	}
	private: System::Void saveConfigFileDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
		// EVENT: When the user enters a valid name to save the config file.
		// SCOPE: Setup Tab.
		// DESCRIPTION: Sets flag to save the configuration.
		configSaveFlag = true;
	}
	private: System::Void loadConfigFileDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
		// EVENT: When the user enters a valid name to load a config file.
		// SCOPE: Setup Tab.
		// DESCRIPTION: Sets flag to load a configuration.
		configLoadFlag = true;
	}
	private: System::Void macroGrid_CellValueChanged(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
		int currRow = e->RowIndex;
		int currCol = e->ColumnIndex;
		if (currRow == -1)	return;
		DataGridViewComboBoxCell^ contents = dynamic_cast<DataGridViewComboBoxCell^>(macroGrid->Rows[currRow]->Cells[0]);
		String^ rowType = Convert::ToString(contents->EditedFormattedValue);

		if (currCol == 0) {
			if (rowType == "Precess") {
				macroGrid->Rows[currRow]->Cells[1]->Value = "";
				macroGrid->Rows[currRow]->Cells[2]->Value = "0";
				macroGrid->Rows[currRow]->Cells[3]->Value = "1";
				macroGrid->Rows[currRow]->Cells[4]->Value = false;
				macroGrid->Rows[currRow]->Cells[5]->Value = "0";
				macroGrid->Rows[currRow]->Cells[6]->Value = "0";
				macroGrid->Rows[currRow]->Cells[7]->Value = "1";
				macroGrid->Rows[currRow]->Cells[8]->Value = "";
				macroGrid->Rows[currRow]->Cells[1]->ReadOnly = true;
				macroGrid->Rows[currRow]->Cells[2]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[3]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[4]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[5]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[6]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[7]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[8]->ReadOnly = true;
				macroGrid->Rows[currRow]->Cells[1]->Style->BackColor = System::Drawing::Color::Gray;
				macroGrid->Rows[currRow]->Cells[2]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[3]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[4]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[5]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[6]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[7]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[8]->Style->BackColor = System::Drawing::Color::Gray;
				macroGrid->Rows[currRow]->Cells[1]->ToolTipText = "";
				macroGrid->Rows[currRow]->Cells[2]->ToolTipText = "Starting angle of precession (degrees)";
				macroGrid->Rows[currRow]->Cells[3]->ToolTipText = "Number of revolutions";
				macroGrid->Rows[currRow]->Cells[4]->ToolTipText = "Check for Counterclockwise precession";
				macroGrid->Rows[currRow]->Cells[5]->ToolTipText = "Inplane Strength of precession (Oe)";
				macroGrid->Rows[currRow]->Cells[6]->ToolTipText = "Outplane Strength of precession (Oe)";
				macroGrid->Rows[currRow]->Cells[7]->ToolTipText = "Precession Frequency (Hz)";
				macroGrid->Rows[currRow]->Cells[8]->ToolTipText = "";
			}
			else if (rowType == "Roll") {
				macroGrid->Rows[currRow]->Cells[1]->Value = "";
				macroGrid->Rows[currRow]->Cells[2]->Value = "0";
				macroGrid->Rows[currRow]->Cells[3]->Value = "1";
				macroGrid->Rows[currRow]->Cells[4]->Value = false;
				macroGrid->Rows[currRow]->Cells[5]->Value = "0";
				macroGrid->Rows[currRow]->Cells[6]->Value = "0";
				macroGrid->Rows[currRow]->Cells[7]->Value = "1";
				macroGrid->Rows[currRow]->Cells[8]->Value = "";
				macroGrid->Rows[currRow]->Cells[1]->ReadOnly = true;
				macroGrid->Rows[currRow]->Cells[2]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[3]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[4]->ReadOnly = true;
				macroGrid->Rows[currRow]->Cells[5]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[6]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[7]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[8]->ReadOnly = true;
				macroGrid->Rows[currRow]->Cells[1]->Style->BackColor = System::Drawing::Color::Gray;
				macroGrid->Rows[currRow]->Cells[2]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[3]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[4]->Style->BackColor = System::Drawing::Color::Gray;
				macroGrid->Rows[currRow]->Cells[5]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[6]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[7]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[8]->Style->BackColor = System::Drawing::Color::Gray;
				macroGrid->Rows[currRow]->Cells[1]->ToolTipText = "";
				macroGrid->Rows[currRow]->Cells[2]->ToolTipText = "Angle of roll (degrees)";
				macroGrid->Rows[currRow]->Cells[3]->ToolTipText = "Number of revolutions";
				macroGrid->Rows[currRow]->Cells[4]->ToolTipText = "";
				macroGrid->Rows[currRow]->Cells[5]->ToolTipText = "Inplane Strength of roll (Oe)";
				macroGrid->Rows[currRow]->Cells[6]->ToolTipText = "Outplane Strength of roll (Oe)";
				macroGrid->Rows[currRow]->Cells[7]->ToolTipText = "Roll Frequency (Hz)";
				macroGrid->Rows[currRow]->Cells[8]->ToolTipText = "";
			}
			else if (rowType == "Wait") {
				macroGrid->Rows[currRow]->Cells[1]->Value = "1";
				macroGrid->Rows[currRow]->Cells[2]->Value = "0";
				macroGrid->Rows[currRow]->Cells[3]->Value = "";
				macroGrid->Rows[currRow]->Cells[4]->Value = false;
				macroGrid->Rows[currRow]->Cells[5]->Value = "0";
				macroGrid->Rows[currRow]->Cells[6]->Value = "0";
				macroGrid->Rows[currRow]->Cells[7]->Value = "";
				macroGrid->Rows[currRow]->Cells[8]->Value = "";
				macroGrid->Rows[currRow]->Cells[1]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[2]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[3]->ReadOnly = true;
				macroGrid->Rows[currRow]->Cells[4]->ReadOnly = true;
				macroGrid->Rows[currRow]->Cells[5]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[6]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[7]->ReadOnly = true;
				macroGrid->Rows[currRow]->Cells[8]->ReadOnly = true;
				macroGrid->Rows[currRow]->Cells[1]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[2]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[3]->Style->BackColor = System::Drawing::Color::Gray;
				macroGrid->Rows[currRow]->Cells[4]->Style->BackColor = System::Drawing::Color::Gray;
				macroGrid->Rows[currRow]->Cells[5]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[6]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[7]->Style->BackColor = System::Drawing::Color::Gray;
				macroGrid->Rows[currRow]->Cells[8]->Style->BackColor = System::Drawing::Color::Gray;
				macroGrid->Rows[currRow]->Cells[1]->ToolTipText = "Duration of held position (s)";
				macroGrid->Rows[currRow]->Cells[2]->ToolTipText = "Inplane Angle of held position (degrees)";
				macroGrid->Rows[currRow]->Cells[3]->ToolTipText = "";
				macroGrid->Rows[currRow]->Cells[4]->ToolTipText = "";
				macroGrid->Rows[currRow]->Cells[5]->ToolTipText = "Inplane Strength of held position (Oe)";
				macroGrid->Rows[currRow]->Cells[6]->ToolTipText = "Outplane Strength of held position (Oe)";
				macroGrid->Rows[currRow]->Cells[7]->ToolTipText = "";
				macroGrid->Rows[currRow]->Cells[8]->ToolTipText = "";
			}
			else if (rowType == "Disk Hop") {
				macroGrid->Rows[currRow]->Cells[1]->Value = "0.1";
				macroGrid->Rows[currRow]->Cells[2]->Value = "";
				macroGrid->Rows[currRow]->Cells[3]->Value = "1";
				macroGrid->Rows[currRow]->Cells[4]->Value = false;
				macroGrid->Rows[currRow]->Cells[5]->Value = "0";
				macroGrid->Rows[currRow]->Cells[6]->Value = "0";
				macroGrid->Rows[currRow]->Cells[7]->Value = "1";
				macroGrid->Rows[currRow]->Cells[8]->Value = "Up";
				macroGrid->Rows[currRow]->Cells[1]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[2]->ReadOnly = true;
				macroGrid->Rows[currRow]->Cells[3]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[4]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[5]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[6]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[7]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[8]->ReadOnly = false;
				macroGrid->Rows[currRow]->Cells[1]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[2]->Style->BackColor = System::Drawing::Color::Gray;
				macroGrid->Rows[currRow]->Cells[3]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[4]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[5]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[6]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[7]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[8]->Style->BackColor = macroGrid->DefaultCellStyle->BackColor;
				macroGrid->Rows[currRow]->Cells[1]->ToolTipText = "Hop Time (s)";
				macroGrid->Rows[currRow]->Cells[2]->ToolTipText = "";
				macroGrid->Rows[currRow]->Cells[3]->ToolTipText = "Number of complete hops";
				macroGrid->Rows[currRow]->Cells[4]->ToolTipText = "Check for Counterclockwise";
				macroGrid->Rows[currRow]->Cells[5]->ToolTipText = "Inplane Strength of held position (Oe)";
				macroGrid->Rows[currRow]->Cells[6]->ToolTipText = "Outplane Strength of held position (Oe)";
				macroGrid->Rows[currRow]->Cells[7]->ToolTipText = "Precession Frequency (Hz)";
				macroGrid->Rows[currRow]->Cells[8]->ToolTipText = "";
			}
		}
		return;
	}

	};
}