#include <iostream>
using namespace System;
using namespace std;

#define WIDTH 44
#define HEIGHT 16

void welcomeMsg();
void mainMenu();
void run();
void settings();

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
	system("CLS");
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
	cout << "this works";
	int i;
	cin >> i;
}

void settings() {
	system("cls");
	cout << "this too";
	int i;
	cin >> i;
}







                                          
                                          