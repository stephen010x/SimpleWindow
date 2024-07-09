#include <windows.h>
#include <stdbool.h>
#include "window.h"

#if WIN_DEFAULT != CW_USEDEFAULT
#pragma GCC error "WIN_DEFAULT must be equal to CW_USEDEFAULT\n\tWIN_DEFAULT is" \
    ## WIN_DEFAULT ## "\n\tCW_USEDEFAULT is" ## CW_USEDEFAULT
#endif

ATOM WINCLASS_DEFAULT;

int swin_init(HWIN* hwin);
DWORD WINAPI msghandler_loop(LPVOID lpParam);


WINDESC WINDESC_DEFAULT = {
    .title = "SimpleWidow",
    .x = WIN_DEFAULT,
    .y = WIN_DEFAULT,
    .width = WIN_DEFAULT,
    .height = WIN_DEFAULT,
};


int winnew(HWIN* phwin, WINDESC* pwind) {
    static bool startup = true;
    const DWORD default_style = WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_VISIBLE;
    HINSTANCE hinst = GetModuleHandle(NULL);
    if (startup == true) {
        int err = swin_init(hinst);
        if (err)
            return err;
        startup == false;
    }
    if (pwind == NULL)
        pwind = WINDESC_DEFAULT;
    phwin->hwnd = CreateWindow(
        WINCLASS_DEFAULT,   // lpClassName
        pwind->title,       // lpWindowName
        default_style,      // dwStyle
        pwind->x,           // x
        pwind->y,           // y
        pwind->width,       // nWidt
        pwind->height,      // nHeight
        NULL,               // hWndParent
        NULL,               // hMenu
        hinst,              // hInstance
        NULL                // lpParam
    );
    if (phwin->hwnd != NULL)
        return WERR_CREATEWINDOW;
    phwin->msgthread = CreateThread(NULL, 0, msghandler_loop, phwin, NULL);
    if (phwin->msgthread == NULL) {
        DestroyWindow(phwin->hwnd);
        return WERR_CREATE_MSGTHREAD;
    }
    if (phwin->winflag & ~WIN_SHOW)
        winshow(&hwin, true);
    return 0;
}

__FORCE_INLINE__ int winnew_ex(HWIN* phwin, char* title, int x, int y, int width, int height) {
    WINDESC wind = {
        .title = title;
        .x = x;
        .y = y;
        .width = width;
        .height = height;
    }
    return winnew(phwin, &wind);
}

int winshow(HWIN* phwin, int sflag) {
    ShowWindow(phwin->hwnd, sflag);
    UpdateWindow(phwin->hwnd);
    return 0;
}

int winget(HWIN* phwin, WINDESC* pwind) {
    RECT rect;
    WINDOWPLACEMENT wp;
    wp.length = sizeof(WINDOWPLACEMENT);
    bool ok = GetWindowPlacement(phwin->hwnd, &wp);
    //if (!ok)
    //    return WERR_GETWINPLACEMENT;
    GetWindowText(pwind->hwnd, pwind->_string_reserved_, sizeof(pwind->_string_reserved_));
    GetWindowRect(pwind->hwnd, &rect);
    pwind->x = rect.left;
    pwind->y = rect.top;
    pwind->width = rect.right - rect.left;
    pwind->height = rect.bottom - rect.top;
    pwind->showflag = wp.showCmd;
    return 0;
}

int winset(HWIN* phwin, WINDESC* pwind, int SFLAG) {
    if (SFLAG & ~SET_TITLE)
        return winset_title(phwin, pwind->title);
    if (SFLAG & ~SET_POS)
        return winset_pos(phwin, pwind->x, pwind->y);
    if (SFLAG & ~SET_SIZE)
        return winset_size(phwin, pwind->width, pwind->height);
    return 0;
}
/*
    EVENT_ALL = 0,
    EVENT_MOUSE,
    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_BTN,
    EVENT_MOUSE_BTN_DOWN,
    EVENT_MOUSE_BTN_UP,
    EVENT_MOUSE_SCROLL,
    EVENT_KEY,
    EVENT_KEY_DOWN,
    EVENT_KEY_UP,
    EVENT_DRAW,
*/
int winevent(HWIN* phwin, WINCALLBACK callback, int EFLAG) {
    switch (EFLAG) {
        case EVENT_ALL:
            phwin->event.draw = callback;
        case EVENT_INPUT:
            phwin->event.key_down = callback;
            phwin->event.key_up = callback;
        case EVENT_MOUSE:
            phwin->event.mouse_move = callback;
            phwin->event.mouse_btn_down = callback;
            phwin->event.mouse_btn_up = callback;
            phwin->event.mouse_scroll = callback;
            break;
        case EVENT_MOUSE_MOVE:
            phwin->event.mouse_move = callback;
            break;
        case EVENT_MOUSE_BTN:
            phwin->event.mouse_btn_down = callback;
            phwin->event.mouse_btn_up = callback;
            break;
        case EVENT_MOUSE_BTN_DOWN:
            phwin->event.mouse_btn_down = callback;
            break;
        case EVENT_MOUSE_BTN_UP:
            phwin->event.mouse_btn_up = callback;
            break;
        case EVENT_MOUSE_SCROLL:
            phwin->event.mouse_scroll = callback;
            break;
        case EVENT_KEY:
            phwin->event.key_down = callback;
            phwin->event.key_up = callback;
            break;
        case EVENT_KEY_DOWN:
            phwin->event.key_down = callback;
            break;
        case EVENT_KEY_UP:
            phwin->event.key_up = callback;
            break;
        case EVENT_DRAW:
            phwin->event.draw = callback;
            break;
        default:
            return WERR_INVALIDPARAM;
    }
    return 0;
}

int winclose(HWIN* phwin) {
    PostQuitMessage(0);
    DWORD err = WaitForSingleObject(phwin->msgthread, 5000);
    switch (err) {
        case WAIT_TIMEOUT:
            return WERR_WINCLOSE_TIMEOUT;
        case WAIT_FAILED:
            return WERR_WINCLOSE_FAILED;
    }
    return 0;
}

int winwait(HWIN* phwin) {
    return WaitForSingleObject(phwin->msgthread, INFINITE) ? WERR_WINWAIT : 0;
}


int swin_init(HWIN* hwin) {
    const WNDCLASSEX wc = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = swin_defaultproc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = hinst,
        .hIcon = NULL,
        .hCursor = NULL,
        .hbrBackground = NULL,
        .lpszMenuName = NULL,
        .lpszClassName = "Default Window Class",
        .hIconSm = NULL,
    };
    ATOM WINCLASS_DEFAULT = RegisterClassEx(&wc);
    return (WINCLASS_DEFAULT != 0) ? 0 : WERR_REGISTERCLASS;
}

DWORD WINAPI msghandler_loop(LPVOID lpParam);


/*
ShowWindow(hwnd, nCmdShow);

// Step 3: The Message Loop
MSG msg = { 0 };
while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
}

// Wait for the thread to finish
WaitForSingleObject(hThread, INFINITE);

// Close the thread handle
CloseHandle(hThread);
*/

























#ifdef NONE


HWIN hwin;
int winnew_ex(HWIN* hwin, char* title, int x, int y, int width, int height);
int winnew(HWIN* hwin);
int winshow(HWIN* hwin, BOOL show);

WINDESC //(used for window creation or advanced window sets)

//create_window()













#include <windows.h>
#include <stdbool.h>


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


// on first call, inits all the default windows descriptors
// creates a new window. Accepts a descriptor struct.
// If descriptor is null, then just use default.
int winnew(WINHANDLE* pwinh, WINDESC* pwind) {
    static bool startup = true;
    HINSTANCE hinst = GetModuleHandle(NULL);
    if (startup == true) {
        startup == false
        swin_init(hinst);
    }
    if (pwind == NULL)
        pwind = WINDESC_DEFAULT;
    
}

int winshow(bool show) {}

ATOM WINCLASS_DEFAULT;

int swin_init(HINSTANCE hinst) {
    WNDCLASS wc = {
        .style = CS_HREDRAW | CS_VREDRAW;
        .lpfnWndProc = swin_defaultproc;
        .cbClsExtra = 0;
        .cbWndExtra = 0;
        .hInstance = hinst;
        .hIcon = NULL;
        .hCursor = NULL;
        .hbrBackground = NULL;
        .lpszMenuName = NULL;
        .lpszClassName = "Default Window Class";
    }
    ATOM WINCLASS_DEFAULT = RegisterClass(&wc);
}

int windel(WINHANDLE* pwh) {
    
}

int winpropget(WINHANDLE* pwh, WINPROP* pwp) {}
int winpropset(WINHANDLE* pwh, WINPROP* pwp) {}

int wingetsize();
int wingetpos();


int wincallback() {
    
}

#define EXTWINSTYLES_DEFAULT 0

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    

    // Step 2: Creating the Window
    HWND hwnd = CreateWindowEx(
        EXTWINSTYLES_DEFAULT,
        WINCLASS_DEFAULT,               // Window class name
        "Learn to Program Windows",     // Window title
        WS_OVERLAPPEDWINDOW,            // Window style

        // Position and size
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Step 3: The Message Loop
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Step 4: Implementing the Window Procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));

            EndPaint(hwnd, &ps);
        }
        return 0;

        case WM_CLOSE:
            if (MessageBox(hwnd, "Really quit?", "My application", MB_OKCANCEL) == IDOK) {
                DestroyWindow(hwnd);
            }
            // Else: user canceled. Do nothing.
            return 0;

        // Handle other messages here

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}


//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

enum {
    SET_NONE = 0;
    SET_TITLE = 1<<0;
    SET_WIDTH = 1<<1;
    SET_HEIGHT = 1<<2;
    SET_X = 1<<3;
    SET_Y = 1<<4;
    SET_MOUSE_CALLBACK = 1<<5;
    SET_KEYBOARD_CALLBACK = 1<<6;
}

#define SET_SIZE (SET_WIDTH | SET_HEIGHT)
#define SET_POS (SET_X | SET_Y)
#define SET_CALLBACKS (SET_MOUSE_CALLBACK | SET_KEYBOARD_CALLBACK)
#define SET_ALL 0xFFFFFFFF



// this is to help me figure out what gui I want
int example() {
    WINHANDLE hwin;
    WNDCLASS wc;
    WINDESC wind = {
        .title = "my window";
        .w = 100;
        .h = 100;
        .x = 100;
        .y = 100;
        .callback = {
            .mouseup = mu_fun;
            .mousedown = md_fun;
            .keyup = ku_fun;
            .keydown = kd_fun;
            .mouseclick = mb_fun;
        }
    }
    winnew(&hwin, &wind, &wc);
    winshow(&hwin);
    winset_width(&hwin, 100);
    winset(&hwin, WIDTH_MEMBER, 100);
    winattach(&hwin, MOUSE_CALLBACK, mymousecallback);
    
    
    
    
    
    WINHANDLE hwin;
    WNDCLASS wc;
    WINDESC wind = {
        .title = "my window";
    }
    
    winset(&hwin, &wind);
}


#endif