/* TODO:
 * Add message box capabilities
 * MessageBox(hwnd, "Really quit?", "My application", MB_OKCANCEL)
 * Add timer capabilities, perhaps as a sister library
 */

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
LRESULT CALLBACK swin_DefaultWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


__FORCE_INLINE__ void put_windata(HWND hwnd, void* pdata) {
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pdata);
}
__FORCE_INLINE__ void* get_windata(HWND hwnd) {
    return GetWindowLongPtr(hwnd, GWLP_USERDATA);
}


WINDESC WINDESC_DEFAULT = {
    .title = "SimpleWidow",
    .x = WIN_DEFAULT,
    .y = WIN_DEFAULT,
    .width = WIN_DEFAULT,
    .height = WIN_DEFAULT,
};


int winnew(HWIN* phwin, WINDESC* pwind) {
    static bool startup = true;
    #define SWIN_DEFAULT_STYLE = WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_VISIBLE;
    HINSTANCE hinst = GetModuleHandle(NULL);
    if (startup == true) {
        int err = swin_init(hinst);
        if (err)
            return err;
        startup == false;
    }
    if (pwind == NULL)
        pwind = WINDESC_DEFAULT;
    if (phwin->winclass == NULL)
        phwin->winclass = WINCLASS_DEFAULT;
    phwin->hwnd = CreateWindow(
        phwin->winclass,    // lpClassName
        pwind->title,       // lpWindowName
        SWIN_DEFAULT_STYLE, // dwStyle
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
    put_windata(phwin->hwnd, &phwin);
    phwin->msgthread = CreateThread(NULL, 0, msghandler_loop, phwin, NULL);
    if (phwin->msgthread == NULL) {
        DestroyWindow(phwin->hwnd);
        return WERR_CREATE_MSGTHREAD;
    }
    winshow(&hwin, phwin->showflag);
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

int winevent(HWIN* phwin, WINCALLBACK callback, int EFLAG) {
    switch (EFLAG) {
        case EVENT_ALL:
            phwin->eventcalls.draw = callback;
            phwin->eventcalls.close = callback;
            phwin->eventcalls.kill = callback;
        case EVENT_INPUT:
            phwin->eventcalls.key_down = callback;
            phwin->eventcalls.key_up = callback;
        case EVENT_MOUSE:
            phwin->eventcalls.mouse_move = callback;
            phwin->eventcalls.mouse_btn_down = callback;
            phwin->eventcalls.mouse_btn_up = callback;
            phwin->eventcalls.mouse_scroll = callback;
            break;
        case EVENT_MOUSE_MOVE:
            phwin->eventcalls.mouse_move = callback;
            break;
        case EVENT_MOUSE_BTN:
            phwin->eventcalls.mouse_btn_down = callback;
            phwin->eventcalls.mouse_btn_up = callback;
            break;
        case EVENT_MOUSE_BTN_DOWN:
            phwin->eventcalls.mouse_btn_down = callback;
            break;
        case EVENT_MOUSE_BTN_UP:
            phwin->eventcalls.mouse_btn_up = callback;
            break;
        case EVENT_MOUSE_SCROLL:
            phwin->eventcalls.mouse_scroll = callback;
            break;
        case EVENT_KEY:
            phwin->eventcalls.key_down = callback;
            phwin->eventcalls.key_up = callback;
            break;
        case EVENT_KEY_DOWN:
            phwin->eventcalls.key_down = callback;
            break;
        case EVENT_KEY_UP:
            phwin->eventcalls.key_up = callback;
            break;
        case EVENT_DRAW:
            phwin->eventcalls.draw = callback;
            break;
        case EVENT_CLOSE:
            // have it so that returning 1 on close will prevent kill.
            phwin->eventcalls.close = callback;
            break;
        case EVENT_KILL:
            phwin->eventcalls.kill = callback;
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
    CloseHandle(hThread);
    return 0;
}

int winwait(HWIN* phwin) {
    DWORD ret = WaitForSingleObject(phwin->msgthread, INFINITE) ? WERR_WINWAIT : 0;
    if (ret == 0)
        CloseHandle(hThread);
    return (int)ret;
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

DWORD WINAPI msghandler_loop(LPVOID lpParam) {
    MSG msg;
    // note that second parameter can optionally be null
    // however, I only want this thread to handle messages
    // for it's assigned window.
    while (GetMessage(&msg, ((HWIN)lpParam).hwnd, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK swin_DefaultWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HWIN* phwin = get_windata(hwnd);
    // experimenting with capitals in my variables
    // as they might simply look better
    // Though I find it harder to type so far
    WINEVENT* pEvent = &phwin->event;
    switch (uMsg) {
        case WM_CREATE:
            POINT mpoint;
            GetCursorPos(&mpoint);
            ScreenToClient(hwnd, &mpoint);
            pEvent->mouse_x = mpoint.x;
            pEvent->mouse_y = mpoint.y;
            return 0;
        case WM_MOVE:
            return 0;
        case WM_SIZE:
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_TIMER:
            // wParam (timer identifier)
            // lParam (pointer to callback)
            return 0;
        case WM_KEYDOWN:
            // wParam (virtual keycode)
            pEvent->etype = EVENT_KEY_UP;
            pEvent->keycode = wParam;
            phwin->eventcalls(phwin, pEvent);
            return 0;
        case WM_KEYUP:
            // wParam (virtual keycode)
            pEvent->etype = EVENT_KEY_UP;
            pEvent->keycode = wParam;
            phwin->eventcalls(phwin, pEvent);
            return 0;
        case WM_MOUSEMOVE:
            // LOWORD(lParam); client win x
            // HIWORD(lParam); client win y
            pEvent->etype = EVENT_MOUSE_MOVE;
            pEvent->mouse_delta_x = LOWORD(lParam) - pEvent->mouse_x;
            pEvent->mouse_delta_y = HIWORD(lParam) - pEvent->mouse_y;
            pEvent->mouse_x = LOWORD(lParam);
            pEvent->mouse_y = HIWORD(lParam);
            phwin->eventcalls(phwin, pEvent);
            return 0;
        case WM_MOUSEHWHEEL:
            pEvent->etype = EVENT_MOUSE_WHEEL;
            pEvent->wheel_delta = HIWORD(wParam); // / WHEEL_DELTA;
            phwin->eventcalls(phwin, pEvent);
            return 0;
        case WM_LBUTTONDOWN:
            pEvent->etype = EVENT_MOUSE_BTN_DOWN;
            pEvent->btncode = MOUSE_LEFT;
            phwin->eventcalls(phwin, pEvent);
            return 0;
        case WM_LBUTTONUP:
            pEvent->etype = EVENT_MOUSE_BTN_UP;
            pEvent->btncode = MOUSE_LEFT;
            phwin->eventcalls(phwin, pEvent);
            return 0;
        case WM_RBUTTONDOWN:
            pEvent->etype = EVENT_MOUSE_BTN_DOWN;
            pEvent->btncode = MOUSE_RIGHT;
            phwin->eventcalls(phwin, pEvent);
            return 0;
        case WM_RBUTTONUP:
            pEvent->etype = EVENT_MOUSE_BTN_UP;
            pEvent->btncode = MOUSE_RIGHT;
            phwin->eventcalls(phwin, pEvent);
            return 0;
        case WM_MBUTTONDOWN:
            pEvent->etype = EVENT_MOUSE_BTN_DOWN;
            pEvent->btncode = MOUSE_MIDDLE;
            phwin->eventcalls(phwin, pEvent);
            return 0;
        case WM_MBUTTONUP:
            pEvent->etype = EVENT_MOUSE_BTN_UP;
            pEvent->btncode = MOUSE_MIDDLE;
            phwin->eventcalls(phwin, pEvent);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}