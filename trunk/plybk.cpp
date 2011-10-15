// plybk.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>

#pragma comment(lib,"Winmm.lib")
using namespace std;

#define KEY1 'w'
#define KEY2 'e'
HWND osu;
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
		cout << "Found " <<  buffer << endl;
		osu = hwnd;
	}
	return true;
}
void getosu() {
	EnumWindows(enumWindowsProc, 0);
}
string getTitle() {
	int txtlen = GetWindowTextLength(osu);
	char buffer[256];
	GetWindowTextA(osu, buffer, txtlen);	
	return string(buffer);
}
int main(int argc, _TCHAR* argv[])
{
	int nomodToDt = 0;
	int htToNomod = 0;
	int htToDt = 0;
	int nomodToHt = 0;

	//sampling rate of 100 ms is too slow, may need to use bass position, as lag can fuck up entire replay

	//shit, using .osu file would have exact timings, need relative to replay, so have to scan for bools i guess
	ifstream toRead("test.txt"); //temporary
	ifstream osuFile("osu.txt"); //also temp
	//ifstream osuFile("test.osu");
	if(!toRead.good()) {//|| !osuFile.good()) {
		cout << "Could not read file" << endl;
		return -1;
	}
	getosu();
	RECT r;

	if(osu == NULL) {
		cout << "couldn't find osu" << endl;
	}
	DWORD proc_id;
	GetWindowThreadProcessId(osu,&proc_id);		
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_id);
	int audioA = 0x002954ac;
	int hit = 0;
start:
	while(getTitle() == "osu!") {
		Sleep(1);
	}
	GetWindowRect(osu,&r);
	int go = 0, second = 0, audioOff = 1;
	DWORD test;

	int initialRep = 0;
	int initialOsu = timeGetTime();
	while(!toRead.eof()) {
		while (!go)
		{
			ReadProcessMemory(hProcess, (LPVOID)audioA, &test, sizeof(test), 0);
			audioOff = test;
			
			if (audioOff == 0 && !second)
			{
				second = 1;
				cout << audioOff << endl;
			}
			if (second && audioOff > 0)
			{
				go = 1;
				initialRep = timeGetTime() - 5;
			}
		}
		string line; //replay
		string line2; //osu
		getline(toRead, line);
		//getline(osuFile, line2);
		int x,y,offset, press1, press2;
		sscanf(line.c_str(), "%d,%d,%d,%d,%d", &x, &y, &offset, &press1, &press2);
		cout << x << "," << y << "," << offset << endl;
		if (nomodToDt)
		{
			offset = offset *(2.0/3.0);
		}
		else if (htToDt)
		{
			offset = offset * (2.0 / 3.0);
			offset = offset * (3.0 / 4.0);
		}
		else if (htToNomod)
		{
			offset = offset * (3.0 / 4.0);
		}
		else if (nomodToHt)
		{
			offset = offset * (4.0 / 3.0);
		}
		int now = timeGetTime() -  initialRep;
		while((offset - now) > 1) {
			Sleep(1);	
			now = timeGetTime() -  initialRep;
			if(getTitle() == "osu!") {
				toRead.clear();
				toRead.seekg(0,ios::beg);
				goto start;
			} else if (GetAsyncKeyState('P'))
			{
				while (getTitle() != "osu!")
				{
				}
				toRead.clear();
				toRead.seekg(0,ios::beg);
				goto start;
			}
		}
		if (press1)
		{
			char button = KEY1;
			keybd_event(VkKeyScan(button), MapVirtualKey(VkKeyScan(button), 0), 0, 0);
		}
		else
		{
			char button = KEY1;
			keybd_event(VkKeyScan(button), MapVirtualKey(VkKeyScan(button), 0), KEYEVENTF_KEYUP, 0);
		}
		if (press2)
		{
			char button = KEY2;
			keybd_event(VkKeyScan(button), MapVirtualKey(VkKeyScan(button), 0), 0, 0);
		}
		else
		{
			char button = KEY2;
			keybd_event(VkKeyScan(button), MapVirtualKey(VkKeyScan(button), 0), KEYEVENTF_KEYUP, 0);
		}
		if( x >= 0 && y >= 0) {
			SetCursorPos(x + r.left + 5,y + r.top + 25);
		}
		else
		{
			SetCursorPos(-100,-100);
		}
	}
	toRead.close();
	system("pause");
	return 0;
}

