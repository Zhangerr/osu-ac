#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include <algorithm>
#include <sstream>
#pragma comment(lib,"Winmm.lib")
using namespace std;

HWND osu;

int readAddresses(int& audio, char& key1, char& key2) {
	ifstream address("address2.txt");
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
		audio = i;
		for(int j = 0; j < 2; j++) {
			getline(address,toGet);	
			switch(j) {
			case 0:
				key1 = toGet[0];
				break;
			case 1 :
				key2 = toGet[0];
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
BOOL CALLBACK enumWindowsProc (HWND hwnd, LPARAM lParam) {

	DWORD procid;

	GetWindowThreadProcessId (hwnd, &procid);
	char buffer[65536];

	int txtlen=GetWindowTextLength(hwnd);
	GetWindowTextA(hwnd, buffer, txtlen);
	if(string(buffer).compare("osu!") == 0) {
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
	getosu();
	RECT r;
	if(osu == NULL) {
		cout << "couldn't find osu" << endl;
		system("pause");
		return -1;
	}
	DWORD proc_id;
	GetWindowThreadProcessId(osu,&proc_id);		
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_id);
	int audioA = 0x0031555C;
	char key1 = 'z';
	char key2 = 'x';
	if(readAddresses(audioA,key1,key2) == -1) {
		cout << "reading addresses failed" << endl;
	}
	cout << key1 << "," << key2 << "," << audioA << endl;
	int hit = 0;
start:
	while(getTitle() == "osu!") {
		Sleep(1);
		if(GetAsyncKeyState('Q') && !nomodToDt) {
			nomodToDt = true;
			htToNomod = false;
			htToDt = false;
			cout << "nomod to dt" << endl;
		} else if (GetAsyncKeyState('W') && !htToNomod) {
			nomodToDt = false;
			htToNomod = true;
			htToDt = false;
			cout << "ht to nomod" << endl;
		} else if (GetAsyncKeyState('E') && !htToDt) {
			nomodToDt = false;
			htToNomod = false;
			htToDt = true;
			cout << "ht to dt" << endl;
		} else if (GetAsyncKeyState('R') && (nomodToDt || htToNomod || htToDt)) {
			nomodToDt = false;
			htToNomod = false;
			htToDt = false;
			cout << "reset" << endl;
		}
	}
		string tempTitle = getTitle();
		string sub = tempTitle.substr(8);
		string sub2 = sub.substr(sub.length() - 5);
		if (sub2 == "-----")
		{
			sub = sub.substr(0, sub.length() - 24);
		}
		char illegal [] = {'*', '\\', '/', '|', '?', '<', '>', '?', '\"', ':'};
		for(int t = 0; t < (sizeof(illegal) / sizeof(char)); t++) {
			replace(sub.begin(), sub.end(), illegal[t], ' ');
		}
	ifstream toRead("replays\\" + sub + ".txt");
	if(!toRead.good()) {
		cout << "Could not read file or missing" << endl;
		system("pause");
		goto start;
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
		string line;
		getline(toRead, line);
		int x,y,offset, press1, press2;
		sscanf(line.c_str(), "%d,%d,%d,%d,%d", &x, &y, &offset, &press1, &press2);
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
			now = timeGetTime() -  initialRep;
			if(getTitle() == "osu!") {
				toRead.close();
				goto start;
			} else if (GetAsyncKeyState('P'))
			{
				while (getTitle() != "osu!")
				{
					Sleep(1);
				}
				toRead.close();
				goto start;
			}
		}
		if (press1)
		{
			char button = key1;
			keybd_event(VkKeyScan(button), MapVirtualKey(VkKeyScan(button), 0), 0, 0);
		}
		else
		{
			char button = key1;
			keybd_event(VkKeyScan(button), MapVirtualKey(VkKeyScan(button), 0), KEYEVENTF_KEYUP, 0);
		}
		if (press2)
		{
			char button = key2;
			keybd_event(VkKeyScan(button), MapVirtualKey(VkKeyScan(button), 0), 0, 0);
		}
		else
		{
			char button = key2;
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

