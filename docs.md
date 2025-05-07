# SimpleWindow Function Documentation


- Types
    - [WINCALLBACK](#WINCALLBACK)
    - [WINEVENT](#WINEVENT)
    - [HWIN](#HWIN)
    - [WINDESC](#WINDESC)
- Functions
    - [winnew_ex](#winnew_ex)
    - [winnew](#winnew)
    - [winshow](#winshow)
    - [winget](#winget)
    - [winset](#winset)
    - [winevent](#winevent)
    - [winclose](#Other-Functions)
    - [winwait](#Other-Functions)
    - [winset_title](#Other-Functions)
    - [winset_size](#Other-Functions)
    - [winset_pos](#Other-Functions)
    - [winset_rect](#Other-Functions)

---

## Types

---

### WINCALLBACK
`typedef int (*WINCALLBACK)(WINHANDLE* phwin, WINEVENT* pwine);`

type for a window event handler callback

---

### WINEVENT
```c
struct WINEVENT {
    int etype;
    struct {
        int mouse_x;
        int mouse_y;
    };
    union {
        struct {
            int mouse_delta_x;
            int mouse_delta_y;
        }
        int wheel_delta;
        int btncode;
        int keycode;
    }; 
};
```
WINEVENT (WINdow EVENT) is a structure used for handling the events handled by SimpleWindow through callbacks.

`etype` can be one of these enumerators: \
&emsp; `EVENT_MOUSE_MOVE` \
&emsp; `EVENT_MOUSE_BTN_DOWN` \
&emsp; `EVENT_MOUSE_BTN_UP` \
&emsp; `EVENT_MOUSE_WHEEL` \
&emsp; `EVENT_KEY_DOWN` \
&emsp; `EVENT_KEY_UP` \
&emsp; `EVENT_DRAW` \
&emsp; `EVENT_CLOSE` \
&emsp; `EVENT_KILL`

---

### HWIN
```c
struct HWIN;
```
HWIN (Handler to the WINdow) is the main window structure, and used as a parameter in all SimpleWindow functions. The programmer does not need to modify any of the members of HWIN unless they wish to perform more complex actions with the WinAPI, or if they wish to attach a graphics context (which is not yet supported by the vanilla SimpleWindow API).

---

### WINDESC
```c
struct WINDESC {
    const char* title;
    int width;
    int height;
    int x;
    int y;
    union {
        struct {
            char rendtype;
            char showflag;
            char __reserved;
            char winflag;
        };
        int flagword;
    };
};
```

WINDESC (WINdow DESCriptor) is a structure used to describe the properties of the window to be created. The use of this structure is optional, and can be replaced by [winnew_ex(...)], which will populate this structure for you.

`rendtype` can be one of these enumerators: \
&emsp; `WIN_OPENGL` \
&emsp; `WIN_SREND`

`showflag` can be one of these enumerators: \
&emsp; `WIN_HIDE` \
&emsp; `WIN_NORMAL` \
&emsp; `WIN_MIN` \
&emsp; `WIN_MAX` \
&emsp; `WIN_SHOW` \
&emsp; `WIN_RESTORE`

`winflag` can be multiple of these enumerators using bitwise 'or': \
&emsp; `WIN_NONE` \
&emsp; `WIN_BORDERLESS` \
&emsp; `WIN_FULLSCREEN`

---

## Functions

---

### winnew_ex
```c
int winnew_ex(HWIN* phwin, char* title, int x, int y, int width, int height);
```

Create a new window. The simplest way to create a window without the need for a [WINDESC]() structure.

**Parameters** \
&emsp; `phwin` pointer to the window handler \
&emsp; `title` title of the window \
&emsp; `x`, `y` x and y coordinate of top left corner of the window \
&emsp; `width`, `height` width and height of window \
&emsp; `height` height of window
    

**Return value** \
&emsp; Returns zero if success \
&emsp; Returns nonzero if error

---

### winnew
```c
int winnew(HWIN* phwin, WINDESC* pwind);
```

Create a new window

**Parameters** \
&emsp; `phwin` pointer to the window handler \
&emsp; `pwind` pointer to window descriptor

**Return value** \
&emsp; Returns zero if success \
&emsp; Returns nonzero if error

---

### winshow
```c
int winshow(HWIN* phwin, int sflag);
```

Primarily used to show or hide a window, but can also fullscreen and minimize windows.

**Parameters** \
&emsp; `phwin` pointer to the window handler \
&emsp; `sflag` expects one of these enumerators, `WIN_HIDE`, `WIN_NORMAL`, `WIN_MIN`, `WIN_MAX`, `WIN_SHOW`, `WIN_RESTORE`
    
**Return value** \
&emsp; Returns zero

---

### winget
```c
int winget(HWIN* phwin, WINDESC* pwind);
```

Returns a `WINDESC` struct that it populates, describing the target window

**Parameters** \
&emsp; `phwin` pointer to the window handler \
&emsp; `pwind` (out) pointer to window descriptor
    
**Return value** \
&emsp; Returns zero

---

### winset
```c
int winset(HWIN* phwin, WINDESC* pwind, int sflag);
```

Sets a specific property specified by `sflag` from the `pwind` structure, and applies it to the window.

**Parameters** \
&emsp; `phwin` pointer to the window handler \
&emsp; `pwind` pointer to window descriptor \
&emsp; `sflag` can be one or more of these enumators using bitwise 'or', `SET_NONE`, `SET_TITLE`, `SET_POS`, `SET_SIZE`, `SET_MOUSEEVENT`, `SET_KEYEVENT`, `SET_ALL`
    
**Return value** \
&emsp; Returns zero if success \
&emsp; Returns nonzero if error

---

### winevent
```c
int winevent(HWIN* phwin, WINCALLBACK callback, int EFLAG);
```

Set window event callback

**Parameters** \
&emsp; `phwin` pointer to the window handler \
&emsp; `callback` pointer to event callback function \
&emsp; `eflag` can be one of these enumators, `EVENT_ALL`, `EVENT_INPUT`, `EVENT_MOUSE`, `EVENT_MOUSE_MOVE`, `EVENT_MOUSE_BTN`, `EVENT_MOUSE_BTN_DOWN`, `EVENT_MOUSE_BTN_UP`, `EVENT_MOUSE_WHEEL`, `EVENT_KEY`, `EVENT_KEY_DOWN`, `EVENT_KEY_UP`, `EVENT_DRAW`, `EVENT_CLOSE`, `EVENT_KILL`
    
**Return value** \
&emsp; Returns zero if success \
&emsp; Returns nonzero if error

---

## Other Functions

---

```c
int winclose(HWIN* phwin);
```

Close window.

---

```c
int winwait(HWIN* phwin);
```

Blocking function. Will wait until window closes.

---

```c
int winset_title(HWIN* phwin, char* title);
```

Set window title.

---

```c
int winset_size(HWIN* phwin, int width, int height);
```

Set window width and height.

---

```c
int winset_pos(HWIN* phwin, int x, int y);
```

Set window corner x and y position.

---

```c
int winset_rect(HWIN* phwin, int x, int y, int width, int height);
```

Set window width, height, and corner x and y position.



