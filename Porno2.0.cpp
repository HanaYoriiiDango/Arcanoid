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

    float slow = 0.8f;

    racket.widht = 300;
    racket.height = 40;
    racket.x = window.width / 2.0f; // ракетка ровно посередне экрана
    racket.y = window.height - racket.height; // выше границы экрана на свою высоту
    racket.speed = 20.0f * slow;

    ball.widht = 40;
    ball.height = 40;
    ball.rad = 30;
    ball.x = (window.width + racket.widht - ball.widht) / 2.0f; // половина экрана, половина ракетки с учетом ширины шарика
    ball.y = racket.y - racket.height; // шарк выше ракетки на ее высоту
    ball.speed = 20; // длина вектора фиксирована
    //ball.speed = (sqrt((ball.x * ball.x) + (ball.y * ball.y))); // длина вектора
    /*
        Длина вектора (по сути она же скорость) - это модуль расстояния на которое сдвигается
        объект. Он рассчитывается по теореме пифагора |a| = ?x?+y?
        резултатом будет некое число, пример: |a| = ?3?+4? = ?25 = 5
    
    */
    //ball.dy = ball.x / ball.speed;
    //ball.dx = ball.y / ball.speed;
    ball.dy = (rand() % 65 + 35) / 100.0f; // здесь будут значения от 0.35 до 0.99
    ball.dx = abs(1 - ball.dy); // нормализуем второй вектор относительно первого 


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

            ShowSprite(block[i][j].x, block[i][j].y, block[i][j].widht, block[i][j].height, block[i][j].hBitmap, true);

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

void ProcessGame() {

    if (game.action) {

        ball.x += ball.speed * ball.dx; // пускаем мячик в рандомном направлении 
        ball.y -= ball.speed * ball.dy; // умножаю вектор скорости на нормаль

    }
    else ball.x = racket.x + (racket.widht - ball.widht) / 2.0f;

}

void CheckWalls() {

    //ball.dx = ball.dx * -1; // если умножать вектор на число, то меняется его длина 
    // но не направление (при условии что число положительное)
    // соответсвенно чтобы "отзеркалить" вектор нужно умножить его на -нормаль
    // 

    if (ball.x <= 0 || ball.x + ball.rad >= window.width) { // столкновение с боками, меняем только x

        ball.dx *= -1;

    }
    else if (ball.y <= 0) { // столкновение с верхней стенкой - меняем y

        ball.dy *= -1;


    }
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
    WNDCLASSEX wc = {}; // вся структура для регистрации класса окна инициализирована нулями

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
        CheckWalls();
        InvalidateRect(hwnd, NULL, FALSE);
    }

        break;

    case WM_KEYDOWN: // обработка нажатий клавиш 

        if (wParam == VK_ESCAPE) DestroyWindow(hwnd); // уничтожаем окно
        if (wParam == VK_LEFT) racket.x -= racket.speed;
        if (wParam == VK_RIGHT) racket.x += racket.speed;
        if (wParam == VK_SPACE) game.action = true;
        InvalidateRect(hwnd, NULL, TRUE);

        break;


    case WM_DESTROY: // при уничтожении окна посылаем сообщение WM_QUIT - завершает цикл сообщений. 

        DeleteObject(ball.hBitmap);
        DeleteObject(racket.hBitmap);
        DeleteObject(window.hBack);
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        break;


    default: // здесь обработка всех сообщений по умолчанию, если мы не написали как их обрабюатывать
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
