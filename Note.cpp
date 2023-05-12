/*
	void SoundSystem() : function for allocating and initializing Fmod systems.
	void Play() : function for playing allocated songs.
	void Map() : function for showing game map.
	void ScoreMap() : function for showing score texts in the upper-right.
	void ReadyMap() : function for showing ready map before game start.
	void ReadyMap() : function for showing blinking texts in ready map.
	void ResultMap() : function for showing score which player got. 
	void init() : function for initializing time , notes and stage status.
	void keyindex() : function for initializing keyindex.
	void Noteindex() : function for initializing Notes.
	void update() : function for updating time and changed information.
	void Render() : function for changing stage mode , clearing the screen and caculating paly time.
	string secondkkbhit() : function to input the second key, and this is function for the doubled notes.
	sring getkeytype() : fuction for returnning the string of key notes.
	int isTwokey() : function for checking the doubled notes.
	void checkkey() : function for checking notes if note collision have occured or not.
	void shownote() : function for showing the falling notes.
	string HitNote() : function for changing the notes that was hit.
	void ControlSync : function for changing the sync with LEFT or RIGHT key.	
*/
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
#include <string>
#include "fmod.hpp"
#include "Screen.hpp"
#include "Note.hpp"

using namespace std;
using namespace FMOD;

//사운드 함수
void SoundSystem() {
	System_Create(&pSystem);
	pSystem->init(4, FMOD_INIT_NORMAL, NULL);
	pSystem->createSound("Main_BGM.wav", FMOD_LOOP_NORMAL | FMOD_HARDWARE, NULL, &pSound[0]); // 오프닝음악
	pSystem->createSound("Game_BGM.mp3", FMOD_DEFAULT, NULL, &pSound[1]); // 게임음악
}
void Play(int Sound_num) {
	pSystem->playSound(FMOD_CHANNEL_FREE, pSound[Sound_num], 0, pChannel);
}

// 맵 함수
// 스테이지 기본 틀
void Map(void) {
	int nNum = 0;
	ScreenPrint(0, 0, "□□□□□□□□□□□□□□□□□□□□□");
	for (int i = 1; i < 29; i++) {
		ScreenPrint(0, i, "□\t\t\t\t\t□");
	}
	ScreenPrint(0, 29, "□□□□□□□□□□□□□□□□□□□□□");
	ScreenPrint(2, 26, "______________________________________");
}
// 우측 점수 출력틀
void ScoreMap() {
	// 경과시간
	char nTime[20];//경과 시간을 나타낸다
	sprintf(nTime, "시간 : %d.%d초", RunningTime / 1000, RunningTime % 1000);
	ScreenPrint(44, 2, nTime);
	// 점수 목록
	if (strcmp(strScore, "★Perfect★") == 0) {
		SetColor(14);
	}
	else if (strcmp(strScore, "★Great★") == 0) {
		SetColor(9);
	}
	ScreenPrint(44, 10, strScore);//Great,Perfect판별
	SetColor(15);
	ScreenPrint(44, 22, "Great : 300점");
	ScreenPrint(44, 23, "Perfect : 500점");
	SetColor(12);
	ScreenPrint(44, 6, "Press \'p\' to Pause");
	SetColor(15);
	// 점수
	char UserScore[20];//사용자 점수를 나타냄
	sprintf(UserScore, "점수 : %d 점", nScore);
	ScreenPrint(44, 4, UserScore);
	ScreenPrint(44, 27, "<<< 히트 구간(G)");
	ScreenPrint(44, 28, "<<< 히트 구간(P)");
	ScreenPrint(44, 29, "<<< 히트 구간(G)");
	//콤보
	char strCombo[20];//콤보를 나타낸다
	sprintf(strCombo, "%d 콤보", nCombo);
	ScreenPrint(44, 13, strCombo);
}
// 게임 실행 전 준비화면
void ReadyMap() {
	ScreenPrint(16, 10, "IVE - I AM");
	ScreenPrint(2, 26, "□□□■■■□□□  ■■■□□□■■■");
	ScreenPrint(12, 20, "Press c to Syncmap");
	ScreenPrint(2, 27, "  A     S     D       J     K      L");
	// 게임 조작 키 설명 
}
// Render함수에서 깜빡이면서 출력
void ReadyMap1() {
	SetColor(10);
	ScreenPrint(11, 15, "Press Enter to Start");
	SetColor(15);
}
// 결과화면 출력
void ResultMap()
{
	pChannel[0]->setPaused(true); // 게임이 끝나면 현재 pChannel[0]에 재생중인 노래를 일시 정지한다.
	SetColor(9); ScreenPrint(15, 10, "GAME END !"); SetColor(15);
	UserScore = "[User] "; // 사용자 점수를 나타냄
	UserScore += to_string(nScore);

	if (nScore >= bestScore) {
		BestScore = "[Best] ";
		bestScore = nScore;
		BestScore += to_string(bestScore);
	}

	SetColor(14);
	ScreenPrint(13, 12, BestScore);
	SetColor(15);
	ScreenPrint(13, 14, UserScore);
	SetColor(10);
	ScreenPrint(10, 18, "Press Enter to Restart");
	ScreenPrint(13, 20, "Press q to Exit");
	SetColor(15);
}
// 싱크화면 출력
void SyncMap()
{
	ScreenPrint(4, 15, "Press ← → to tune the Sync! : "+to_string(Syncnum));
	ScreenPrint(10, 20, "Press Enter to Start ");
}

// 초기화 함수
//초기 시간과 노트들 그리고 스코어 초기화
void init() {
	curNoteIndex = 0; 
	nScore = 0; //
	nCombo = 0;
	Control.MovTime = 52;
	Control.OldTime = 0;
	Control.nMagic = 1;
	Stage = READY;
	RunningTime = 0;
	NoteInit();
	SyncTime = 0;
	PauseTime = 0;
}
//키와 노트 string를 KeyNote구조체에 초기화 시켜주는 함수
void KeyIndexInit() {
	//index는 입력받은 키의 종류 (note.h에 상수로 선언)
	KeyIndex[none].inputKey = "none";
	KeyIndex[none].nKey = "                                      ";
	KeyIndex[a].inputKey = "a";
	KeyIndex[a].nKey = "■■■";
	KeyIndex[s].inputKey = "s";
	KeyIndex[s].nKey = "      ■■■";
	KeyIndex[d].inputKey = "d";
	KeyIndex[d].nKey = "            ■■■";
	KeyIndex[j].inputKey = "j";
	KeyIndex[j].nKey = "                    ■■■";
	KeyIndex[k].inputKey = "k";
	KeyIndex[k].nKey = "                          ■■■";
	KeyIndex[l].inputKey = "l";
	KeyIndex[l].nKey = "                                ■■■";
	KeyIndex[aj].inputKey = "aj";
	KeyIndex[aj].nKey = "■■■              ■■■";
	KeyIndex[ks].inputKey = "ks";
	KeyIndex[ks].nKey = "      ■■■              ■■■";
	KeyIndex[dl].inputKey = "dl";
	KeyIndex[dl].nKey = "            ■■■              ■■■";
	KeyIndex[as].inputKey = "as";
	KeyIndex[as].nKey = "■■■■■■";
	KeyIndex[ad].inputKey = "ad";
	KeyIndex[ad].nKey = "■■■      ■■■";
	KeyIndex[ak].inputKey = "ak";
	KeyIndex[ak].nKey = "■■■                    ■■■";
	KeyIndex[al].inputKey = "al";
	KeyIndex[al].nKey = "■■■                          ■■■";
	KeyIndex[sd].inputKey = "sd";
	KeyIndex[sd].nKey = "      ■■■■■■";
	KeyIndex[sj].inputKey = "sj";
	KeyIndex[sj].nKey = "      ■■■        ■■■";
	KeyIndex[sl].inputKey = "sl";
	KeyIndex[sl].nKey = "      ■■■                    ■■■";
	KeyIndex[dj].inputKey = "dj";
	KeyIndex[dj].nKey = "            ■■■  ■■■";
	KeyIndex[dk].inputKey = "dk";
	KeyIndex[dk].nKey = "            ■■■        ■■■";
	KeyIndex[jk].inputKey = "jk";
	KeyIndex[jk].nKey = "                    ■■■■■■";
	KeyIndex[jl].inputKey = "jl";
	KeyIndex[jl].nKey = "                    ■■■      ■■■";
	KeyIndex[kl].inputKey = "kl";
	KeyIndex[kl].nKey = "                          ■■■■■■";
}
// 악보
void NoteInit(void) {
	for (int i = 0; i < ALLNOTE; i++) {
		Note[i] = " ";
	}
	// Part1 : 4초 ~ 17초
	Note[37 + 15 + Control.nMagic] = nKeyD;  // 다른
	Note[41 + 15 + Control.nMagic] = nKeyS;
	Note[45 + 15 + Control.nMagic] = nKeyJ;  // 문을
	Note[47 + 15 + Control.nMagic] = nKeyK;
	Note[51 + 15 + Control.nMagic] = nKeyAL; // 열어
	Note[56 + 15 + Control.nMagic] = nKeyDJ;

	Note[75 + 15 + Control.nMagic] = nKeyA;  // 따라
	Note[78 + 15 + Control.nMagic] = nKeyS;
	Note[82 + 15 + Control.nMagic] = nKeyD;  // 갈
	Note[83 + 15 + Control.nMagic] = nKeyL;
	Note[86 + 15 + Control.nMagic] = nKeyK;  // 필욘
	Note[89 + 15 + Control.nMagic] = nKeyJ;  // 없어
	Note[93 + 15 + Control.nMagic] = nKeyDJ;
	
	Note[114 + 15 + Control.nMagic] = nKeyA; // 넌 너의 길로
	Note[118 + 15 + Control.nMagic] = nKeyD;
	Note[122 + 15 + Control.nMagic] = nKeyS;
	Note[132 + 15 + Control.nMagic] = nKeyK; // 난 나의 길로
	Note[135 + 15 + Control.nMagic] = nKeyJ;
	Note[138 + 15 + Control.nMagic] = nKeyL;

	Note[150 + 15 + Control.nMagic] = nKeyDJ; // 음
	Note[154 + 15 + Control.nMagic] = nKeySK;
	Note[160 + 15 + Control.nMagic] = nKeyAL;

	Note[184 + 12 + Control.nMagic] = nKeyA;  // 하루하루마다
	Note[186 + 12 + Control.nMagic] = nKeyL;
	Note[188 + 12 + Control.nMagic] = nKeyD;
	Note[190 + 12 + Control.nMagic] = nKeyJ;
	Note[192 + 12 + Control.nMagic] = nKeySK;
	Note[194 + 12 + Control.nMagic] = nKeySK;

	Note[212 + 12 + Control.nMagic] = nKeyKL; // 색이 달라진 느낌
	Note[216 + 12 + Control.nMagic] = nKeyAS;
	Note[220 + 12 + Control.nMagic] = nKeyJ;
	Note[224 + 12 + Control.nMagic] = nKeyD;
	Note[228 + 12 + Control.nMagic] = nKeyS;
	Note[234 + 12 + Control.nMagic] = nKeyK;

	// Part2 : 17초 ~ 36초
	Note[236 + 25 + Control.nMagic] = nKeyA;  // 밝게 (17초)
	Note[238 + 25 + Control.nMagic] = nKeyS;
	Note[241 + 25 + Control.nMagic] = nKeyK;  // 빛이
	Note[243 + 25 + Control.nMagic] = nKeyJ;
	Note[246 + 25 + Control.nMagic] = nKeyAJ; // 나는
	Note[248 + 25 + Control.nMagic] = nKeyAJ;
	
	Note[259 + 25 + Control.nMagic] = nKeySK; // 길
	Note[261 + 25 + Control.nMagic] = nKeySK; // 을
	Note[263 + 25 + Control.nMagic] = nKeySK; // 찾
	Note[265 + 25 + Control.nMagic] = nKeySK; // 아

	Note[274 + 25 + Control.nMagic] = nKeyK;  // I'm
	Note[279 + 25 + Control.nMagic] = nKeyL;  // on
	Note[284 + 25 + Control.nMagic] = nKeyAJ; // my

	Note[291 + 25 + Control.nMagic] = nKeyAJ; // way~ (21초)
	Note[301 + 30 + Control.nMagic] = nKeyA;
	Note[303 + 30 + Control.nMagic] = nKeyS;
	Note[305 + 30 + Control.nMagic] = nKeySK; // way~ (22초)
	Note[315 + 30 + Control.nMagic] = nKeyS;
	Note[317 + 30 + Control.nMagic] = nKeyD;
	Note[319 + 30 + Control.nMagic] = nKeyDL; // way~ (23초)

	Note[337 + 33 + Control.nMagic] = nKeyK;  // MR 화음 (24초)
	Note[338 + 33 + Control.nMagic] = nKeyJ;
	Note[339 + 33 + Control.nMagic] = nKeyK;
	Note[340 + 33 + Control.nMagic] = nKeyL;

	Note[354 + 33 + Control.nMagic] = nKeySK; // 넌 (25초)
	Note[359 + 33 + Control.nMagic] = nKeyS;  // 그
	Note[364 + 33 + Control.nMagic] = nKeyS;  // 냥
	Note[369 + 33 + Control.nMagic] = nKeyL;  // 믿
	Note[374 + 33 + Control.nMagic] = nKeyK;  // 으
	Note[379 + 33 + Control.nMagic] = nKeyK;  // 면
	Note[384 + 33 + Control.nMagic] = nKeyAJ; // 돼

	Note[400 + 33 + Control.nMagic] = nKeyK;  // I'm
	Note[405 + 33 + Control.nMagic] = nKeyL;  // on
	Note[410 + 33 + Control.nMagic] = nKeyAJ; // my

	Note[421 + 33 + Control.nMagic] = nKeyAJ; // way~ (29초)
	Note[433 + 33 + Control.nMagic] = nKeyA;
	Note[435 + 33 + Control.nMagic] = nKeyS;
	Note[437 + 33 + Control.nMagic] = nKeySK; // way~ (30초)
	Note[446 + 33 + Control.nMagic] = nKeyS;
	Note[448 + 33 + Control.nMagic] = nKeyD;
	Note[453 + 33 + Control.nMagic] = nKeyDL; // way~ (31초)

	Note[467 + 33 + Control.nMagic] = nKeyK;  // MR 화음 (32초)
	Note[468 + 33 + Control.nMagic] = nKeyL;
	Note[469 + 33 + Control.nMagic] = nKeyK;
	Note[470 + 33 + Control.nMagic] = nKeyJ;

	Note[480 + 40 + Control.nMagic] = nKeySK; // 보 (33초)
	Note[485 + 40 + Control.nMagic] = nKeyS;  // 이
	Note[490 + 40 + Control.nMagic] = nKeyS;  // 는
	Note[495 + 40 + Control.nMagic] = nKeyDL; // 그
	Note[500 + 40 + Control.nMagic] = nKeyA;  // 대
	Note[505 + 40 + Control.nMagic] = nKeyS;  // 로
	Note[510 + 40 + Control.nMagic] = nKeySK; // 야
	
	// Part3 : 36초 ~ 52초
	Note[599 - 40 + Control.nMagic] = nKeyAL; // mr
	Note[607 - 40 + Control.nMagic] = nKeyAL;
	Note[615 - 40 + Control.nMagic] = nKeyDJ;

	Note[626 - 40 + Control.nMagic] = nKeyAS; // 너
	Note[628 - 40 + Control.nMagic] = nKeyKL; // 어
	Note[630 - 40 + Control.nMagic] = nKeySD; // 느
	Note[632 - 40 + Control.nMagic] = nKeyJK; // 은

	Note[642 - 40 + Control.nMagic] = nKeySD; // 누
	Note[644 - 40 + Control.nMagic] = nKeyJK; // 군
	Note[646 - 40 + Control.nMagic] = nKeyAS; // 가
	Note[648 - 40 + Control.nMagic] = nKeyKL; // 의

	Note[656 - 40 + Control.nMagic] = nKeyA;  // dreams
	Note[659 - 40 + Control.nMagic] = nKeyS;
	Note[662 - 40 + Control.nMagic] = nKeyA;

	Note[665 - 40 + Control.nMagic] = nKeyL; // come true
	Note[668 - 40 + Control.nMagic] = nKeyK;
	Note[671 - 40 + Control.nMagic] = nKeyL;
	Note[674 - 40 + Control.nMagic] = nKeyDJ;

	Note[682 - 40 + Control.nMagic] = nKeyS; // 제일
	Note[686 - 40 + Control.nMagic] = nKeyD;
	Note[690 - 40 + Control.nMagic] = nKeyJ;
	Note[694 - 40 + Control.nMagic] = nKeyD;

	Note[702 - 40 + Control.nMagic] = nKeyK; // 좋은 
	Note[706 - 40 + Control.nMagic] = nKeyJ;
	Note[710 - 40 + Control.nMagic] = nKeyD;
	Note[714 - 40 + Control.nMagic] = nKeyJ;

	Note[728 - 40 + Control.nMagic] = nKeyD; // 어느날의
	Note[731 - 40 + Control.nMagic] = nKeyJ;
	Note[734 - 40 + Control.nMagic] = nKeyD;
	Note[737 - 40 + Control.nMagic] = nKeyJ;
	Note[740 - 40 + Control.nMagic] = nKeyD;
	Note[743 - 40 + Control.nMagic] = nKeyJ;
	Note[748 - 40 + Control.nMagic] = nKeyAL;

	Note[760 - 40 + Control.nMagic] = nKeyA; // 머물고픈
	Note[763 - 40 + Control.nMagic] = nKeyL;
	Note[766 - 40 + Control.nMagic] = nKeyS;
	Note[769 - 40 + Control.nMagic] = nKeyK;

	Note[778 - 40 + Control.nMagic] = nKeyD; // 어딘가의 
	Note[781 - 40 + Control.nMagic] = nKeyJ;
	Note[784 - 40 + Control.nMagic] = nKeyA;
	Note[787 - 40 + Control.nMagic] = nKeyL;

	Note[798 - 40 + Control.nMagic] = nKeyAJ; // 낮
	Note[806 - 40 + Control.nMagic] = nKeyDL; // 선
	Note[814 - 40 + Control.nMagic] = nKeySK; // 뷰

	Note[828 - 40 + Control.nMagic] = nKeyAL; // mr

	Note[838 - 40 + Control.nMagic] = nKeyA;  // I'II 
	Note[841 - 40 + Control.nMagic] = nKeyA;
	Note[844 - 40 + Control.nMagic] = nKeyA;

	Note[847 - 40 + Control.nMagic] = nKeyL; // be
	Note[850 - 40 + Control.nMagic] = nKeyL;
	Note[853 - 40 + Control.nMagic] = nKeyL;

	Note[856 - 40 + Control.nMagic] = nKeyS; // far
	Note[859 - 40 + Control.nMagic] = nKeyD;
	Note[861 - 40 + Control.nMagic] = nKeyS;

	Note[864 - 40 + Control.nMagic] = nKeyK; // a~~(어~~)
	Note[867 - 40 + Control.nMagic] = nKeyJ;
	Note[870 - 40 + Control.nMagic] = nKeyK;

	Note[873 - 40 + Control.nMagic] = nKeyA; // wa
	Note[876 - 40 + Control.nMagic] = nKeyS;
	Note[879 - 40 + Control.nMagic] = nKeyD;

	Note[881 - 40 + Control.nMagic] = nKeyL; // y~~~~
	Note[884 - 40 + Control.nMagic] = nKeyK;
	Note[887 - 40 + Control.nMagic] = nKeyJ;

	Note[891 - 40 + Control.nMagic] = nKeyAL; // that
	Note[898 - 40 + Control.nMagic] = nKeyDK; // 's
	Note[902 - 40 + Control.nMagic] = nKeySJ; // my~

}
// 기본 기능 함수
void Update() {
	clock_t Curtime = clock();
	switch (Stage) {
	case READY:
		Oldtime = Curtime;
		break;
	case RUNNING:
		// 게임 시작 후 시간 측정변수
		RunningTime = Curtime - Oldtime - PauseTime - SyncTime;
		break;
	case PAUSE:
		break;
	}
}
void Render(int nkey) {
	clock_t Curtime = clock(); // 지금까지 흐른 시간
	ScreenClear();
	//출력코드

	Map();
	ScoreMap();
	if (Stage == SYNC)
	{
		SyncMap();
	}
	switch (Stage) {
	case READY://대기상태
		Oldtime1 = Curtime;
		ReadyMap();
		if (Curtime % 1000 > 500) {
			ReadyMap1();
		}//0.5초 단위로 화면을 출력
		break;
	case PAUSE:
		return;
	case RUNNING:
		if (RunningTime > 3100) //3초 이후부터
		{
			if (Curtime - Control.OldTime > Control.MovTime)
			{
				Control.OldTime = Curtime;
				curNoteIndex++;//노트가 저장된 배열의 인덱스를 증가
			}
			ShowNote();
		}
		if (RunningTime > 69000)
		{
			Stage = RESULT;
		}
		break;
	case RESULT:
		ResultMap();
		break;
	}

	ScreenFlipping();
}
void Release() {

}

// 키 함수
// 두 번째 키 입력을 받는 함수
string secondkbhit(int inputKey, string inputKeyStr) {
	int inputKey2; // 두 번째로 입력된 키를 받기 위한 변수
	for (int i = 0; i < 1000; i++) {
		if (_kbhit()) {
			inputKey2 = _getch();
			if (inputKey2 == 'a' || inputKey2 == 's' || inputKey2 == 'd' || inputKey2 == 'j' || inputKey2 == 'k' || inputKey2 == 'l') {
				if (inputKey == inputKey2) { //이전 키가 길게 눌렸을 경우 인식하는 것을 방지
					continue;
				}
				else if (inputKey < inputKey2) { //알파벳 순서가 빠른 것을 inputKeyStr에 먼저 배치
					inputKeyStr += inputKey2;
				}
				else {
					inputKeyStr = inputKey2;
					inputKeyStr += inputKey;
				}
				break;
			}
		}
	}
	return inputKeyStr;
}
// 키에 해당하는 노트 문자열을 반환해주는 함수
string GetKeyType(string inputKeyStr) {
	string nKeyStr = "";
	for (int i = 0; i < NumOfKey; i++) {
		if (inputKeyStr == KeyIndex[i].inputKey) {
			nKeyStr = KeyIndex[i].nKey;
			break;
		}
	}
	return nKeyStr;
}
// 현재 노트가 두 개의 노트인지 확인해주는 함수
int isTwoKey(string nKeyStr) {
	for (int i = 0; i < NumOfKey; i++) {
		if (nKeyStr == KeyIndex[i].nKey) { //note가 KeyIndex 구조체의 nKey string과 같다면
			if (i >= aj) { //상수 aj보다 크다면 두 개의 노트를 가지고 있으므로 1 리턴
				return 1;
			}
			else {
				return 0;
			}
		}
	}
	return 0;
}
// 충돌처리
void CheckKey(string inputKeyStr) {
	string nKeyStr; // 입력한 키의 종류
	nKeyStr = GetKeyType(inputKeyStr);
	if (Note[curNoteIndex] == nKeyStr) { // Perfect판별 구간의 Note와 입력한 KeyType가 일치하는 경우
		nScore += 500;
		nCombo++;
		if (nCombo >= 10) {
			nScore += nCombo * 50;
		}
		Note[curNoteIndex] = HitNote(nKeyStr);
		sprintf(strScore, "%s", "★Perfect★");
	}
	else if ((curNoteIndex > 0 && (Note[curNoteIndex - 1] == nKeyStr)) || (Note[curNoteIndex + 1] == nKeyStr)) { // Great 판별 구간의 Note와 입력한 KeyType가 일치하는 경우
		nScore += 300;
		nCombo++;
		if (nCombo >= 10) {
			nScore += nCombo * 30;
		}
		Note[curNoteIndex + 1] = HitNote(nKeyStr);
		Note[curNoteIndex - 1] = HitNote(nKeyStr);
		sprintf(strScore, "%s", "★Great★");
	}
	else {
		nCombo = 0;
	}
}

// 노트 함수
// 2차원 배열을 아래로 떨어지게끔 해주는 함수
void ShowNote() {
	for (int i = 0; i < 27; i++) {
		if (28 - i >= 27) { // 히트 라인 밑으로 내려간 블록 전부다 노란색으로 변경.
			SetColor(14);
		}
		else SetColor(15);
		ScreenPrint(2, 28 - i, Note[curNoteIndex + i]);
	}
}
// 히트를 성공한 노트를 히트된 노트 string으로 반환
string HitNote(string inputKey) {
	for (int i = 0; i < inputKey.length(); i++) {
		if (inputKey[i] != ' ') {
			inputKey[i] = 'oo';
		}
	}
	return inputKey;
}

// 싱크 함수
void ControlSync(int inputKey) {
	if (inputKey == LEFT) { // 싱크 줄이기
		if (Syncnum <= -30) {
			Syncnum = Syncnum;
		}
		else {
			Syncnum--;
		}
	}
	else  { // 싱크 높이기
		if (Syncnum >= 30) {
			Syncnum = Syncnum;
		}
		else {
			Syncnum++;
		}
	}
	Control.nMagic = Syncnum;
}
