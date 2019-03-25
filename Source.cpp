#include <iostream>
#include <iomanip>
#include <chrono>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
using namespace std;

/*
conio.h
char _getch() -> gets keycode
bool _kbhit() -> checks if any key is buffered
*/

double getTime(); //Returns timer with nanosecond precision
double getTimeSince(double); //Returns timer with nanosecond precision since a point in time
double wait(double); //Pauses for the given amount of seconds, returns how much extra time was waited
double waitWithCountdown(double); //Like wait, but with a 3... 2... 1... GO! countdown
double waitForInput(char); //Like wait, but until a specific input is given
bool keyIsDown(char, bool = true, bool = true); //Return a boolean representing whether or not the key is pressed
int randomInt(int, int); //Generate a random integer between two given integers, including both
void ShowConsoleCursor(bool); //false to turn it off
void SetCursorPosition(int, int);
void OutputAtPosition(int, int, char);
void OutputAtPosition(int, int, string);

const char solidBlock = 219u;
const char pongBall = 'O';
const char blankSpace = ' ';
const char upArrow = 38;
const char downArrow = 40;
const double FPS = 1.0 / 60.0;
const int barHeight = 5;
const int gameWidth = 69, gameHeight = 21; // 69, 21
const double gameCenterX = gameWidth / 2.0 + 1, gameCenterY = gameHeight / 2.0 + 1;
const int pongDefaultSpeed = 20;
const int barDefaultTimer = 2;
const long double pi = 3.141592653589793238462643383279502884L;
const bool debugByDefault = false;


int main() {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	system(("MODE " + to_string(gameWidth) + "," + to_string(gameHeight)).c_str());
	ShowConsoleCursor(false);
	srand((unsigned)time(0));

	int leftScore = 0, rightScore = 0;
	bool debugMode = debugByDefault;

	while (true) {
		//Initial variables
		double pongX = gameCenterX, pongY = gameCenterY;
		int pongSpeed = pongDefaultSpeed;
		int prevPongX = static_cast<int>(pongX), prevPongY = static_cast<int>(pongY);
		int pongAngle = randomInt(145, 215) + 180 * randomInt(0, 1);
		double leftBarY = gameCenterY, rightBarY = gameCenterY;
		int leftBarTimer = 0, rightBarTimer = 0;
		double timer = 0, dt = 0;

		//Initial draw
		for (int i = -2; i <= 2; i++) {
			OutputAtPosition(1, static_cast<int>(gameCenterY + i), solidBlock);
			OutputAtPosition(gameWidth, static_cast<int>(gameCenterY + i), solidBlock);
		}
		OutputAtPosition(static_cast<int>(pongX), static_cast<int>(pongY), pongBall);

		SetConsoleTitle(("Pong | P1 Score: " + to_string(leftScore) + " | P2 Score: " + to_string(rightScore) + " | Ball Speed: " + to_string(pongSpeed) + " | Press space to start...").c_str());
		waitForInput(blankSpace);

		SetConsoleTitle(("Pong | P1 Score: " + to_string(leftScore) + " | P2 Score: " + to_string(rightScore) + " | Ball Speed: " + to_string(pongSpeed)).c_str());

		[&] { //Lambda / anonymous function / succ me daddy
			while (timer += (dt = FPS + wait(FPS))) {
				//Controls
				if (keyIsDown('W') && !keyIsDown('S') && leftBarY > barHeight / 2.0 + 1) {
					if (!leftBarTimer) {
						OutputAtPosition(1, static_cast<int>(leftBarY - 3), solidBlock);
						OutputAtPosition(1, static_cast<int>(leftBarY + 2), blankSpace);
						leftBarY--;
					}
					leftBarTimer++;
					leftBarTimer %= barDefaultTimer;
				}
				else if (keyIsDown('S') && !keyIsDown('W') && leftBarY < gameHeight + 1 - barHeight / 2.0) {
					if (!leftBarTimer) {
						OutputAtPosition(1, static_cast<int>(leftBarY + 3), solidBlock);
						OutputAtPosition(1, static_cast<int>(leftBarY - 2), blankSpace);
						leftBarY++;
					}
					leftBarTimer++;
					leftBarTimer %= barDefaultTimer;
				}
				else {
					leftBarTimer = 0;
				}
				if (keyIsDown(upArrow) && !keyIsDown(downArrow) && rightBarY > barHeight / 2.0 + 1) {
					if (!rightBarTimer) {
						OutputAtPosition(gameWidth, static_cast<int>(rightBarY - 3), solidBlock);
						OutputAtPosition(gameWidth, static_cast<int>(rightBarY + 2), blankSpace);
						rightBarY--;
					}
					rightBarTimer++;
					rightBarTimer %= barDefaultTimer;
				}
				else if (keyIsDown(downArrow) && !keyIsDown(upArrow) && rightBarY < gameHeight + 1 - barHeight / 2.0) {
					if (!rightBarTimer) {
						OutputAtPosition(gameWidth, static_cast<int>(rightBarY + 3), solidBlock);
						OutputAtPosition(gameWidth, static_cast<int>(rightBarY - 2), blankSpace);
						rightBarY++;
					}
					rightBarTimer++;
					rightBarTimer %= barDefaultTimer;
				}
				else {
					rightBarTimer = 0;
				}
				if (keyIsDown('Q', true, false)) {
					if (debugMode) {
						string empty = " ";
						for (int i = 7; i < gameWidth; i++) {
							empty += " ";
						}
						OutputAtPosition(5, 1, empty.c_str());
					}
					debugMode = !debugMode;
				}

				//Calculate old + new ball positions
				prevPongX = static_cast<int>(pongX), prevPongY = static_cast<int>(pongY);
				double pongDX = cos(pongAngle / 180.0 * pi) * dt * pongSpeed, pongDY = sin(pongAngle / 180.0 * pi) * dt * pongSpeed;
				double pongNX = pongX + pongDX, pongNY = pongY + pongDY;
				double pongXExtra, pongYExtra, barY, interpolation = 0;

				{
					pongXExtra = pongDX < 0 ? 2.5 - pongNX : pongNX - gameWidth + 0.5, pongYExtra = pongDY < 0 ? 1.5 - pongNY : pongNY - gameHeight - 0.5;
					while (pongXExtra > 0 || pongYExtra > 0) {
						interpolation = max((pongAngle % 180 == 90 ? 0 : pongXExtra / abs(pongDX)), (pongAngle % 180 == 0 ? 0 : pongYExtra / abs(pongDY)));
						dt *= interpolation;

						//Implement ball bouncing from paddle
						if (pongAngle % 180 != 90 && pongXExtra / abs(pongDX) > pongYExtra / abs(pongDY)) {
							pongNY -= pongDY * (1 - interpolation);
							barY = pongNX <= 2.5 ? leftBarY : rightBarY;
							if (abs(pongNY - barY) > barHeight / 2.0) {
								pongNY += pongDY * abs(1 / pongDX);
								if (pongDX < 0) {
									rightScore++;
									OutputAtPosition(1, static_cast<int>(pongNY > barY ? min(gameHeight - 0.5, max(barY + barHeight / 2.0 + 0.5, pongNY)) : max(1.5, min(barY - barHeight / 2.0 - 0.5, pongNY))), pongBall);
								}
								else {
									leftScore++;
									OutputAtPosition(gameWidth, static_cast<int>(pongNY > barY ? min(gameHeight - 0.5, max(barY + barHeight / 2.0 + 0.5, pongNY)) : max(1.5, min(barY - barHeight / 2.0 - 0.5, pongNY))), pongBall);
								}

								OutputAtPosition(prevPongX, prevPongY, blankSpace);

								SetConsoleTitle(("Pong | P1 Score: " + to_string(leftScore) + " | P2 Score: " + to_string(rightScore) + " | Ball Speed: " + to_string(pongSpeed) + " | Press space to continue...").c_str());
								waitForInput(blankSpace);

								system("CLS");
								return; //Exit lambda hehe xd
							}
							else {
								pongAngle = static_cast<int>(pongDX > 0 ? 180 - 70 * ((pongNY - barY) / barHeight) : 360 + 70 * ((pongNY - barY) / barHeight));
								pongDX = cos(pongAngle / 180.0 * pi) * dt * pongSpeed, pongDY = sin(pongAngle / 180.0 * pi) * dt * pongSpeed;
								pongNX = (pongDX > 0 ? 2.5 : gameWidth - 0.5) + pongDX;
								pongNY += pongDY;
								pongSpeed += 2;
								SetConsoleTitle(("Pong | P1 Score: " + to_string(leftScore) + " | P2 Score: " + to_string(rightScore) + " | Ball Speed: " + to_string(pongSpeed)).c_str());
							}
						}
						//Implement ball bouncing from floor + ceiling
						else {
							pongNX -= pongDX * (1 - interpolation);
							pongDX *= dt;
							pongDY *= -dt;
							pongAngle = 360 - pongAngle;
							pongNY = (pongDY > 0 ? 1.5 : gameHeight + 0.5) + pongDY;
						}
						pongXExtra = pongDX < 0 ? 2.5 - pongNX : pongNX - gameWidth + 0.5, pongYExtra = pongDY < 0 ? 1.5 - pongNY : pongNY - gameHeight - 0.5;
					}
				}
				pongX = pongNX, pongY = pongNY;

				if (debugMode) {
					OutputAtPosition(5, 1, (to_string(dt) + ' ' + to_string(pongDX) + ' ' + to_string(pongDY) + ' ' + to_string(interpolation) + "            ").c_str());
				}

				//Redraw if necessary
				if (static_cast<int>(pongX) != prevPongX || static_cast<int>(pongY) != prevPongY) {
					OutputAtPosition(static_cast<int>(pongX), static_cast<int>(pongY), pongBall);
					OutputAtPosition(prevPongX, prevPongY, blankSpace);
				}
			}
		}();
	}

	return 0;
}

double getTime() { //Note: This is not time since epoch like time(0), should only be used for measuring time durations
				   //http://www.cplusplus.com/reference/chrono/high_resolution_clock/now/ for timer
				   //http://www.cplusplus.com/reference/chrono/time_point_cast/ for setting time unit
				   //http://www.cplusplus.com/reference/chrono/time_point/time_since_epoch/ measures difference from epoch
				   //http://www.cplusplus.com/reference/chrono/duration/count/ for turning time into a number
	return chrono::time_point_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now()).time_since_epoch().count() / 1e9;
}
double getTimeSince(double startTime) {
	return chrono::time_point_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now()).time_since_epoch().count() / 1e9 - startTime;
}
double wait(double waitTime) {
	double startTime = getTime();

	while (waitTime > getTimeSince(startTime)) {}

	return getTimeSince(startTime + waitTime);
}
double waitWithCountdown(double waitTime) {
	int ticks = -2;
	double now, startTime = getTime(), timeScale = 3.0 / waitTime;
	while (3 > (now = getTimeSince(startTime) * timeScale)) {
		if (now * 4 - 1 > ticks) {
			ticks = static_cast<int>(now * 4);
			cout << '\r' << static_cast<int>(3 - now + 0.99) << setfill('.') << setw(ticks % 4 + 4) << "   \r";
		}
	}

	cout << "\rGO! \rGO!";
	return getTimeSince(startTime + waitTime);
}
double waitForInput(char sentinel) {
	double startTime = getTime();
	char input = '\0';

	/*while (input != sentinel) {
		if (_kbhit()) {
			input = _getch();
		}
	}*/
	while (!keyIsDown(sentinel, true, false)) {}

	return getTimeSince(startTime);
}

bool keyIsDown(char key, bool pressed, bool held) {
	int keyState = GetAsyncKeyState(static_cast<int>(key));
	return (pressed && (keyState & 1)) || (held && (keyState & 0xA000));
}

int randomInt(int min, int max) {
	return rand() % (max - min + 1) + min;
}

void ShowConsoleCursor(bool showFlag) { //https://stackoverflow.com/questions/18028808/remove-blinking-underscore-on-console-cmd-prompt
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

void SetCursorPosition(int x, int y) {
	COORD coord;
	coord.X = x - 1;
	coord.Y = y - 1;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void OutputAtPosition(int x, int y, char output) {
	SetCursorPosition(x, y);
	cout << output;
}

void OutputAtPosition(int x, int y, string output) {
	SetCursorPosition(x, y);
	cout << output.c_str();
}