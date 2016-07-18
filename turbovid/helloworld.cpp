#include <iostream>
#include <string>
#include <new>
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <Shlwapi.h>

#pragma comment(lib, "mfplat")
#pragma comment(lib, "mf")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "shlwapi")

using namespace System;
using namespace std;

#define WIDTH 44
#define HEIGHT 16

enum eAVEncH264VProfile {
	eAVEncH264VProfile_unknown = 0,
	eAVEncH264VProfile_Simple = 66,
	eAVEncH264VProfile_Base = 66,
	eAVEncH264VProfile_Main = 77,
	eAVEncH264VProfile_High = 100,
	eAVEncH264VProfile_422 = 122,
	eAVEncH264VProfile_High10 = 110,
	eAVEncH264VProfile_444 = 144,
	eAVEncH264VProfile_Extended = 88,
	eAVEncH264VProfile_ScalableBase = 83,
	eAVEncH264VProfile_ScalableHigh = 86,
	eAVEncH264VProfile_MultiviewHigh = 118,
	eAVEncH264VProfile_StereoHigh = 128,
	eAVEncH264VProfile_ConstrainedBase = 256,
	eAVEncH264VProfile_UCConstrainedHigh = 257,
	eAVEncH264VProfile_UCScalableConstrainedBase = 258,
	eAVEncH264VProfile_UCScalableConstrainedHigh = 259

};

void welcomeMsg();
void mainMenu();
void run();
void settings();

struct H264ProfileInfo {
	UINT32 profile;
	MFRatio fps;
	MFRatio frame_size;
	UINT32 bitrate;
};

H264ProfileInfo h264_profiles[] = {
	{ eAVEncH264VProfile_Base,{ 15, 1 },{ 176, 144 },   128000 },
	{ eAVEncH264VProfile_Base,{ 15, 1 },{ 352, 288 },   384000 },
	{ eAVEncH264VProfile_Base,{ 30, 1 },{ 352, 288 },   384000 },
	{ eAVEncH264VProfile_Base,{ 29970, 1000 },{ 320, 240 },   528560 },
	{ eAVEncH264VProfile_Base,{ 15, 1 },{ 720, 576 },  4000000 },
	{ eAVEncH264VProfile_Main,{ 25, 1 },{ 720, 576 }, 10000000 },
	{ eAVEncH264VProfile_Main,{ 30, 1 },{ 352, 288 }, 10000000 },
};

int main() {
	Console::SetWindowSize(WIDTH, HEIGHT);
	mainMenu();

	return 0;
}

void welcomeMsg() {
	cout << "\n  _              _                _     _ \n";
	cout << " | |            | |              (_)   | |\n";
	cout << " | |_ _   _ _ __| |__   _____   ___  __| |\n";
	cout << " | __| | | | '__| '_ \\ / _ \\ \\ / / |/ _` |\n";
	cout << " | |_| |_| | |  | |_) | (_) \\ V /| | (_| |\n";
	cout << "  \\__|\\__,_|_|  |_.__/ \\___/ \\_/ |_|\\__,_|\n\n";
	cout << "		version 0.9.0\n\n";
	cout << "	developed by hamzamuhammad\n\n";
	cout << " [1:Run]       [2:Settings]       [3:Exit]\n\n";
	cout << " Type in your choice, then press enter: ";
}

void mainMenu() {
	system("cls");
	welcomeMsg();
	int menuOption;
	cin >> menuOption;
	switch (menuOption) {
		case 1:
			run();
			break;
		case 2:
			settings();
			break;
		case 3:
			exit(0);
	}
}

void run() {
	system("cls");
	cout << "Convert a video file from .mkv to .mp4!\n\n";
	cout << "Enter full path of .mkv file: ";
	string path;
	getline(cin, path);

}

void settings() {
	// TO-DO
}







                                          
                                          