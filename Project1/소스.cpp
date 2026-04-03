#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <string>
#include <ctime>

#pragma comment (lib, "Gdiplus.lib")
using namespace Gdiplus;
using namespace std;

// 전역 변수 (C2065 및 식별자 오류 해결)
const int TILE_SIZE = 64;
ULONG_PTR gdiplusToken;
int currentLevel = 0;
int pX, pY;
vector<string> board;

// 클리어 보장 맵 데이터
vector<vector<vector<string>>> stagePool = {
    { { "  ##### ", "###   # ", "# @oo # ", "#  .. # ", "####### " } },
    { { "####### ", "#     # ", "# o#o # ", "# .@. # ", "####### " } }
};

void InitLevel(int levelIdx) {
    if (levelIdx >= (int)stagePool.size()) levelIdx = 0;
    board = stagePool[levelIdx][0];
    for (int y = 0; y < (int)board.size(); ++y) {
        for (int x = 0; x < (int)board[y].size(); ++x) {
            if (board[y][x] == '@') { pX = x; pY = y; }
        }
    }
}

void Move(int dx, int dy) {
    int nx = pX + dx, ny = pY + dy;
    if (board[ny][nx] == ' ' || board[ny][nx] == '.') {
        board[pY][pX] = (board[pY][pX] == 'B') ? '.' : ' ';
        pX = nx; pY = ny;
        board[pY][pX] = (board[pY][pX] == '.') ? 'B' : '@';
    }
    else if (board[ny][nx] == 'o' || board[ny][nx] == 'O') {
        int bnx = nx + dx, bny = ny + dy;
        if (board[bny][bnx] == ' ' || board[bny][bnx] == '.') {
            board[bny][bnx] = (board[bny][bnx] == '.') ? 'O' : 'o';
            board[ny][nx] = (board[ny][nx] == 'O') ? '.' : ' ';
            board[pY][pX] = (board[pY][pX] == 'B') ? '.' : ' ';
            pX = nx; pY = ny;
            board[pY][pX] = (board[pY][pX] == '.') ? 'B' : '@';
        }
    }
}

// WndProc 구현 (LNK2019 해결)
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_KEYDOWN:
        if (wp == VK_UP) Move(0, -1);
        else if (wp == VK_DOWN) Move(0, 1);
        else if (wp == VK_LEFT) Move(-1, 0);
        else if (wp == VK_RIGHT) Move(1, 0);
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Graphics g(hdc);
        // C2102 해결을 위해 브러시 객체 생성 후 주소 전달
        SolidBrush wallBrush(Color::Brown), boxBrush(Color::Orange), targetBrush(Color::Pink), playerBrush(Color::Blue);
        for (int y = 0; y < (int)board.size(); ++y) {
            for (int x = 0; x < (int)board[y].size(); ++x) {
                Rect r(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                if (board[y][x] == '#') g.FillRectangle(&wallBrush, r);
                else if (board[y][x] == 'o') g.FillRectangle(&boxBrush, r);
                else if (board[y][x] == '.') g.FillEllipse(&targetBrush, r);
                else if (board[y][x] == '@' || board[y][x] == 'B') g.FillEllipse(&playerBrush, r);
            }
        }
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY: PostQuitMessage(0); break;
    default: return DefWindowProc(hWnd, msg, wp, lp);
    }
    return 0;
}

int APIENTRY wWinMain(HINSTANCE hI, HINSTANCE hP, LPWSTR lp, int nS) {
    srand((unsigned int)time(NULL));
    GdiplusStartupInput gsi;
    GdiplusStartup(&gdiplusToken, &gsi, NULL);
    InitLevel(0);
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc; wc.hInstance = hI; wc.lpszClassName = L"SokobanApp";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);
    HWND hWnd = CreateWindowW(L"SokobanApp", L"Fixed Sokoban", WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL, NULL, hI, NULL);
    ShowWindow(hWnd, nS);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}