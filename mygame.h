/*
 * mygame.h: ���ɮ��x�C��������class��interface
 * Copyright (C) 2002-2008 Woei-Kae Chen <wkc@csie.ntut.edu.tw>
 *
 * This file is part of game, a free game development framework for windows.
 *
 * game is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * game is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	 2004-03-02 V4.0
 *      1. Add CGameStateInit, CGameStateRun, and CGameStateOver to
 *         demonstrate the use of states.
 *   2005-09-13
 *      Rewrite the codes for CBall and CEraser.
 *   2005-09-20 V4.2Beta1.
 *   2005-09-29 V4.2Beta2.
 *   2006-02-08 V4.2
 *      1. Rename OnInitialUpdate() -> OnInit().
 *      2. Replace AUDIO_CANYON as AUDIO_NTUT.
 *      3. Add help bitmap to CGameStateRun.
 *   2006-09-09 V4.3
 *      1. Rename Move() and Show() as OnMove and OnShow() to emphasize that they are
 *         event driven.
 *   2008-02-15 V4.4
 *      1. Add namespace game_framework.
 *      2. Replace the demonstration of animation as a new bouncing ball.
 *      3. Use ShowInitProgress(percent) to display loading progress. 
*/

/////////////////////////////////////////////////////////////////////////////
// Constants
/////////////////////////////////////////////////////////////////////////////
// �w�q�U�ح��Ī��s��
enum AUDIO_ID {
	AUDIO_STBGS,				// 0 - �C���}�l�e���I������
	AUDIO_SELECT,				// 1 - ��歵��
	AUDIO_SCUESS,				// 2 - ���\�L������
	AUDIO_FAILED,				// 3 - ����(GAME OVER)��������
	AUDIO_PICK,					// 4 - �߰_�_���n��
	AUDIO_DOOR,					// 5 - �}���n��
	AUDIO_MAP1,					// 6 - �a��1����
	AUDIO_MAP2,					// 7 - �a��2����
	AUDIO_MAP3,					// 8 - �a��3����
	AUDIO_MAP4,					// 9 - �a��4����
	AUDIO_MAP5,					// 10- �a��5����
	AUDIO_KEY					// 11 - �ߨ�KEY����
};

namespace game_framework {
/////////////////////////////////////////////////////////////////////////////
// �o��class���ѥi�H����L�ηƹ�������l
// �����N�i�H��g���ۤv���{���F
/////////////////////////////////////////////////////////////////////////////
//�a�ϳ]�p
/////////////////////////////////////////////////////////////////////////////

class CMap{
public:
	CMap(const char* fn);
	void Initialize(const char* fn);
	void LoadMap();
	void OnShow();
    void MoveMap(char direction);
	int Isempty(int x,int y);
	int DetialDoor(int x,int y);
	int computsx(int x);
	int computsy(int y);
	int Getcoutobstacle();
	int Getcounttreasure();
	int Getcountdoors();
	int Getcountkeys();
	char KeyColor(int x,int y);
	void Setsxsy(int x,int y);
	void toempty(int x,int y);
	void setdoor(int x,int y,char type);
private:
	CMovingBitmap background;
	char map[160][120];
	int sx,sy;
	int countobstacle;
	int counttreasures;
	int countdoors;
	int countkeys;
	void readmapfromfile(const char* fn);
};

/////////////////////////////////////////////////////////////////////////////
//��ê������
/////////////////////////////////////////////////////////////////////////////

class Obstacle{
public:
	Obstacle();
	void OnShow(CMap* gamemap);
	void Load();
	void setxy(int setx, int sety);
private:
	int x,y;
	CMovingBitmap bmp;
};

/////////////////////////////////////////////////////////////////////////////
//�����I����
/////////////////////////////////////////////////////////////////////////////
class MovingNode{
public:
	MovingNode(int sx, int sy);
	int Getx();
	int Gety();
	int Getg();
	int Getf();
	MovingNode* GetParent();
	void caculateparameter(const int sx, const int sy,const int tx,const int ty);
	void setparent(MovingNode* settarget);
private:
	int x,y;
	int f,g,h;
	MovingNode* parent;
	bool isInlist;
};

/////////////////////////////////////////////////////////////////////////////
//�ɶ�����
/////////////////////////////////////////////////////////////////////////////
//�NCInteger�ର�ɤ����,�Ω����d�ɶ��p��
class CintTime{
public:
	CintTime();
	void Initialize();//��l��
	void SetTime(int);//�]�w�ɶ�,����
	int GetTime();//���o�ثe�ɶ�,����
	void LoadBitmap(bool CTFirst);
	void OnShow();//��ܹϧ�
private:
	void calTime();//�N����ഫ���ɤ����
	int min,sec;
	CInteger	Cint;

};

/////////////////////////////////////////////////////////////////////////////
//Door
/////////////////////////////////////////////////////////////////////////////

class Door{
public:
	Door();
	void Initialize(CMap* gamemap);
	void LoadDoor();
	void setClose(bool closeflag,CMap* gamemap);
	bool HitObject(int sx,int sy);
	void setDoor(int sx,int sy,char type,char color);
	bool GetisClose();
	void OnShow(CMap* gamemap);
	char Getdoortype();
	char Getdoorcolor();
private:
	bool isClose;
	bool isVertical;
	int x,y;
	static const int doorsize = 8;
	CMovingBitmap doorpicture;
	char doortype;
	char doorcolor;
};

/////////////////////////////////////////////////////////////////////////////
//Key
/////////////////////////////////////////////////////////////////////////////

class Key{
public:
	Key();
	void Initialize();
	void Onshow(CMap* gamemap);
	void setKey(int sx,int sy,char color);
	void LoadKey();
	char getColor();
	bool hitThief(int sx,int sy);
	bool getisOnMap();
	void setOnMap(bool flag);
private:
	int x,y;
	static const int width =2;
	static const int high =2;
	bool isOnMap;
	char keycolor;
	CMovingBitmap keypicture;
};

/////////////////////////////////////////////////////////////////////////////
//�s�骫��
/////////////////////////////////////////////////////////////////////////////

class Thief{
public:
	Thief();
	void Initialize();				// �]�w���l����l��
	void LoadBitmap();				// ���J�ϧ�
	void OnMove(CMap* gamemap,Door* door,Key* key);		// ���ʥD��
	void OnShow(CMap* gamemap);			// �N���l�ϧζK��e��
	void SetMovingDown(bool flag);	// �]�w�O�_���b���U����
	void SetMovingLeft(bool flag);	// �]�w�O�_���b��������
	void SetMovingRight(bool flag); // �]�w�O�_���b���k����
	void SetMovingUp(bool flag);	// �]�w�O�_���b���W����
	void SetXY(int sx,int sy);		// �]�w�p���K�ϥ��W���y��
	void ChangeCounter(int counter);  //������ܼƦr����k
	void reststep();
	int Getx();
	int Gety();
	
protected:
	CMovingBitmap rogue[4][4];		// ���l���ʵe
	CMovingBitmap *play;
	int step;					//�樫���A
	int x,y;					//�ù��y��
	bool isMovingDown;			// �O�_���b���U����
	bool isMovingLeft;			// �O�_���b��������
	bool isMovingRight;			// �O�_���b���k����
	bool isMovingUp;			// �O�_���b���W����
	void contralstep(int direction);
	bool checkempty(char direction,CMap* Gamemap,Key *key,Door *door);
	CMovingBitmap Tcounter[10];
	bool isGetallTreasure;
	vector <Key*> Getkeys;
};

/////////////////////////////////////////////////////////////////////////////
//���~����
/////////////////////////////////////////////////////////////////////////////

class Monster{
public:
	Monster();
	void Initialize();				// �]�w���l����l��
	void LoadBitmap();				// ���J�ϧ�
	void OnMove(CMap* gamemap,Thief* thief,Door* door,bool cheatMode);		// ���ʩ��~
	void OnShow(CMap* gamemap);		// �N���l�ϧζK��e��
	void reststep();
	int Getx();
	int Gety();
	void SetXY(int x, int y);
	void Setcommenpath( vector<MovingNode*> &scriptpath);
	bool GetisTrace();
protected:
	CMovingBitmap guide[4][4];		// ���l���ʵe
	CMovingBitmap *monster_1;
	int mstep;					//�樫���A
	int gx,gy;					//ĵ�éҦb�y��
	static const int sightrange = 10;
	unsigned int nowstep;
	char nowdirection;
	vector<MovingNode*> path;
	vector<MovingNode*> returnpath;
	vector<MovingNode*> commenpath;
	void findpath(const int tx, const int ty,CMap* gamemap);
	bool isMoving;
	bool isTrace;
	bool isBack;
	bool isComeback;      //���`�b�J�w���|�����ݮ� �Otrue ��ܭn�v���^��_�l��  
	void contralstep(int direction);
	bool checkempty(int x, int y,char direction,CMap* Gamemap);
	bool looksight(Thief* thief,CMap* gamemap,bool cheatmode);
	int mcounter;
	unsigned int commenwalkstep;
	bool opendoor;
	Door *thedoor;
	bool wait;
	int waitcounter;
};

/////////////////////////////////////////////////////////////////////////////
//�_��
/////////////////////////////////////////////////////////////////////////////

class Treasure{
public:
	Treasure();
	void Initialize();
	void LoadBitmap();
	void OnShow(CMap* gamemap);
	void SetXY(int sx,int sy);
	void SetSize(int h, int w);
	void Setlife(bool flag);
	bool HitThief(Thief* thief);
	bool Isalife();
	int Getheight();
	int Getwidth();
	int Getx();
	int Gety();
private:
	int x,y;
	int height,width;
	CMovingBitmap treasure_bitmap;
	bool islife;
};

/////////////////////////////////////////////////////////////////////////////
//���ù�����
/////////////////////////////////////////////////////////////////////////////

class redscreen{
public:
	redscreen();
	void Initialize();
	void LoadBitmap();
	void OnShow();
	void SetisFlash(bool flasflag);
	bool GetisFlash();
private:
	int counter;
	bool isFlash;
	CMovingBitmap redscreen_bitmap;

};

/////////////////////////////////////////////////////////////////////////////
//��������
/////////////////////////////////////////////////////////////////////////////

class help{
public:
	help();
	void Initialize();
	void LoadBitmap();
	void OnShow();
	void SetOk(bool flag);
	bool GetState();
private:
	bool flag;
	CMovingBitmap help_bitmap;

};

/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����C���}�Y�e������
// �C��Member function��Implementation���n����
/////////////////////////////////////////////////////////////////////////////

class CGameStateInit : public CGameState {
public:
	CGameStateInit(CGame *g);
	void OnInit();  								// �C������Ȥιϧγ]�w
	void OnBeginState();							// �]�w�C�������һݪ��ܼ�
	void OnKeyUp(UINT, UINT, UINT); 				// �B�z��LUp���ʧ@
protected:
	void OnShow();									// ��ܳo�Ӫ��A���C���e��
private:
	CMovingBitmap stbg;								//�C���}�l�}�l�����I����
	CMovingBitmap About;							//����s�@����
	CMovingBitmap ExMouse;							//�b�Y�Ϯ�
	bool mu,About_bool;

};

/////////////////////////////////////////////////////////////////////////////
// �o��class���C����������d����
/////////////////////////////////////////////////////////////////////////////

class CGameStateSelect : public CGameState {
public:
	CGameStateSelect(CGame *g);
	void OnInit();  								// �C������Ȥιϧγ]�w
	void OnBeginState();							// �]�w�C�������һݪ��ܼ�
	void OnKeyUp(UINT, UINT, UINT); 				// �B�z��LUp���ʧ@
protected:
	void OnShow();									// ��ܳo�Ӫ��A���C���e��
private:
	CMovingBitmap Select;						//�C���}�l�}�l�����I����
	CMovingBitmap ExMouse;							//�b�Y�Ϯ�

};

/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����C�����檫��(�Ĥ@��)�A�D�n���C���{�����b�o��
// �C��Member function��Implementation���n����
/////////////////////////////////////////////////////////////////////////////

class CGameStateRun : public CGameState {
public:
	CGameStateRun(CGame *g);
	~CGameStateRun();
	void OnBeginState();							// �]�w�C�������һݪ��ܼ�
	void OnInit();  								// �C������Ȥιϧγ]�w
	void OnKeyDown(UINT, UINT, UINT);
	void OnKeyUp(UINT, UINT, UINT);
protected:
	void OnMove();									// ���ʹC������
	void OnShow();									// ��ܳo�Ӫ��A���C���e��
private:;
    const char      *fn;
	CMap            gamemap;
	Thief			thief;
	Monster			*monster;
	Obstacle        *obstacles;
	Treasure        *treasure;
	Door            *door;
	Key             *key;
	redscreen		redscreen;
	int             treasure_remind;
	int counter;									// �˼Ƥ��p�ƾ�
	static const int limatetime = 306;				// �����d�C���ɶ�
	static const int stage=1;
	bool cheatmode;									// �@���Ҧ����A
	bool dkeybordisdown;
	bool wkeybordisdown;
	int score,time;										//���Z����
	help	help;
	CMovingBitmap	cheat;
	bool togetalltreasure;
	CintTime		Tcc;

};

/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����C�����檫��(�ĤG��)�A�D�n���C���{�����b�o��
// �C��Member function��Implementation���n����
/////////////////////////////////////////////////////////////////////////////

class CGameStateRun2 : public CGameState {
public:
	CGameStateRun2(CGame *g);
	~CGameStateRun2();
	void OnBeginState();							// �]�w�C�������һݪ��ܼ�
	void OnInit();  								// �C������Ȥιϧγ]�w
	void OnKeyDown(UINT, UINT, UINT);
	void OnKeyUp(UINT, UINT, UINT);
protected:
	void OnMove();									// ���ʹC������
	void OnShow();									// ��ܳo�Ӫ��A���C���e��
private:;
    const char      *fn;
	CMap            gamemap;
	Thief			thief;
	Monster			*monster;
	Obstacle        *obstacles;
	Treasure        *treasure;
	Door            *door;
	Key             *key;
	redscreen		redscreen;
	int             treasure_remind;
	int counter;									// �˼Ƥ��p�ƾ�
	static const int limatetime = 533;				// �����d�C���ɶ�
	static const int stage=2;
	bool cheatmode;									// �@���Ҧ����A
	int score,time;									//���Z����
	help	help;
	CMovingBitmap	cheat;
	CintTime		Tcc;
	bool togetalltreasure;
	bool dkeybordisdown;
	bool wkeybordisdown;
};


/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����C�����檫��(�L���e��)
/////////////////////////////////////////////////////////////////////////////
class CGameStatePASS : public CGameState {
public:
	CGameStatePASS(CGame *g);
	void OnInit();  								// �C������Ȥιϧγ]�w
protected:
	void OnBeginState();
	void CalST();								//�p��ɶ��P����
	void OnKeyUp(UINT, UINT, UINT);
	void OnShow();									// ��ܳo�Ӫ��A���C���e��
	void readfromfile();
private:
	int counter;									// �˼Ƥ��p�ƾ�
	//�x�s�L�������n��ܪ���T
	int min,sec;
	int time;//�x�s�নint���`���
	int score;
	int min1,min2;
	int sec1,sec2;
	int stage;//���d��m
	CMovingBitmap	PASS;							//�C���}�l�}�l�����I����
	CMovingBitmap	t1;
	CMovingBitmap	t2;
	CMovingBitmap	t3;
	CMovingBitmap	t4;
	CMovingBitmap	t5;
	CMovingBitmap	t6;
	CMovingBitmap	t7;
	CMovingBitmap	t8;
	CMovingBitmap	t9;
	CMovingBitmap	t0;
	CIntegerST		CiST;
	bool mu;

};

/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����}�����A(Game WIN)
/////////////////////////////////////////////////////////////////////////////

class CGameStateWIN : public CGameState {
public:
	CGameStateWIN(CGame *g);
	void OnBeginState();							// �]�w�C�������һݪ��ܼ�
	void OnInit();
protected:
	void OnKeyUp(UINT, UINT, UINT);
	void OnMove();									// ���ʹC������
	void OnShow();									// ��ܳo�Ӫ��A���C���e��
private:
	int counter;									// �˼Ƥ��p�ƾ�
	CMovingBitmap	WIN;
};


/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����������A(Game Over)
// �C��Member function��Implementation���n����
/////////////////////////////////////////////////////////////////////////////

class CGameStateOver : public CGameState {
public:
	CGameStateOver(CGame *g);
	void OnBeginState();							// �]�w�C�������һݪ��ܼ�
	void OnInit();
protected:
	void OnKeyUp(UINT, UINT, UINT);
	void OnMove();									// ���ʹC������
	void OnShow();									// ��ܳo�Ӫ��A���C���e��
private:
	int counter;									// �˼Ƥ��p�ƾ�
	CMovingBitmap	Over;
};

}