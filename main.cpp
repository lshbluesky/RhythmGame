#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <windows.h>
#include <time.h>
#include <memory.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <vector>
#include "fmod.hpp"
#include "Screen.hpp"
#include "Note.hpp"
#include "Note.cpp"

using namespace std;
using namespace FMOD;

// 첫 메인 화면을 그리기 위한 클래스 생성
class MainMenu {
public:
	void DrawMainMenu() {
		cout << "\n\n\n\n";
		cout << "\t\t"; cout << "      rrrrrrrrrrr      dddddddddddd\n";
		cout << "\t\t"; cout << "      rr        rr     dd          dd\n";
		cout << "\t\t"; cout << "      rr         rr    dd            dd\n";
		cout << "\t\t"; cout << "      rr         rr    dd             dd\n";
		cout << "\t\t"; cout << "      rr         rr    dd             dd\n";
		cout << "\t\t"; cout << "      rr        rr     dd             dd\n";
		cout << "\t\t"; cout << "      rrrrrrrrrr       dd             dd\n";
		cout << "\t\t"; cout << "      rr       rr      dd            dd\n";
		cout << "\t\t"; cout << "      rr         rr    dd          dd\n";
		cout << "\t\t"; cout << "      rr           rr  ddccccccccdd\n\n";
		cout << "\t\t"; cout << "둠칫~둠칫 ENTER키를 누르고 너의 리듬을 보여줘!\n\n";
		getchar();
	}

	void InitMainMenu() {
		MainMenu::DrawMainMenu();
	}
};

// 메인 함수
int main(void) {

	MainMenu MainControl; // 메인 화면을 그리는 객체 생성
	MainControl.InitMainMenu(); // 메인 화면을 그리는 객체로 메인 화면 출력

	SoundSystem(); // FMOD 사용 준비

	while (Stage != END) {
	
		Play(0); // pSound[0] (=Main_BGM.wav)를 실행
		
		ScreenInit(); 
		KeyIndexInit();
		init(); // stage를 ready 상태로 만들고 노트들 초기화
		
		int inputKey=0; //입력 키
		while (1) {
			if (_kbhit()) {
				inputKey = _getch();

				if (inputKey == ENTER) { // 엔터 키를 입력하는 경우
					if (Stage == READY) {
						pChannel[0]->stop();
						Play(1); // pSound[0] (=Game_BGM.wav)를 실행
					}
					else if (Stage == PAUSE) { // 스테이지 Pause 상태일 때 엔터를 누를 경우
						PauseEnd = clock();
						PauseTime += PauseEnd - PauseStart;
						pChannel[0]->setPaused(false); // 현재 pChannel[0]에 있는 노래의 일시 정지를 해제한다.
					}
					else if (Stage == SYNC) { // 스테이지가 Sync 상태일 때 엔터를 누를 경우
						NoteInit();
						pChannel[0]->stop();
						Play(1);
						SyncEnd = clock();
						SyncTime += SyncEnd - SyncStart;
					}
					else if (Stage == RESULT) {
						break;
					}
					else
					{
						PauseStart = clock();
						pChannel[0]->setPaused(true); // 현재 pChannel[0]에 재생중인 노래를 일시 정지한다.
						Stage = PAUSE;
						break;
					}
					Stage = RUNNING; // 엔터 입력 시 running시작 음악 호출
				}

				if (inputKey == 'p') {
					if (Stage == RUNNING) {
						PauseStart = clock();
						pChannel[0]->setPaused(true); // 현재 pChannel[0]에 재생중인 노래를 일시 정지한다.
						Stage = PAUSE;
					}
					else if (Stage == PAUSE && inputKey == 'p') { // 스테이지 Pause 상태일 때 p를 누를 경우
						PauseEnd = clock();
						PauseTime += PauseEnd - PauseStart;
						pChannel[0]->setPaused(false); // 현재 pChannel[0]에 있는 노래의 일시 정지를 해제한다.
						Stage = RUNNING;
					}
				}

				if (Stage == READY && inputKey == 'c') {
					SyncStart = clock();
					Stage = SYNC;
					SyncMap();
				}

				if (inputKey == 'a' || inputKey == 's' || inputKey == 'd' || inputKey == 'j' || inputKey == 'k' || inputKey == 'l') { 
					if (Stage == PAUSE) { // PAUSE 상태일 경우 키 입력을 무시
						continue;
					}

					string inputKeyStr; // CheckKey()의 인자로 줄 변수, inputKey를 string으로 변환할 변수 선언 
					inputKeyStr = inputKey; // int inputKey를 string 변수로 변환
					if (isTwoKey(Note[curNoteIndex]) || (curNoteIndex > 0 && isTwoKey(Note[curNoteIndex - 1])) || isTwoKey(Note[curNoteIndex + 1])) { // hit 구간 노트가 두 개라면
						inputKeyStr = secondkbhit(inputKey, inputKeyStr); // inputKey와 비교를 위해 'inputKey'와 string 반환을 위한 'inputKeyStr'을 인자로 줌 
					}
					CheckKey(inputKeyStr);
				}

				if (Stage == RESULT) {
					if (inputKey == 'q') {
						ScreenClear();
						return 0;
					}
				}

				if (Stage == SYNC) {
					if (inputKey != LEFT && inputKey != RIGHT) {
						continue;
					}
					ControlSync(inputKey);
				}
			}

			Update();  // 데이터 갱신
			Render(inputKey);  // 화면출력

		}
		Release(); // 해제
		ScreenRelease();
	}
	return 0;
}
