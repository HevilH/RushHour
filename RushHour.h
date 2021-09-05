#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <math.h>
#include "resource.h"

#define TIMER_ID             1
#define TIMER_ELAPSE         30
#define WNDWIDTH             500
#define WNDHEIGHT            360
#define BLOCK_COLOR_NUM      4
#define ROOF_COLOR_NUM       2
#define MAX_TERRIAN_NUM      10

   
#define HERO_SIZE_X          30
#define HERO_SIZE_Y          50
#define HERO_MAX_FRAME_NUM   3
#define BUILDING_SIZE_X      500
#define BUILDING_SIZE_Y      250
#define ROOF_SIZE_X          65
#define ROOF_SIZE_Y          33
#define BLOCK_SIZE_X         65
#define BLOCK_SIZE_Y         65
#define GAME_STATUS_SIZE_X   40
#define GAME_STATUS_SIZE_Y   30



typedef struct
{
	HBITMAP	hBmp;
	POINT	pos;
	SIZE	size;
	int     curFrameIndex;
	int     maxFrameSize;
	bool    isJump;
	int     jmpTime;
	int     jmpDistance;
}Hero;


typedef struct
{
	HBITMAP	hBmp;
	POINT	pos;
	SIZE	size;
}Building;


typedef struct
{
	HBITMAP hBlockBmp;
	HBITMAP hRoofBmp;
	POINT	pos;
	SIZE    size;
	int     roofWidth;
	int     roofHeight;
	int     blockWidth;
	int     blockHeight;
	int     blockNum;
}Terrian;


typedef struct
{
	HBITMAP	hBmp;
	POINT   pos;
	SIZE	size;
	BOOL	isPaused;
	int     totalDist;
	bool    isEnd;
}GameStatus;


static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("RushHour");


HBITMAP m_hBackgroundBmp;
HBITMAP m_hBuildingBmp;
HBITMAP m_hHeroBmp;
HBITMAP m_hGameStatusBmp;
HBITMAP	m_hBlockBmp[BLOCK_COLOR_NUM];
HBITMAP	m_hRoofkBmp[ROOF_COLOR_NUM];
HBITMAP m_hEndBmp;


int	m_blockBmpNames[] = { IDB_BLUE_BLOCK, IDB_GREEN_BLOCK, IDB_ORANGE_BLOCK, IDB_PINK_BLOCK };


int	m_roofBmpNames[] = { IDB_BLACK_ROOF, IDB_GREY_ROOF };


Hero          m_hero;
Building      m_building;
Terrian       m_terrian[MAX_TERRIAN_NUM];
GameStatus    m_gameStatus;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

VOID Init(HWND hWnd, WPARAM wParam, LPARAM lParam);


Hero CreateHero(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp, int curFrameIndex, int maxFrameSize);

Building CreateBuilding(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp);


GameStatus CreateGameStatus(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp);


Terrian CreateTerrian(LONG posX, LONG posY, LONG sizeX, LONG sizeY, 
					  HBITMAP hBlockBmp, HBITMAP hRoofBmp, int roofHeight, int blockHeight);


VOID Render(HWND hWnd);

VOID TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam);

VOID HeroUpdate();

VOID TerrianUpdate();

VOID GameStatusUpdate();

BOOL Paused(POINT);
//键盘按下事件处理
VOID KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

VOID KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

VOID LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

VOID Collision();
