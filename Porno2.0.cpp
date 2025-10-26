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
    int width, height;

} window;

void InitWindow() {

    RECT r;
    GetClientRect(window.hwnd, &r);

    window.width = r.right - r.left;
    window.height = r.bottom - r.top;

}

void ShowBmp() {




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

        window.hBack = (HBITMAP)LoadImageW(NULL, L"fon.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

        break;

    case WM_PAINT:  // ����� ����� ��� ��������� � ����  
    {   
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        HDC mem_dc;
        HBITMAP hOld;
        BITMAP bm;

        GetObject(window.hBack, sizeof(BITMAP), &bm);

        mem_dc = CreateCompatibleDC(hdc);
        hOld = (HBITMAP)SelectObject(mem_dc, window.hBack);

        StretchBlt(hdc, 0, 0, window.width, window.height, mem_dc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

        SelectObject(mem_dc, hOld); // ���� ������������ ��� ����� �������� 

        DeleteObject(hOld);
        DeleteDC(mem_dc);

        EndPaint(hwnd, &ps);

    }

        break;
    
    case WM_KEYDOWN: // ��������� ������� ������ 

        if (wParam == VK_ESCAPE) DestroyWindow(hwnd); // ���������� ����

    case WM_DESTROY: // ��� ����������� ���� �������� ��������� WM_QUIT - ��������� ���� ���������. 

        PostQuitMessage(0);
        break;


    default: // ����� ��������� ���� ��������� �� ���������, ���� �� �� �������� ��� �� �������������
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
