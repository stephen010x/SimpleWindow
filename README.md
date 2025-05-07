# SimpleWindow

A simple window API for the Windows operating system, written in C.

This project is designed to be used in conjunction with [SimpleGraphics](), which is being developed alongisde SimpleWindow. But SimpleWindow's relationship with SimpleGraphics is not exclusive.

---

## Included features
- Simple window creation
- Show and hide windows
- Set window position, width, and height, and title
- Attach event callbacks to windows
    - mouse events
    - keyboard events
    - draw events
    - window close and kill events
- Windows 10 Support

## Features to be added
- Attach graphical contexts
- Thread safety
- Working DLL implementation
- Linux X-Server Support

## API Documentation
[Documentation](docs.md)

## Example Usage

```c
// main.c
#include "stdio.h"
#include "SimpleWindow/src/header.h"


// forward declare keyboard event callback
typeof(*WINCALLBACK) keyevent;


int main(void) {
    int err;
    HWIN window;
    WINDESC windesc;

    // populate struct that describes window properties
    windesc = {
        .title = "MyWindow",
        .width = WIN_DEFAULT,
        .height = WIN_DEFAULT,
        .x = WIN_DEFAULT,
        .y = WIN_DEFAULT,
    };

    // create window
    err = winnew(&window, &windesc);
    if (err) return err;

    // set event callback
    winevent(&window, &keyevent, EVENT_KEY);

    // make window visable
    winshow(&window, WIN_SHOW);

    // wait until window closes
    winwait(&window);
}


int keyevent(WINHANDLE* phwin, WINEVENT* pwine) {
    // handle key events
    switch (pwine->etype) {

        case EVENT_KEY_DOWN:
            printf("keycode %d down\n", pwine->keycode);
            break;

        case EVENT_KEY_UP:
            printf("keycode %d up\n", pwine->keycode);
            break;
    }
    
    // return value currently doesn't matter
    return 0;
}
```
