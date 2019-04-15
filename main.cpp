#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif
#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <math.h>

#define MENU_NULL 0

#define MENU_LINE_DDA 10
#define MENU_LINE_MIDPOINT 11
#define MENU_LINE_PARAMETRIC 12

#define MENU_CIRCLE_MIDPOINT 22
#define MENU_CIRCLE_DDA 21

#define COLOR_RED 30
#define COLOR_BLUE 31
#define COLOR_GREEN 32

#define MENU_EXIT 2

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");
HWND button;
HMENU hMenu;

struct point{
    int x;
    int y;
};

point p1,p2;
COLORREF mainColorRef = RGB(255, 0, 0);

void createButton(HWND hwnd);
void createMainMenu(HWND hwnd);
void menuCommandHandler(WPARAM wParam, HWND hwnd);

// Line Algorithms Methods
void LineDDA(HDC hdc, point p1, point p2,COLORREF color);
void LineParametric(HDC hdc, point p1, point p2,COLORREF color);
void LineMidPoint(HDC hdc, point p1, point p2,COLORREF color);
//////////////////////////

// Circle Algorithms Methods
void Draw8Points(HDC hdc,int xc,int yc, int a, int b,COLORREF color);
void CircleMidPoint(HDC hdc,int xc,int yc, int R,COLORREF color);
void CircleIterativePolar(HDC hdc,int xc,int yc, int R,COLORREF color);
////////////////////////////

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

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
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("20160322"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           960,                 /* The programs width */
           540,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_COMMAND:
            menuCommandHandler(wParam, hwnd);
            break;
        case WM_CREATE:
            //createButton(hwnd);
            createMainMenu(hwnd);
            break;
        case WM_LBUTTONDOWN:
            p1.x = LOWORD(lParam);
            p1.y = HIWORD(lParam);
            break;
        case WM_LBUTTONUP:
            p2.x = LOWORD(lParam);
            p2.y = HIWORD(lParam);
            break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void menuCommandHandler(WPARAM wParam, HWND hwnd){
    switch(wParam){
        case MENU_EXIT:
            DestroyWindow(hwnd);
            break;
        case MENU_LINE_DDA:
            LineDDA(GetDC(hwnd), p1,p2, mainColorRef);
            break;
        case MENU_LINE_MIDPOINT:
            LineMidPoint(GetDC(hwnd), p1,p2, mainColorRef);
            break;
        case MENU_LINE_PARAMETRIC:
            LineParametric(GetDC(hwnd), p1,p2, mainColorRef);
            break;
        case MENU_CIRCLE_DDA:{
            int r = sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
            CircleIterativePolar(GetDC(hwnd), p1.x, p1.y, r, mainColorRef);
            break;
        }
        case MENU_CIRCLE_MIDPOINT:{
            int r = sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
            CircleMidPoint(GetDC(hwnd), p1.x, p1.y, r, mainColorRef);
            break;
        }
        case COLOR_RED:
            mainColorRef = RGB(255,0,0);
            break;
        case COLOR_BLUE:
            mainColorRef = RGB(0,0,255);
            break;
        case COLOR_GREEN:
            mainColorRef = RGB(0,255,0);
            break;
        default:
            std::cout << "Word Parameter Value is : " << wParam << std::endl;
            std::cout << "\tP1 is : (" << p1.x <<" ," << p1.y << ")" << std::endl;
            std::cout << "\tP2 is : (" << p2.x <<" ," << p2.y << ")" << std::endl;
    }
}

void createButton(HWND hwnd){
    button = CreateWindow("button", "Draw Line",
                                       WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                       20,440,150,35,
                                       hwnd, NULL, GetModuleHandle(NULL), NULL);
}

void createMainMenu(HWND hwnd){
    hMenu = CreateMenu();

    HMENU lineMenu = CreateMenu();
    AppendMenu(lineMenu, MF_STRING, MENU_LINE_DDA, "DDA");
    AppendMenu(lineMenu, MF_STRING, MENU_LINE_MIDPOINT, "Midpoint");
    AppendMenu(lineMenu, MF_STRING, MENU_LINE_PARAMETRIC, "Parametric");

    HMENU CircleMenu = CreateMenu();
    AppendMenu(CircleMenu, MF_STRING, MENU_CIRCLE_MIDPOINT, "Midpoint");
    AppendMenu(CircleMenu, MF_STRING, MENU_CIRCLE_DDA, "DDA");

    HMENU drawMenu = CreateMenu();
    AppendMenu(drawMenu, MF_POPUP, (UINT_PTR) lineMenu, "Line Algorithms");
    AppendMenu(drawMenu, MF_POPUP, (UINT_PTR) CircleMenu, "Circle Algorithms");

    HMENU colorMenu = CreateMenu();
    AppendMenu(colorMenu, MF_STRING, COLOR_RED, "Red");
    AppendMenu(colorMenu, MF_STRING, COLOR_BLUE, "Blue");
    AppendMenu(colorMenu, MF_STRING, COLOR_GREEN, "Green");

    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR) drawMenu, "Draw");
    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR) colorMenu, "Color");
    AppendMenuA(hMenu, MF_STRING, MENU_EXIT, "Exit");
    SetMenu(hwnd, hMenu);
}

// Line Algorithms
void LineDDA(HDC hdc, point p1, point p2,COLORREF color){
    int xs = p1.x, xe = p2.x;
    int ys = p1.y, ye = p2.y;
    int dx=xe-xs;
    int dy=ye-ys;
    SetPixel(hdc,xs,ys,color);
    if(abs(dx)>=abs(dy))
    {
        int x=xs,xinc= dx>0?1:-1;
        double y=ys,yinc=(double)dy/dx*xinc;
        while(x!=xe)
        {
            x+=xinc;
            y+=yinc;
            SetPixel(hdc,x,round(y),color);
        }
    }
    else
    {
        int y=ys,yinc= dy>0?1:-1;
        double x=xs,xinc=(double)dx/dy*yinc;
        while(y!=ye)
        {
            x+=xinc;
            y+=yinc;
            SetPixel(hdc,round(x),y,color);
        }
    }
}

void LineParametric(HDC hdc, point p1, point p2,COLORREF color){
    int x1 = p1.x, x2 = p2.x;
    int y1 = p1.y, y2 = p2.y;
    for(double t=0.0; t<=1; t+=0.001)
    {
        int x=round(x1+t*(x2-x1));
        int y=round(y1+t*(y2-y1));
        SetPixel(hdc,x,y,color);
    }
}

void LineMidPoint(HDC hdc, point p1, point p2,COLORREF color){
    int x1 = p1.x, x2 = p2.x;
    int y1 = p1.y, y2 = p2.y;
    int dx=x2-x1;
    int dy=y2-y1;
    int d=2*dx+dy;
    int d1=2*dx;
    int d2=2*dx+2*dy;
    int x=x1;
    int y=y1;
    SetPixel(hdc,x1,y1,color);
    while(y<y2)
    {
        if(d>0)
        {
            y++;
            d=d+d1;
        }
        else
        {
            x--;
            y++;
            d+=d2;
        }
        SetPixel(hdc,x,y,color);
    }
}
//////////////////

// Circle Algorithms
void Draw8Points(HDC hdc,int xc,int yc, int a, int b,COLORREF color){
    SetPixel(hdc, xc+a, yc+b, color);
    SetPixel(hdc, xc-a, yc+b, color);
    SetPixel(hdc, xc-a, yc-b, color);
    SetPixel(hdc, xc+a, yc-b, color);
    SetPixel(hdc, xc+b, yc+a, color);
    SetPixel(hdc, xc-b, yc+a, color);
    SetPixel(hdc, xc-b, yc-a, color);
    SetPixel(hdc, xc+b, yc-a, color);
}

void CircleMidPoint(HDC hdc,int xc,int yc, int R,COLORREF color){
    int x=0,y=R;
    int d=1-R;
    int c1=3, c2=5-2*R;
    Draw8Points(hdc,xc,yc,x,y,color);
    while(x<y)
    {
        if(d<0)
        {
            d+=c1;
            c2+=2;
        }
        else
        {
            d+=c2;
            c2+=4;
            y--;
        }
        c1+=2;
        x++;
        Draw8Points(hdc,xc,yc,x,y,color);
    }
}

void CircleIterativePolar(HDC hdc,int xc,int yc, int R,COLORREF color){
    double x=R,y=0;
    double dtheta=1.0/R;
    double cdtheta=cos(dtheta),sdtheta=sin(dtheta);
    Draw8Points(hdc,xc,yc,R,0,color);
    while(x>y)
    {
        double x1=x*cdtheta-y*sdtheta;
        y=x*sdtheta+y*cdtheta;
        x=x1;
        Draw8Points(hdc,xc,yc,round(x),round(y),color);
    }
}
////////////////////



