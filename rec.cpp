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
#include <algorithm>
using namespace std;

#pragma comment(lib, "Psapi")
#pragma comment(lib,"Winmm.lib")
HWND osu;

BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lParam) {
	DWORD procid;
	GetWindowThreadProcessId (hwnd, &procid);
	char buffer[65536];
	int txtlen=GetWindowTextLength(hwnd);
	GetWindowTextA(hwnd, buffer, txtlen);
	if(string(buffer).compare("osu!") == 0) {
		cout << "found osu" << endl;
		osu = hwnd;
	}
	return true;
}
int readAddresses(int& audio, int& base, int& rmb, int&lmb) {
	ifstream address("address.txt");
	if(address.good()) {
		for(int j = 0; j < 4; j++) {
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
			switch(j) {
			case 0:
				audio = i;
				break;
			case 1: 
				base = i;
				break;
			case 2:
				rmb = i;
				break;
			case 3:
				lmb = i;
				break;
			}
		}
		address.close();
		return 0;
	} else {
		cout << "could not open address.txt" << endl;
	}
	return -1;
}
int main(int argc, _TCHAR* argv[])
{
	EnumWindows(enumWindowsProc, 0);
	if(osu == NULL) {
		cout <<"no hwnd " << GetLastError() << endl;
	} else {
		DWORD proc_id;
		GetWindowThreadProcessId(osu,&proc_id);		
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_id);

		if(!hProcess)
		{ 
		}
		else
		{
			//my values incase i forgot to include address.txt, wont work for others
			int audioA = 0x002954ac;
			int baseA = 0x02de97fc;	
			int rmbA = 0x002953f0;
			int lmbA = 0x00295398;
			int result = readAddresses(audioA,baseA,rmbA,lmbA);
			if(result != 0) {
				cout << "failed to open settings, using default" << endl;
			}
start :
			DWORD test;
			int mouseA = 0;
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
					started = true;
				}
				Sleep(1);				
			}
			string song = sTitle.substr(8);
			string sub2 = song.substr(song.length() - 5);
			if (sub2 == "-----")
			{
				song = song.substr(0, song.length() - 24);
			}
			char illegal [] = {'*', '\\', '/', '|', '?', '<', '>', '?', '\"', ':'};
			for(int t = 0; t < (sizeof(illegal) / sizeof(char)); t++) {
				replace(song.begin(), song.end(), illegal[t], ' ');
			}
			ofstream replay("replays\\" + song + ".txt");
			if(!replay.good()) {
				cout << "could not open file for writing" << endl;
				return -1;
			} else {
				cout << "writing " << song << ".txt" << endl;
			}
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
						ReadProcessMemory(hProcess, (LPVOID)baseA, &test, sizeof(test), 0);
						mouseA = test;
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
				x = test;
				ReadProcessMemory(hProcess,(LPVOID)(mouseA+8), &test, sizeof(test),0);
				y = test;
				diff = timeGetTime() - initial;
				replay << x << "," << y << "," << diff << "," << rmb << "," << lmb << endl;
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
			goto start;
		}
	}
	system("pause");
	return 0; 
}

