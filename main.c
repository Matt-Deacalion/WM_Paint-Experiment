/*
 * An experiment/example of how to completely customise a window in Windows,
 * I capture various win32 messages (e.g. WM_PAINT) and do my own thing with them.
 *
 * Matt. Stevens - 25/3/2006
 */
#define _WIN32_IE   0x0400
#include <windows.h>
#include <commctrl.h>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name a global variable  */
char szClassName[] = "WindowsApp";

struct
{
    WNDPROC wndProcOldComboBox;
    WNDPROC wndProcOldStatic;
    WNDPROC wndProcOldButton;
    WNDPROC wndProcOldTabControl;
    WNDPROC wndProcOldStatus;
    HBRUSH hbrBrush;
    HBRUSH hbrBrushPen;
    HBRUSH hbrBrushTitle;
    HPEN hpnPen;
    HPEN hpnPenBrush;
    COLORREF crTextColor;
    COLORREF crBkTextColor;
    HFONT hfn;
    HFONT hfnWebdings;
    HFONT hfnTitle;
    HFONT hfnWebdingsTitle;
    RECT rectBtnClose;
    RECT rectBtnMinimize;
    RECT rectBtnOptions;
    int iTitleBtnPressed;
    HWND hwndTabs;
} CustomGui;
LRESULT CALLBACK CustomGuiWindowProcedure (HWND, UINT, WPARAM, LPARAM);

void HookControlWindowProc(LPCTSTR clsName, WNDPROC *lpOldProc, HINSTANCE hThisInstance)
{
    HWND hwnd;
    hwnd = CreateWindow (clsName, "", 0, 0, 0, 100, 100, HWND_DESKTOP, NULL, hThisInstance, NULL);
    *lpOldProc = (WNDPROC) GetClassLong(hwnd, GCL_WNDPROC);
    SetClassLong(hwnd, GCL_WNDPROC, (LONG) CustomGuiWindowProcedure);
    DestroyWindow(hwnd);
}

void InitCustomGui(HINSTANCE hThisInstance)
{
    CustomGui.hbrBrush = CreateSolidBrush(RGB(0, 0, 0));
    CustomGui.hbrBrushPen = CreateSolidBrush(RGB(200, 200, 200));
    CustomGui.hbrBrushTitle = CreateSolidBrush(RGB(100, 100, 100));
    CustomGui.hpnPen = CreatePen(PS_SOLID, 0, RGB(200, 200, 200));
    CustomGui.hpnPenBrush = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
    CustomGui.crTextColor = RGB(200, 200, 200);
    CustomGui.crBkTextColor = RGB(0, 0, 0);
    CustomGui.hfn = CreateFont(16, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
    CustomGui.hfnWebdings = CreateFont(15, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Webdings");
    CustomGui.hfnTitle = CreateFont(13, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
    CustomGui.hfnWebdingsTitle = CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Webdings");
    CustomGui.iTitleBtnPressed = -1;
    SetRect(&CustomGui.rectBtnClose, 0, 0, 0, 0);
    SetRect(&CustomGui.rectBtnMinimize, 0, 0, 0, 0);
    SetRect(&CustomGui.rectBtnOptions, 0, 0, 0, 0);

    HookControlWindowProc(WC_COMBOBOX, &CustomGui.wndProcOldComboBox, hThisInstance);
    HookControlWindowProc(WC_STATIC, &CustomGui.wndProcOldStatic, hThisInstance);
    HookControlWindowProc(WC_BUTTON, &CustomGui.wndProcOldButton, hThisInstance);
    HookControlWindowProc(WC_TABCONTROL, &CustomGui.wndProcOldTabControl, hThisInstance);
    HookControlWindowProc(STATUSCLASSNAME, &CustomGui.wndProcOldStatus, hThisInstance);
}

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    HWND hwnd;               /* This is the handle for our window */
    HWND hwndCombo, hwndTab, hwndStatus;
    MSG messages;            /* messages to the application */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    TCITEM tcItem;
    int iStatusParts[3] = { 200, 350, -1 };
    InitCommonControls();

    InitCustomGui(hThisInstance);

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, lets create the program */
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Matt's App",       /* Title Text */
           WS_OVERLAPPED|WS_MINIMIZEBOX /*WS_OVERLAPPEDWINDOW*/, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           485,                 /* The programs width */
           390,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);

    CreateWindow (
           "STATIC",         /* Classname */
           "Web Server:",       /* Title Text */
           SS_SIMPLE|WS_CHILD|WS_VISIBLE, /* default window */
           5,       /* Windows decides the position */
           5,       /* where the window ends up on the screen */
           83,                 /* The programs width */
           16,                 /* and height in pixels */
           hwnd,        /* The window is a child-window to the desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    hwndCombo = CreateWindow (
           "COMBOBOX",         /* Classname */
           "http://www.yahoo.com",       /* Title Text */
           WS_VSCROLL|CBS_DISABLENOSCROLL|/*CBS_OWNERDRAWFIXED|*/CBS_DROPDOWN|WS_CHILD|WS_VISIBLE, /* default window */
           90,       /* Windows decides the position */
           1,       /* where the window ends up on the screen */
           382,                 /* The programs width */
           20,                 /* and height in pixels */
           hwnd,        /* The window is a child-window to the desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Add some options to the drop down */
    SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) "http://www.google.com");
    SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) "http://www.rentacoder.com");
    SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) "http://www.mail.com");
    SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) "http://www.com");
    SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) "http://www.paypal.com");
    SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) "http://www.abc.com");
    SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) "http://www.search.com");

    CreateWindow (
           "STATIC",         /* Classname */
           "Status:",       /* Title Text */
           SS_SIMPLE|WS_CHILD|WS_VISIBLE, /* default window */
           8,       /* Windows decides the position */
           40,       /* where the window ends up on the screen */
           60,                 /* The programs width */
           16,                 /* and height in pixels */
           hwnd,        /* The window is a child-window to the desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    CreateWindow (
           "STATIC",         /* Classname */
           "Server:",       /* Title Text */
           SS_SIMPLE|WS_CHILD|WS_VISIBLE, /* default window */
           8,       /* Windows decides the position */
           60,       /* where the window ends up on the screen */
           60,                 /* The programs width */
           16,                 /* and height in pixels */
           hwnd,        /* The window is a child-window to the desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    CreateWindow (
           "BUTTON",         /* Classname */
           "Refresh",       /* Title Text */
           WS_CHILD|WS_VISIBLE, /* default window */
           200,       /* Windows decides the position */
           30,       /* where the window ends up on the screen */
           100,                 /* The programs width */
           26,                 /* and height in pixels */
           hwnd,        /* The window is a child-window to the desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    CreateWindow (
           "BUTTON",         /* Classname */
           "Try Server",       /* Title Text */
           WS_CHILD|WS_VISIBLE, /* default window */
           306,       /* Windows decides the position */
           30,       /* where the window ends up on the screen */
           100,                 /* The programs width */
           26,                 /* and height in pixels */
           hwnd,        /* The window is a child-window to the desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    CreateWindow (
           "BUTTON",         /* Classname */
           "Abort",       /* Title Text */
           WS_CHILD|WS_VISIBLE, /* default window */
           412,       /* Windows decides the position */
           30,       /* where the window ends up on the screen */
           60,                 /* The programs width */
           26,                 /* and height in pixels */
           hwnd,        /* The window is a child-window to the desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    hwndTab = CreateWindow (
           WC_TABCONTROL,         /* Classname */
           "Abort",       /* Title Text */
           /*TCS_OWNERDRAWFIXED|*/WS_CHILD|WS_VISIBLE, /* default window */
           0,       /* Windows decides the position */
           80,       /* where the window ends up on the screen */
           400,                 /* The programs width */
           250,                 /* and height in pixels */
           hwnd,        /* The window is a child-window to the desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    tcItem.mask = TCIF_TEXT;
    tcItem.pszText = "Status";
    TabCtrl_InsertItem(hwndTab, 0, &tcItem);
    tcItem.pszText = "History";
    TabCtrl_InsertItem(hwndTab, 1, &tcItem);
    tcItem.pszText = "Options";
    TabCtrl_InsertItem(hwndTab, 2, &tcItem);
    CustomGui.hwndTabs = hwndTab;

    hwndStatus = CreateWindow (
           STATUSCLASSNAME,         /* Classname */
           "",       /* Title Text */
           WS_CHILD|WS_VISIBLE, /* default window */
           0,       /* Windows decides the position */
           0,       /* where the window ends up on the screen */
           100,                 /* The programs width */
           100,                 /* and height in pixels */
           hwnd,        /* The window is a child-window to the desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    SendMessage(hwndStatus, (UINT) SB_SETPARTS, (WPARAM) 3, (LPARAM) iStatusParts);
    SendMessage(hwndStatus, (UINT) SB_SETTEXT, (WPARAM) 0, (LPARAM) "Hello world");
    SendMessage(hwndStatus, (UINT) SB_SETTEXT, (WPARAM) 1, (LPARAM) "Request done.");
    SendMessage(hwndStatus, (UINT) SB_SETTEXT, (WPARAM) 2, (LPARAM) "Disconnected...");

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* return value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rect, rectClient;
    int len;
    LPTSTR lpTstr;
    POINT pt;
    LRESULT lResult;
    SIZE sz;

    lResult = 0;
    switch (message)                 /* handle the messages */
    {
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        case WM_NCHITTEST:
            return HTCAPTION;
        case WM_NCLBUTTONDOWN:
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            GetWindowRect(hwnd, &rect);
            pt.x -= rect.left;
            pt.y -= rect.top;
            if (PtInRect(&CustomGui.rectBtnClose, pt))
            {
               CustomGui.iTitleBtnPressed = 1;
               return 0;
            }
            else if (PtInRect(&CustomGui.rectBtnMinimize, pt))
            {
               CustomGui.iTitleBtnPressed = 2;
               return 0;
            }
            else if (PtInRect(&CustomGui.rectBtnOptions, pt))
            {
               CustomGui.iTitleBtnPressed = 3;
               return 0;
            }
            else
            {
                return DefWindowProc (hwnd, message, wParam, lParam);
            }
            break;
        case WM_NCLBUTTONUP:
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            GetWindowRect(hwnd, &rect);
            pt.x -= rect.left;
            pt.y -= rect.top;
            if (PtInRect(&CustomGui.rectBtnClose, pt) && CustomGui.iTitleBtnPressed == 1)
            {
                SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, -1);
            }
            else if (PtInRect(&CustomGui.rectBtnMinimize, pt) && CustomGui.iTitleBtnPressed == 2)
            {
                SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, -1);
            }
            else if (PtInRect(&CustomGui.rectBtnOptions, pt) && CustomGui.iTitleBtnPressed == 3)
            {
                TabCtrl_SetCurSel(CustomGui.hwndTabs, 2);
            }
            else
            {
                return DefWindowProc (hwnd, message, wParam, lParam);
            }
            CustomGui.iTitleBtnPressed = -1;
            break;
        case WM_ACTIVATE:
        case WM_ACTIVATEAPP:
        case WM_NCACTIVATE:
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        case WM_SIZE:
        case WM_SIZING:
        case WM_WINDOWPOSCHANGED:
        case WM_SHOWWINDOW:
        case WM_SYSCOMMAND:
        case WM_CAPTURECHANGED:
            lResult = DefWindowProc (hwnd, message, wParam, lParam);
        case WM_NCPAINT:
            hdc = GetWindowDC(hwnd);
            GetWindowRect(hwnd, &rect);
            // Find coordinates of the client area
            GetClientRect(hwnd, &rectClient);
            pt.x = rectClient.left;
            pt.y = rectClient.top;
            ClientToScreen(hwnd, &pt);
            rectClient.left = pt.x;
            rectClient.top = pt.y;
            pt.x = rectClient.right;
            pt.y = rectClient.bottom;
            ClientToScreen(hwnd, &pt);
            rectClient.right = pt.x;
            rectClient.bottom = pt.y;
            // now mask off the inside of the control so we can paint the borders
            OffsetRect(&rectClient, -rect.left, -rect.top);
            ExcludeClipRect(hdc, rectClient.left, rectClient.top, rectClient.right, rectClient.bottom);
            OffsetRect(&rect, -rect.left, -rect.top);
            FillRect(hdc, &rect, CustomGui.hbrBrush);
            rect.bottom = rect.top + GetSystemMetrics(SM_CYCAPTION);
            InflateRect(&rect, -GetSystemMetrics(SM_CXEDGE), -GetSystemMetrics(SM_CYEDGE));
            FillRect(hdc, &rect, CustomGui.hbrBrushTitle);
            SelectObject(hdc, CustomGui.hfnTitle);
            SetTextColor(hdc, CustomGui.crTextColor);
            SetBkColor(hdc, CustomGui.crBkTextColor);
            len = GetWindowTextLength(hwnd);
            lpTstr = (LPTSTR) malloc((len + 1) * sizeof (TCHAR));
            GetWindowText(hwnd, lpTstr, len + 1);
            SetBkMode(hdc, TRANSPARENT);
            InflateRect(&rect, -3, 0);
            DrawText(hdc, lpTstr, len, &rect, /*DT_CENTER|*/DT_VCENTER|DT_SINGLELINE);
            free(lpTstr);
            rect.left = rect.right - (rect.bottom - rect.top);
            InflateRect(&rect, -1, 0);
            SelectObject(hdc, CustomGui.hfnWebdingsTitle);
            SetTextColor(hdc, CustomGui.crBkTextColor);
            DrawText(hdc, "\x72", 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            CustomGui.rectBtnClose = rect;
            OffsetRect(&rect, -(rect.right - rect.left + 3), 0);
            DrawText(hdc, "\x30", 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            CustomGui.rectBtnMinimize = rect;
            SelectObject(hdc, CustomGui.hfnTitle);
            SetTextColor(hdc, CustomGui.crTextColor);
            OffsetRect(&rect, -(rect.right - rect.left + 10), 0);
            lpTstr = "[Options]";
            GetTextExtentPoint32(hdc, lpTstr, strlen(lpTstr), &sz);
            rect.left = rect.right - sz.cx - 2;
            DrawText(hdc, lpTstr, strlen(lpTstr), &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            CustomGui.rectBtnOptions = rect;
            ReleaseDC(hwnd, hdc);
            break;
       case WM_CTLCOLORBTN:
       case WM_CTLCOLORDLG:
       case WM_CTLCOLOREDIT:
       case WM_CTLCOLORLISTBOX:
       case WM_CTLCOLORMSGBOX:
       case WM_CTLCOLORSCROLLBAR:
       case WM_CTLCOLORSTATIC:
            hdc = (HDC) wParam;
            SetTextColor(hdc, CustomGui.crTextColor);
            SetBkColor(hdc, CustomGui.crBkTextColor);
            return (LRESULT) CustomGui.hbrBrush;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return lResult;
}

LRESULT CALLBACK CustomGuiWindowProcedure (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT      lResult;
    HDC          hdc;
    RECT         rect, rectItem;
    PAINTSTRUCT  ps;
    LPTSTR       lpTstr;
    int          len, nParts, i;
    int          iPartsCoord[128];
    WNDPROC      wndProc;
    int          nTab, nCurTab;
    TCITEM       tcItem;

    len = 256;
    lpTstr = malloc((len + 1) * sizeof(TCHAR));
    GetClassName(hwnd, lpTstr, len);
    if (!stricmp(lpTstr, STATUSCLASSNAME))
        wndProc = CustomGui.wndProcOldStatus;
    else if (!stricmp(lpTstr, WC_TABCONTROL))
        wndProc = CustomGui.wndProcOldTabControl;
    else if (!stricmp(lpTstr, WC_BUTTON))
        wndProc = CustomGui.wndProcOldButton;
    else if (!stricmp(lpTstr, WC_STATIC))
        wndProc = CustomGui.wndProcOldStatic;
    else if (!stricmp(lpTstr, WC_COMBOBOX))
        wndProc = CustomGui.wndProcOldComboBox;
    else
        wndProc = NULL;
    free(lpTstr);

    switch(msg)
    {
        case WM_MOUSEMOVE:
             return 0;
        case WM_ERASEBKGND:
             break;
            hdc = (HDC) wParam;
            // find coordinates of client area
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, CustomGui.hbrBrush);
            return 1;
       case WM_CTLCOLORBTN:
       case WM_CTLCOLORDLG:
       case WM_CTLCOLOREDIT:
       case WM_CTLCOLORLISTBOX:
       case WM_CTLCOLORMSGBOX:
       case WM_CTLCOLORSCROLLBAR:
       case WM_CTLCOLORSTATIC:
            hdc = (HDC) wParam;
            SetTextColor(hdc, CustomGui.crTextColor);
            SetBkColor(hdc, CustomGui.crBkTextColor);
            return (LRESULT) CustomGui.hbrBrush;
        case WM_PAINT:
            // custom painting will go here
            hdc = BeginPaint(hwnd, &ps);
            // find coordinates of client area
            GetClientRect(hwnd, &rect);
            // clear draw area
            FillRect(hdc, &rect, CustomGui.hbrBrush);
            // custom status bar
            if (wndProc == CustomGui.wndProcOldStatus)
            {
                // get parts count
                nParts = SendMessage(hwnd, (UINT) SB_GETPARTS, (WPARAM) sizeof (iPartsCoord) / sizeof (int), (WPARAM) iPartsCoord);
                for (i = 0; i < nParts; i ++)
                {
                    // part bounding rectangle
                    SendMessage(hwnd, (UINT) SB_GETRECT, (WPARAM) i, (WPARAM) &rect);
                    // clear part rectangle
                    FillRect(hdc, &rect, CustomGui.hbrBrush);
                    // deflate the rectangle by the size of the borders
                    InflateRect(&rect, -1, -1);
                    // left space for dividing line
                    rect.left += 2;
                    // select font and set text colors for painting
                    SelectObject(hdc, CustomGui.hfn);
                    SetTextColor(hdc, CustomGui.crTextColor);
                    SetBkColor(hdc, CustomGui.crBkTextColor);
                    // get part text len
                    lResult = SendMessage(hwnd, (UINT) SB_GETTEXTLENGTH, (WPARAM) i, 0);
                    // if ownerdraw has no text to draw
                    if (HIWORD(lResult) == SBT_OWNERDRAW)
                       continue;
                    // get text string
                    len = LOWORD(lResult);
                    lpTstr = malloc((len + 1) * sizeof(TCHAR));
                    lResult = SendMessage(hwnd, (UINT) SB_GETTEXT, (WPARAM) i, (LPARAM) lpTstr);
                    // output text
                    DrawText(hdc, lpTstr, len, &rect, /*DT_CENTER|*/DT_VCENTER|DT_SINGLELINE);
                    free(lpTstr);
                    // set brush and pen for painting parts dividing lines
                    SelectObject(hdc, CustomGui.hbrBrush);
                    SelectObject(hdc, CustomGui.hpnPen);
                    if (i == 0)
                    {
                        // first line
                        MoveToEx(hdc, 0, rect.top + 2, NULL);
                        LineTo(hdc, 0, rect.bottom - 2);
                    }
                    // next line
                    MoveToEx(hdc, iPartsCoord[i], rect.top + 2, NULL);
                    LineTo(hdc, iPartsCoord[i], rect.bottom - 2);
                }
                // set address of control's original window procedure
                wndProc = CustomGui.wndProcOldStatus;
            }
            else if (wndProc == CustomGui.wndProcOldTabControl)
            {
                TabCtrl_AdjustRect(hwnd, FALSE, &rect);
                SelectObject(hdc, CustomGui.hbrBrush);
                SelectObject(hdc, CustomGui.hpnPen);
                rect.top -= 1;
                Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                nTab = TabCtrl_GetItemCount(hwnd);
                nCurTab = TabCtrl_GetCurSel(hwnd);
                for (i = 0; i < nTab; i ++)
                {
                    TabCtrl_GetItemRect(hwnd, i, &rectItem);
                    InflateRect(&rectItem, -2, 0);
                    SelectObject(hdc, CustomGui.hpnPen);
                    SelectObject(hdc, CustomGui.hbrBrush);
                    if (i == nCurTab)
                    {
                        rectItem.bottom += 2;
                        Rectangle(hdc, rectItem.left, rectItem.top, rectItem.right, rectItem.bottom);
                        SelectObject(hdc, CustomGui.hpnPenBrush);
                    }
                    else
                    {
                        FillRect(hdc, &rectItem, CustomGui.hbrBrushPen);
                        rectItem.bottom += 2;
                        SelectObject(hdc, CustomGui.hpnPenBrush);
                        MoveToEx(hdc, rectItem.left, rectItem.bottom - 2, NULL);
                        LineTo(hdc, rectItem.right, rectItem.bottom - 2);
                        SelectObject(hdc, CustomGui.hpnPen);
                    }
                    MoveToEx(hdc, rectItem.left + 1, rectItem.bottom - 1, NULL);
                    LineTo(hdc, rectItem.right - 1, rectItem.bottom - 1);
                    InflateRect(&rectItem, 0, -1);
                    SelectObject(hdc, CustomGui.hfn);
                    SetTextColor(hdc, i == nCurTab ? CustomGui.crTextColor : CustomGui.crBkTextColor);
                    SetBkColor(hdc, i == nCurTab ? CustomGui.crBkTextColor : CustomGui.crTextColor);
                    tcItem.mask = TCIF_TEXT;
                    tcItem.cchTextMax = 1024;
                    tcItem.pszText = malloc(tcItem.cchTextMax);
                    if (!TabCtrl_GetItem(hwnd, i, &tcItem))
                       continue;
                    DrawText(hdc, tcItem.pszText, strlen(tcItem.pszText), &rectItem, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
                    free(tcItem.pszText);
                }
                wndProc = CustomGui.wndProcOldTabControl;
            }
            else if (wndProc == CustomGui.wndProcOldButton)
            {
                SelectObject(hdc, CustomGui.hbrBrush);
                SelectObject(hdc, CustomGui.hpnPen);
                FillRect(hdc, &rect, CustomGui.hbrBrush);
                // Deflate the rectangle by the size of the borders
                InflateRect(&rect, -1, -1);
                Rectangle(hdc, rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1);
                lResult = SendMessage(hwnd, (UINT) BM_GETSTATE, 0, 0);
                if (lResult & BST_PUSHED)
                {
                    SelectObject(hdc, CustomGui.hpnPen);
                    Rectangle(hdc, rect.left + 2, rect.top + 2, rect.right - 2, rect.bottom - 2);
                }
                SetTextColor(hdc, CustomGui.crTextColor);
                SetBkColor(hdc, CustomGui.crBkTextColor);
                SelectObject(hdc, CustomGui.hfn);
                len = GetWindowTextLength(hwnd);
                lpTstr = (LPTSTR) malloc((len + 1) * sizeof (TCHAR));
                GetWindowText(hwnd, lpTstr, len + 1);
                SetBkMode(hdc, TRANSPARENT);
                InflateRect(&rect, -3, -3);
                if (lResult & BST_FOCUS)
                {
                    rect.left += 2;
                    rect.top += 2;
                    DrawText(hdc, lpTstr, len, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
                    rect.left -= 2;
                    rect.top -= 2;
                }
                DrawText(hdc, lpTstr, len, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
                free(lpTstr);
                wndProc = CustomGui.wndProcOldButton;
            }
            else if (wndProc == CustomGui.wndProcOldStatic)
            {
                SelectObject(hdc, CustomGui.hbrBrush);
                FillRect(hdc, &rect, CustomGui.hbrBrush);
                // Deflate the rectangle by the size of the borders
                InflateRect(&rect, -1, -1);
                SelectObject(hdc, CustomGui.hfn);
                SetTextColor(hdc, CustomGui.crTextColor);
                SetBkColor(hdc, CustomGui.crBkTextColor);
                len = GetWindowTextLength(hwnd);
                lpTstr = (LPTSTR) malloc((len + 1) * sizeof (TCHAR));
                GetWindowText(hwnd, lpTstr, len + 1);
                SetBkMode(hdc, TRANSPARENT);
                DrawText(hdc, lpTstr, len, &rect, /*DT_CENTER|*/DT_VCENTER|DT_SINGLELINE);
                free(lpTstr);
                wndProc = CustomGui.wndProcOldStatic;
            }
            else if (wndProc == CustomGui.wndProcOldComboBox)
            {
                // Deflate the rectangle by the size of the borders
                InflateRect(&rect, -GetSystemMetrics(SM_CXEDGE) - 1, -GetSystemMetrics(SM_CYEDGE) - 1);
                // Remove the drop-down button as well
                rect.right -= GetSystemMetrics(SM_CXVSCROLL);
                // Make a mask from the rectangle, so the borders aren't included
                IntersectClipRect(hdc, rect.left, rect.top, rect.right, rect.bottom);
                // Draw the combo-box into our DC
                CallWindowProc(CustomGui.wndProcOldComboBox, hwnd, msg, (WPARAM)hdc, lParam);
                // Remove the clipping region
                SelectClipRgn(hdc, NULL);
                // now mask off the inside of the control so we can paint the borders
                ExcludeClipRect(hdc, rect.left, rect.top, rect.right, rect.bottom);
                // paint a flat colour
                GetClientRect(hwnd, &rect);
                SelectObject(hdc, CustomGui.hbrBrush);
                SelectObject(hdc, CustomGui.hpnPen);
                Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                rect.left = rect.right - GetSystemMetrics(SM_CXVSCROLL);
                Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                InflateRect(&rect, -1, -1);
                SelectObject(hdc, CustomGui.hfnWebdings);
                SetTextColor(hdc, CustomGui.crTextColor);
                SetBkColor(hdc, CustomGui.crBkTextColor);
                DrawText(hdc, "\x36", 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            }
            // done
            EndPaint(hwnd, &ps);
            return 0;
    }
    // preserve original window's functionality
    if (wndProc)
       return CallWindowProc(wndProc, hwnd, msg, wParam, lParam);
    else
       return 0;
}
