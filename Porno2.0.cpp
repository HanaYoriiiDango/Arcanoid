#include <windows.h>
#include <algorithm>
#include <wingdi.h> 
#include <vector>

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
    float x, y, speed;
    int widht, height;
    HBITMAP hBitmap;

};

struct {
    bool action = false;

} game;

sprite ball;
sprite racket;

void InitWindow() {

    RECT r;
    GetClientRect(window.hwnd, &r);

    window.width = r.right - r.left;
    window.height = r.bottom - r.top;

    float slow = 0.8f;

    racket.widht = 300;
    racket.height = 40;
    racket.x = window.width / 2.0f; // ������� ����� ��������� ������
    racket.y = window.height - racket.height; // ���� ������� ������ �� ���� ������
    racket.speed = 20.0f * slow;

    ball.widht = 40;
    ball.height = 40;
    ball.x = (window.width + racket.widht - ball.widht) / 2.0f; // �������� ������, �������� ������� � ������ ������ ������
    ball.y = racket.y - racket.height; // ���� ���� ������� �� �� ������

}


void ShowSprite(sprite Object) {

    HDC memDC = CreateCompatibleDC(window.mem_dc);
    HBITMAP hold = (HBITMAP)SelectObject(memDC, Object.hBitmap);

    if (Object.hBitmap) {

        // ������� ������� ����� ����� ����������� 
        TransparentBlt(window.hdc, Object.x, Object.y, Object.widht, Object.height, memDC, 0, 0, Object.widht, Object.height, RGB(0, 0, 0));

    }

    SelectObject(memDC, hold);

    DeleteObject(hold);
    DeleteDC(memDC);
    ReleaseDC(window.hwnd, memDC);
}

void ShowBack() {

    HBITMAP hOld;

    BITMAP bmp;
    GetObject(window.hBack, sizeof(BITMAP), &bmp);

    window.mem_dc = CreateCompatibleDC(window.hdc);
    hOld = (HBITMAP)SelectObject(window.mem_dc, window.hBack);

    StretchBlt(window.hdc, 0, 0, window.width, window.height, window.mem_dc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

    ShowBmp(racket);
    ShowBmp(ball);

    SelectObject(window.mem_dc, hOld); // ���� ������������ ��� ����� �������� 

    DeleteObject(hOld);
    DeleteDC(window.mem_dc);
    ReleaseDC(hwnd, window.mem_dc);

}

void ShowGame() {




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
    WNDCLASSEX wc = {}; // ��� ��������� ��� ����������� ������ ���� ���������������� ������

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

        ShowCursor(FALSE); // ����� ������

        window.hBack = (HBITMAP)LoadImageW(NULL, L"fon.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        racket.hBitmap = (HBITMAP)LoadImageW(NULL, L"racket.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        ball.hBitmap = (HBITMAP)LoadImageW(NULL, L"ball.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

        break;

    case WM_PAINT:  // ����� ����� ��� ��������� � ����  
    {   
        PAINTSTRUCT ps;
        window.hdc = BeginPaint(hwnd, &ps);
        
        ShowGame();

        EndPaint(hwnd, &ps);

    }

        break;
    
    case WM_KEYDOWN: // ��������� ������� ������ 

        if (wParam == VK_ESCAPE) DestroyWindow(hwnd); // ���������� ����
        if (wParam == VK_LEFT) racket.x -= racket.speed;
        if (wParam == VK_RIGHT) racket.x += racket.speed;
        if (wParam == VK_SPACE) game.action = true;
        else ball.x = racket.x + (racket.widht - ball.widht) / 2.0f;

        InvalidateRect(hwnd, NULL, TRUE);

        break;

    case WM_DESTROY: // ��� ����������� ���� �������� ��������� WM_QUIT - ��������� ���� ���������. 

        DeleteObject(ball.hBitmap);
        DeleteObject(racket.hBitmap);
        DeleteObject(window.hBack);
        PostQuitMessage(0);
        break;


    default: // ����� ��������� ���� ��������� �� ���������, ���� �� �� �������� ��� �� �������������
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
