#include <iostream>
#include <Windows.h>
#include <psapi.h>
#include <TlHelp32.h>
#include <inttypes.h>

using namespace std;

int GetProcessID(const wchar_t* ExeName)
{
	PROCESSENTRY32 ProcEntry = { 0 };
	HANDLE SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (!SnapShot)
		return NULL;

	ProcEntry.dwSize = sizeof(ProcEntry);

	if (!Process32First(SnapShot, &ProcEntry))
		return NULL;

	do
	{
		if (!wcscmp(ProcEntry.szExeFile, ExeName))
		{
			CloseHandle(SnapShot);
			return ProcEntry.th32ProcessID;
		}
	} while (Process32Next(SnapShot, &ProcEntry));

	CloseHandle(SnapShot);
	return NULL;
}

DWORD_PTR GetProcessBaseAddress(int processID)
{
	DWORD_PTR   baseAddress = 0;
	HANDLE      processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	HMODULE     *moduleArray;
	LPBYTE      moduleArrayBytes;
	DWORD       bytesRequired;

	if (processHandle)
	{
		if (EnumProcessModules(processHandle, NULL, 0, &bytesRequired))
		{
			if (bytesRequired)
			{
				moduleArrayBytes = (LPBYTE)LocalAlloc(LPTR, bytesRequired);

				if (moduleArrayBytes)
				{
					unsigned int moduleCount;

					moduleCount = bytesRequired / sizeof(HMODULE);
					moduleArray = (HMODULE *)moduleArrayBytes;

					if (EnumProcessModules(processHandle, moduleArray, bytesRequired, &bytesRequired))
					{
						baseAddress = (DWORD_PTR)moduleArray[0];
					}

					LocalFree(moduleArrayBytes);
				}
			}
		}

		CloseHandle(processHandle);
	}

	return baseAddress;
}
int main()
{
	int32_t ammo = 9999;
	int32_t health = 9999;
	int32_t timer = 0;


	HWND ac = FindWindowA(NULL, "AssaultCube");
	if (ac == NULL)
	{
		cout << "AssaultCube Not Found." << endl;
		Sleep(3000);
		exit(-1);
	}
	else
	{
		DWORD procID;
		GetWindowThreadProcessId(ac, &procID);
		int pid = GetProcessID(L"ac_client.exe");
		HANDLE achandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

		if (procID = NULL)
		{
			cout << "Cannot Obtain Process." << endl;
			Sleep(3000);
			exit(-1);
		}
		else
		{
			uint8_t BAddr[4];
			DWORD_PTR LocalPlayer;
			DWORD_PTR Base = GetProcessBaseAddress(pid);
			printf("[+] Base Address: 0x%" PRIx32 "\n", Base);
			ReadProcessMemory(achandle, (PBYTE*)(Base + 0x109B74), &LocalPlayer, sizeof(LocalPlayer), 0);
			printf("[+] LocalPlayer Address: 0x%" PRIx32 "\n", LocalPlayer);
			bool flying = false;
			DWORD old_protect;
			for (;;)
			{
				if (GetAsyncKeyState(VK_NUMPAD1))
				{
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x150), &ammo, sizeof(ammo), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x13c), &ammo, sizeof(ammo), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x158), &ammo, sizeof(ammo), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x14C), &ammo, sizeof(ammo), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x148), &ammo, sizeof(ammo), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x144), &ammo, sizeof(ammo), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x140), &ammo, sizeof(ammo), 0);
				}
				if (GetAsyncKeyState(VK_NUMPAD2)) 
				{
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0xF8), &health, sizeof(health), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0xFC), &health, sizeof(health), 0);

				}
				if (GetAsyncKeyState(VK_NUMPAD3))
				{
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x180), &timer, sizeof(timer), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x178), &timer, sizeof(timer), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x174), &timer, sizeof(timer), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x170), &timer, sizeof(timer), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x16C), &timer, sizeof(timer), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x168), &timer, sizeof(timer), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x164), &timer, sizeof(timer), 0);
					WriteProcessMemory(achandle, (LPVOID*)(LocalPlayer + 0x160), &timer, sizeof(timer), 0);

				}
				if (GetAsyncKeyState(VK_NUMPAD4))
				{
					flying = !flying;
					if (flying)
					{
						BYTE on = 0x01;
						WriteProcessMemory(achandle, (LPVOID*)0x45ADD8, &on, sizeof(on), 0);
						cout << "[+] Fly OFF [ON]" << endl;
					}

					else
					{
						BYTE off = 0x00;
						WriteProcessMemory(achandle, (LPVOID*)0x45ADD8, &off, sizeof(off), 0);
						cout << "[+] Fly [OFF] ON" << endl;
					}
					Sleep(300);
				}
				Sleep(1);
			}
		}

	}


	return 0;
}






/*
Here are the addresses for availability incase you want to add on to this source for yourself.

Client Address: 0x400000
Player Base: "ac_client.exe"+0x109B74/509B74

Player Position XY: 0x4
Player Position XY: 0x8
Player Position Z: 0xC
Player Yaw: 0x40
Player Pitch: 0x44
Player Roll: 0x48
Player Health: 0xF8
Player Armor: 0xFC
Player Pistol: 0x114
Player Carbine: 0x118
Player Shotgun: 0x11C
Player MachineGun: 0x120
Player Sniper: 0x124 
Player A-Rifle: 0x128
Player Pistol Clip: 0x13C
Player Carbine Clip: 0x140
Player Shotgun Clip 0x144
Player MachineGun Clip: 0x148
Player Sniper Clip: 0x14C
Player A-Rifle Clip: 0x150
Player Grenade Ammo: 0x158
Time Between Knifes: 0x160
Pistol Timer: 0x164
Carbine Timer: 0x168
Shotgun Timer: 0x16C
Machine Gun Timer: 0x170
Sniper Timer: 0x174
A-Rifle Timer: 0x178
Grenade Timer(?): 0x180
TeamNum1: 0x204
Mouse Button Down(?): 0x224
TeamNum2: 0x32C

Entity Base: "ac_client.exe"+0x110D90/510D90
EntityBase Offsets: 0x0
0x0+(4 bytes each player, starts at 0)*/
