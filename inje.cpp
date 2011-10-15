// inje.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <Psapi.h>
#include <cstring>
#include <fstream>
#include <sstream>
using namespace std;

#pragma comment(lib, "Psapi")
#pragma comment(lib,"Winmm.lib")
HWND osu;
FILE* f;

void DisplayProcessNameAndID( DWORD processID )
{
   TCHAR szProcessName[MAX_PATH] = TEXT("");
  
   // Get a handle to the process.  
   HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
	  PROCESS_VM_READ,
	  FALSE, processID );
  
   // Get the process name.  
   if (NULL != hProcess )
   {
	  HMODULE hMod;
	  DWORD cbNeeded;
	 
	  //Given a handle to a process, this returns all the modules running within the process.
	  //The first module is the executable running the process,
	  //and subsequent handles describe DLLs loaded into the process.
	  if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
	  {
		 //This function returns the short name for a module,
		 //typically the file name portion of the EXE or DLL
		 GetModuleBaseName( hProcess, hMod, szProcessName,
			sizeof(szProcessName)/sizeof(TCHAR) );
	  }
   }
  
   // Display the process name and identifier.
  // CString str;
   printf("Text:%s, PID : %u\n", szProcessName, processID );
  // AfxMessageBox(str);
  
   //close the process handle
   CloseHandle( hProcess );
}
BOOL CALLBACK enumWindowsProc (HWND hwnd, LPARAM lParam) {

DWORD procid;

GetWindowThreadProcessId (hwnd, &procid);
//if ((HANDLE)procid == g_hProc) {
//LPWSTR tses;
 char buffer[65536];
 
  int txtlen=GetWindowTextLength(hwnd);
  GetWindowTextA(hwnd, buffer, txtlen);
//GetWindowText(hwnd, tses, sizeof(tses));
  if(string(buffer).compare("osu!") == 0) { //0 is exact match apparently
	cout << buffer << endl;
	osu = hwnd;
  }
return true;
}

int main(int argc, _TCHAR* argv[])
{
	 EnumWindows(enumWindowsProc, 0);
	// system("pause");
	//HWND  hWnd = FindWindow(0,L"osu!");
	   // Get the list of process identifiers.  
  // DWORD aProcesses[1024], cbNeeded, cProcesses;
//   unsigned int i;

   //This returns a list of handles to processes running on the system as an array.
/*   if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
	  return 0 ;
   
   // Calculate how many process identifiers were returned.  
   cProcesses = cbNeeded / sizeof(DWORD);
  
   // Display the name and process identifier for each process.  
   for ( i = 0; i < cProcesses; i++ )
	  if( aProcesses[i] != 0 )
		 DisplayProcessNameAndID( aProcesses[i] );  */
	if(osu == NULL) {
		cout <<"no hwnd " << GetLastError() << endl;
	} else {
		DWORD proc_id;
		GetWindowThreadProcessId(osu,&proc_id);		
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_id);

		if(!hProcess) //** If cannot 'OpenProcess', then it displays the error message 
		{ 
			MessageBox(NULL, L"Cannot open process!", L"Error!", MB_OK + MB_ICONERROR); 
		}
		else
		{
			int audioA = 0x002954ac;
			int baseA = 0x02de97fc;	
			int rmbA = 0x002953f0;
			int lmbA = 0x00295398;
			ifstream address("address.txt");
			if(address.good()) {
				string toGet;
				getline(address,toGet);
				stringstream ss;
				ss << toGet;
				int i;
				
				ss >> hex >> i;
				if(ss.fail()) {
					cout << "Error parsing address" << endl;
					return -1;
				}
				cout << "using address " << i << endl;
				baseA = i;
				address.close();
			} else {
			cout << "could not open address.txt" << endl;
			}
start :
			baseA = 0x036039d0;
			DWORD test;
			ReadProcessMemory(hProcess, (LPVOID)baseA, &test, sizeof(test), 0);
			int mouseA = test;
			bool started = false, offset = 0;
			string sTitle;
			char title [128];
			
			while (!started)
			{
				title [128];
				int txtlen = GetWindowTextLength(osu);
				GetWindowTextA(osu, title, txtlen);	
				sTitle = title;
				if (sTitle != "osu!")
				{		
					started = true;//1; 1 or 0 is for backwards c compatibility, true/false is prob better in c++
				}
				Sleep(1);				
			}
			string song = sTitle.substr(8);
			//char filename [256];
			//cout << filename << "\n";
			//sprintf(filename, "%s.rep", song); //dunno why this doesnt work, whatever
			//Note, sprintf is part of c io lib, should use iostream/fstream for c++, more type safe or w/e
			ofstream replay(song + ".txt");
			if(!replay.good()) {
			cout << "could not open file for writing" << endl;
			return -1;
			} else {
				cout << "writing " << song << ".txt" << endl;
			}
		    
	//		f = fopen(title, "wb+");
			//DWORD test;
			cout << "starting read" << endl;
			int initial = 0;
			int diff = 0;
			int go = 0, second = 0;
			int audioOff = 0;
			while(started) {
				while (!go)
				{
					ReadProcessMemory(hProcess, (LPVOID)audioA, &test, sizeof(test), 0);
					audioOff = test;
					if (audioOff == 0)
					{
						second = 1;
					}
					if (second && audioOff > 0)
					{
						go = 1;
						initial = timeGetTime();
					}
				}
				int x,y, rmb, lmb;
			ReadProcessMemory(hProcess, (LPVOID)rmbA, &test, sizeof(test), 0);
			rmb = test;
			ReadProcessMemory(hProcess, (LPVOID)lmbA, &test, sizeof(test), 0);
			lmb = test;
			ReadProcessMemory(hProcess,(LPVOID)(mouseA+4), &test, sizeof(test),0);
			//cout << test << endl;
			x = test;
			ReadProcessMemory(hProcess,(LPVOID)(mouseA+8), &test, sizeof(test),0);
			//cout << test << endl;
			y = test;
			diff = timeGetTime() - initial;
			replay << x << "," << y << "," << diff << "," << rmb << "," << lmb << endl;
			cout << x << "," << y << "," << diff << "," << rmb << "," << lmb << endl;
			int txtlen = GetWindowTextLength(osu);
			GetWindowTextA(osu, title, txtlen);	
			sTitle = title;
			if (sTitle == "osu!")
			{		
				started = false;
			}
			Sleep(2);
			}
			replay.close();
			cout << "replay record finished, listening again" << endl;
			goto start; //oh well ;_;
		//	cout << &hProcess << endl;
		//	cout << GetModuleBase(hProcess, string("C:\\Users\\Alex\\Documents\\visual studio 2010\\Projects\\CEMe\\Debug\\CEMe.exe"))  + 0x0B8000 << endl;

		//	cout << test << endl;
			////DWORD test2 = 31337;
		//	WriteProcessMemory(hProcess,(LPVOID)(0x003D8000), &test2,sizeof(test2),0);
		}
	}
	//CloseHandle(hProcess);
	system("pause");
	return 0; 
}

