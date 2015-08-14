	/*
 * mygame.cpp: 本檔案儲遊戲本身的class的implementation
 * Copyright (C) 2002-2008 Woei-Kae Chen <wkc@csie.ntut.edu.tw>
 *
 * This file is part of game, a free game development framework for windows
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
 * History:
 *   2002-03-04 V3.1
 *          Add codes to demostrate the use of CMovingBitmap::ShowBitmap(CMovingBitmap &).
 *	 2004-03-02 V4.0
 *      1. Add CGameStateInit, CGameStateRun, and CGameStateOver to
 *         demonstrate the use of states.
 *      2. Demo the use of CInteger in CGameStateRun.
 *   2005-09-13
 *      Rewrite the codes for CBall and CEraser.
 *   2005-09-20 V4.2Beta1.
 *   2005-09-29 V4.2Beta2.
 *      1. Add codes to display IDC_GAMECURSOR in GameStateRun.
 *   2006-02-08 V4.2
 *      1. Revise sample screens to display in English only.
 *      2. Add code in CGameStateInit to demo the use of PostQuitMessage().
 *      3. Rename OnInitialUpdate() -> OnInit().
 *      4. Fix the bug that OnBeginState() of GameStateInit is not called.
 *      5. Replace AUDIO_CANYON as AUDIO_NTUT.
 *      6. Add help bitmap to CGameStateRun.
 *   2006-09-09 V4.3
 *      1. Rename Move() and Show() as OnMove and OnShow() to emphasize that they are
 *         event driven.
 *   2006-12-30
 *      1. Bug fix: fix a memory leak problem by replacing PostQuitMessage(0) as
 *         PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE,0,0).
 *   2008-02-15 V4.4
 *      1. Add namespace game_framework.
 *      2. Replace the demonstration of animation as a new bouncing ball.
 *      3. Use ShowInitProgress(percent) to display loading progress. 
 *   2010-03-23 V4.6
 *      1. Demo MP3 support: use lake.mp3 to replace lake.wav.
*/

#include "stdafx.h"
#include "Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "gamelib.h"
#include "mygame.h"
#include <time.h>
#include <fstream>
#include <math.h>
#include <vector>
#include <list>
#include <string>
namespace game_framework {
////////////////////////////////////////////////////
//紅螢幕物件
///////////////////////////////////////////////////
redscreen::redscreen(){
}
void redscreen::Initialize(){
	isFlash = false;
	counter =0;
};
void redscreen::LoadBitmap(){
	redscreen_bitmap.LoadBitmap("Bitmaps/red.bmp");
};
void redscreen::OnShow(){
	if(isFlash){
	    if(counter == 0){
			redscreen_bitmap.ShowBitmap();
			counter++;
		}
		else if(counter==25){
		    counter=0;
		}
		else{
		    counter++;
		}
	}
};
void redscreen::SetisFlash(bool flag){
	isFlash = flag;
	if(!flag){
	    counter = 0;
	}
}
////////////////////////////////////////////////////
//help class
///////////////////////////////////////////////////
help::help(){
}
void help::Initialize(){
	flag = false;
};
void help::LoadBitmap(){
	help_bitmap.LoadBitmap("Bitmaps/help.bmp",RGB(255,255,255));
};
void help::OnShow(){
	if(!flag){
		help_bitmap.SetTopLeft(0,0);
		help_bitmap.ShowBitmap();
	}else{
	
	}
};
void help::SetOk(bool flag1){
	flag = flag1;
}

bool help::GetState(){
	return flag;
}

/////////////////////////////////////////////////////////////////////////////
//寶物CLASS
/////////////////////////////////////////////////////////////////////////////
Treasure::Treasure(){
}
bool Treasure::Isalife(){
    return islife;
}
void Treasure::Initialize(){
	height=2;
	width=2;
	islife=true;
}
void Treasure::Setlife(bool flag){
	islife = flag;
}

void Treasure::OnShow(CMap* gamemap){
	if(islife){
		treasure_bitmap.SetTopLeft(gamemap->computsx(x)*20,gamemap->computsy(y)*20);
	    treasure_bitmap.ShowBitmap();
	}
}
bool Treasure::HitThief(Thief* thief){
	int tx1 = thief->Getx();
	int tx2 = thief->Getx()+3;
	int ty1 = thief->Gety();
	int ty2 = thief->Gety()+4;
	if(((x>=tx1&&x<=tx2)||(x+1>=tx1&&x+1<=tx2))&&((y>=ty1&&y<=ty2)||(y+1>=ty1&&y+1<=ty2))){
	    return true;
	}
	else{
		return false;
	}
}
void Treasure::SetXY(int sx,int sy){
    x = sx;
	y = sy;
}
void Treasure::SetSize(int h,int w){
    height = h;
	width = w;
}
void Treasure::LoadBitmap(){
    treasure_bitmap.LoadBitmap("Bitmaps/treasure.bmp",RGB(0,0,0));

}
int Treasure::Getx(){
    return x;
}
int Treasure::Gety(){
    return y;
}
int Treasure::Getheight(){
    return height;
}
int Treasure::Getwidth(){
	return width;
}

/////////////////////////////////////////////////////////////////////////////
// 障礙物CLASS
/////////////////////////////////////////////////////////////////////////////
Obstacle::Obstacle():x(0),y(0){
}
//載入當障礙物的藍色圖片
void Obstacle::Load(){
	bmp.LoadBitmap(IDB_BLUE);
}
void Obstacle::OnShow(CMap* gamemap){
	if(gamemap->computsx(x)>=0&&gamemap->computsy(y)>=0){
		//設定障礙物顯示位置(放大倍數)
		bmp.SetTopLeft(gamemap->computsx(x)*20,gamemap->computsy(y)*20);
		//顯示障礙物
		bmp.ShowBitmap();
	}
}
void Obstacle::setxy(int setx,int sety){
    x = setx;
	y = sety;
}
/////////////////////////////////////////////////////////////////////////////
// 移動點CLASS
/////////////////////////////////////////////////////////////////////////////
MovingNode::MovingNode(int sx, int sy):x(sx),y(sy){
	parent = NULL;
	isInlist = false;
}
int MovingNode::Getx(){
	return x;
}
int MovingNode::Gety(){
	return y;
}
int MovingNode::Getg(){
	return g;
}
int MovingNode::Getf(){
	return f;
}
MovingNode* MovingNode::GetParent(){
	return parent;
}
void MovingNode::setparent(MovingNode* settarget){
    parent = settarget;
}
void MovingNode::caculateparameter(const int sx, const int sy,const int tx,const int ty){
	h = abs(tx-x) + abs(ty-y);
	if(parent==NULL)
		g=0;
	else
	    g = abs(x-sx)+abs(y-sy);
	f = g+h;
}
/////////////////////////////////////////////////////////////////////////////
// 地圖CLASS
/////////////////////////////////////////////////////////////////////////////
CMap::CMap(const char* fn){
	Initialize(fn);
}
int CMap::DetialDoor(int x,int y){
	if(map[x][y]=='h')
		return 1;
	else if(map[x][y]=='v')
		return 2;
	else if(map[x][y]=='Y')
		return 3;
	else if(map[x][y]=='Z')
		return 4;
	else if(map[x][y]=='R')
		return 5;
	else if(map[x][y]=='S')
		return 6;
	else if(map[x][y]=='G')
		return 7;
	else if(map[x][y]=='H')
	    return 8;
	else if(map[x][y]=='P')
		return 9;
	else if(map[x][y]=='Q')
		return 10;
	else
		return -1;
}
char CMap::KeyColor(int x, int y){
    return map[x][y];
}
void CMap::Initialize(const char* fn){
	sx = 0;
	sy = 0;
	countkeys=0;
	countobstacle=0;
	counttreasures=0;
	countdoors= 0;
	readmapfromfile(fn);//讀取地圖檔案
    for(int i=0;i<160;i++){
	    for(int j=0 ; j<120;j++){
		    if(Isempty(i,j)==0)
			    countobstacle++;
			else if(Isempty(i,j)==2){
				counttreasures++;
			}
			else if(Isempty(i,j)==4)
				countdoors++;
			else if(Isempty(i,j)==5){
				countkeys++;
			}
		}
	}	
}
//載入遊戲所需地圖檔案
void CMap::LoadMap(){
	background.LoadBitmap("Bitmaps/floor.bmp");
}
//由讀進來的地圖檔判斷障礙物位置及可走路徑
int CMap::Isempty(int x, int y){
	if(map[x][y]==' ')//可走路徑
	    return 1;
	else if(map[x][y]=='o')//障礙物
		return 0;
	else if(map[x][y]=='t')
		return 2;
	else if(map[x][y]=='e')
		return 999;
	else if(map[x][y]=='h'||map[x][y]=='v')
		return 4;
	else if(map[x][y]=='Y'||map[x][y]=='Z')
		return 4;
	else if(map[x][y]=='R'||map[x][y]=='S')
		return 4;
	else if(map[x][y]=='G'||map[x][y]=='H')
		return 4;
	else if(map[x][y]=='P'||map[x][y]=='Q')
		return 4;
	else if(map[x][y]=='y'||map[x][y]=='r'||map[x][y]=='g'||map[x][y]=='p')
		return 5;
	else			
		return -1;
}
void CMap::OnShow(){
	int x = 0 - sx*20;
    int y = 0 - sy*20;
	background.SetTopLeft(x,y);
	background.ShowBitmap();
}
void CMap::MoveMap(char direction){
	if(direction =='r'&&sx<128)
		sx++;
	if(direction =='l'&&sx>0)
		sx--;
	if(direction =='u'&&sy>0)
		sy--;
	if(direction =='d'&&sy<96)
		sy++;
}
void CMap::toempty(int x , int y){
    map[x][y] = ' ';
}
void CMap::setdoor(int x, int y,char type){
    map[x][y]=type;
}
//取得人物目前位置x與地圖的左上角格子相差多少
int CMap::computsx(int x){
	return (x-sx);
}
//取得人物目前位置y與地圖的左上角格子相差多少
int CMap::computsy(int y){
	return (y-sy);
}
//回傳障礙物個數總數
int CMap::Getcoutobstacle(){
	return countobstacle;
}
int CMap::Getcounttreasure(){
    return counttreasures;
}
int CMap::Getcountdoors(){
    return countdoors;
}
int CMap::Getcountkeys(){
    return countkeys;
}
void CMap::readmapfromfile(const char* fn){//地圖檔讀取地方
	ifstream inf;
	char temp[120][161];
	inf.open(fn);
	GAME_ASSERT(inf,"地圖檔讀取失敗.");
	for(int i=0;i<120;i++)
		inf.read(temp[i],161);
	for(int i=0;i<120;i++){
		for(int j=0;j<160;j++){
			map[j][i]=temp[i][j];
		}
	}
	inf.close();
}
void CMap::Setsxsy(int x,int y){
	if(x<0)
		x=0;
	if(x>159)
		x=159;
	if(y<0)
	    y=0;
	if(y>119)
		x=119;
	sx=x;
	sy=y;
}
/////////////////////////////////////////////////////////////////////////////
//Key
/////////////////////////////////////////////////////////////////////////////
Key::Key(){
}
void Key::Initialize(){
    isOnMap = true;
}
char Key::getColor(){
    return keycolor;
}
void Key::LoadKey(){
	if(keycolor == 'y'){
		keypicture.LoadBitmap("Bitmaps/yellow-key.bmp",RGB(255,255,255));
	}
	else if(keycolor == 'r'){
	    keypicture.LoadBitmap("Bitmaps/red-key.bmp",RGB(255,255,255));
	}
	else if(keycolor == 'g'){
	    keypicture.LoadBitmap("Bitmaps/green-key.bmp",RGB(255,255,255));
	}
	else if(keycolor== 'p'){
	    keypicture.LoadBitmap("Bitmaps/purple-key.bmp",RGB(255,255,255));
	}
}
void Key::setKey(int sx,int sy, char color){
    x = sx;
	y = sy;
	keycolor = color;
}
void Key::Onshow(CMap* gamemap){
	if(isOnMap){
		keypicture.SetTopLeft(gamemap->computsx(x)*20,gamemap->computsy(y)*20);
		keypicture.ShowBitmap();
	}
}
bool Key::getisOnMap(){
    return isOnMap;
}
bool Key::hitThief(int sx,int sy){
	int tx1 = sx;
	int tx2 = sx+3;
	int ty1 = sy;
	int ty2 = sy+4;
	if(((x>tx1&&x<tx2)||(x+width>tx1&&x+width<tx2))&&((y>ty1&&y<ty2)||(y+high>ty1&&y+high<ty2))){
	    return true;
	}
	else{
		return false;
	}
}
void Key::setOnMap(bool flag){
    isOnMap = flag;
}

/////////////////////////////////////////////////////////////////////////////
// Door
/////////////////////////////////////////////////////////////////////////////
Door::Door(){	
}
void Door::Initialize(CMap* gamemap){
	isClose = false;
	setClose(true,gamemap);
}
void Door::LoadDoor(){
	if(!isVertical){
	     if(doortype=='n')
			 doorpicture.LoadBitmap("Bitmaps/hdoor.bmp");
		 else if(doortype=='k'){
		     if(doorcolor == 'y')
			     doorpicture.LoadBitmap("Bitmaps/yellow-door-h.bmp");
			 else if(doorcolor == 'g')
				 doorpicture.LoadBitmap("Bitmaps/green-door-h.bmp");
			 else if(doorcolor == 'r')
				 doorpicture.LoadBitmap("Bitmaps/red-door-h.bmp");
			 else if(doorcolor == 'p')
				 doorpicture.LoadBitmap("Bitmaps/purple-door-h.bmp");
		 }
	}
	else{
		if(doortype=='n')
			 doorpicture.LoadBitmap("Bitmaps/vdoor.bmp");
		 else if(doortype=='k'){
		     if(doorcolor == 'y')
			     doorpicture.LoadBitmap("Bitmaps/yellow-door-v.bmp");
		 	 else if(doorcolor == 'g')
				 doorpicture.LoadBitmap("Bitmaps/green-door-v.bmp");
			 else if(doorcolor == 'r')
				 doorpicture.LoadBitmap("Bitmaps/red-door-v.bmp");
			 else if(doorcolor == 'p')
				 doorpicture.LoadBitmap("Bitmaps/purple-door-v.bmp");    
		 }
	}
}
void Door::setClose(bool flag,CMap* gamemap){
    if(flag&&isClose){
		return;
	}
	else if(flag&&!isClose){
	    isClose = flag;
		if(doortype=='n'&&isVertical){
		    for(int i=y;i<y+doorsize;i++){
				gamemap->setdoor(x,i,'v');
			}
		}
		else if(doortype=='n'&&!isVertical){
		    for(int i=x;i<x+doorsize;i++){
			    gamemap->setdoor(i,y,'h');
			}
		}
		else if(doortype == 'k'){
			if(doorcolor == 'y'){
			    if(isVertical){
				    for(int i=y;i<x+doorsize;i++){
						gamemap->setdoor(x,i,'Y');
					}
				}
				else{
				    for(int i=x;i<x+doorsize;i++){
						gamemap->setdoor(i,y,'Z');
					}
				}
			}
		}
	}
	else if(!flag&&isClose){
	    isClose = flag;
		if(isVertical){
		    for(int i=y;i<y+doorsize;i++){
				gamemap->toempty(x,i);
			}
		}
		else{
		    for(int i=x;i<x+doorsize;i++){
				gamemap->toempty(i,x);
			}
		}
	}
	else{
	    return;
	}
}
bool Door::HitObject(int sx,int sy){
    int tx1= sx;
	int tx2= sx+3;
	int ty1= sy;
	int ty2= sy+4;
	if(isVertical){
	    if((x>=tx1&&x<=tx2)&&((ty1>=y&&ty1<=y+doorsize-1)||(ty2>=y&&ty2<=y+doorsize-1))){
		    return true;
		}
		else{
		    return false;
		}
	}
	else{
	    if(((tx1>=x&&tx1<=x+doorsize-1)||(tx2>=x&&tx2<=x+doorsize-1))&&(y>=ty1&&y<=ty2)){
		    return true;
		}
		else{
		    return false;
		}
	}
}
bool Door::GetisClose(){
	return isClose;
}
void Door::OnShow(CMap* gamemap){
	if(isClose){
		doorpicture.SetTopLeft(gamemap->computsx(x)*20,gamemap->computsy(y)*20);
		doorpicture.ShowBitmap();
	}
}
void Door::setDoor(int sx, int sy, char type,char color){
    x=sx;
	y=sy;
	doorcolor = color;
	if(type =='h'){
	    isVertical = false;
		doortype = 'n';
	}
	else if(type =='v'){
	    isVertical = true;
		doortype = 'n';
	}
	else if(type =='Y'){
	    isVertical = true;
		doortype = 'k';
	}
	else if(type =='Z'){
	    isVertical = false;
		doortype = 'k';
	}
	else if(type == 'G'){
		isVertical = true;
		doortype = 'k';
	}
	else if(type == 'H'){
	    isVertical = false;
		doortype = 'k';
	}
	else if(type == 'R'){
	    isVertical = true;
		doortype = 'k';
	}
	else if(type == 'S'){
	    isVertical = false;
		doortype = 'k';
	}
	else if(type =='P'){
	    isVertical = true;
		doortype = 'k';
	}
	else if(type == 'Q'){
	    isVertical = false;
		doortype = 'k';
	}

}
char Door::Getdoorcolor(){
    return doorcolor;
}
char Door::Getdoortype(){
    return doortype;
}
/////////////////////////////////////////////////////////////////////////////
// 這個class為專屬於顯示計時器的所有方法
/////////////////////////////////////////////////////////////////////////////
CintTime::CintTime(){
	Initialize();

}

void CintTime::Initialize(){//初始化
	min=sec=0;
}

void CintTime::SetTime(int number){//設定時間,為秒
	if(number>=60){
		min=number/60;
		sec=number%60;
	}
	else{
		sec=number;
		min=0;
	}
	

}

int CintTime::GetTime(){//取得目前時間,為秒
	return min*60+sec;
}
void CintTime::LoadBitmap(bool CTFirst){//載入圖形
	if(!CTFirst){
		Cint.LoadBitmap();
	}
	Cint.SetIsLodBitmap(true);
}

void CintTime::OnShow(){//顯示圖形

		//分
		Cint.SetInteger(min);
		Cint.SetTopLeft(270,10);
		Cint.ShowBitmap();

		//秒
		Cint.SetInteger(sec);
		Cint.SetTopLeft(350,10);
		Cint.ShowBitmap();

}
/////////////////////////////////////////////////////////////////////////////
// 這個class為盜賊(Thief)的所有方法
/////////////////////////////////////////////////////////////////////////////
Thief::Thief(){
	Initialize();
}
void Thief::Initialize()
{   
	//主角位置預設為0;
	x=1,y=1;
	//主角預設往下第一張圖片
	play = &rogue[0][0];
	step = 0;
	//預設移動狀態都是否
	isMovingLeft = isMovingRight= isMovingUp = isMovingDown = false;
	isGetallTreasure = false;
}
void Thief::SetXY(int sx,int sy){
    x=sx;
	y=sy;
}
void Thief::LoadBitmap()
{
	rogue[0][0].LoadBitmap(IDB_SHARP_D1,RGB(255,255,255));
	rogue[0][1].LoadBitmap(IDB_SHARP_D2,RGB(255,255,255));
	rogue[0][2].LoadBitmap(IDB_SHARP_D3,RGB(255,255,255));
	rogue[0][3].LoadBitmap(IDB_SHARP_D4,RGB(255,255,255));
	rogue[1][0].LoadBitmap(IDB_SHARP_U1,RGB(255,255,255));
	rogue[1][1].LoadBitmap(IDB_SHARP_U2,RGB(255,255,255));
	rogue[1][2].LoadBitmap(IDB_SHARP_U3,RGB(255,255,255));
	rogue[1][3].LoadBitmap(IDB_SHARP_U4,RGB(255,255,255));
	rogue[2][0].LoadBitmap(IDB_SHARP_R1,RGB(255,255,255));
	rogue[2][1].LoadBitmap(IDB_SHARP_R2,RGB(255,255,255));
	rogue[2][2].LoadBitmap(IDB_SHARP_R3,RGB(255,255,255));
	rogue[2][3].LoadBitmap(IDB_SHARP_R4,RGB(255,255,255));
	rogue[3][0].LoadBitmap(IDB_SHARP_L1,RGB(255,255,255));
	rogue[3][1].LoadBitmap(IDB_SHARP_L2,RGB(255,255,255));
	rogue[3][2].LoadBitmap(IDB_SHARP_L3,RGB(255,255,255));
	rogue[3][3].LoadBitmap(IDB_SHARP_L4,RGB(255,255,255));


}
//用來重設圖片走路的方法(每次走路都不一樣的圖片,重複顯示)
void Thief::contralstep(int direction){
	step++;
	if(step>3)
		step=0;
	play=&rogue[direction][step];
}
void Thief::reststep(){
	step=0;
}
void Thief::OnMove(CMap* gamemap,Door* door,Key* key)
{
	//判斷盜賊是否開門
	for(int i=0;i<gamemap->Getcountdoors();i++){
		    if(door[i].GetisClose()&&(door[i].HitObject(x,y))){
			    door[i].setClose(false,gamemap);
				//開門音效
				CAudio::Instance()->Play(AUDIO_DOOR, false);	
				break;
			}
	}
	if (isMovingLeft){
		if(checkempty('l',gamemap,key,door)&&x>0){
			x--;
			if(gamemap->computsx(x)<16)//移動視窗用(移動地圖讓地圖跟著人跑)
			    gamemap->MoveMap('l');
		}
		contralstep(3);
	}
	else if (isMovingRight){
		if(checkempty('r',gamemap,key,door)&&x<156){
			x++;
			if(gamemap->computsx(x)>16)//移動視窗用(移動地圖讓地圖跟著人跑)
			    gamemap->MoveMap('r');
		}
		contralstep(2);
	}
	else if (isMovingUp){
		if(checkempty('u',gamemap,key,door)&&y>0){
			y--;
			if(gamemap->computsy(y)<12)//移動視窗用(移動地圖讓地圖跟著人跑)
			    gamemap->MoveMap('u');
		}
		contralstep(1);
	}
	else if (isMovingDown){
		if(checkempty('d',gamemap,key,door)&&y<116){
			y++;
			if(gamemap->computsy(y)>12)//移動視窗用(移動地圖讓地圖跟著人跑)
			    gamemap->MoveMap('d');
		}
		contralstep(0);
	}
	else{}
}

//判斷按鍵的方向傳入布林值T為按下去,F為否
void Thief::SetMovingDown(bool flag)
{
	isMovingDown = flag;
}
void Thief::SetMovingLeft(bool flag)
{
	isMovingLeft = flag;
}
void Thief::SetMovingRight(bool flag)
{
	isMovingRight = flag;
}
void Thief::SetMovingUp(bool flag)
{
	isMovingUp = flag;
}
//設定盜賊在螢幕上顯示的位置並顯示
void Thief::OnShow(CMap* gamemap)
{
	play->SetTopLeft(gamemap->computsx(x)*20,gamemap->computsy(y)*20);
	play->ShowBitmap();
}
bool Thief::checkempty(char direction,CMap* gamemap,Key *key , Door *door){
	if(direction=='r'){
		for(int i=y;i<=y+4;i++){
			if(gamemap->Isempty(x+4,i)==0)
				return false;
		}
		for(int i=0;i< gamemap->Getcountdoors();i++){
			if(door[i].Getdoortype()=='k'&&door[i].HitObject(x+1,y)&&door[i].GetisClose()){
				if(gamemap->Getcountkeys() == 0){
				    return false;
				}
				for(int j=0;j<gamemap->Getcountkeys();j++){
					if(door[i].Getdoorcolor()==key[j].getColor()){
						if(key[j].getisOnMap()){
						    return false;
						}
						else{
							return true;
						}
					}
				}
				return false;
			}
		}
		return true;
	}
	else if(direction=='l'){
	    for(int i=y;i<=y+4;i++){
			if(gamemap->Isempty(x-1,i)==0)
				return false;
				
		}
		for(int i=0;i< gamemap->Getcountdoors();i++){
			if(door[i].Getdoortype() == 'k' &&door[i].HitObject(x-1,y)&&door[i].GetisClose()){
				if(gamemap->Getcountkeys() ==0){
				    return false;
				}
				for(int j=0;j<gamemap->Getcountkeys();j++){
					if(door[i].Getdoorcolor()==key[j].getColor()){
						if(key[j].getisOnMap()){
						    return false;
						}
						else{
						    return true;
						}
					}
				}
				return false;
			}
		}
		return true;
	}
	else if(direction=='u'){
		for(int i=x;i<=x+3;i++){
			if(gamemap->Isempty(i,y-1)==0)
				return false;
		}
		for(int i=0;i< gamemap->Getcountdoors();i++){
			if(door[i].Getdoortype()=='k'&&door[i].HitObject(x,y-1)&&door[i].GetisClose()){
				if(gamemap->Getcountkeys() == 0){
				    return false;
				}
				for(int j=0;j<gamemap->Getcountkeys();j++){
					if(door[i].Getdoorcolor()==key[j].getColor()){
						if(key[j].getisOnMap()){
						    return false;
						}
						else{
							return true;
						}
					}
				}
				return false;
			}
		}
		return true;
	}
	else if(direction=='d'){
	    for(int i=x;i<=x+3;i++){
			if(gamemap->Isempty(i,y+5)==0)
				return false;
		}
		for(int i=0;i< gamemap->Getcountdoors();i++){
			if(door[i].Getdoortype()=='k'&&door[i].HitObject(x,y+1)&&door[i].GetisClose()){
				if(gamemap->Getcountkeys() == 0){
				    return false;
				}
				for(int j=0;j<gamemap->Getcountkeys();j++){
					if(door[i].Getdoorcolor()==key[j].getColor()){
						if(key[j].getisOnMap()){
						    return false;
						}
						else{
							return true;
						}
					}
				}
				return false;
			}
		}
		return true;
	}
	else
		return false;
}

int Thief::Getx(){
	return x;
}
int Thief::Gety(){
    return y;
}

/////////////////////////////////////////////////////////////////////////////
// 這個class為怪物(Monster)的所有方法
/////////////////////////////////////////////////////////////////////////////
//初始化
Monster::Monster()
{
	Initialize();
}
void Monster::Initialize()
{
	//怪物位置預設為42 10;
	gx=12,gy=86;
	//怪物預設往下第一張圖片
	monster_1 = &guide[0][0];
	mstep = 0;
	//預設移動狀態都是否
	isMoving=false;
	isTrace=false;
	isBack=false;
	nowstep = 0;
	mcounter=0;
	path.clear();
	returnpath.clear();
	nowdirection='d';
	commenwalkstep = 0;
	isComeback = false;
	thedoor=NULL;
	opendoor = false;
	waitcounter = 0;
}
void Monster::LoadBitmap()
{
	guide[0][0].LoadBitmap(IDB_GUIDED1,RGB(255,255,255));
	guide[0][1].LoadBitmap(IDB_GUIDED2,RGB(255,255,255));
	guide[0][2].LoadBitmap(IDB_GUIDED3,RGB(255,255,255));
	guide[0][3].LoadBitmap(IDB_GUIDED4,RGB(255,255,255));
	guide[1][0].LoadBitmap(IDB_GUIDEU1,RGB(255,255,255));
	guide[1][1].LoadBitmap(IDB_GUIDEU2,RGB(255,255,255));
	guide[1][2].LoadBitmap(IDB_GUIDEU3,RGB(255,255,255));
	guide[1][3].LoadBitmap(IDB_GUIDEU4,RGB(255,255,255));
	guide[2][0].LoadBitmap(IDB_GUIDER1,RGB(255,255,255));
	guide[2][1].LoadBitmap(IDB_GUIDER2,RGB(255,255,255));
	guide[2][2].LoadBitmap(IDB_GUIDER3,RGB(255,255,255));
	guide[2][3].LoadBitmap(IDB_GUIDER4,RGB(255,255,255));
	guide[3][0].LoadBitmap(IDB_GUIDEL1,RGB(255,255,255));
	guide[3][1].LoadBitmap(IDB_GUIDEL2,RGB(255,255,255));
	guide[3][2].LoadBitmap(IDB_GUIDEL3,RGB(255,255,255));
	guide[3][3].LoadBitmap(IDB_GUIDEL4,RGB(255,255,255));
}
void Monster::contralstep(int direction){
	mstep++;
	if(mstep>3)
		mstep=0;
	if(direction ==0 && nowdirection != 'd')
		mstep=0;
	else if(direction==1 && nowdirection != 'u')
		mstep=0;
	else if(direction ==2 && nowdirection !='r')
		mstep=0;
	else if(direction ==3 && nowdirection !='l')
		mstep=0;
	else{}
	monster_1=&guide[direction][mstep];
}
void Monster::reststep(){
	mstep=0;
}
void Monster::Setcommenpath(vector<MovingNode*> &scriptpath){
    commenpath = scriptpath;
}
void Monster::findpath(const int tx, const int ty, CMap* gamemap){
	list<MovingNode*> openlist;
	list<MovingNode*> closelist;
	MovingNode* expendpoint = new MovingNode(gx,gy);
	expendpoint->setparent(NULL);
	expendpoint->caculateparameter(gx,gy,tx,ty);
	openlist.push_back(expendpoint);
	list<MovingNode*>::iterator it;
	MovingNode* targetpoint = NULL;
	if(tx==gx&&ty==gy){
	    return;
	}
	while(!openlist.empty()){
		for(it =openlist.begin(), expendpoint=*it ;it != openlist.end();it++){
		    MovingNode* now = *it;
		    if(now->Getf()<expendpoint->Getf()){
			    expendpoint=now;
			}
			else{
				if(now->Getf()==expendpoint->Getf()){
					if(now->Getg()<expendpoint->Getg()){
						expendpoint=now;
					}
					else{
					}
				}
			}
		}
		if(expendpoint->Getx()==tx&&expendpoint->Gety()==ty){
			targetpoint = expendpoint;
			break;
		}
		if(checkempty(expendpoint->Getx(),expendpoint->Gety(),'r',gamemap)){
		    bool  inlist=false;
			for(it = openlist.begin();it!=openlist.end();it++){
			    MovingNode* now=*it;
				if(now->Getx() == (expendpoint->Getx()+1) && now->Gety() == expendpoint->Gety()){
				    inlist=true;
					break;
				}
			}
			if(!inlist){
			    for(it = closelist.begin();it!=closelist.end();it++){
				    MovingNode* now=*it;
					if(now->Getx() == (expendpoint->Getx()+1) && now->Gety() == expendpoint ->Gety()){
					    inlist=true;
						break;
					}
				}
			}
			if(!inlist){
				MovingNode* temp= new MovingNode(expendpoint->Getx()+1,expendpoint->Gety());
				temp->setparent(expendpoint);
				temp->caculateparameter(gx,gy,tx,ty);
				openlist.push_back(temp);
			}
		}
		if(checkempty(expendpoint->Getx(),expendpoint->Gety(),'l',gamemap)){
		    bool inlist=false;
			for(it = openlist.begin();it!=openlist.end();it++){
				MovingNode* now = *it;
				if(now->Getx()==(expendpoint->Getx()-1)&& now->Gety() == expendpoint->Gety()){
				    inlist=true;
					break;
				}
			}
			if(!inlist){
			    for(it=closelist.begin();it!=closelist.end();it++){
					MovingNode* now = *it;
				    if(now->Getx()==(expendpoint->Getx()-1)&& now->Gety() == expendpoint->Gety()){
				        inlist=true;
					    break;
					}
				}
			}
			if(!inlist){
			    MovingNode* temp= new MovingNode(expendpoint->Getx()-1,expendpoint->Gety());
				temp->setparent(expendpoint);
				temp->caculateparameter(gx,gy,tx,ty);
				openlist.push_back(temp);
			}
		}
		if(checkempty(expendpoint->Getx(),expendpoint->Gety(),'d',gamemap)){
		    bool inlist=false;
			for(it=openlist.begin();it!=openlist.end();it++){
			    MovingNode* now = *it;
				if(now->Getx()==expendpoint->Getx() && now->Gety() == (expendpoint->Gety()+1)){
				    inlist=true;
					break;
				}
			}
			if(!inlist){
				for(it=closelist.begin();it!=closelist.end();it++){
					MovingNode* now=*it;
					if(now->Getx()==expendpoint->Getx()&& now->Gety()== (expendpoint->Gety()+1)){
					    inlist=true;
						break;
					}
				}
			}
			if(!inlist){
			    MovingNode* temp = new MovingNode(expendpoint->Getx(),expendpoint->Gety()+1);
				temp->setparent(expendpoint);
				temp->caculateparameter(gx,gy,tx,ty);
				openlist.push_back(temp);
			}
		}
		if(checkempty(expendpoint->Getx(),expendpoint->Gety(),'u',gamemap)){
			bool inlist=false;
			for(it=openlist.begin();it!=openlist.end();it++){
			    MovingNode* now = *it;
				if(now->Getx()==expendpoint->Getx()&&now->Gety() == (expendpoint->Gety()-1)){
				    inlist=true;
					break;
				}
			}
			if(!inlist){
				for(it=closelist.begin();it!=closelist.end();it++){
				    MovingNode* now = *it;
					if(now->Getx()==expendpoint->Getx() && now->Gety() == (expendpoint->Gety() -1)){
					    inlist=true;
						break;
					}
				}
			}
			if(!inlist){
			    MovingNode * temp = new MovingNode(expendpoint->Getx(),expendpoint->Gety()-1);
				temp->setparent(expendpoint);
				temp->caculateparameter(gx,gy,tx,ty);
				openlist.push_back(temp);
			}
		}
		closelist.push_back(expendpoint);
		openlist.remove(expendpoint);
	}
	for(MovingNode* now = targetpoint; now->GetParent()!=NULL;now=now->GetParent()){
		path.push_back(new MovingNode(now->Getx(),now->Gety()));
	}
	for(it=openlist.begin();it!=openlist.end();it++){
	    MovingNode* now=*it;
		if(now!=NULL)
			delete now;
	}
	openlist.clear();
	for(it=closelist.begin();it!=closelist.end();it++){
	    MovingNode* now=*it;
		if(now!=NULL)
			delete now;
	}
	closelist.clear();
	 
}

bool Monster::checkempty(int x,int y,char direction,CMap* gamemap){
	if(direction=='r'){
		for(int i=y;i<=y+4;i++){
			if(gamemap->Isempty(x+4,i)!=1&&gamemap->Isempty(x+4,i)!=4)
				return false;
		}
		return true;
	}
	else if(direction=='l'){
	    for(int i=y;i<=y+4;i++){
			if(gamemap->Isempty(x-1,i)!=1&&gamemap->Isempty(x-1,i)!=4)
				return false;
				
		}
		return true;
	}
	else if(direction=='u'){
		for(int i=x;i<=x+3;i++){
			if(gamemap->Isempty(i,y-1)!=1&&gamemap->Isempty(i,y-1)!=4)
				return false;
		}
		return true;
	}
	else if(direction=='d'){
	    for(int i=x;i<=x+3;i++){
			if(gamemap->Isempty(i,y+5)!=1&&gamemap->Isempty(i,y+5)!=4)
				return false;
		}
		return true;
	}
	else
		return false;
}
void Monster::SetXY(int x,int y){
    gx = x; 
	gy = y;
}
bool Monster::looksight(Thief* thief ,CMap* gamemap,bool cheatmode){
    if(cheatmode){
	    return false;
	}
	int mx1 = gx-sightrange;
	if(mx1 < 0){
	    mx1 =0;
	}
	int my1 = gy-sightrange;
	if(my1 < 0){
        my1 =0;
	}
    int mx2 = gx+sightrange+3;
    if(mx2>160){
        mx2 = 160;
    }
    int my2 = gy+sightrange+4;
    if( my2 >120){
        my2 = 120;
    }
    int tx1 = thief->Getx(); 
    int ty1 = thief->Gety();
    int tx2 = thief->Getx()+3;
    int ty2 = thief->Gety()+4;
    if(((tx1>=mx1&&tx1<=mx2)||(tx2>=mx1&&ty2<=mx2))&&((ty1>=my1&&ty1<=my2)||(ty2>=my1&&ty2<=my2))){
 	   if(nowdirection == 'r'){
           for(int i=gx;i<=mx2;i++){
			   if(i>=gx+4&&(gamemap->Isempty(i,gy-1)==0||gamemap->Isempty(i,gy-1)==4)){
			       break;
			   }
			   for(int j=i,k=gy-1;j<=mx2&&k>=my1;j++,k--){
				   if(gamemap->Isempty(j,k)==0||gamemap->Isempty(j,k)==4){
					   break;
				   }
				   else{
					   if(j>=tx1&&j<=tx2&&k>=ty1&&k<=ty2){
						   return true;
					   }
				   }
			   }
		   }
           for(int i=gx;i<=mx2;i++){
			   if(i>=gx+4&&(gamemap->Isempty(i,gy+5)==0||gamemap->Isempty(i,gy+5)==4)){
			       break;
			   }
			   for(int j=i,k=gy+5;j<=mx2&&k<=my2;j++,k++){
				   if(gamemap->Isempty(j,k)==0||gamemap->Isempty(j,k)==4){
					   break;
				   }
				   else{
				       if(j>=tx1&&j<=tx2&&k>=ty1&&k<=ty2){
					       return true;
					   }
				   }
			   }
		   }
		   for(int i = gy;i<=gy+4;i++){
		       for(int j=gx+4;i<=mx2;j++){
				   if(gamemap->Isempty(j,i)==0||gamemap->Isempty(j,i)==4){
					   break;
				   }
				   else{
					   if(i>=ty1&&i<=ty2&&j>=tx1&&j<=tx2){
					       return true;
					   }
				   }
			   }
		   }
		   return false;
	   }
	   else if(nowdirection =='l'){
	       for(int i=gx+3;i>=mx1;i--){
			   if(i<=gx-1&&(gamemap->Isempty(i,gy-1)==0||gamemap->Isempty(i,gy-1)==4)){
			       break;
			   }
		       for(int j=i,k=gy-1;j>=mx1&&k>=my1;j--,k--){
				   if(gamemap->Isempty(j,k)==0||gamemap->Isempty(j,k)==4){
					   break;
				   }
				   else{
				       if(j>=tx1&&j<=tx2&&k>=ty1&&k<=ty2){
						   return true;
					   }
				   }
			   }
		   }
		   for(int i=gx+3;i>=mx1;i--){
			   if(i<=gx-1&&(gamemap->Isempty(i,gy+5)==0||gamemap->Isempty(i,gy+5)==4)){
			       break;
			   }
			   for(int j=i,k=gy+5;j>=mx1&&k<=my2;j--,k++){
				   if(gamemap->Isempty(j,k)==0||gamemap->Isempty(j,k)==4){
					   break;
				   }
				   else{
				       if(j>=tx1&&j<=tx2&&k>=ty1&&k<=ty2){
					       return true;
					   }
				   }
			   }
		   }
		   for(int i=gy;i>=gy+4;i++){
			   for(int j=gx-1;i>=mx1;j--){
			       if(gamemap->Isempty(j,i)==0||gamemap->Isempty(j,i)==4){
					   break;
				   }
				   else{
				       if(i>=ty1&&i<=ty2&&j>=tx1&&j<=tx2){
						   return true;
					   }
				   }
			   }
		   }
		   return false;
	   }
	   else if(nowdirection =='u'){
		   for(int i = gy+4;i>=my1;i--){
			   if(i<=gy-1&&(gamemap->Isempty(gx+4,i)==0||gamemap->Isempty(gx+4,i)==4)){
			       break;
			   }
		       for(int j=gx+4,k=i;j<=mx2&&k>=my1;j++,k--){
				   if(gamemap->Isempty(j,k)==0||gamemap->Isempty(j,k)==4){
				       break;
				   }
				   else{
					   if(j>=tx1&&j<=tx2&&k>=ty1&&k<=ty2){
						   return true;
					   }
				   }
			   }
			}
           for(int i = gy+4;i>=my1;i--){
			   if(i<=gy-1&&(gamemap->Isempty(gx-1,i)==0||gamemap->Isempty(gx-1,i)==4)){
			       break;
			   }
			   for(int j=gx-1,k=i;j>=mx1&&k>=my1;j--,k--){
				   if(gamemap->Isempty(j,k)==0||gamemap->Isempty(j,k)==4){
					   break;
				   }
				   else{
				       if(j>=tx1&&j<=tx2&&k>=ty1&&k<=ty2){
					       return true;
					   }
				   }
			   }
		   }
		   for(int i=gx;i<=gx+3;i++){
		       for(int j=gy-1;i>=my1;j--){
				   if(gamemap->Isempty(i,j)==0||gamemap->Isempty(i,j)==4){
				       break;
				   }
				   else{
				       if(i>=tx1&&i<=tx2&&j>=ty1&&j<=ty2){
						   return true;
					   }
				   }
			   }
		   }
		   return false;
	   }
	   else if(nowdirection =='d'){
		   for(int i=gy;i<=my2;i++){
			   if(i>=gy+5&&(gamemap->Isempty(gx+4,i)==0||gamemap->Isempty(gx+4,i)==4)){
			       break;
			   }
		       for(int j=gx+4,k=i;j<=mx2&&k<=my2;j++,k++){
				   if(gamemap->Isempty(j,k)==0||gamemap->Isempty(j,k)==0){
				       break;
				   }
				   else{
				       if(j>=tx1&&j<=tx2&&k>=ty1&&k<=ty2){
					       return true;
					   }
				   }
			   }
		   }
		   for(int i=gy;i<=my2;i++){
			   if(i>=gy+5&&(gamemap->Isempty(gx-1,i)==0||gamemap->Isempty(gx-1,i)==4)){
			       break;
			   }
			   for(int j=gx-1,k=i;j>=mx1&&k<=my2;j--,k++){
				   if(gamemap->Isempty(j,k)==0||gamemap->Isempty(j,k)==4){
				       break;
				   }
				   else{
				       if(j>=tx1&&j<=tx2&&k>=ty1&&k<=ty2){
						   return true;
					   }
				   }
			   }
		   }
		   for(int i=gx;i<=gx+3;i++){
		       for(int j=gy+5;j<=my2;j++){
				   if(gamemap->Isempty(i,j)==0||gamemap->Isempty(i,j)==4){
				       break;
				   }
				   else{
					   if(i>=tx1&&i<=tx2&&j>=ty1&&j<=ty2){
						   return true;
					   }
				   }
			   }
		   }
		   return false;
	   }
	   return false;
    }
    else{
        return false;
    }
}
void Monster::OnMove(CMap* gamemap,Thief* thief,Door* door,bool cheatmode)
{

	for(int i=0;i<gamemap->Getcountdoors();i++){
		    if((door[i].HitObject(gx,gy))){
				if(door[i].GetisClose())
				    door[i].setClose(false,gamemap);
				opendoor = true;
				thedoor = &door[i];
				break;
			}
	}
	if((!isTrace||isBack)&&opendoor&&thedoor!=NULL){
		if(thedoor->HitObject(gx,gy)==false){
		    thedoor->setClose(true,gamemap);
		if(nowdirection == 'r'){
			    monster_1 = &guide[3][0];
			}
			else if(nowdirection == 'l'){
			    monster_1 = &guide[2][0];
			}
			else if(nowdirection =='d'){
			    monster_1 = &guide[1][0];
			}
			else{
			    monster_1 = &guide[0][0];
			}
			opendoor = false;
			wait = true;
		}
	}
	if(wait){
		if(waitcounter<10){
			waitcounter++;
		    return;
		}
		else{
		    waitcounter=0;
			wait = false;
		}

	}
	if(!isMoving&&!isTrace&&!isBack){
		if(looksight(thief,gamemap,cheatmode)){
			isMoving=false;
			isTrace=true;
			nowstep=0;
		}
		else{
			findpath(commenpath[commenwalkstep]->Getx(),commenpath[commenwalkstep]->Gety(),gamemap);
			nowstep=0;
			isMoving = true;
			if(commenwalkstep+1 >= commenpath.size()&&!isComeback){
			    isComeback = true;
			}
			else if(commenwalkstep==0 && isComeback){
			    isComeback = false;
			}
			if(isComeback){
			    commenwalkstep--;
			}
			else if(!isComeback){
				commenwalkstep++;
			}
		}
	}
	else if(isMoving && !isTrace&&!isBack){
		if(nowstep<path.size()&&!path.empty()){
			if(mcounter<1){//NON-TRACE SPEED
				mcounter++;
				return;
			}
			else if(mcounter>=1){//NON-TRACE SPEED
				mcounter=0;
			}
			if(path[path.size()-nowstep-1]->Getx() - gx ==1 && path[path.size()-nowstep-1]->Gety() == gy){
				contralstep(2);
				nowdirection = 'r';
			}
			else if(path[path.size()-nowstep-1]->Getx() - gx ==-1 && path[path.size()-nowstep-1]->Gety() == gy){
			    contralstep(3);
				nowdirection = 'l';
			}
			else if(path[path.size()-nowstep-1]->Getx() == gx && path[path.size()-nowstep-1]->Gety()- gy ==1){
			    contralstep(0);
				nowdirection = 'd';
			}
			else if(path[path.size()-nowstep-1]->Getx() == gx && path[path.size()-nowstep-1]->Gety()- gy ==-1){
			    contralstep(1);
				nowdirection = 'u';
			}
			gx = path[path.size()-nowstep-1]->Getx();
			gy = path[path.size()-nowstep-1]->Gety();
			nowstep++;
			if(looksight(thief,gamemap,cheatmode)){
				isMoving=false;
				isTrace = true;
				for(unsigned int i=0;i<path.size();i++){
			        if(path[i]!=NULL)
				        delete path[i];
			    } 
			    nowstep=0;
			    path.clear();
		    }
			if(nowstep>=path.size()){
			    isMoving=false;
				for(unsigned int i=0;i<path.size();i++){
				    if(path[i]!=NULL)
						delete path[i];
				}
				nowstep=0;
				path.clear();
			}
		}
		else{
			if(looksight(thief,gamemap,cheatmode)){
				isTrace = true;
			}
			isMoving=false;
			nowstep=0;
			path.clear();
		}
	}
	else if(!isMoving&&isTrace&&!isBack){
		findpath(thief->Getx(),thief->Gety(),gamemap);
		nowstep=0;
		isMoving=true;
	}
	else if(!isBack&&isTrace&&isMoving){
		if(cheatmode){
		    isBack=true;
			isMoving=false;
			isTrace=false;
			for(unsigned int i=0;i<path.size();i++){
			    if(path[i]!=NULL)
				    delete path[i];
			    }
			nowstep=0;
			path.clear();
		}
		if(nowstep<path.size()&&!path.empty()){
		     int mx1 = gx-15;
	         if(mx1 < 0){
	            mx1 =0;
	        }
	        int my1 = gy-15;
	        if(my1 < 0){
	            my1 =0;
	        }
	        int mx2 = gx+18;
		    if(mx2>160){
	            mx2 = 160;
	        }
	        int my2 = gy+18;
	        if( my2 >120){
	            my2 = 120;
	        }
	        int tx1 = thief->Getx(); 
	        int ty1 = thief->Gety();
	        int tx2 = thief->Getx()+3;
	        int ty2 = thief->Gety()+4;
		    if(mcounter<10){//TRACE SPEED
				mcounter++;
			}
			else if(mcounter>=10){//TRACE SPEED
				mcounter=0;
				return;
			}
			if(mstep%2==0){
			    returnpath.push_back(new MovingNode(path[path.size()-nowstep-1]->Getx(),path[path.size()-nowstep-1]->Gety()));
		    }
			if(path[path.size()-nowstep-1]->Getx() - gx ==1 && path[path.size()-nowstep-1]->Gety() == gy){
				contralstep(2);
				nowdirection = 'r';
			}
			else if(path[path.size()-nowstep-1]->Getx() - gx ==-1 && path[path.size()-nowstep-1]->Gety() == gy){
			    contralstep(3);
				nowdirection = 'l';
			}
			else if(path[path.size()-nowstep-1]->Getx() == gx && path[path.size()-nowstep-1]->Gety()- gy ==1){
			    contralstep(0);
				nowdirection = 'd';
			}
			else if(path[path.size()-nowstep-1]->Getx() == gx && path[path.size()-nowstep-1]->Gety()- gy ==-1){
			    contralstep(1);
				nowdirection = 'u';
			}
			gx = path[path.size()-nowstep-1]->Getx();
			gy = path[path.size()-nowstep-1]->Gety();
			nowstep++;
			if(!(((tx1>=mx1&&tx1<=mx2)||(tx2>=mx1&&ty2<=mx2))&&((ty1>=my1&&ty1<=my2)||(ty2>=my1&&ty2<=my2)))){
				isBack=true;
				isMoving=false;
				isTrace=false;
				for(unsigned int i=0;i<path.size();i++){
			    if(path[i]!=NULL)
				    delete path[i];
			    }
			    nowstep=0;
			    path.clear();
		    }
		}
		if(nowstep>=path.size()){
		    isMoving=false;
			for(unsigned int i=0;i<path.size();i++){
			    if(path[i]!=NULL)
				    delete path[i];
			}
			nowstep=0;
			path.clear();
		}
	}
	else if(isBack&&!isMoving&&!isTrace){
		if(returnpath.size()>0){
			path.clear();
		    findpath(returnpath[returnpath.size()-1]->Getx(),returnpath[returnpath.size()-1]->Gety(),gamemap);
			nowstep=0;
			isMoving=true;
		}
		else {
			nowstep=0;
		    isBack=false;
			isMoving=false;
		}
	}
	else if(!isTrace &&isBack&&isMoving){
		if(nowstep<path.size()&&!path.empty()){
			if(mcounter<4){
				mcounter++;
			}
			else if(mcounter>=4){
				mcounter=0;
				return;
			}
			if(path[path.size()-nowstep-1]->Getx() - gx ==1 && path[path.size()-nowstep-1]->Gety() == gy){
				contralstep(2);
				nowdirection = 'r';
			}
			else if(path[path.size()-nowstep-1]->Getx() - gx ==-1 && path[path.size()-nowstep-1]->Gety() == gy){
			    contralstep(3);
				nowdirection = 'l';
			}
			else if(path[path.size()-nowstep-1]->Getx() == gx && path[path.size()-nowstep-1]->Gety()- gy ==1){
			    contralstep(0);
				nowdirection = 'd';
			}
			else if(path[path.size()-nowstep-1]->Getx() == gx && path[path.size()-nowstep-1]->Gety()- gy ==-1){
			    contralstep(1);
				nowdirection = 'u';
			}
			gx = path[path.size()-nowstep-1]->Getx();
			gy = path[path.size()-nowstep-1]->Gety();
			nowstep++;
			if(looksight(thief,gamemap,cheatmode)){
				isBack=false;
				isMoving=false;
				isTrace=true;
				for(unsigned int i=0;i<path.size();i++){
			        if(path[i]!=NULL)
				        delete path[i];
			    } 
			    nowstep=0;
			    path.clear();
		    }
		}
		if(nowstep>=path.size()){
		    isMoving=false;
			for(unsigned int i=0;i<path.size();i++){
			    if(path[i]!=NULL)
				    delete path[i];
			}delete returnpath[returnpath.size()-1];
			returnpath.pop_back();
			nowstep=0;
			path.clear();
		}
	}
}
void Monster::OnShow(CMap* gamemap)
{
	monster_1->SetTopLeft(gamemap->computsx(gx)*20,gamemap->computsy(gy)*20);
	monster_1->ShowBitmap();
}
int Monster::Getx(){
	return gx;
}
int Monster::Gety(){
    return gy;
}
bool Monster::GetisTrace(){
    return isTrace;
}

/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲開頭畫面物件
/////////////////////////////////////////////////////////////////////////////

CGameStateInit::CGameStateInit(CGame *g): CGameState(g){
}

void CGameStateInit::OnInit(){
	//
	// 當圖很多時，OnInit載入所有的圖要花很多時間。為避免玩遊戲的人
	//     等的不耐煩，遊戲會出現「Loading ...」，顯示Loading的進度。
	//
	ShowInitProgress(0);	// 一開始的loading進度為0%
	//
	// 開始載入資料
	//
	stbg.LoadBitmap(IDB_STBG);
	About.LoadBitmap("Bitmaps/ABOUT.bmp");							//載入關於製作
	ExMouse.LoadBitmap("Bitmaps/SELECT_ICON.bmp",RGB(255,255,255));	//箭頭圖案
	ExMouse.SetTopLeft(250,250);					//先設定座標
	// 此OnInit動作會接到CGameStaterRun::OnInit()，所以進度還沒到100%
	//
	//載入所有音效檔案,僅在第一次執行
	CAudio::Instance()->Load(AUDIO_STBGS,  "sounds\\STARTbgs.mp3");	// 載入0 - 遊戲開始畫面背景音樂	
	CAudio::Instance()->Load(AUDIO_SELECT,  "sounds\\SELECT.wav");	// 載入1 - 選單音效
	CAudio::Instance()->Load(AUDIO_SCUESS,  "sounds\\SCUESS.wav");	// 載入 2 - 成功過關音效
	CAudio::Instance()->Load(AUDIO_FAILED,  "sounds\\FAILED.wav");		// 載入3 - 失敗(GAME OVER)頁面音樂
	CAudio::Instance()->Load(AUDIO_PICK,  "sounds\\PICK.wav");		// 載入4 - 撿起道具聲音
	CAudio::Instance()->Load(AUDIO_DOOR,  "sounds\\DOOR.wav");		// 載入5 - 開門聲音
	CAudio::Instance()->Load(AUDIO_MAP1,  "sounds\\map1.mp3");		// 載入6 - 地圖1音樂
	CAudio::Instance()->Load(AUDIO_MAP2,  "sounds\\map2.mp3");		// 載入7 - 地圖2音樂
	CAudio::Instance()->Load(AUDIO_MAP3,  "sounds\\map3.mp3");		// 載入8 - 地圖3音樂
	CAudio::Instance()->Load(AUDIO_MAP4,  "sounds\\map4.mp3");		// 載入9 - 地圖4音樂
	CAudio::Instance()->Load(AUDIO_MAP5,  "sounds\\map5.mp3");		// 載入10- 地圖5音樂
	CAudio::Instance()->Load(AUDIO_KEY,  "sounds\\KEY.mp3");		// 載入10- 地圖5音樂
	mu=true; //判斷是否是重新開始
	ShowInitProgress(20);	//
}


void CGameStateInit::OnBeginState(){
	if(mu==true){//重新開始才執行撥放音樂
			CAudio::Instance()->Play(AUDIO_STBGS, false);
	}
	About_bool = false;
}

void CGameStateInit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){
	const char KEY_UP    = 0x26; // keyboard上箭頭
	const char KEY_DOWN  = 0x28; // keyboard下箭頭
	const char KEY_SPACE = ' ';  // keyboard空白

	if(((nChar == KEY_UP)&&(ExMouse.Top()!=250))||((nChar == KEY_DOWN)&&(ExMouse.Top()!=379))){
		CAudio::Instance()->Play(AUDIO_SELECT, false);
	
	}
	//執行選單動作
	if (nChar == KEY_SPACE){
		if(ExMouse.Top()==250){
			GotoGameState(GAME_STATE_RUN);  // 切換至GAME_STATE_RUN(開始第一關遊戲)
		}
		else if(ExMouse.Top()==293){		//選擇關卡
			GotoGameState(GAME_STATE_Select);  // 切換至GAME_STATE_Select
		}
		else if(ExMouse.Top()==336){		//關於製作
			if(About_bool){
				About_bool=false;
			}else{
				About_bool=true;
			}
		}
		else if(ExMouse.Top()==379){		//離開遊戲
			PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE,0,0);
		}
	}
	else if((nChar == KEY_UP)&&(ExMouse.Top()>250)){	//往上移動失效
		ExMouse.SetTopLeft(250,ExMouse.Top()-43);
	}
	else if(nChar == KEY_DOWN&&(ExMouse.Top()<379)){	//往下移動失效
		ExMouse.SetTopLeft(250,ExMouse.Top()+43);
	}

}
void CGameStateInit::OnShow(){
	
	//貼背景圖
	stbg.SetTopLeft(0,0);
	stbg.ShowBitmap();
	//貼選單
	ExMouse.ShowBitmap();								//箭頭圖案
	if(About_bool==true){
		About.SetTopLeft(0,0);
		About.ShowBitmap();
	}else{
	
	}
}								


/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲選擇關卡畫面
/////////////////////////////////////////////////////////////////////////////

CGameStateSelect::CGameStateSelect(CGame *g):CGameState(g){
}

void CGameStateSelect::OnInit(){
	Select.LoadBitmap("Bitmaps/Select_bg.bmp");
	ExMouse.LoadBitmap("Bitmaps/SELECT_ICON.bmp",RGB(255,255,255));	//箭頭圖案
	ExMouse.SetTopLeft(250,293);					//先設定座標
	ShowInitProgress(50);	// 一開始的loading進度為0%
}


void CGameStateSelect::OnBeginState(){

}

void CGameStateSelect::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){
	const char KEY_UP    = 0x26; // keyboard上箭頭
	const char KEY_DOWN  = 0x28; // keyboard下箭頭
	const char KEY_SPACE = ' ';  // keyboard空白
	const char KEY_Q = 0x51;  // keyboard

	if(((nChar == KEY_UP)&&(ExMouse.Top()!=293))||((nChar == KEY_DOWN)&&(ExMouse.Top()!=336))){
		CAudio::Instance()->Play(AUDIO_SELECT, false);
	
	}
	//執行選單動作
	if (nChar == KEY_SPACE){
		if(ExMouse.Top()==293){
			GotoGameState(GAME_STATE_RUN);  // 第1關
		}
		else if(ExMouse.Top()==336){		// 第2關
			GotoGameState(GAME_STATE_RUN2);  // 第1關
		}
	}else if((nChar == KEY_UP)&&(ExMouse.Top()>293)){	//往上移動失效
		ExMouse.SetTopLeft(250,ExMouse.Top()-43);
	}else if(nChar == KEY_DOWN&&(ExMouse.Top()<336)){	//往下移動失效
		ExMouse.SetTopLeft(250,ExMouse.Top()+43);
	}else if(nChar == KEY_Q){
		GotoGameState(GAME_STATE_INIT);
	}
}
void CGameStateSelect::OnShow(){
	
	//貼背景圖
	Select.SetTopLeft(0,0);
	Select.ShowBitmap();
	//貼選單
	ExMouse.ShowBitmap();								//箭頭圖案
}								

/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的破關狀態(Game WIN)
/////////////////////////////////////////////////////////////////////////////

CGameStateWIN::CGameStateWIN(CGame *g): CGameState(g){
}

void CGameStateWIN::OnMove()
{
}

void CGameStateWIN::OnBeginState()
{
	CAudio::Instance()->Stop(6);//map1 audio
	CAudio::Instance()->Stop(7);//map2 audio
	CAudio::Instance()->Play(AUDIO_SCUESS, false);
}

void CGameStateWIN::OnInit(){
	WIN.LoadBitmap("Bitmaps/WIN.bmp");	
	ShowInitProgress(60);	// 一開始的loading進度為0%
}

void CGameStateWIN::OnShow()
{
	WIN.SetTopLeft(0,0);
	WIN.ShowBitmap();
}

void CGameStateWIN::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){
	const char KEY_SPACE = ' ';
	if (nChar == KEY_SPACE){
		//CAudio::Instance()->Stop(3);
		GotoGameState(GAME_STATE_INIT);						// 切換至GAME_STATE_RUN
	}
}


/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的結束狀態(Game Over)
/////////////////////////////////////////////////////////////////////////////

CGameStateOver::CGameStateOver(CGame *g): CGameState(g){
}

void CGameStateOver::OnMove()
{
	counter--;
	if (counter < 0){
		CAudio::Instance()->Stop(3);
		GotoGameState(GAME_STATE_INIT);
	}
}

void CGameStateOver::OnBeginState()
{
	counter = 30 * 9; // 9 seconds
	//if(!isLoadOver){

	//}
	CAudio::Instance()->Stop(6);
	CAudio::Instance()->Play(AUDIO_FAILED, true);	
}

void CGameStateOver::OnInit()
{	

	//
	// 開始載入資料
	ShowInitProgress(100);
	CAudio::Instance()->Play(AUDIO_STBGS, true);			// 撥放[開始遊戲]背景音樂
	Over.LoadBitmap(IDB_OVER);							//結束頁面底圖
	
}

void CGameStateOver::OnShow()
{
	Over.SetTopLeft(0,0);
	Over.ShowBitmap();
	CDC *pDC = CDDraw::GetBackCDC();			// 取得 Back Plain 的 CDC 
	CFont f,*fp;
	f.CreatePointFont(160,"微軟正黑體");		// 產生 font f; 160表示16 point的字
	fp=pDC->SelectObject(&f);					// 選用 font f
	pDC->SetBkColor(RGB(0,0,0));
	pDC->SetTextColor(RGB(255,255,255));
	char str[80];								// Demo 數字對字串的轉換
	sprintf(str, "%d", counter / 30);
	pDC->TextOut(623,455,str);
	pDC->SelectObject(fp);						// 放掉 font f (千萬不要漏了放掉)
	CDDraw::ReleaseBackCDC();					// 放掉 Back Plain 的 CDC
}

void CGameStateOver::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){
	const char KEY_SPACE = ' ';
	if (nChar == KEY_SPACE){
		CAudio::Instance()->Stop(3);
		GotoGameState(GAME_STATE_INIT);						// 切換至GAME_STATE_RUN
	}
}
/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲執行物件(過關畫面)
/////////////////////////////////////////////////////////////////////////////
CGameStatePASS::CGameStatePASS(CGame *g):CGameState(g){
}

void CGameStatePASS::OnInit(){
	PASS.LoadBitmap(IDB_PASS);
	//剩餘時間的數字圖
	t0.LoadBitmap("Bitmaps/t0.bmp",RGB(255,255,255));
	t1.LoadBitmap("Bitmaps/t1.bmp",RGB(255,255,255));
	t2.LoadBitmap("Bitmaps/t2.bmp",RGB(255,255,255));
	t3.LoadBitmap("Bitmaps/t3.bmp",RGB(255,255,255));
	t4.LoadBitmap("Bitmaps/t4.bmp",RGB(255,255,255));
	t5.LoadBitmap("Bitmaps/t5.bmp",RGB(255,255,255));
	t6.LoadBitmap("Bitmaps/t6.bmp",RGB(255,255,255));
	t7.LoadBitmap("Bitmaps/t7.bmp",RGB(255,255,255));
	t8.LoadBitmap("Bitmaps/t8.bmp",RGB(255,255,255));
	t9.LoadBitmap("Bitmaps/t9.bmp",RGB(255,255,255));
	//載入成績顯示圖形
	CiST.LoadBitmap();
	mu=true;//判斷是否已經執行到這裡
	ShowInitProgress(40);	// 一開始的loading進度為0%
}

void CGameStatePASS::OnBeginState(){
	CAudio::Instance()->Stop(6);//map1 audio
	CAudio::Instance()->Stop(7);//map2 audio
	if(mu==true)			//第二次重玩才會執行
		CAudio::Instance()->Play(AUDIO_SCUESS, false);
}

void CGameStatePASS::CalST(){
	//讀取檔案時間與分數
	readfromfile();
	//將時間轉回成分秒制
	if(time>=60){
		min=time/60;
		sec=time%60;
	}
	else{
		sec=time;
		min=0;
	}
	min1=min/10;
	min2=min%10;
	sec1=sec/10;
	sec2=sec%10;
	//載入成績成圖片
	CiST.SetInteger(score);
}

void CGameStatePASS::OnShow(){
	//設定背景圖顯示與位置
	PASS.ShowBitmap();
	PASS.SetTopLeft(0,0);
	//////成績顯示
	CiST.SetTopLeft(227,320);
	CiST.ShowBitmap();

	//呼叫計算成績與時間
	CalST();
	//分的第一位數顯示
	if(min1==1){
		t1.SetTopLeft(305,178);
		t1.ShowBitmap();
	}else if(min1==2){
		t2.SetTopLeft(303,178);
		t2.ShowBitmap();	
	}else if(min1==3){
		t3.SetTopLeft(303,178);
		t3.ShowBitmap();	
	}else if(min1==4){
		t4.SetTopLeft(303,178);
		t4.ShowBitmap();	
	}else if(min1==5){
		t5.SetTopLeft(303,178);
		t5.ShowBitmap();	
	}else if(min1==6){
		t6.SetTopLeft(303,178);
		t6.ShowBitmap();	
	}else if(min1==7){
		t7.SetTopLeft(303,178);
		t7.ShowBitmap();	
	}else if(min1==8){
		t8.SetTopLeft(303,178);
		t8.ShowBitmap();	
	}else if(min1==9){
		t9.SetTopLeft(303,178);
		t9.ShowBitmap();	
	}
	//分的第二位數顯示
	if(min2<1){
		t0.SetTopLeft(323,178);
		t0.ShowBitmap();	
	}else if(min2==1){
		t1.SetTopLeft(323,178);
		t1.ShowBitmap();
	}else if(min2==2){
		t2.SetTopLeft(323,178);
		t2.ShowBitmap();	
	}else if(min2==3){
		t3.SetTopLeft(323,178);
		t3.ShowBitmap();	
	}else if(min2==4){
		t4.SetTopLeft(323,178);
		t4.ShowBitmap();	
	}else if(min2==5){
		t5.SetTopLeft(323,178);
		t5.ShowBitmap();	
	}else if(min2==6){
		t6.SetTopLeft(323,178);
		t6.ShowBitmap();	
	}else if(min2==7){
		t7.SetTopLeft(323,178);
		t7.ShowBitmap();	
	}else if(min2==8){
		t8.SetTopLeft(323,178);
		t8.ShowBitmap();	
	}else if(min2==9){
		t9.SetTopLeft(323,178);
		t9.ShowBitmap();	
	}
	//秒的第一位數顯示
	if(sec1==0){
		t0.SetTopLeft(388,178);
		t0.ShowBitmap();	
	}else if(sec1==1){
		t1.SetTopLeft(388,178);
		t1.ShowBitmap();
	}else if(sec1==2){
		t2.SetTopLeft(388,178);
		t2.ShowBitmap();	
	}else if(sec1==3){
		t3.SetTopLeft(388,178);
		t3.ShowBitmap();	
	}else if(sec1==4){
		t4.SetTopLeft(388,178);
		t4.ShowBitmap();	
	}else if(sec1==5){
		t5.SetTopLeft(388,178);
		t5.ShowBitmap();	
	}else if(sec1==6){
		t6.SetTopLeft(388,178);
		t6.ShowBitmap();	
	}else if(sec1==7){
		t7.SetTopLeft(388,178);
		t7.ShowBitmap();	
	}else if(sec1==8){
		t8.SetTopLeft(388,178);
		t8.ShowBitmap();	
	}else if(sec1==9){
		t9.SetTopLeft(388,178);
		t9.ShowBitmap();	
	}
	//秒的第二位數顯示
	if(sec2==0){
		t0.SetTopLeft(408,178);
		t0.ShowBitmap();	
	}else if(sec2==1){
		t1.SetTopLeft(408,178);
		t1.ShowBitmap();
	}else if(sec2==2){
		t1.SetTopLeft(408,178);
		t2.ShowBitmap();	
	}else if(sec2==3){
		t3.SetTopLeft(408,178);
		t3.ShowBitmap();	
	}else if(sec2==4){
		t4.SetTopLeft(408,178);
		t4.ShowBitmap();	
	}else if(sec2==5){
		t5.SetTopLeft(408,178);
		t5.ShowBitmap();	
	}else if(sec2==6){
		t6.SetTopLeft(408,178);
		t6.ShowBitmap();	
	}else if(sec2==7){
		t7.SetTopLeft(408,178);
		t7.ShowBitmap();	
	}else if(sec2==8){
		t8.SetTopLeft(408,178);
		t8.ShowBitmap();	
	}else if(sec2==9){
		t9.SetTopLeft(408,178);
		t9.ShowBitmap();	
	}

}

void CGameStatePASS::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){
	const char KEY_SPACE = ' ';
	if ((nChar == KEY_SPACE)){
		if(stage==1)
		    GotoGameState(GAME_STATE_RUN2);						// 切換至GAME_STATE_RUN2
		else if(stage==2)
			GotoGameState(GAME_STATE_WIN);
	}
}

void CGameStatePASS::readfromfile(){
	ifstream inf;
	inf.open("map/ST.bin",ios::binary);
	GAME_ASSERT(inf,"儲存遊戲時間/得分失敗!\n(不影響遊戲進行，可進到下一關)");
	inf.read((char*)&score,sizeof(int));
	inf.read((char*)&time,sizeof(int));
	inf.read((char*)&stage,sizeof(int));
	inf.close();
}


/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲執行物件，主要的遊戲程式都在這裡
/////////////////////////////////////////////////////////////////////////////

CGameStateRun::CGameStateRun(CGame *g)
: CGameState(g),fn("map/map1.txt"),gamemap(fn)//設定該關卡的地圖檔案
{
	obstacles = new Obstacle[gamemap.Getcoutobstacle()];//產生障礙物件
	monster =  new Monster[4];//產生怪物數量為4
	treasure = new Treasure[gamemap.Getcounttreasure()];//產生寶物物件
	door    = new Door[gamemap.Getcountdoors()];//產生門物件
	if(gamemap.Getcountkeys()!=0){
		key = new Key[gamemap.Getcountkeys()];
	}
	else{
	    key = NULL;
	}
}

CGameStateRun::~CGameStateRun()
{
	delete [] obstacles;
	delete [] monster;
	delete [] treasure;
	delete [] door;
	delete [] key;
}

void CGameStateRun::OnBeginState()
{
	score=0;//該關關卡成績計算初始為0
	gamemap.Initialize(fn);	//初始化遊戲地圖
	thief.Initialize();	//初始化盜賊
	redscreen.Initialize();//初始化紅幕閃爍
	help.Initialize();//初始化遊戲說明
	for(int i=0;i<4;i++){
		monster[i].Initialize();
	}
	for(int i=0;i<gamemap.Getcounttreasure();i++){
		treasure[i].Initialize();
	}
	for(int i=0;i<gamemap.Getcountdoors();i++){
		door[i].Initialize(&gamemap);
	}
	for(int i=0;i<gamemap.Getcountkeys();i++){
		key[i].Initialize();
	}
	//設定怪物行走路徑
	int path0[9][2]={{57,8},{77,8},{97,8},{123,8},{123,20},{103,20},{83,20},{63,20},{57,20}};
	int path1[8][2]={{147,27},{127,27},{107,27},{87,27},{67,27},{47,27},{23,27},{23,32}};
	int path2[11][2]={{1,38},{1,50},{21,50},{41,50},{61,50},{81,50},{101,50},{112,50},{112,38},{132,38},{154,38}};
	int path3[11][2]={{1,59},{22,59},{22,83},{44,83},{64,83},{64,94},{84,94},{104,94},{124,94},{144,94},{152,94}};
	//生設定怪物行走路徑
	monster[0].SetXY(path0[0][0],path0[0][1]);
	monster[1].SetXY(path1[0][0],path1[0][1]);
	monster[2].SetXY(path2[0][0],path2[0][1]);
	monster[3].SetXY(path3[0][0],path3[0][1]);
	//產生怪物行走路徑Link
	vector<MovingNode*> scriptpath0;
	vector<MovingNode*> scriptpath1;
	vector<MovingNode*> scriptpath2;
	vector<MovingNode*> scriptpath3;
	for(int i=0;i<9;i++){
		scriptpath0.push_back(new MovingNode(path0[i][0],path0[i][1]));
	}
	for(int i=0;i<8;i++){
	    scriptpath1.push_back(new MovingNode(path1[i][0],path1[i][1]));
	}
	for(int i=0;i<11;i++){
	    scriptpath2.push_back(new MovingNode(path2[i][0],path2[i][1]));
	}
	for(int i=0;i<11;i++){
		scriptpath3.push_back(new MovingNode(path3[i][0],path3[i][1]));
	}
	monster[0].Setcommenpath(scriptpath0);
	monster[1].Setcommenpath(scriptpath1);
	monster[2].Setcommenpath(scriptpath2);
	monster[3].Setcommenpath(scriptpath3);
	//關閉開始畫面音樂,播放第一關音樂
	CAudio::Instance()->Stop(0);
	CAudio::Instance()->Play(AUDIO_MAP1, true);	
	gamemap.Setsxsy(thief.Getx()-16,thief.Gety()-12);	//產生盜賊的位置
	Tcc.SetTime(limatetime);	//設定關卡遊戲時間
	counter = 30 * Tcc.GetTime(); //設定遊戲關卡可以玩的時間,時間到進入遊戲失敗頁
	treasure_remind = gamemap.Getcounttreasure();//取得寶物總數量
	cheatmode = false;//作弊模式預設為不啟動
	dkeybordisdown = false;
	wkeybordisdown = false;
	togetalltreasure = false;
}

void CGameStateRun::OnMove()							// 移動遊戲元素
{
	if(!help.GetState()){
	    return;
	}
	if(togetalltreasure&&treasure_remind!=0){
		treasure_remind = 0;
		for(int i=0;i<gamemap.Getcounttreasure();i++){
			treasure[i].Setlife(false);
		}
	}
	//如果希望修改cursor的樣式，則將下面程式的commment取消即可
	//SetCursor(AfxGetApp()->LoadCursor(IDC_GAMECURSOR));
	counter--;	//時間計算依照每秒30運行次數倒數
	thief.OnMove(&gamemap,door,key);
	for(int i=0;i<4;i++)
	    monster[i].OnMove(&gamemap,&thief,door,cheatmode);
	for(int i=0;i<gamemap.Getcounttreasure();i++){
		if(treasure[i].Isalife()&&treasure[i].HitThief(&thief)){//寶物存活且被盜賊擊中
			CAudio::Instance()->Play(AUDIO_PICK, false);
			treasure_remind--;
			treasure[i].Setlife(false);//將寶物設定已取得
			score+=1000;		//增加成績分數(寶物)
		}
	}
	for(int i=0;i<gamemap.Getcountkeys();i++){
		if(key[i].hitThief(thief.Getx(),thief.Gety())&&key[i].getisOnMap()){
		    CAudio::Instance()->Play(AUDIO_KEY, false);		//播放KEY音效
			key[i].setOnMap(false);
			score+=15;			//增加成績分數(KEY)
		}
	}
	if(cheatmode == false){
	    for(int i=0;i<4;i++){
	        if(thief.Getx()-monster[i].Getx()>=-4&&thief.Getx()-monster[i].Getx()<=4&&thief.Gety()-monster[i].Gety()<=5&&thief.Gety()-monster[i].Gety()>=-5){
				CAudio::Instance()->Play(AUDIO_MAP1, false);
				GotoGameState(GAME_STATE_OVER);
	        }
	    }
	}
	for(int i=0;i<4;i++){
		if(monster[i].GetisTrace()){//偵測到被怪物追蹤
			redscreen.SetisFlash(true);//設定閃爍螢幕
			break;
		}
		else if(i==3&& !monster[i].GetisTrace()){//偵測到不被怪物追蹤
		    redscreen.SetisFlash(false);//將閃爍解除
		}
	}
	if(treasure_remind==0){//已經取得所有寶物了,可以進到過關條件
		if(gamemap.Isempty(thief.Getx(),thief.Gety())==999){
			//寫入成績/時間
			int savetime = Tcc.GetTime();
			ofstream ouf;
			ouf.open("map/ST.bin",ios::out|ios::binary);
			GAME_ASSERT(ouf,"寫入成績/時間失敗.");
			ouf.write ((char*)&score,sizeof(int));
			ouf.write ((char*)&savetime,sizeof(int));
			ouf.write ((char*)&stage,sizeof(int));
			ouf.close();
			CAudio::Instance()->Play(AUDIO_MAP1, false);
		    GotoGameState(GAME_STATE_PASS);
		}
		else if(gamemap.Isempty(thief.Getx()+3,thief.Gety())==999){

				//寫入成績/時間
			int savetime = Tcc.GetTime();
			ofstream ouf;
			ouf.open("map/ST.bin",ios::out|ios::binary);
			GAME_ASSERT(ouf,"寫入成績/時間失敗.");
			ouf.write ((char*)&score,sizeof(int));
			ouf.write ((char*)&savetime,sizeof(int));
			ouf.write ((char*)&stage,sizeof(int));
			ouf.close();
			CAudio::Instance()->Play(AUDIO_MAP1, false);
		    GotoGameState(GAME_STATE_PASS);
		}
		else if(gamemap.Isempty(thief.Getx(),thief.Gety()+4)==999){

				//寫入成績/時間
			int savetime = Tcc.GetTime();
			ofstream ouf;
			ouf.open("map/ST.bin",ios::out|ios::binary);
			GAME_ASSERT(ouf,"寫入成績/時間失敗.");
			ouf.write ((char*)&score,sizeof(int));
			ouf.write ((char*)&savetime,sizeof(int));
			ouf.write ((char*)&stage,sizeof(int));
			ouf.close();
			CAudio::Instance()->Play(AUDIO_MAP1, false);
		    GotoGameState(GAME_STATE_PASS);
		}
		else if(gamemap.Isempty(thief.Getx()+3,thief.Gety()+4)==999){

			//寫入成績/時間
			int savetime = Tcc.GetTime();
			ofstream ouf;
			ouf.open("map/ST.bin",ios::out|ios::binary);
			GAME_ASSERT(ouf,"寫入成績/時間失敗.");
			ouf.write ((char*)&score,sizeof(int));
			ouf.write ((char*)&savetime,sizeof(int));
			ouf.write ((char*)&stage,sizeof(int));
			ouf.close();
			CAudio::Instance()->Play(AUDIO_MAP1, false);
		    GotoGameState(GAME_STATE_PASS);
		}
	}
	//如果計數器剛好被整除,代表執行到剛好秒的位子
	if((counter%30)==0){
		Tcc.SetTime(Tcc.GetTime()-1);		//剛好要到換時間的地方就呼叫改變時間
	}else if(counter <0){//時間到,進入失敗畫面
		CAudio::Instance()->Play(AUDIO_MAP1, false);
		GotoGameState(GAME_STATE_OVER);
	}else{
	}
}

void CGameStateRun::OnInit(){  								//開始載入資料,遊戲的初值及圖形設定
	ShowInitProgress(50);	// 接個前一個狀態的進度，此處進度視為33%
	cheat.LoadBitmap(IDB_CHEAT,RGB(255,255,255));
	help.LoadBitmap();	//載入說明圖檔
	thief.LoadBitmap();//載入盜賊圖檔
	redscreen.LoadBitmap();//載入紅螢幕圖檔
	for(int i=0;i<4;i++){//載入四個怪物圖檔
	    monster[i].LoadBitmap();
	}
	Tcc.LoadBitmap(false);//載入時間倒數圖檔
	gamemap.LoadMap();//載入地圖圖檔
	for(int i=0;i<gamemap.Getcounttreasure();i++)
		treasure[i].LoadBitmap();
	for(int i=0;i<gamemap.Getcoutobstacle();i++)
		obstacles[i].Load();
	int k=0;
	for(int i=0;i<160;i++){
		for(int j=0;j<120;j++){
			if(gamemap.Isempty(i,j)==0){
				obstacles[k].setxy(i,j);
				k++;
			}
		}
	}
	k=0;
	for(int i=0;i<160;i++){
		for(int j=0;j<120;j++){
			if(gamemap.Isempty(i,j)==2){
			    treasure[k].SetXY(i,j);
				gamemap.toempty(i,j);
				k++;
			}
		}
	}
	k=0;
	for(int i=0;i<160;i++){
		for(int j=0;j<120;j++){
			if(gamemap.DetialDoor(i,j)==1){
				door[k].setDoor(i,j,'h','n');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==2){
			    door[k].setDoor(i,j,'v','n');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==3){
				door[k].setDoor(i,j,'Y','y');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==4){
				door[k].setDoor(i,j,'Z','y');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==5){
				door[k].setDoor(i,j,'R','r');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==6){
				door[k].setDoor(i,j,'S','r');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==7){
				door[k].setDoor(i,j,'G','g');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==8){
				door[k].setDoor(i,j,'H','g');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==9){
				door[k].setDoor(i,j,'P','p');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==10){
				door[k].setDoor(i,j,'Q','p');
				k++;
			}
		}
	}
	GAME_ASSERT(gamemap.Getcountdoors()==k,"取得門數量錯誤!");
	k=0;
	for(int i=0;i<160;i++){
		for(int j=0;j<120;j++){
			if(gamemap.Isempty(i,j)==5){
				key[k].setKey(i,j,gamemap.KeyColor(i,j));
				gamemap.toempty(i,j);
				k++;
			}
		}
	}
	for(int i=0;i<gamemap.Getcountdoors();i++)
		door[i].LoadDoor();
	for(int i=0;i<gamemap.Getcountkeys();i++)
		key[i].LoadKey();

	ShowInitProgress(80);	// 完成部分Loading動作，提高進度
	// 繼續載入其他資料
	// 此OnInit動作會接到CGameStaterOver::OnInit()，所以進度還沒到100%
}

void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
	const char KEY_LEFT  = 0x25; // keyboard左箭頭
	const char KEY_UP    = 0x26; // keyboard上箭頭
	const char KEY_RIGHT = 0x27; // keyboard右箭頭
	const char KEY_DOWN  = 0x28; // keyboard下箭頭
	const char KEY_D     = 'D';
	const char KEY_SPACE     = ' ';
	const char KEY_W     ='W';

	if((nChar == KEY_SPACE)&&(help.GetState()==false)){//一開始出現說明頁,玩家要按空白鍵繼續遊戲
		help.SetOk(true);//設定解除遊戲說明頁
		Tcc.SetTime(limatetime);//重設時間為一開始的時間,也就是不列入看說明的時間
	}
	//盜賊上下左右移動鍵盤判定
	if (nChar == KEY_LEFT){
		thief.SetMovingLeft(true);
	}
	if (nChar == KEY_RIGHT){
		thief.SetMovingRight(true);
	}
	if (nChar == KEY_UP){
		thief.SetMovingUp(true);
	}
	if (nChar == KEY_DOWN){
		thief.SetMovingDown(true);
	}
	if (nChar = KEY_D){
		dkeybordisdown = true;
	}
	if(nChar = KEY_W){
		wkeybordisdown = true;
	}
}

void CGameStateRun::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	const char KEY_LEFT  = 0x25; // keyboard左箭頭
	const char KEY_UP    = 0x26; // keyboard上箭頭
	const char KEY_RIGHT = 0x27; // keyboard右箭頭
	const char KEY_DOWN  = 0x28; // keyboard下箭頭
	const char KEY_D     = 'D';
	const char KEY_SPACE     = ' ';
	const char KEY_W    = 'W';
	
	if((nChar == KEY_SPACE)&&(help.GetState()==false)){//一開始出現說明頁,玩家要按空白鍵繼續遊戲
		help.SetOk(true);//設定解除遊戲說明頁
		Tcc.SetTime(limatetime);//重設時間為一開始的時間,也就是不列入看說明的時間
	}
	//盜賊上下左右移動鍵盤判定
	if (nChar == KEY_LEFT){
		thief.SetMovingLeft(false);
		thief.reststep();
	}
	if (nChar == KEY_RIGHT){
		thief.SetMovingRight(false);
		thief.reststep();
	}
	if (nChar == KEY_UP){
		thief.SetMovingUp(false);
		thief.reststep();
	}
	if (nChar == KEY_DOWN){
		thief.SetMovingDown(false);
		thief.reststep();
	}
	//作弊模式
	if(nChar == KEY_D){
	   if(dkeybordisdown){
	       if(cheatmode){
		       cheatmode = false;
		   }
		   else{
		       cheatmode = true;
		   }
		   dkeybordisdown = false;
	   }
	}
	if(nChar == KEY_W){
	   if(wkeybordisdown){
		   togetalltreasure=true;
	   }
	}
}

void CGameStateRun::OnShow()
{
	//  注意：Show裡面千萬不要移動任何物件的座標，移動座標的工作應由Move做才對，
	//        否則當視窗重新繪圖時(OnDraw)，物件就會移動，看起來會很怪。換個術語
	//        說，Move負責MVC中的Model，Show負責View，而View不應更動Model。
	gamemap.OnShow();//地圖背景(地板)顯示
	thief.OnShow(&gamemap);	//盜賊顯示
	for(int i=0;i<gamemap.Getcountkeys();i++)
		key[i].Onshow(&gamemap);
	for(int i=0;i<4;i++)
	    monster[i].OnShow(&gamemap);//怪獸顯示
	for(int i=0 ;i<gamemap.Getcoutobstacle();i++)//不斷重新產生障礙地圖
		obstacles[i].OnShow(&gamemap);
	for(int i=0;i<gamemap.Getcounttreasure();i++)
		treasure[i].OnShow(&gamemap);
 	for(int i=0;i<gamemap.Getcountdoors();i++)
		door[i].OnShow(&gamemap);
	Tcc.OnShow();//專屬於每關卡計時器的物件
	redscreen.OnShow();//顯示紅螢幕物件
	help.OnShow();//顯示說明物件
	if(cheatmode==true){
		cheat.SetTopLeft(-1,421);
		cheat.ShowBitmap();
	}else{
	
	}
}


/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲執行物件，主要的遊戲程式都在這裡
/////////////////////////////////////////////////////////////////////////////

CGameStateRun2::CGameStateRun2(CGame *g)
: CGameState(g),fn("map/map2.txt"),gamemap(fn)//設定該關卡的地圖檔案
{
	obstacles = new Obstacle[gamemap.Getcoutobstacle()];//產生障礙物件
	monster =  new Monster[4];//產生怪物數量為4
	treasure = new Treasure[gamemap.Getcounttreasure()];//產生寶物物件
	door    = new Door[gamemap.Getcountdoors()];//產生門物件
	if(gamemap.Getcountkeys()!=0){
		key = new Key[gamemap.Getcountkeys()];
	}
	else{
	    key = NULL;
	}
}

CGameStateRun2::~CGameStateRun2()
{
	delete [] obstacles;
	delete [] monster;
	delete [] treasure;
	delete [] door;
	delete [] key;
}

void CGameStateRun2::OnBeginState()
{
	score=0;//該關關卡成績計算初始為0
	gamemap.Initialize(fn);	//初始化遊戲地圖
	thief.Initialize();	//初始化盜賊
	redscreen.Initialize();//初始化紅幕閃爍
	help.Initialize();//初始化遊戲說明
	togetalltreasure = false;
	for(int i=0;i<4;i++){
		monster[i].Initialize();
	}
	for(int i=0;i<gamemap.Getcounttreasure();i++){
		treasure[i].Initialize();
	}
	for(int i=0;i<gamemap.Getcountdoors();i++){
		door[i].Initialize(&gamemap);
	}
	for(int i=0;i<gamemap.Getcountkeys();i++){
		key[i].Initialize();
	}
	//設定怪物行走路徑
	int path0[13][2]={{101,40},{89,40},{89,44},{106,44},{106,53},{86,53},{66,53},{59,53},{59,62},{39,62},{19,62},{8,62},{8,81}};
	int path1[12][2]={{69,111},{49,111},{45,111},{45,114},{25,114},{4,114},{4,94},{4,90},{23,90},{23,95},{39,95},{39,105}};
	int path2[8][2]={{128,114},{154,114},{154,94},{134,94},{129,94},{129,80},{109,80},{94,80}};
	int path3[6][2]={{139,61},{119,61},{99,61},{79,61},{75,61},{75,80}};
	//生設定怪物行走路徑
	monster[0].SetXY(path0[0][0],path0[0][1]);
	monster[1].SetXY(path1[0][0],path1[0][1]);
	monster[2].SetXY(path2[0][0],path2[0][1]);
	monster[3].SetXY(path3[0][0],path3[0][1]);
	//產生怪物行走路徑Link
	vector<MovingNode*> scriptpath0;
	vector<MovingNode*> scriptpath1;
	vector<MovingNode*> scriptpath2;
	vector<MovingNode*> scriptpath3;
	for(int i=0;i<13;i++){
		scriptpath0.push_back(new MovingNode(path0[i][0],path0[i][1]));
	}
	for(int i=0;i<12;i++){
	    scriptpath1.push_back(new MovingNode(path1[i][0],path1[i][1]));
	}
	for(int i=0;i<8;i++){
	    scriptpath2.push_back(new MovingNode(path2[i][0],path2[i][1]));
	}
	for(int i=0;i<6;i++){
		scriptpath3.push_back(new MovingNode(path3[i][0],path3[i][1]));
	}
	monster[0].Setcommenpath(scriptpath0);
	monster[1].Setcommenpath(scriptpath1);
	monster[2].Setcommenpath(scriptpath2);
	monster[3].Setcommenpath(scriptpath3);
	//關閉開始畫面音樂,播放第一關音樂
	CAudio::Instance()->Stop(0);
	CAudio::Instance()->Stop(6);
	CAudio::Instance()->Play(AUDIO_MAP2, true);	
	gamemap.Setsxsy(thief.Getx()-16,thief.Gety()-12);	//產生盜賊的位置
	Tcc.SetTime(limatetime);	//設定關卡遊戲時間
	counter = 30 * Tcc.GetTime(); //設定遊戲關卡可以玩的時間,時間到進入遊戲失敗頁
	treasure_remind = gamemap.Getcounttreasure();//取得寶物總數量
	cheatmode = false;//作弊模式預設為不啟動
	wkeybordisdown = false;
	dkeybordisdown = false;
}

void CGameStateRun2::OnMove()							// 移動遊戲元素
{
	
	//如果希望修改cursor的樣式，則將下面程式的commment取消即可
	//SetCursor(AfxGetApp()->LoadCursor(IDC_GAMECURSOR));
	if(!help.GetState()){
	    return;
	}
	if(togetalltreasure&&treasure_remind!=0){
		treasure_remind = 0;
		for(int i=0;i<gamemap.Getcounttreasure();i++){
			treasure[i].Setlife(false);
		}
	}
	counter--;	//時間計算依照每秒30運行次數倒數
	thief.OnMove(&gamemap,door,key);
	for(int i=0;i<4;i++)
	    monster[i].OnMove(&gamemap,&thief,door,cheatmode);
	for(int i=0;i<gamemap.Getcounttreasure();i++){
		if(treasure[i].Isalife()&&treasure[i].HitThief(&thief)){//寶物存活且被盜賊擊中
			CAudio::Instance()->Play(AUDIO_PICK, false);
			treasure_remind--;
			treasure[i].Setlife(false);//將寶物設定已取得
			score+=1000;		//增加成績分數(寶物)
		}
	}
	for(int i=0;i<gamemap.Getcountkeys();i++){
		if(key[i].hitThief(thief.Getx(),thief.Gety())&&key[i].getisOnMap()){
		    CAudio::Instance()->Play(AUDIO_KEY, false);		//播放KEY音效
			key[i].setOnMap(false);
			score+=15;			//增加成績分數(KEY)
		}
	}
	if(cheatmode == false){
	    for(int i=0;i<4;i++){
	        if(thief.Getx()-monster[i].Getx()>=-4&&thief.Getx()-monster[i].Getx()<=4&&thief.Gety()-monster[i].Gety()<=5&&thief.Gety()-monster[i].Gety()>=-5){
				CAudio::Instance()->Play(AUDIO_MAP2, false);	
				GotoGameState(GAME_STATE_OVER);
	        }
	    }
	}
	for(int i=0;i<4;i++){
		if(monster[i].GetisTrace()){//偵測到被怪物追蹤
			redscreen.SetisFlash(true);//設定閃爍螢幕
			break;
		}
		else if(i==3&& !monster[i].GetisTrace()){//偵測到不被怪物追蹤
		    redscreen.SetisFlash(false);//將閃爍解除
		}
	}
	if(treasure_remind==0){//已經取得所有寶物了,可以進到過關條件
		if(gamemap.Isempty(thief.Getx(),thief.Gety())==999){
			//寫入成績/時間
			int savetime = Tcc.GetTime();
			ofstream ouf;
			ouf.open("map/ST.bin",ios::out|ios::binary);
			GAME_ASSERT(ouf,"寫入成績/時間失敗.");
			ouf.write ((char*)&score,sizeof(int));
			ouf.write ((char*)&savetime,sizeof(int));
			ouf.write ((char*)&stage,sizeof(int));
			ouf.close();
			CAudio::Instance()->Play(AUDIO_MAP2, false);	
		    GotoGameState(GAME_STATE_PASS);
		}
		else if(gamemap.Isempty(thief.Getx()+3,thief.Gety())==999){

			//寫入成績/時間
			int savetime = Tcc.GetTime();
			ofstream ouf;
			ouf.open("map/ST.bin",ios::out|ios::binary);
			GAME_ASSERT(ouf,"寫入成績/時間失敗.");
			ouf.write ((char*)&score,sizeof(int));
			ouf.write ((char*)&savetime,sizeof(int));
			ouf.write ((char*)&stage,sizeof(int));
			ouf.close();
			CAudio::Instance()->Play(AUDIO_MAP2, false);	
		    GotoGameState(GAME_STATE_PASS);
		}
		else if(gamemap.Isempty(thief.Getx(),thief.Gety()+4)==999){

			//寫入成績/時間
			int savetime = Tcc.GetTime();
			ofstream ouf;
			ouf.open("map/ST.bin",ios::out|ios::binary);
			GAME_ASSERT(ouf,"寫入成績/時間失敗.");
			ouf.write ((char*)&score,sizeof(int));
			ouf.write ((char*)&savetime,sizeof(int));
			ouf.write ((char*)&stage,sizeof(int));
			ouf.close();
			CAudio::Instance()->Play(AUDIO_MAP2, false);	
		    GotoGameState(GAME_STATE_PASS);
		}
		else if(gamemap.Isempty(thief.Getx()+3,thief.Gety()+4)==999){

			//寫入成績/時間
			int savetime = Tcc.GetTime();
			ofstream ouf;
			ouf.open("map/ST.bin",ios::out|ios::binary);
			GAME_ASSERT(ouf,"寫入成績/時間失敗.");
			ouf.write ((char*)&score,sizeof(int));
			ouf.write ((char*)&savetime,sizeof(int));
			ouf.write ((char*)&stage,sizeof(int));
			ouf.close();
			CAudio::Instance()->Play(AUDIO_MAP2, false);	
		    GotoGameState(GAME_STATE_PASS);
		}
	}
	//如果計數器剛好被整除,代表執行到剛好秒的位子
	if((counter%30)==0){
		Tcc.SetTime(Tcc.GetTime()-1);		//剛好要到換時間的地方就呼叫改變時間
	}else if(counter <0){//時間到,進入失敗畫面
		CAudio::Instance()->Play(AUDIO_MAP2, false);	
		GotoGameState(GAME_STATE_OVER);
	}else{
	}
}

void CGameStateRun2::OnInit(){  								//開始載入資料,遊戲的初值及圖形設定
	//ShowInitProgress(50);	// 接個前一個狀態的進度，此處進度視為33%
	Tcc.LoadBitmap(true);
	cheat.LoadBitmap(IDB_CHEAT,RGB(255,255,255));
	help.LoadBitmap();	//載入說明圖檔
	thief.LoadBitmap();//載入盜賊圖檔
	redscreen.LoadBitmap();//載入紅螢幕圖檔
	for(int i=0;i<4;i++){//載入四個怪物圖檔
	    monster[i].LoadBitmap();
	}
	gamemap.LoadMap();//載入地圖圖檔
	for(int i=0;i<gamemap.Getcounttreasure();i++)
		treasure[i].LoadBitmap();
	for(int i=0;i<gamemap.Getcoutobstacle();i++)
		obstacles[i].Load();
	int k=0;
	for(int i=0;i<160;i++){
		for(int j=0;j<120;j++){
			if(gamemap.Isempty(i,j)==0){
				obstacles[k].setxy(i,j);
				k++;
			}
		}
	}
	k=0;
	for(int i=0;i<160;i++){
		for(int j=0;j<120;j++){
			if(gamemap.Isempty(i,j)==2){
			    treasure[k].SetXY(i,j);
				gamemap.toempty(i,j);
				k++;
			}
		}
	}
	k=0;
	for(int i=0;i<160;i++){
		for(int j=0;j<120;j++){
			if(gamemap.DetialDoor(i,j)==1){
				door[k].setDoor(i,j,'h','n');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==2){
			    door[k].setDoor(i,j,'v','n');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==3){
				door[k].setDoor(i,j,'Y','y');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==4){
				door[k].setDoor(i,j,'Z','y');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==5){
				door[k].setDoor(i,j,'R','r');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==6){
				door[k].setDoor(i,j,'S','r');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==7){
				door[k].setDoor(i,j,'G','g');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==8){
				door[k].setDoor(i,j,'H','g');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==9){
				door[k].setDoor(i,j,'P','p');
				k++;
			}
			else if(gamemap.DetialDoor(i,j)==10){
				door[k].setDoor(i,j,'Q','p');
				k++;
			}
		}
	}
	GAME_ASSERT(gamemap.Getcountdoors()==k,"取得門數量錯誤!");
	k=0;
	for(int i=0;i<160;i++){
		for(int j=0;j<120;j++){
			if(gamemap.Isempty(i,j)==5){
				key[k].setKey(i,j,gamemap.KeyColor(i,j));
				gamemap.toempty(i,j);
				k++;
			}
		}
	}
	for(int i=0;i<gamemap.Getcountdoors();i++)
		door[i].LoadDoor();
	for(int i=0;i<gamemap.Getcountkeys();i++)
		key[i].LoadKey();

	// 繼續載入其他資料
	// 此OnInit動作會接到CGameStaterOver::OnInit()，所以進度還沒到100%
}

void CGameStateRun2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
	const char KEY_LEFT  = 0x25; // keyboard左箭頭
	const char KEY_UP    = 0x26; // keyboard上箭頭
	const char KEY_RIGHT = 0x27; // keyboard右箭頭
	const char KEY_DOWN  = 0x28; // keyboard下箭頭
	const char KEY_D     = 'D';
	const char KEY_SPACE     = ' ';
	const char KEY_W     ='W';

	if((nChar == KEY_SPACE)&&(help.GetState()==false)){//一開始出現說明頁,玩家要按空白鍵繼續遊戲
		help.SetOk(true);//設定解除遊戲說明頁
		Tcc.SetTime(limatetime);//重設時間為一開始的時間,也就是不列入看說明的時間
	}
	//盜賊上下左右移動鍵盤判定
	if (nChar == KEY_LEFT){
		thief.SetMovingLeft(true);
	}
	if (nChar == KEY_RIGHT){
		thief.SetMovingRight(true);
	}
	if (nChar == KEY_UP){
		thief.SetMovingUp(true);
	}
	if (nChar == KEY_DOWN){
		thief.SetMovingDown(true);
	}
	if (nChar = KEY_D){
		dkeybordisdown = true;
	}
	if(nChar = KEY_W){
		wkeybordisdown = true;
	}
}

void CGameStateRun2::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	const char KEY_LEFT  = 0x25; // keyboard左箭頭
	const char KEY_UP    = 0x26; // keyboard上箭頭
	const char KEY_RIGHT = 0x27; // keyboard右箭頭
	const char KEY_DOWN  = 0x28; // keyboard下箭頭
	const char KEY_D     = 'D';
	const char KEY_SPACE     = ' ';
	const char KEY_W    = 'W';
	
	if((nChar == KEY_SPACE)&&(help.GetState()==false)){//一開始出現說明頁,玩家要按空白鍵繼續遊戲
		help.SetOk(true);//設定解除遊戲說明頁
		Tcc.SetTime(limatetime);//重設時間為一開始的時間,也就是不列入看說明的時間
	}
	//盜賊上下左右移動鍵盤判定
	if (nChar == KEY_LEFT){
		thief.SetMovingLeft(false);
		thief.reststep();
	}
	if (nChar == KEY_RIGHT){
		thief.SetMovingRight(false);
		thief.reststep();
	}
	if (nChar == KEY_UP){
		thief.SetMovingUp(false);
		thief.reststep();
	}
	if (nChar == KEY_DOWN){
		thief.SetMovingDown(false);
		thief.reststep();
	}
	//作弊模式
	if(nChar == KEY_D){
	   if(dkeybordisdown){
	       if(cheatmode){
		       cheatmode = false;
		   }
		   else{
		       cheatmode = true;
		   }
		   dkeybordisdown = false;
	   }
	}
	if(nChar == KEY_W){
	   if(wkeybordisdown){
		   togetalltreasure=true;
	   }
	}
}

void CGameStateRun2::OnShow()
{
	//  注意：Show裡面千萬不要移動任何物件的座標，移動座標的工作應由Move做才對，
	//        否則當視窗重新繪圖時(OnDraw)，物件就會移動，看起來會很怪。換個術語
	//        說，Move負責MVC中的Model，Show負責View，而View不應更動Model。
	gamemap.OnShow();//地圖背景(地板)顯示
	thief.OnShow(&gamemap);	//盜賊顯示
	for(int i=0;i<gamemap.Getcountkeys();i++)
		key[i].Onshow(&gamemap);
	for(int i=0;i<4;i++)
	    monster[i].OnShow(&gamemap);//怪獸顯示
	for(int i=0 ;i<gamemap.Getcoutobstacle();i++)//不斷重新產生障礙地圖
		obstacles[i].OnShow(&gamemap);
	for(int i=0;i<gamemap.Getcounttreasure();i++)
		treasure[i].OnShow(&gamemap);
 	for(int i=0;i<gamemap.Getcountdoors();i++)
		door[i].OnShow(&gamemap);

	Tcc.OnShow();//專屬於每關卡計時器的物件
	redscreen.OnShow();//顯示紅螢幕物件
	help.OnShow();//顯示說明物件
	if(cheatmode==true){
		cheat.SetTopLeft(-1,421);
		cheat.ShowBitmap();
	}else{
	
	}
}

}