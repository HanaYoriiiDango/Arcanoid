#include <windows.h>
#include <algorithm>
#include <wingdi.h> 
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS) - ������� wWinMain, � �� main  ? Alt+251.
//configuration::advanced::character set - not set - ���� ���������� � ���������� c typedef ? Alt+0215
//linker::input::additional dependensies Msimg32.lib; Winmm.lib ?

struct { // ���� ������ ����� ���������
    HWND hwnd;
    HBITMAP hBack;
    HDC hdc, mem_dc;
    int width, height;

} window;

struct sprite {
    float x, y, speed, rad, dx, dy;
    int widht, height;
    bool active = true;
    HBITMAP hBitmap;

};

struct {
    bool action = false;

} game;

const int line = 15, column = 7;
sprite ball;
sprite racket;
sprite block[line][column];

void InitWindow() {

    RECT r;
    GetClientRect(window.hwnd, &r);

    window.width = r.right - r.left;
    window.height = r.bottom - r.top;

}

void InitGame() {

    srand(time(nullptr));

    window.hBack = (HBITMAP)LoadImageW(NULL, L"fon.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    racket.hBitmap = (HBITMAP)LoadImageW(NULL, L"racket.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    ball.hBitmap = (HBITMAP)LoadImageW(NULL, L"ball.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    auto bmpBlock = (HBITMAP)LoadImageW(NULL, L"kotek.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    float slow = 0.4f;

    racket.widht = 300;
    racket.height = 40;
    racket.x = window.width / 2.0f; // ������� ����� ��������� ������
    racket.y = window.height - racket.height; // ���� ������� ������ �� ���� ������
    racket.speed = 40.0f * slow;

    ball.widht = 40;
    ball.height = 40;
    ball.rad = 30;
    ball.x = (window.width + racket.widht - ball.widht) / 2.0f; // �������� ������, �������� ������� � ������ ������ ������
    ball.y = racket.y - racket.height; // ���� ���� ������� �� �� ������
    ball.speed = 20; // ����� ������� �����������
    //ball.speed = (sqrt((ball.x * ball.x) + (ball.y * ball.y))); // ����� �������
    /*
        ����� ������� (�� ���� ��� �� ��������) - ��� ������ ���������� �� ������� ����������
        ������. �� �������������� �� ������� �������� |a| = ?x?+y?
        ���������� ����� ����� �����, ������: |a| = ?3?+4? = ?25 = 5
    
    */
    //ball.dy = ball.x / ball.speed;
    //ball.dx = ball.y / ball.speed;
    ball.dy = (rand() % 65 + 35) / 100.0f; // ����� ����� �������� �� 0.35 �� 0.99
    ball.dx = abs(1 - ball.dy); // ����������� ������ ������ ������������ ������� 


    for (int i = 0; i < line; i++) {
        for (int j = 0; j < column; j++) {

            block[i][j].widht = window.width / line;
            block[i][j].height = window.height / 4 / column;
            block[i][j].x = block[i][j].widht * i;
            block[i][j].y = block[i][j].height * j + window.height / 4;
            block[i][j].hBitmap = bmpBlock;

        }
    }
}

void ShowSprite(int x, int y, int w, int h, HBITMAP hBitmap, bool transparent) {

    BITMAP bm;
    HDC memDC = CreateCompatibleDC(window.mem_dc);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

    if (hBitmap) {

        GetObject(hBitmap, sizeof(BITMAP), &bm);

        if (transparent) TransparentBlt(window.mem_dc, x, y, w, h, memDC, 0, 0, w, h, RGB(0, 0, 0));
        else StretchBlt(window.mem_dc, x, y, w, h, memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

    }

    SelectObject(memDC, hOldBitmap);

    DeleteObject(hOldBitmap);
    DeleteDC(memDC);

}

void ShowObject() {

    ShowSprite(0, 0, window.width, window.height, window.hBack, false);
    ShowSprite(racket.x, racket.y, racket.widht, racket.height, racket.hBitmap, true);
    ShowSprite(ball.x, ball.y, ball.widht, ball.height, ball.hBitmap, true);

    for (int i = 0; i < line; i++) {
        for (int j = 0; j < column; j++) {

            if (block[i][j].active) {

                ShowSprite(block[i][j].x, block[i][j].y, block[i][j].widht, block[i][j].height, block[i][j].hBitmap, true);

            }
        }
    }
}

void ShowGame() {

    window.mem_dc = CreateCompatibleDC(window.hdc);
    HBITMAP hMemBmp = CreateCompatibleBitmap(window.hdc, window.width, window.height);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(window.mem_dc, hMemBmp);

    ShowObject();
    
    for (int i = 0; i < ball.speed; i++) {
       
        SetPixel(window.mem_dc, ball.x + (ball.dx * i), ball.y + (ball.dy * i), RGB(0, 0, 0));
        //SetPixel(window.mem_dc, 150 + i, 150 + j, RGB(0, 0, 0));

      
    }

    BitBlt(window.hdc, 0, 0, window.width, window.height, window.mem_dc, 0, 0, SRCCOPY);

    SelectObject(window.mem_dc, hOldBmp);
    DeleteObject(hOldBmp);
    DeleteDC(window.mem_dc);
}

void ProcessGame() {

    if (game.action) {

        ball.x += ball.speed * ball.dx; // ������� ����� � ��������� ����������� 
        ball.y -= ball.speed * ball.dy; // ������� ������ �������� �� �������

    }
    else ball.x = racket.x + (racket.widht - ball.widht) / 2.0f;

}

void LimitRacket() {

    racket.x = min(racket.x, window.width - racket.widht);
    racket.x = max(racket.x, 0);

}

void CheckWalls() {

    //ball.dx = ball.dx * -1; // ���� �������� ������ �� �����, �� �������� ��� ����� 
    // �� �� ����������� (��� ������� ��� ����� �������������)
    // ������������� ����� "�����������" ������ ����� �������� ��� �� -�������
    // 

    if (ball.x <= 0 || ball.x + ball.rad >= window.width) { // ������������ � ������, ������ ������ x

        ball.dx *= -1.0f;

    }
    else if (ball.y <= 0) { // ������������ � ������� ������� - ������ y

        ball.dy *= -1.0f;

    }
}

void CheckRacket() {

    if (ball.y + ball.rad >= racket.y) {

        if (ball.x >= racket.x && ball.x <= racket.x + racket.widht) {

            ball.dy *= -1.0f;
            
        }
    }
}

void CollisionBlock() {

    /*bool collisionHandled = false; // ���� ��� ������������, ���� �� ���������� ������������
    for (int i = 0; i < line; i++) {
        for (int j = 0; j < column; j++) {

            if (block[i][j].active && !collisionHandled) { // ��������� ������ ���� ������������ ��� �� ����������

                if (ball.x + ball.rad >= block[i][j].x && ball.x - ball.rad <= block[i][j].x + block[i][j].widht &&
                    ball.y + ball.rad >= block[i][j].y && ball.y - ball.rad <= block[i][j].y + block[i][j].height) {

                    // ����������, � ����� ������� ��������� ������������
                    float overlapLeft = (ball.x + ball.rad) - block[i][j].x; // ���������� �� ����� ������� �����
                    float overlapRight = (block[i][j].x + block[i][j].widht) - (ball.x - ball.rad); // ���������� �� ������ ������� �����
                    float overlapTop = (ball.y + ball.rad) - block[i][j].y; // ���������� �� ������� ������� �����
                    float overlapBottom = (block[i][j].y + block[i][j].height) - (ball.y - ball.rad); // ���������� �� ������ ������� �����

                    // ������� ����������� ���������� �������
                    float minOverlap = overlapLeft; // ������������, ��� ����������� �������� � overlapLeft
                    if (overlapRight < minOverlap) minOverlap = overlapRight;
                    if (overlapTop < minOverlap) minOverlap = overlapTop;
                    if (overlapBottom < minOverlap) minOverlap = overlapBottom;

                    // �������� ����������� ���� � ����������� �� ������� ������������
                    if (minOverlap == overlapLeft || minOverlap == overlapRight) {
                        
                        ball.dx = -ball.dx; // ������ �� �����������
                    }
                    else {
                        
                        ball.dy = -ball.dy; // ������ �� ���������
                    }

                    collisionHandled = true; // ������������ ����������, ������ �� ��������� ������ �����
                    block[i][j].active = false; // ������������ ����
                    return;
                }
            }
        }
    }*/

    bool collision = true;

    for (int i = 0; i < line; i++) {
        for (int j = 0; j < column; j++) {

            if (block[i][j].active && collision) {

                if (ball.x + ball.rad >= block[i][j].x && 
                    ball.x - ball.rad <= block[i][j].x + block[i][j].widht &&
                    ball.y + ball.rad >= block[i][j].y && 
                    ball.y - ball.rad <= block[i][j].y + block[i][j].height) {

                    float minLeft = (ball.x + ball.rad) - block[i][j].x;
                    float minRight = (block[i][j].x + block[i][j].widht) - (ball.x - ball.rad);
                    float minTop = (ball.y + ball.rad) - block[i][j].y;
                    float minBottom = (block[i][j].y + block[i][j].height) - (ball.y - ball.rad);

                    float X = max(minLeft, minRight);
                    float Y = min(minTop, minBottom);

                    if (X < Y) ball.dx *= -1;
                    else ball.dy *= -1;

                    block[i][j].active = false;
                    collision = false;

                }
            }
        }
    }
}

void CheckEndGame() {

    if (ball.y + ball.rad > window.height) {

        game.action = false;
        ball.x = racket.x + (racket.widht - ball.widht) / 2.0f;
        ball.y = racket.y - racket.height;
        ball.dy = (rand() % 65 + 35) / 100.0f; // ����� ����� �������� �� 0.35 �� 0.99
        ball.dx = abs(1 - ball.dy); // ����������� ������ ������ ������������ ������� 

    }
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // ������ ���������� �������
// ��� ���������� ������ ��� ����������� � main ����� ����� ����� ���������� ������������ ��� ������ ���� 

int APIENTRY wWinMain( // ����� ����� � ���������� ������� (WINAPI - ��� �������� � �����)
	_In_ HINSTANCE hInstance, // "�������" ���������� 
	_In_opt_ HINSTANCE hPrevInstance, // ���������� ����������� ����������, � ����������� ������ NULL
	_In_ PWSTR pCmdLine, // ��������� ������ ����������, LPWSTR - ��������� �� ������ � ������� �������
	_In_ int nCmdShow // �������� � �������, ������������� ��������� ��������� ���� 

	// _In_ - ������� ��������� SAL (Source Annotation Language). ������������� ���,
	// ���� ��������� ���������� ���������� ��� �����������
	// ������������� ����� � �������� ����������, �.� � ������ ���� ���������� ������� �� ������

) 
{ // 
    // ������ ����� ����
    const wchar_t* CLASS_NAME = L"Main";  // L ������� ��� ������� ��������
    WNDCLASSEX wc = {}; // ��� ��������� ��� ����������� ������ ���� ���������������� ������ ����

    // �� ��� ����� ��������� ����� ���� ���� ���:
    wc.cbSize = sizeof(WNDCLASSEX); // ������� ������, ��� � �� ����� �����
    wc.lpfnWndProc = WndProc; // ����� ���������� �����������
    wc.hInstance = hInstance; // ���������� ����������
    wc.hCursor = NULL; // ������� ������
    wc.lpszClassName = CLASS_NAME;  // ��� ������������� ������

    // ����������� ����� ����
    //RegisterClassEx(&wc); // �� ����� �� ����� �������� �� �������� ������ ����� �����������

    if (!RegisterClassEx(&wc)) { // ��������� ����� 
        MessageBox(NULL, L"������ ����������� ������ ����!", L"������", MB_ICONERROR);
        return 0;
    }

    window.width = GetSystemMetrics(SM_CXSCREEN); // ��� ���������� ������ � ������ ���� 
    window.height = GetSystemMetrics(SM_CYSCREEN);

    window.hwnd = CreateWindowEx(0, CLASS_NAME, L"Arcanoid", WS_POPUP | WS_MAXIMIZE, 0, 0, window.width, window.height, NULL, NULL, hInstance, NULL);

    if (!window.hwnd) { // �������� �� �������� ���� 

        MessageBox(
            NULL, // ����� ���������� ����,NULL ������ ��� ��� �������������
            L"������ �������� ����!", // ����� ��������� 
            L"������", // �������� ���� 
            MB_ICONERROR // ���� ������� ������ ����������� ���� � ������� �� � �������� ������
        );
        return 0; // ���������� �����, ���������� ��������� 

    }

    // ��� ������� ���� ������ � ���� � ����� ����� ��������� ���������� ���� 
    // � �������������� ������� ���������
    // ������ ���� ������������ ����� WS_VISIBLE, �� �� ���������� �� ���������� 
    InitWindow();

    ShowWindow(window.hwnd, SW_SHOW);
    UpdateWindow(window.hwnd);

    MSG msg = {}; // �������� ��� �������� ���������  

    // GeMassage() ��������� ��������� �� ������� 
    // ���� ����� �������� �� ��� ��� ���� � ������� �� �������� WM_QUIT

    while (GetMessage(&msg, NULL, 0, 0)) {

        TranslateMessage(&msg); // ������������ ���� ������ � ����������� ��� � ��������� 
        DispatchMessage(&msg); // �������� ��������� � ����������

    }
}

// ��������� ���������
LRESULT CALLBACK WndProc(

    // LRESULT ��� �������� ���������� ��������� ���������
    // CALLBACK - ���������� � ������ ������� 
    // "����� ������ ������� (�������), �������� �� ��� �� ���� ����� (������� CALLBACK)".
    // ������� �������� ��� ��: ����� ���������� ������� ��� ��� ����������-���������� � ��������� ��

    HWND hwnd, // ���������� (��������������) ����
    UINT msg, // ������� ��� ������� 
    WPARAM wParam, // ��� ���� � �����������
    LPARAM lParam // WPARAM (��������� ������), LPARAM (�������� �������) 

) {

    switch (msg) {

    case WM_CREATE: // ����� �������� ���� �������� ��� �������� ���� 

        InitGame();
        ShowCursor(FALSE); // ����� ������
        SetTimer(hwnd, 1, 16, NULL);

        break;

    case WM_PAINT:  // ����� ����� ��� ��������� � ����  
    {   
        PAINTSTRUCT ps;
        window.hdc = BeginPaint(hwnd, &ps);
        ShowGame();
        EndPaint(hwnd, &ps);

    }

        break;

    case WM_TIMER: 
    {
        ProcessGame();
        LimitRacket();
        CheckWalls();
        CheckRacket();
        CollisionBlock();
        CheckEndGame();
        InvalidateRect(hwnd, NULL, FALSE);
    }

        break;

    case WM_KEYDOWN: // ��������� ������� ������ 

        if (wParam == VK_ESCAPE) DestroyWindow(hwnd); // ���������� ����
        if (wParam == VK_LEFT) racket.x -= racket.speed;
        if (wParam == VK_RIGHT) racket.x += racket.speed;
        if (wParam == VK_SPACE) game.action = true;
        InvalidateRect(hwnd, NULL, TRUE);

        break;


    case WM_DESTROY: // ��� ����������� ���� �������� ��������� WM_QUIT - ��������� ���� ���������. 

        DeleteObject(ball.hBitmap);
        DeleteObject(racket.hBitmap);
        DeleteObject(window.hBack);
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        break;


    default: // ����� ��������� ���� ��������� �� ���������, ���� �� �� �������� ��� �� �������������
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
