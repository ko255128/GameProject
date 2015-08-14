/*
 * mygame.h: 本檔案儲遊戲本身的class的interface
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
// 定義各種音效的編號
enum AUDIO_ID {
	AUDIO_STBGS,				// 0 - 遊戲開始畫面背景音樂
	AUDIO_SELECT,				// 1 - 選單音效
	AUDIO_SCUESS,				// 2 - 成功過關音效
	AUDIO_FAILED,				// 3 - 失敗(GAME OVER)頁面音樂
	AUDIO_PICK,					// 4 - 撿起寶物聲音
	AUDIO_DOOR,					// 5 - 開門聲音
	AUDIO_MAP1,					// 6 - 地圖1音樂
	AUDIO_MAP2,					// 7 - 地圖2音樂
	AUDIO_MAP3,					// 8 - 地圖3音樂
	AUDIO_MAP4,					// 9 - 地圖4音樂
	AUDIO_MAP5,					// 10- 地圖5音樂
	AUDIO_KEY					// 11 - 撿到KEY音樂
};

namespace game_framework {
/////////////////////////////////////////////////////////////////////////////
// 這個class提供可以用鍵盤或滑鼠控制的擦子
// 看懂就可以改寫成自己的程式了
/////////////////////////////////////////////////////////////////////////////
//地圖設計
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
//障礙物物件
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
//移動點物件
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
//時間物件
/////////////////////////////////////////////////////////////////////////////
//將CInteger轉為時分秒制,用於關卡時間計算
class CintTime{
public:
	CintTime();
	void Initialize();//初始化
	void SetTime(int);//設定時間,為秒
	int GetTime();//取得目前時間,為秒
	void LoadBitmap(bool CTFirst);
	void OnShow();//顯示圖形
private:
	void calTime();//將秒數轉換為時分秒制
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
//盜賊物件
/////////////////////////////////////////////////////////////////////////////

class Thief{
public:
	Thief();
	void Initialize();				// 設定擦子為初始值
	void LoadBitmap();				// 載入圖形
	void OnMove(CMap* gamemap,Door* door,Key* key);		// 移動主角
	void OnShow(CMap* gamemap);			// 將擦子圖形貼到畫面
	void SetMovingDown(bool flag);	// 設定是否正在往下移動
	void SetMovingLeft(bool flag);	// 設定是否正在往左移動
	void SetMovingRight(bool flag); // 設定是否正在往右移動
	void SetMovingUp(bool flag);	// 設定是否正在往上移動
	void SetXY(int sx,int sy);		// 設定小偷貼圖左上角座標
	void ChangeCounter(int counter);  //改變顯示數字的方法
	void reststep();
	int Getx();
	int Gety();
	
protected:
	CMovingBitmap rogue[4][4];		// 擦子的動畫
	CMovingBitmap *play;
	int step;					//行走狀態
	int x,y;					//螢幕座標
	bool isMovingDown;			// 是否正在往下移動
	bool isMovingLeft;			// 是否正在往左移動
	bool isMovingRight;			// 是否正在往右移動
	bool isMovingUp;			// 是否正在往上移動
	void contralstep(int direction);
	bool checkempty(char direction,CMap* Gamemap,Key *key,Door *door);
	CMovingBitmap Tcounter[10];
	bool isGetallTreasure;
	vector <Key*> Getkeys;
};

/////////////////////////////////////////////////////////////////////////////
//怪獸物件
/////////////////////////////////////////////////////////////////////////////

class Monster{
public:
	Monster();
	void Initialize();				// 設定擦子為初始值
	void LoadBitmap();				// 載入圖形
	void OnMove(CMap* gamemap,Thief* thief,Door* door,bool cheatMode);		// 移動怪獸
	void OnShow(CMap* gamemap);		// 將擦子圖形貼到畫面
	void reststep();
	int Getx();
	int Gety();
	void SetXY(int x, int y);
	void Setcommenpath( vector<MovingNode*> &scriptpath);
	bool GetisTrace();
protected:
	CMovingBitmap guide[4][4];		// 擦子的動畫
	CMovingBitmap *monster_1;
	int mstep;					//行走狀態
	int gx,gy;					//警衛所在座標
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
	bool isComeback;      //平常在既定路徑的尾端時 是true 表示要逐漸回到起始端  
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
//寶物
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
//紅螢幕物件
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
//說明頁面
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
// 這個class為遊戲的遊戲開頭畫面物件
// 每個Member function的Implementation都要弄懂
/////////////////////////////////////////////////////////////////////////////

class CGameStateInit : public CGameState {
public:
	CGameStateInit(CGame *g);
	void OnInit();  								// 遊戲的初值及圖形設定
	void OnBeginState();							// 設定每次重玩所需的變數
	void OnKeyUp(UINT, UINT, UINT); 				// 處理鍵盤Up的動作
protected:
	void OnShow();									// 顯示這個狀態的遊戲畫面
private:
	CMovingBitmap stbg;								//遊戲開始開始頁面背景圖
	CMovingBitmap About;							//關於製作底圖
	CMovingBitmap ExMouse;							//箭頭圖案
	bool mu,About_bool;

};

/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的選擇關卡頁面
/////////////////////////////////////////////////////////////////////////////

class CGameStateSelect : public CGameState {
public:
	CGameStateSelect(CGame *g);
	void OnInit();  								// 遊戲的初值及圖形設定
	void OnBeginState();							// 設定每次重玩所需的變數
	void OnKeyUp(UINT, UINT, UINT); 				// 處理鍵盤Up的動作
protected:
	void OnShow();									// 顯示這個狀態的遊戲畫面
private:
	CMovingBitmap Select;						//遊戲開始開始頁面背景圖
	CMovingBitmap ExMouse;							//箭頭圖案

};

/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲執行物件(第一關)，主要的遊戲程式都在這裡
// 每個Member function的Implementation都要弄懂
/////////////////////////////////////////////////////////////////////////////

class CGameStateRun : public CGameState {
public:
	CGameStateRun(CGame *g);
	~CGameStateRun();
	void OnBeginState();							// 設定每次重玩所需的變數
	void OnInit();  								// 遊戲的初值及圖形設定
	void OnKeyDown(UINT, UINT, UINT);
	void OnKeyUp(UINT, UINT, UINT);
protected:
	void OnMove();									// 移動遊戲元素
	void OnShow();									// 顯示這個狀態的遊戲畫面
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
	int counter;									// 倒數之計數器
	static const int limatetime = 306;				// 此關卡遊戲時間
	static const int stage=1;
	bool cheatmode;									// 作弊模式狀態
	bool dkeybordisdown;
	bool wkeybordisdown;
	int score,time;										//成績紀錄
	help	help;
	CMovingBitmap	cheat;
	bool togetalltreasure;
	CintTime		Tcc;

};

/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲執行物件(第二關)，主要的遊戲程式都在這裡
// 每個Member function的Implementation都要弄懂
/////////////////////////////////////////////////////////////////////////////

class CGameStateRun2 : public CGameState {
public:
	CGameStateRun2(CGame *g);
	~CGameStateRun2();
	void OnBeginState();							// 設定每次重玩所需的變數
	void OnInit();  								// 遊戲的初值及圖形設定
	void OnKeyDown(UINT, UINT, UINT);
	void OnKeyUp(UINT, UINT, UINT);
protected:
	void OnMove();									// 移動遊戲元素
	void OnShow();									// 顯示這個狀態的遊戲畫面
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
	int counter;									// 倒數之計數器
	static const int limatetime = 533;				// 此關卡遊戲時間
	static const int stage=2;
	bool cheatmode;									// 作弊模式狀態
	int score,time;									//成績紀錄
	help	help;
	CMovingBitmap	cheat;
	CintTime		Tcc;
	bool togetalltreasure;
	bool dkeybordisdown;
	bool wkeybordisdown;
};


/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲執行物件(過關畫面)
/////////////////////////////////////////////////////////////////////////////
class CGameStatePASS : public CGameState {
public:
	CGameStatePASS(CGame *g);
	void OnInit();  								// 遊戲的初值及圖形設定
protected:
	void OnBeginState();
	void CalST();								//計算時間與分數
	void OnKeyUp(UINT, UINT, UINT);
	void OnShow();									// 顯示這個狀態的遊戲畫面
	void readfromfile();
private:
	int counter;									// 倒數之計數器
	//儲存過關頁面要顯示的資訊
	int min,sec;
	int time;//儲存轉成int的總秒數
	int score;
	int min1,min2;
	int sec1,sec2;
	int stage;//關卡位置
	CMovingBitmap	PASS;							//遊戲開始開始頁面背景圖
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
// 這個class為遊戲的破關狀態(Game WIN)
/////////////////////////////////////////////////////////////////////////////

class CGameStateWIN : public CGameState {
public:
	CGameStateWIN(CGame *g);
	void OnBeginState();							// 設定每次重玩所需的變數
	void OnInit();
protected:
	void OnKeyUp(UINT, UINT, UINT);
	void OnMove();									// 移動遊戲元素
	void OnShow();									// 顯示這個狀態的遊戲畫面
private:
	int counter;									// 倒數之計數器
	CMovingBitmap	WIN;
};


/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的結束狀態(Game Over)
// 每個Member function的Implementation都要弄懂
/////////////////////////////////////////////////////////////////////////////

class CGameStateOver : public CGameState {
public:
	CGameStateOver(CGame *g);
	void OnBeginState();							// 設定每次重玩所需的變數
	void OnInit();
protected:
	void OnKeyUp(UINT, UINT, UINT);
	void OnMove();									// 移動遊戲元素
	void OnShow();									// 顯示這個狀態的遊戲畫面
private:
	int counter;									// 倒數之計數器
	CMovingBitmap	Over;
};

}