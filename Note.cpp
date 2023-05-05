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

//���� �Լ�
void SoundSystem() {
	System_Create(&pSystem);
	pSystem->init(4, FMOD_INIT_NORMAL, NULL);
	pSystem->createSound("Main_BGM.wav", FMOD_LOOP_NORMAL | FMOD_HARDWARE, NULL, &pSound[0]); // ����������
	pSystem->createSound("Game_BGM.mp3", FMOD_DEFAULT, NULL, &pSound[1]); // ��������
}
void Play(int Sound_num) {
	pSystem->playSound(FMOD_CHANNEL_FREE, pSound[Sound_num], 0, pChannel);
}

// �� �Լ�
// �������� �⺻ Ʋ
void Map(void) {
	int nNum = 0;
	ScreenPrint(0, 0, "����������������������");
	for (int i = 1; i < 29; i++) {
		ScreenPrint(0, i, "��\t\t\t\t\t��");
	}
	ScreenPrint(0, 29, "����������������������");
	ScreenPrint(2, 26, "______________________________________");
}
// ���� ���� ���Ʋ
void ScoreMap() {
	// ����ð�
	char nTime[20];//��� �ð��� ��Ÿ����
	sprintf(nTime, "�ð� : %d.%d��", RunningTime / 1000, RunningTime % 1000);
	ScreenPrint(44, 2, nTime);
	// ���� ���
	if (strcmp(strScore, "��Perfect��") == 0) {
		SetColor(14);
	}
	else if (strcmp(strScore, "��Great��") == 0) {
		SetColor(9);
	}
	ScreenPrint(44, 10, strScore);//Great,Perfect�Ǻ�
	SetColor(15);
	ScreenPrint(44, 22, "Great : 300��");
	ScreenPrint(44, 23, "Perfect : 500��");
	SetColor(12);
	ScreenPrint(44, 6, "Press \'p\' to Pause");
	SetColor(15);
	// ����
	char UserScore[20];//����� ������ ��Ÿ��
	sprintf(UserScore, "���� : %d ��", nScore);
	ScreenPrint(44, 4, UserScore);
	ScreenPrint(44, 27, "<<< ��Ʈ ����(G)");
	ScreenPrint(44, 28, "<<< ��Ʈ ����(P)");
	ScreenPrint(44, 29, "<<< ��Ʈ ����(G)");
	//�޺�
	char strCombo[20];//�޺��� ��Ÿ����
	sprintf(strCombo, "%d �޺�", nCombo);
	ScreenPrint(44, 13, strCombo);
}
// ���� ���� �� �غ�ȭ��
void ReadyMap() {
	ScreenPrint(16, 10, "IVE - I AM");
	ScreenPrint(2, 26, "����������  ����������");
	ScreenPrint(12, 20, "Press c to Syncmap");
	ScreenPrint(2, 27, "  A     S     D       J     K      L");
	// ���� ���� Ű ���� 
}
// Render�Լ����� �����̸鼭 ���
void ReadyMap1() {
	SetColor(10);
	ScreenPrint(11, 15, "Press Enter to Start");
	SetColor(15);
}
// ���ȭ�� ���
void ResultMap()
{
	pChannel[0]->setPaused(true); // ������ ������ ���� pChannel[0]�� ������� �뷡�� �Ͻ� �����Ѵ�.
	SetColor(9); ScreenPrint(15, 10, "GAME END !"); SetColor(15);
	UserScore = "[User] "; // ����� ������ ��Ÿ��
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
// ��ũȭ�� ���
void SyncMap()
{
	ScreenPrint(4, 15, "Press �� �� to tune the Sync! : "+to_string(Syncnum));
	ScreenPrint(10, 20, "Press Enter to Start ");
}

// �ʱ�ȭ �Լ�
//�ʱ� �ð��� ��Ʈ�� �׸��� ���ھ� �ʱ�ȭ
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
//Ű�� ��Ʈ string�� KeyNote����ü�� �ʱ�ȭ �����ִ� �Լ�
void KeyIndexInit() {
	//index�� �Է¹��� Ű�� ���� (note.h�� ����� ����)
	KeyIndex[none].inputKey = "none";
	KeyIndex[none].nKey = "                                      ";
	KeyIndex[a].inputKey = "a";
	KeyIndex[a].nKey = "����";
	KeyIndex[s].inputKey = "s";
	KeyIndex[s].nKey = "      ����";
	KeyIndex[d].inputKey = "d";
	KeyIndex[d].nKey = "            ����";
	KeyIndex[j].inputKey = "j";
	KeyIndex[j].nKey = "                    ����";
	KeyIndex[k].inputKey = "k";
	KeyIndex[k].nKey = "                          ����";
	KeyIndex[l].inputKey = "l";
	KeyIndex[l].nKey = "                                ����";
	KeyIndex[aj].inputKey = "aj";
	KeyIndex[aj].nKey = "����              ����";
	KeyIndex[ks].inputKey = "ks";
	KeyIndex[ks].nKey = "      ����              ����";
	KeyIndex[dl].inputKey = "dl";
	KeyIndex[dl].nKey = "            ����              ����";
}
// �Ǻ�
void NoteInit(void) {
	for (int i = 0; i < ALLNOTE; i++) {
		Note[i] = " ";
	}

	// Part2 : 17�� ~ 36��
	
	Note[236 + Control.nMagic] = nKeyA;  // ��� (17��)
	Note[238 + Control.nMagic] = nKeyS;
	Note[241 + Control.nMagic] = nKeyK;  // ����
	Note[243 + Control.nMagic] = nKeyJ;
	Note[246 + Control.nMagic] = nKeyAJ; // ����
	Note[248 + Control.nMagic] = nKeyAJ;
	
	Note[259 + Control.nMagic] = nKeySK; // ��
	Note[261 + Control.nMagic] = nKeySK; // ��
	Note[263 + Control.nMagic] = nKeySK; // ã
	Note[265 + Control.nMagic] = nKeySK; // ��

	Note[274 + Control.nMagic] = nKeyK;  // I'm on my
	Note[279 + Control.nMagic] = nKeyL;  // on
	Note[284 + Control.nMagic] = nKeyAJ; // my
	
	Note[291 + Control.nMagic] = nKeyAJ; // way~ (21��)
	Note[301 + Control.nMagic] = nKeyA;
	Note[303 + Control.nMagic] = nKeyS;
	Note[305 + Control.nMagic] = nKeySK; // way~ (22��)
	Note[315 + Control.nMagic] = nKeyS;
	Note[317 + Control.nMagic] = nKeyD;
	Note[319 + Control.nMagic] = nKeyDL; // way~ (23��)

	Note[337 + Control.nMagic] = nKeyK;  // MR ȭ�� (24��)
	Note[338 + Control.nMagic] = nKeyJ;
	Note[339 + Control.nMagic] = nKeyK;
	Note[340 + Control.nMagic] = nKeyL;

	Note[354 + Control.nMagic] = nKeySK; // �� (25��)
	Note[358 + Control.nMagic] = nKeyS;  // ��
	Note[362 + Control.nMagic] = nKeyS;  // ��
	Note[367 + Control.nMagic] = nKeyL;  // ��
	Note[371 + Control.nMagic] = nKeyK;  // ��
	Note[375 + Control.nMagic] = nKeyK;  // ��
	Note[379 + Control.nMagic] = nKeyAJ; // ��
	
	Note[400 + Control.nMagic] = nKeyK;  // I'm
	Note[405 + Control.nMagic] = nKeyL;  // on
	Note[410 + Control.nMagic] = nKeyAJ; // my

	Note[416 + Control.nMagic] = nKeyAJ; // way~ (29��)
	Note[428 + Control.nMagic] = nKeyA;
	Note[430 + Control.nMagic] = nKeyS;
	Note[432 + Control.nMagic] = nKeySK; // way~ (30��)
	Note[441 + Control.nMagic] = nKeyS;
	Note[443 + Control.nMagic] = nKeyD;
	Note[445 + Control.nMagic] = nKeyDL; // way~ (31��)

	Note[460 + Control.nMagic] = nKeyK;  // MR ȭ�� (32��)
	Note[461 + Control.nMagic] = nKeyL;
	Note[462 + Control.nMagic] = nKeyK;
	Note[463 + Control.nMagic] = nKeyJ;

	Note[479 + Control.nMagic] = nKeySK; // �� (33��)
	Note[483 + Control.nMagic] = nKeyS;  // ��
	Note[488 + Control.nMagic] = nKeyS;  // ��
	Note[492 + Control.nMagic] = nKeyDL; // ��
	Note[496 + Control.nMagic] = nKeyA;  // ��
	Note[500 + Control.nMagic] = nKeyS;  // ��
	Note[504 + Control.nMagic] = nKeySK; // ��

}
// �⺻ ��� �Լ�
void Update() {
	clock_t Curtime = clock();
	switch (Stage) {
	case READY:
		Oldtime = Curtime;
		break;
	case RUNNING:
		// ���� ���� �� �ð� ��������
		RunningTime = Curtime - Oldtime - PauseTime - SyncTime;
		break;
	case PAUSE:
		break;
	}
}
void Render(int nkey) {
	clock_t Curtime = clock(); // ���ݱ��� �帥 �ð�
	ScreenClear();
	//����ڵ�

	Map();
	ScoreMap();
	if (Stage == SYNC)
	{
		SyncMap();
	}
	switch (Stage) {
	case READY://������
		Oldtime1 = Curtime;
		ReadyMap();
		if (Curtime % 1000 > 500) {
			ReadyMap1();
		}//0.5�� ������ ȭ���� ���
		break;
	case PAUSE:
		return;
	case RUNNING:
		if (RunningTime > 3100) //3�� ���ĺ���
		{
			if (Curtime - Control.OldTime > Control.MovTime)
			{
				Control.OldTime = Curtime;
				curNoteIndex++;//��Ʈ�� ����� �迭�� �ε����� ����
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

// Ű �Լ�
// �� ��° Ű �Է��� �޴� �Լ�
string secondkbhit(int inputKey, string inputKeyStr) {
	int inputKey2; // �� ��°�� �Էµ� Ű�� �ޱ� ���� ����
	for (int i = 0; i < 1000; i++) {
		if (_kbhit()) {
			inputKey2 = _getch();
			if (inputKey2 == 'a' || inputKey2 == 's' || inputKey2 == 'd' || inputKey2 == 'j' || inputKey2 == 'k' || inputKey2 == 'l') {
				if (inputKey == inputKey2) { //���� Ű�� ��� ������ ��� �ν��ϴ� ���� ����
					continue;
				}
				else if (inputKey < inputKey2) { //���ĺ� ������ ���� ���� inputKeyStr�� ���� ��ġ
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
// Ű�� �ش��ϴ� ��Ʈ ���ڿ��� ��ȯ���ִ� �Լ�
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
// ���� ��Ʈ�� �� ���� ��Ʈ���� Ȯ�����ִ� �Լ�
int isTwoKey(string nKeyStr) {
	for (int i = 0; i < NumOfKey; i++) {
		if (nKeyStr == KeyIndex[i].nKey) { //note�� KeyIndex ����ü�� nKey string�� ���ٸ�
			if (i >= aj) { //��� aj���� ũ�ٸ� �� ���� ��Ʈ�� ������ �����Ƿ� 1 ����
				return 1;
			}
			else {
				return 0;
			}
		}
	}
	return 0;
}
// �浹ó��
void CheckKey(string inputKeyStr) {
	string nKeyStr; // �Է��� Ű�� ����
	nKeyStr = GetKeyType(inputKeyStr);
	if (Note[curNoteIndex] == nKeyStr) { // Perfect�Ǻ� ������ Note�� �Է��� KeyType�� ��ġ�ϴ� ���
		nScore += 500;
		nCombo++;
		if (nCombo >= 10) {
			nScore += nCombo * 50;
		}
		Note[curNoteIndex] = HitNote(nKeyStr);
		sprintf(strScore, "%s", "��Perfect��");
	}
	else if ((curNoteIndex > 0 && (Note[curNoteIndex - 1] == nKeyStr)) || (Note[curNoteIndex + 1] == nKeyStr)) { // Great �Ǻ� ������ Note�� �Է��� KeyType�� ��ġ�ϴ� ���
		nScore += 300;
		nCombo++;
		if (nCombo >= 10) {
			nScore += nCombo * 30;
		}
		Note[curNoteIndex + 1] = HitNote(nKeyStr);
		Note[curNoteIndex - 1] = HitNote(nKeyStr);
		sprintf(strScore, "%s", "��Great��");
	}
	else {
		nCombo = 0;
	}
}

// ��Ʈ �Լ�
// 2���� �迭�� �Ʒ��� �������Բ� ���ִ� �Լ�
void ShowNote() {
	for (int i = 0; i < 27; i++) {
		if (28 - i >= 27) { // ��Ʈ ���� ������ ������ ��� ���δ� ��������� ����.
			SetColor(14);
		}
		else SetColor(15);
		ScreenPrint(2, 28 - i, Note[curNoteIndex + i]);
	}
}
// ��Ʈ�� ������ ��Ʈ�� ��Ʈ�� ��Ʈ string���� ��ȯ
string HitNote(string inputKey) {
	for (int i = 0; i < inputKey.length(); i++) {
		if (inputKey[i] != ' ') {
			inputKey[i] = 'oo';
		}
	}
	return inputKey;
}

// ��ũ �Լ�
void ControlSync(int inputKey) {
	if (inputKey == LEFT) { // ��ũ ���̱�
		if (Syncnum <= -30) {
			Syncnum = Syncnum;
		}
		else {
			Syncnum--;
		}
	}
	else  { // ��ũ ���̱�
		if (Syncnum >= 30) {
			Syncnum = Syncnum;
		}
		else {
			Syncnum++;
		}
	}
	Control.nMagic = Syncnum;
}