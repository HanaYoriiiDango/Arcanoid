#include <windows.h>
#include <algorithm>
#include <wingdi.h> 
#include <vector>

using namespace std;

//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS) - ожидает wWinMain, а не main  ? Alt+251.
//configuration::advanced::character set - not set - могу обращаться к структурам c typedef ? Alt+0215
//linker::input::additional dependensies Msimg32.lib; Winmm.lib ?

struct { // если делать через структуру
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

        window.hBack = (HBITMAP)LoadImageW(NULL, L"fon.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

        break;

    case WM_PAINT:  // здесь будет вся отричовка в окне  
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

        SelectObject(mem_dc, hOld); // если закомментить все равно работает 

        DeleteObject(hOld);
        DeleteDC(mem_dc);

        EndPaint(hwnd, &ps);

    }

        break;
    
    case WM_KEYDOWN: // обработка нажатий клавиш 

        if (wParam == VK_ESCAPE) DestroyWindow(hwnd); // уничтожаем окно

    case WM_DESTROY: // при уничтожении окна посылаем сообщение WM_QUIT - завершает цикл сообщений. 

        PostQuitMessage(0);
        break;


    default: // здесь обработка всех сообщений по умолчанию, если мы не написали как их обрабюатывать
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
