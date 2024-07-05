#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Step 1: Registering the Window Class
    const char CLASS_NAME[] = "Sample Window Class";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = WindowProc;          // Pointer to the window procedure
    wc.hInstance = hInstance;             // Handle to the application instance
    wc.lpszClassName = CLASS_NAME;        // Name of the window class

    RegisterClass(&wc);

    // Step 2: Creating the Window
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class name
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
