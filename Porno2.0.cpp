#include <windows.h>
#include <algorithm>
#include <wingdi.h> 
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS) - ожидает wWinMain, а не main  ? Alt+251.
//configuration::advanced::character set - not set - могу обращаться к структурам c typedef ? Alt+0215
//linker::input::additional dependensies Msimg32.lib; Winmm.lib ?

struct { // если делать через структуру
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

const int line = 12, column = 8;
sprite GG;
sprite racket;
sprite beds[line][column];

POINT p;


void InitWindow() {

    RECT r;
    GetClientRect(window.hwnd, &r);

    window.width = r.right - r.left;
    window.height = r.bottom - r.top;

}

void InitGame() {

    srand(time(nullptr));

    window.hBack = (HBITMAP)LoadImageW(NULL, L"fon.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    GG.hBitmap = (HBITMAP)LoadImageW(NULL, L"ball.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    auto bmpBlock = (HBITMAP)LoadImageW(NULL, L"beds.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    GG.widht = 40;
    GG.height = 40;
    GG.rad = 30;
    GG.speed = 30;

    



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
    ShowSprite(GG.x, GG.y, GG.widht, GG.height, GG.hBitmap, true);

    for (int i = 0; i < line; i++) {
        for (int j = 0; j < column; j++) {
            
            if (beds[i][j].active) {

                ShowSprite(beds[i][j].x, beds[i][j].y, beds[i][j].widht, beds[i][j].height, beds[i][j].hBitmap, true);

            }
        }
    }
}

void ShowGame() {

    window.mem_dc = CreateCompatibleDC(window.hdc);
    HBITMAP hMemBmp = CreateCompatibleBitmap(window.hdc, window.width, window.height);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(window.mem_dc, hMemBmp);

    ShowObject();

    BitBlt(window.hdc, 0, 0, window.width, window.height, window.mem_dc, 0, 0, SRCCOPY);

    SelectObject(window.mem_dc, hOldBmp);
    DeleteObject(hOldBmp);
    DeleteDC(window.mem_dc);
}

void CollisionBlock() {

    bool collisionHand = false;

    for (int i = 0; i < line; i++) {
        for (int j = 0; j < column; j++) {

            if (GG.x + GG.rad >= beds[i][j].x &&
                GG.x <= beds[i][j].x + beds[i][j].widht &&
                GG.y <= beds[i][j].y + beds[i][j].height &&
                GG.y + GG.rad >= beds[i][j].y ) {

                if (!collisionHand && beds[i][j].active) {

                    float minLeft = (GG.x + GG.rad) - beds[i][j].x;
                    float minRight = (beds[i][j].x + beds[i][j].widht) - GG.x;
                    float minTop = (GG.y + GG.rad) - beds[i][j].y;
                    float minBottom = (beds[i][j].y + beds[i][j].height) - GG.y;

                    float CoordX = min(minLeft, minRight);
                    float CoordY = min(minTop, minBottom);

                    if (CoordX < CoordY) {

                        beds[i][j].active = false;

                    }
                    else {
                    

                        beds[i][j].active = false;
                    
                    }

                    collisionHand = true;

                }
            }
        }
    }
}

void ProcessInput(WPARAM wParam) {

    float slow = 0.8f;

    if (wParam == VK_ESCAPE) DestroyWindow(window.hwnd); // уничтожаем окно
    if (wParam == 'W') GG.y -= GG.speed * slow;
    if (wParam == 'A') GG.x -= GG.speed * slow;
    if (wParam == 'S') GG.y += GG.speed * slow;
    if (wParam == 'D') GG.x += GG.speed * slow;

}

void ProcessGame() {

    CollisionBlock();

}

void ClearGame() {

    DeleteObject(GG.hBitmap);
    DeleteObject(window.hBack);

}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // просто объявление функции
// так правильнее потому что компилятору в main нужно знать какой обработчик использовать для класса окна 

int APIENTRY wWinMain( // Точка входа с поддержкой юникода (WINAPI - для примеров в доках)
	_In_ HINSTANCE hInstance, // "паспорт" приложения 
	_In_opt_ HINSTANCE hPrevInstance, // дескриптор предыдущего экземпляра, в современных всегда NULL
	_In_ PWSTR pCmdLine, // командная строка приложения, LPWSTR - указатель на строку в формате юникода
	_In_ int nCmdShow // параметр с флагами, определяющими начальное состояние окна 

	// _In_ - система аннотаций SAL (Source Annotation Language). Документируют код,
	// явно показывая назначения параметров для компилятора
	// предотвращает краши и ускоряет разработку, т.к в случае чего компилятор сообщит об ошибке

) 
{ // 
    // Создаю класс окна
    const wchar_t* CLASS_NAME = L"Main";  // L префикс для широких символов
    WNDCLASSEX wc = {}; // вся структура для регистрации класса окна инициализирована нулями пппп

    // то что нужно настроить более явно пишу тут:
    wc.cbSize = sizeof(WNDCLASSEX); // размеры класса, так и не понял зачем
    wc.lpfnWndProc = WndProc; // какой обработчик испоьзовать
    wc.hInstance = hInstance; // дескриптор приложения
    wc.hCursor = NULL; // скрываю курсор
    wc.lpszClassName = CLASS_NAME;  // имя используемого класса

    // Регистрирую класс окна
    //RegisterClassEx(&wc); // хз нужна ли здесь проверка на создание класса через мэссэджбокс

    if (!RegisterClassEx(&wc)) { // оказалась нужна 
        MessageBox(NULL, L"Ошибка регистрации класса окна!", L"Ошибка", MB_ICONERROR);
        return 0;
    }

    window.width = GetSystemMetrics(SM_CXSCREEN); // эти переменные укажем в размер окна 
    window.height = GetSystemMetrics(SM_CYSCREEN);

    window.hwnd = CreateWindowEx(0, CLASS_NAME, L"Arcanoid", WS_POPUP | WS_MAXIMIZE, 0, 0, window.width, window.height, NULL, NULL, hInstance, NULL);

    if (!window.hwnd) { // проверка на создание окна 

        MessageBox(
            NULL, // здесь дескриптор окна,NULL значит что нет родитльеского
            L"Ошибка создания окна!", // текст сообщения 
            L"Ошибка", // заголовк окна 
            MB_ICONERROR // флаг который задает отображение окна с кнопкой ок и иконокой ошибки
        );
        return 0; // возвращаем нольб, заверщшаем программу 

    }

    // эти функции идут всегда в паре и нужны чтобы управлять видимостью окна 
    // и оптимизировать процесс отрисовку
    // однако если использовать стиль WS_VISIBLE, то их применение не обязателно 
    InitWindow();

    ShowWindow(window.hwnd, SW_SHOW);
    UpdateWindow(window.hwnd);

    MSG msg = {}; // структур дял хранения сообщений  

    // GeMassage() извлекает сообщения из очереди 
    // цикл будет работать до тех пор пока в очередь не поступит WM_QUIT

    while (GetMessage(&msg, NULL, 0, 0)) {

        TranslateMessage(&msg); // обрабатывает ввод клавиш и преобразует его в сообщения 
        DispatchMessage(&msg); // передает сообщения в обработчик

    }
}

// Обработка сообщений
LRESULT CALLBACK WndProc(

    // LRESULT для возврата результата обработки сообщения
    // CALLBACK - соглашение о вызове функций 
    // "Когда придет посылка (событие), распакуй ее вот по этой схеме (функция CALLBACK)".
    // Система работает так же: когда происходит событие она ище инструкцию-обработчик и выполняет ее

    HWND hwnd, // дескриптор (идентификатьор) окна
    UINT msg, // числвой код события 
    WPARAM wParam, // доп инфа о сообщзениях
    LPARAM lParam // WPARAM (обработка клавиш), LPARAM (движение курсора) 

) {

    switch (msg) {

    case WM_CREATE: // здесь загрузка всех ресурсов при поздании окна 

        InitGame();
        ShowCursor(FALSE); // скрыл курсор
        SetTimer(hwnd, 1, 16, NULL);

        break;

    case WM_PAINT:  // здесь будет вся отричовка в окне  
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
        InvalidateRect(hwnd, NULL, FALSE);

    }

        break;

    case WM_KEYDOWN: // обработка нажатий клавиш 

        ProcessInput(wParam);
        InvalidateRect(hwnd, NULL, TRUE);

        break;


    case WM_DESTROY: // при уничтожении окна посылаем сообщение WM_QUIT - завершает цикл сообщений. 

        ClearGame();
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        break;


    default: // здесь обработка всех сообщений по умолчанию, если мы не написали как их обрабюатывать
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
