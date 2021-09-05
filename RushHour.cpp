#include "RushHour.h"

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Mega Plane"),
			MB_OK);

		return 1;
	}


	hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, 
		CW_USEDEFAULT, CW_USEDEFAULT,
		WNDWIDTH, WNDHEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Mega Plane"),
			MB_OK);

		return 1;
	}

	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);


	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		Init(hWnd, wParam, lParam);
		break;
	case WM_PAINT:
		Render(hWnd);
		break;
	case WM_KEYDOWN:
		KeyDown(hWnd, wParam, lParam);
		break;
	case WM_KEYUP:
		KeyUp(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		LButtonDown(hWnd, wParam, lParam);
		break;
	case WM_TIMER:
		TimerUpdate(hWnd, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

VOID Init(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

	m_hBackgroundBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BACKGROUND));

	m_hBuildingBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BUILDING));

	m_hHeroBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_HERO));

	m_hGameStatusBmp =  LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_GAME_STATUS));

	m_hEndBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_END));

	int k;
	for (k = 0; k < BLOCK_COLOR_NUM; ++k)
	{
		m_hBlockBmp[k] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
			MAKEINTRESOURCE(m_blockBmpNames[k]));
	}

	for (k = 0; k < ROOF_COLOR_NUM; ++k)
	{
		m_hRoofkBmp[k] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
			MAKEINTRESOURCE(m_roofBmpNames[k]));
	}


	m_hero = CreateHero(54, 180, HERO_SIZE_X, HERO_SIZE_Y, m_hHeroBmp, 0, HERO_MAX_FRAME_NUM);
	m_building = CreateBuilding(0, 70, BUILDING_SIZE_X, BUILDING_SIZE_Y, m_hBuildingBmp);

	for (k = 0; k < MAX_TERRIAN_NUM; ++k)
	{
		if (k % 4 == 0)
		{
			m_terrian[k] = CreateTerrian(k * 64, 400, 64, 300, m_hBlockBmp[k % 4], m_hRoofkBmp[k % 2], ROOF_SIZE_Y, BLOCK_SIZE_Y);
			continue;
		}
		m_terrian[k] = CreateTerrian(k * 64, 180 + 50 * (k % 2), BLOCK_SIZE_X, 300, m_hBlockBmp[k % 4], m_hRoofkBmp[k % 2], ROOF_SIZE_Y, BLOCK_SIZE_Y);
	}


	m_gameStatus = CreateGameStatus(0, 0, GAME_STATUS_SIZE_X, GAME_STATUS_SIZE_Y, m_hGameStatusBmp);


	SetTimer(hWnd, TIMER_ID, TIMER_ELAPSE, NULL);
}

VOID Render(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(hWnd, &ps);

	HDC	hdcBmp, hdcBuffer;
	HBITMAP	cptBmp;

	cptBmp = CreateCompatibleBitmap(hdc, WNDWIDTH, WNDHEIGHT);
	hdcBmp = CreateCompatibleDC(hdc);
	hdcBuffer = CreateCompatibleDC(hdc);
	SelectObject(hdcBuffer, cptBmp);
	SelectObject(hdcBmp, m_hBackgroundBmp);

	TCHAR	szDist[13];
	if (m_gameStatus.isEnd == true)
	{
		SelectObject(hdcBmp, m_hEndBmp);
		TransparentBlt(hdc, 0, 0, 500, 360, hdcBmp, 0, 0, 500, 360, RGB(0, 255, 0));
		SetTextColor(hdcBuffer, RGB(255, 255, 255));
		SetBkMode(hdcBuffer, TRANSPARENT);
		TextOut(hdcBuffer, 400, 15, szDist, wsprintf(szDist, _T("Score:%d"), m_gameStatus.totalDist));
		return;
	}

	BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT, hdcBmp, 0, 0, SRCCOPY);

	

	SelectObject(hdcBmp, m_building.hBmp);
	TransparentBlt(
		hdcBuffer, m_building.pos.x, m_building.pos.y,
		m_building.size.cx, m_building.size.cy,
		hdcBmp, 0, 0, m_building.size.cx, m_building.size.cy,
		RGB(255, 255, 255)
		);

	SelectObject(hdcBmp, m_hero.hBmp);
	TransparentBlt(
		hdcBuffer, m_hero.pos.x, m_hero.pos.y,
		m_hero.size.cx, m_hero.size.cy,
		hdcBmp, 0, m_hero.size.cy * m_hero.curFrameIndex, m_hero.size.cx, m_hero.size.cy,
		RGB(255, 255, 255)
		);


	int k;
	for (k = 0; k < MAX_TERRIAN_NUM; ++k)
	{
		Terrian terrian = m_terrian[k];
		SelectObject(hdcBmp, terrian.hRoofBmp);
		TransparentBlt(
			hdcBuffer, terrian.pos.x, terrian.pos.y,
			terrian.roofWidth, terrian.roofHeight,
			hdcBmp, 0, 0, terrian.roofWidth, terrian.roofHeight,
			RGB(255, 255, 255)
			);
		SelectObject(hdcBmp, terrian.hBlockBmp);
		int t;
		for (t = 0; t < terrian.blockNum; ++t)
		{
			TransparentBlt(
				hdcBuffer, terrian.pos.x, terrian.pos.y + terrian.roofHeight + terrian.blockHeight * t,
				terrian.blockWidth, terrian.blockHeight,
				hdcBmp, 0, 0, terrian.blockWidth, terrian.blockHeight,
				RGB(255, 255, 255)
				);
		}
	}


	SelectObject(hdcBmp, m_gameStatus.hBmp);
	TransparentBlt(hdcBuffer, m_gameStatus.pos.x, m_gameStatus.pos.y, m_gameStatus.size.cx, m_gameStatus.size.cy,
		hdcBmp, 0, m_gameStatus.size.cy * m_gameStatus.isPaused,
		m_gameStatus.size.cx, m_gameStatus.size.cy, RGB(255, 255, 255));

	


	SetTextColor(hdcBuffer, RGB(0, 0, 0));
	SetBkMode(hdcBuffer, TRANSPARENT);
	TextOut(hdcBuffer, 400, 15, szDist, wsprintf(szDist, _T("Score:%d"), m_gameStatus.totalDist));


	BitBlt(hdc, 0, 0, WNDWIDTH, WNDHEIGHT, hdcBuffer, 0, 0, SRCCOPY);


	DeleteObject(cptBmp);
	DeleteDC(hdcBuffer);
	DeleteDC(hdcBmp);


	EndPaint(hWnd, &ps);
}

Hero CreateHero(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp, int curFrameIndex, int maxFrameSize)
{
	Hero hero;
	hero.hBmp = hBmp;
	hero.pos.x = posX;
	hero.pos.y = posY;
	hero.size.cx = sizeX;
	hero.size.cy = sizeY;
	hero.curFrameIndex = curFrameIndex;
	hero.maxFrameSize = maxFrameSize;
	hero.isJump = false;
	hero.jmpTime = 0;
	hero.jmpDistance = 20;
	return hero;
}

Building CreateBuilding(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp)
{
	Building building;
	building.hBmp = hBmp;
	building.pos.x = posX;
	building.pos.y = posY;
	building.size.cx = sizeX;
	building.size.cy = sizeY;
	return building;
}

GameStatus CreateGameStatus(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp)
{
	GameStatus gameStatus;
	gameStatus.pos.x = posX;
	gameStatus.pos.y = posY;
	gameStatus.size.cx = sizeX;
	gameStatus.size.cy = sizeY;
	gameStatus.hBmp = hBmp;
	gameStatus.totalDist = 0;
	gameStatus.isPaused = false;
	gameStatus.isEnd = false;
	return gameStatus;
}

Terrian CreateTerrian(LONG posX, LONG posY, LONG sizeX, LONG sizeY, 
					  HBITMAP hBlockBmp, HBITMAP hRoofBmp, int roofHeight, int blockHeight)
{
	Terrian terrian;
	terrian.pos.x = posX;
	terrian.pos.y = posY;
	terrian.size.cx = sizeX;
	terrian.size.cy = sizeY;
	terrian.hBlockBmp = hBlockBmp;
	terrian.hRoofBmp = hRoofBmp;
	terrian.roofWidth = sizeX;
	terrian.roofHeight = roofHeight;
	terrian.blockWidth = sizeX;
	terrian.blockHeight = blockHeight;
	terrian.blockNum = (int)ceil((sizeY - roofHeight) * 1.0 / blockHeight); 
	return terrian;
}

VOID TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HeroUpdate();
	TerrianUpdate();
	GameStatusUpdate();
	Collision();
	if (m_hero.pos.x + m_hero.size.cx <= 0 || m_hero.pos.y > 340)
	{
		m_gameStatus.isEnd = true;
		KillTimer(hWnd, TIMER_ID);
	}
	InvalidateRect(hWnd, NULL, FALSE);
}

VOID HeroUpdate()
{
	if (m_hero.isJump == true) {
		m_hero.pos.y -= m_hero.jmpDistance;
		m_hero.jmpDistance -= 2;
	}
	++m_hero.curFrameIndex;
	m_hero.curFrameIndex = m_hero.curFrameIndex >= m_hero.maxFrameSize ? 0 : m_hero.curFrameIndex;
}

VOID TerrianUpdate()
{
	int k;
	for (k = 0; k < MAX_TERRIAN_NUM; ++k)
	{
		m_terrian[k].pos.x -= 4;
		if (m_terrian[k].pos.x + m_terrian[k].size.cx < 0)
		{
			m_terrian[k].pos.x += MAX_TERRIAN_NUM * m_terrian[k].size.cx;
		}
	}
}

VOID GameStatusUpdate()
{
	++m_gameStatus.totalDist;
}

BOOL Paused(POINT ptMouse)
{
	RECT rPause;

	rPause.left = m_gameStatus.pos.x;
	rPause.top = m_gameStatus.pos.y;
	rPause.right = m_gameStatus.size.cx;
	rPause.bottom = m_gameStatus.size.cy;

	return PtInRect(&rPause, ptMouse);
}

VOID KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//TODO
	switch (wParam)
	{
	case VK_UP:
		if (m_hero.jmpTime < 2) 
		{
			m_hero.jmpTime++;
			m_hero.isJump = true;
			m_hero.jmpDistance = 20;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case VK_DOWN:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	default:
		break;
	}
}

VOID KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_UP:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case VK_DOWN:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	default:
		break;
	}
}

VOID LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	POINT ptMouse;
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);


	if (Paused(ptMouse))
	{
		if (!m_gameStatus.isPaused)
		{
			KillTimer(hWnd, TIMER_ID);
			m_gameStatus.isPaused = TRUE;
		}
		else
		{
			SetTimer(hWnd, TIMER_ID, TIMER_ELAPSE, NULL);
			m_gameStatus.isPaused = FALSE;
		}
		InvalidateRect(hWnd, NULL, FALSE);
	}
}


VOID Collision() {
	int cur = 0, pre = 0, next = 0;

	RECT heroRange;
	heroRange.left = m_hero.pos.x;
	heroRange.top = m_hero.pos.y;
	heroRange.right = m_hero.pos.x + m_hero.size.cx;
	heroRange.bottom = m_hero.pos.y + m_hero.size.cy;

	for (int k = 0; k < MAX_TERRIAN_NUM; k++)
	{
		if (m_terrian[k].pos.x < m_hero.pos.x + m_hero.size.cx && m_terrian[k].pos.x + m_terrian[k].size.cx > m_hero.pos.x + m_hero.size.cx)
		{
			cur = k;
		}
	}

	if (cur == MAX_TERRIAN_NUM - 1)
		next = 0;
	else
		next = cur + 1;
	if (cur == 0)
		pre = MAX_TERRIAN_NUM - 1;
	else
		pre = cur - 1;

	RECT terrianRange[3];
	terrianRange[0].left = m_terrian[cur].pos.x;
	terrianRange[0].top = m_terrian[cur].pos.y;
	terrianRange[0].right = m_terrian[cur].pos.x + m_terrian[cur].size.cx;
	terrianRange[0].bottom = m_terrian[cur].pos.y + m_terrian[cur].size.cy;
	terrianRange[1].left = m_terrian[next].pos.x;
	terrianRange[1].top = m_terrian[next].pos.y;
	terrianRange[1].right = m_terrian[next].pos.x + m_terrian[next].size.cx;
	terrianRange[1].bottom = m_terrian[next].pos.y + m_terrian[next].size.cy;
	terrianRange[2].left = m_terrian[pre].pos.x;
	terrianRange[2].top = m_terrian[pre].pos.y;
	terrianRange[2].right = m_terrian[pre].pos.x + m_terrian[pre].size.cx;
	terrianRange[2].bottom = m_terrian[pre].pos.y + m_terrian[pre].size.cy;

	if (heroRange.bottom >= terrianRange[1].top + 10 && heroRange.top <= terrianRange[1].bottom && heroRange.left <= terrianRange[1].right && heroRange.right >= terrianRange[1].left)
	{
		m_hero.pos.x -= 4;
	}
	if (heroRange.bottom >= terrianRange[0].top && heroRange.top <= terrianRange[0].bottom && heroRange.left <= terrianRange[0].right && heroRange.right >= terrianRange[0].left)
	{

		m_hero.pos.y = terrianRange[0].top - m_hero.size.cy;
		if (m_hero.jmpDistance < 0)
		{
			m_hero.isJump = false;
			m_hero.jmpTime = 0;
		}
		if (terrianRange[0].top < 180)
		{
			m_hero.jmpDistance = -17;
		}
		else if (terrianRange[0].top == 180)
		{
			m_hero.jmpDistance = -22;
		}
	}
	if (heroRange.bottom < terrianRange[0].top && !m_hero.isJump)
	{
		if (!(heroRange.bottom >= terrianRange[2].top && heroRange.top <= terrianRange[2].bottom && heroRange.left <= terrianRange[2].right && heroRange.right >= terrianRange[2].left))
		{
			m_hero.isJump = true;
			m_hero.jmpDistance = 0;
		}
	}

}
