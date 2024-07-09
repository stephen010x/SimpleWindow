#ifndef WINDOW_H
#define WINDOW_H

#define __FORCE_INLINE__ __attribute__((always_inline))
#define WIN_DEFAULT 0x80000000

extern WNDCLASS;
typedef union {
    int value;
    struct {
        // consider adding 3 byte locations, and one byte error.
        short loc;
        short err;
    };
} ERRORCODE;

enum SWIN_ERRORS {
    // error types
    WERR_NONE = 0,
    WERR_GENERIC = 1,
    WERR_INVALIDPARAM,
    WERR_REGISTERCLASS,
    WERR_CREATEWINDOW,
    WERR_CREATE_MSGTHREAD,
    WERR_GETWINPLACEMENT,
    WERR_WINCLOSE_TIMEOUT,
    WERR_WINCLOSE_FAIL,
    WERR_WINWAIT,
    // error locations
    /* none yet*/
};

enum WINSET_FLAGS {
    SET_NONE = 0,
    SET_TITLE = 1<<0,
    SET_POS = 1<<1,
    SET_SIZE = 1<<2,
    //SET_WIDTH = 1<<1,
    //SET_HEIGHT = 1<<2,
    //SET_X = 1<<3,
    //SET_Y = 1<<4,
    SET_MOUSEEVENT = 1<<5,
    SET_KEYEVENT = 1<<6,
    SET_ICON = 1<<7, // unimplemented
    SET_ALL = 0xFFFFFFFF
};

/*enum WINEVENT_FLAGS {
    GENERIC_EVENT = 0,
    MOUSE_EVENT,
    KEY_EVENT,
    DRAW_EVENT,
};*/

enum WINEVENT_FLAGS {
    EVENT_ALL = 0,
    EVENT_INPUT,
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
}

enum NEWWIN_FLAGS {
    WIN_NONE = 0,
    //WIN_SHOW = 1<<0,
    WIN_BORDERLESS = 1<<1,
    WIN_FULLSCREEN = 1<<2,
    WIN_OPENGL = 0b01,
    WIN_SREND = 0b10,
};

enum WINSHOW_FLAGS {
    WIN_HIDE = false,
    WIN_NORMAL = true,
    WIN_MIN = 2,
    WIN_MAX = 3,
    WIN_SHOW = 5,
    WIN_RESTORE = 9,
};

enum EVENT_KEYFLAGS {
    KEY_NULL = 0,
    KEY_DOWN,
    KEY_UP,
};

enum EVENT_MOUSEFLAGS {
    MOUSE_NULL = 0,
    MOUSE_MOVE,
    MOUSE_SCROLL,
    MOUSE_BTN_DOWN,
    MOUSE_BTN_UP,
};

enum MOUSE_CLICKCODES {
    MOUSE_RIGHT = 1,
    MOUSE_LEFT,
    MOUSE_MIDDLE
};

#define SET_SIZE (SET_WIDTH | SET_HEIGHT)
#define SET_POS (SET_X | SET_Y)
#define SET_CALLBACKS (SET_MOUSE_CALLBACK | SET_KEYBOARD_CALLBACK)

typedef void (WINCALLBACK*)(WINHANDLE* phwin, WINEVENT* pwine);

/*typedef struct {
    int left;
    int top;
    int right;
    int bottom;
} WINRECT;*/

typedef struct {
    #ifdef _WINDOWS_
    HWND hwnd;
    HANDLE msgthread;
    #else
    void* hwnd;
    void* msgthread;
    #endif
    struct {
        WINCALLBACK mouse_move;
        WINCALLBACK mouse_btn_down;
        WINCALLBACK mouse_btn_up;
        WINCALLBACK mouse_scroll;
        WINCALLBACK key_up;
        WINCALLBACK key_down;
        WINCALLBACK draw;
    } event;
    /*struct {
        struct {
            WINCALLBACK move;
            WINCALLBACK button;
            WINCALLBACK scroll;
        } mouse;
        struct {
            WINCALLBACK up
        } keyboard;
        WINCALLBACK draw;
    } event;*/
    /*union {
        struct {
            WINCALLBACK generic;
            WINCALLBACK mouse;
            WINCALLBACK keyboard;
            WINCALLBACK draw;
        } event;
        struct {
            WINCALLBACK genevent;
            WINCALLBACK mouseevent;
            WINCALLBACK keyevent;
            WINCALLBACK drawevent;
            //WINCALLBACK frameevent; //add using rend
        };
    };*/
    //WINCALLBACK genevent;
    WNDCLASS* winclass;
} HWIN;

typedef struct {
    union {
        char* title;
        char* t;
    };
    union {
        struct {
            int width;
            int height;
        };
        struct {
            int w;
            int h;
        };
        struct {
            int x;
            int y;
        } size;
    };
    union {
        struct {
            int x;
            int y;
        };
        struct {
            int xpos;
            int ypos;
        };
        struct {
            int x;
            int y;
        } pos;
    };
    union {
        struct {
            char rendtype;
            char showflag;
            char __reserved;
            char winflag;
        };
        int flagword;
    };
    // okay...
    // this is actually both a hilarious and
    // a brilliant idea on my part.
    // A string pointer that points to a string
    // within the same struct.
    // Genius. No heap required.
    char _string_reserved_[256];
} WINDESC;

/*typedef struct {
    int etype;
    union {
        union {
            struct {
                int win_x;
                int win_y;
                int screen_x;
                int screen_y;
                int delta_x;
                int delta_y;
            };
            struct {
                int wheel_delta;
            };
        } mouse;
        int keycode;
    };
} WINEVENT;*/

typedef struct {
    int etype;
    union {
        struct { /*consider: mwX mwY msX msY*/
            int mouse_win_x;
            int mouse_win_y;
            int mouse_screen_x;
            int mouse_screen_y;
            int mouse_delta_x;
            int mouse_delta_y;
        };
        int wheel_delta;
        int mouse_button;
        int keycode;
    };
} WINEVENT;


int winnew_ex(HWIN* phwin, char* title, int x, int y, int width, int height);
int winnew(HWIN* phwin, WINDESC* pwind);
int winshow(HWIN* phwin, int sflag);
int winget(HWIN* phwin, WINDESC* pwind);
int winset(HWIN* phwin, WINDESC* pwind, int SFLAG);
int winevent(HWIN* phwin, WINCALLBACK callback, int EFLAG);
int winclose(HWIN* phwin);
int winwait(HWIN* phwin); // waits until window closes
//int winstep(); // alternative to loop thread? Nah.

/*char* winget_title(HWIN* hwin);
int winget_width(HWIN* hwin);
int winget_height(HWIN* hwin);
int winget_xpos(HWIN* hwin);
int winget_ypos(HWIN* hwin);*/

/*int winset_title(HWIN* phwin, char* title);
int winset_size(HWIN* phwin, int width, int height);
int winset_pos(HWIN* phwin, int x, int y);
int winset_rect(HWIN* phwin, int x, int y, int width, int height);*/

__FORCE_INLINE__ int winset_title(HWIN* phwin, char* title) {
    return SetWindowText(phwin->hwnd, title) ? 0 : 1;
}
__FORCE_INLINE__ int winset_size(HWIN* phwin, int width, int height) {
    return SetWindowPos(phwin->hwnd, NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE) ? 0 : 1;
}
__FORCE_INLINE__ int winset_pos(HWIN* phwin, int x, int y) {
    return SetWindowPos(phwin->hwnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE) ? 0 : 1;
}
__FORCE_INLINE__ int winset_rect(HWIN* phwin, int x, int y, int width, int height) {
    return SetWindowPos(phwin->hwnd, NULL, x, y, width, height, SWP_NOZORDER) ? 0 : 1;
}


// NOTES:
// I think I will need to multithread here
// To acheive a framerate will probably require opengl
// The sibling library to this should be called SimpleRend, or "rend"

#endif