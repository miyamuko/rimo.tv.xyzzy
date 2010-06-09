// これは メイン DLL ファイルです。f

#pragma comment(lib, "Vfw32.Lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#include <windows.h>
#include <atlbase.h>

CComModule	_Module;

#include <atlcom.h>
#include <atlhost.h>

#include "xyzzy-rimo.h"

#ifdef _DEBUG
#define D(m)     MessageBox(NULL, m, "debug", MB_OK);
#define D1(m, arg)     {                    \
    char buf[256];                          \
    _snprintf(buf, 256, m, arg);            \
    MessageBox(NULL, buf, "debug", MB_OK);  \
}
#else
#define D(m)
#define D1(m, arg)
#endif

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()


static const char* windowClassName = "xyzzy-rimo-window";

static HINSTANCE rimoInstance;
static HWND rimoWindow;
static HWND browserWindow;
static HANDLE waiterThread;
static HANDLE navigateCompleteEvent;
static CComQIPtr<IWebBrowser2> ie;
static BOOL shutdownRequest;

static int pos_x;
static int pos_y;
static int width;
static int height;

static DWORD lastError;
static BOOL initialized;

static void SaveLastError();

static void ReleaseWebBrowser();
static HWND CreateBrowserWindow(HWND parent);
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
static HWND XyRimoCreateWindowImpl(HWND parent, int x, int y, int w, int h);
static DWORD XyRimoCloseWindowImpl();
static void ReleaseWebBrowserImpl();

static BOOL NavigateRimo();
static DWORD WINAPI WaitForRimo(LPVOID lpParam);
static DWORD WaitForRimoImpl();
static BOOL WaitForNaviagte();
static BOOL DisableScrollBar();


class Monitor : private CRITICAL_SECTION
{
public :
    Monitor() {
        InitializeCriticalSection (this);
    }
    ~Monitor() {
        DeleteCriticalSection (this);
    }

    void Aquire() {
        D1("aquire %p", GetCurrentThreadId());
        EnterCriticalSection (this);
    }
    void Release() {
        D1("release %p", GetCurrentThreadId());
        LeaveCriticalSection (this);
    }
} ;

class ScopedLock {
public:
    ScopedLock(Monitor& m) : _m(m) {
        _m.Aquire();
    }
    ~ScopedLock() {
        _m.Release();
    }
private:
    Monitor& _m;
};

//static HANDLE lock = CreateMutex(NULL, FALSE, NULL);
static Monitor mutex;


static void
ResetGlobalVariables()
{
    D(__FUNCTION__);

    ScopedLock lock(mutex);

    rimoInstance = 0;
    rimoWindow = 0;
    browserWindow = 0;
    waiterThread = 0;
    navigateCompleteEvent = 0;
    shutdownRequest = FALSE;
    ie = static_cast<IWebBrowser2*>(0);
}


static void
SaveLastError()
{
    lastError = GetLastError();
}

DWORD
XyRimoGetLastError()
{
    return lastError;
}

void
XyRimoFormatError(char* buffer, size_t size)
{
    //    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, lastError, 0, buffer, size);
}

HWND
XyRimoGetWindow()
{
    return rimoWindow;
}


BOOL
XyRimoUpdateWindow()
{
    D(__FUNCTION__);

    if (!rimoWindow) {
        return FALSE;
    }
    UpdateWindow(rimoWindow);
    UpdateWindow(browserWindow);

    return TRUE;
}

BOOL
XyRimoShowWindow()
{
    D(__FUNCTION__);

    if (!rimoWindow) {
        return FALSE;
    }
    return ShowWindow(rimoWindow, SW_SHOWNA);
}

BOOL
XyRimoHideWindow()
{
    D(__FUNCTION__);

    if (!rimoWindow) {
        return FALSE;
    }
    return ShowWindow(rimoWindow, SW_HIDE);
}

BOOL
XyRimoMoveWindow(int x, int y, int w, int h)
{
    D(__FUNCTION__);

    if (!rimoWindow) {
        return FALSE;
    }

    pos_x = x;
    pos_y = y;
    width = w;
    height = h;

    SetWindowPos(rimoWindow, 0, pos_x, pos_y, width, height,
                 SWP_NOACTIVATE | SWP_NOZORDER);
    SetWindowPos(browserWindow, 0, 0, 0, width, height,
                 SWP_NOACTIVATE | SWP_NOZORDER);

    return TRUE;
}


#define CLOSE_WAIT_TIME (5 * 1000) // 3 secs
#define GRACEFUL_SHUTDOWN   0x1
#define TERMINATED          0x2
#define DESTROY_FAIL_1      0x4
#define DESTROY_FAIL_2      0x8
#define SEH_ERROR           0xff

DWORD
XyRimoCloseWindow()
{
    D(__FUNCTION__);

    shutdownRequest = TRUE;
    __try {
        return XyRimoCloseWindowImpl();
    } __except(1) {
        // ignore error
    }

    return SEH_ERROR;
}

static DWORD
XyRimoCloseWindowImpl()
{
    D(__FUNCTION__);
    ScopedLock lock(mutex);

    DWORD ret = 0x0;

    if (!rimoWindow) {
        return ret;
    }

    if (waiterThread) {
        D("wait for nav comp");
        if (WaitForSingleObject(navigateCompleteEvent, CLOSE_WAIT_TIME) != WAIT_OBJECT_0) {
            D("terminate!");
            ret |= TERMINATED;
            TerminateThread(waiterThread, 255);
        } else {
            ret |= GRACEFUL_SHUTDOWN;
        }
    }

    ReleaseWebBrowser();
    if (!DestroyWindow(browserWindow)) {
        ret |= DESTROY_FAIL_1;
        SaveLastError();
    }

    if (!DestroyWindow(rimoWindow)) {
        ret |= DESTROY_FAIL_2;
        SaveLastError();
    }

    ResetGlobalVariables();

    return ret;
}

static void
ReleaseWebBrowser()
{
    D(__FUNCTION__);

    __try {
        ReleaseWebBrowserImpl();
    } __except(1) {
        // ignore error
    }

    ie = static_cast<IWebBrowser2*>(0);
}

static void
ReleaseWebBrowserImpl()
{
    ScopedLock lock(mutex);
    if (ie) {
        ie.Release();
    }
}

HWND
XyRimoCreateWindow(HWND parent, int x, int y, int w, int h)
{
    D(__FUNCTION__);

    __try {
        return XyRimoCreateWindowImpl(parent, x, y, w, h);
    } __except(1) {
        // ignore error
    }

    return 0;
}

static HWND
XyRimoCreateWindowImpl(HWND parent, int x, int y, int w, int h)
{
    D(__FUNCTION__);

    if (rimoWindow) {
        XyRimoMoveWindow(x, y, w, h);
        return XyRimoGetWindow();
    }

    ScopedLock lock(mutex);

    pos_x = x;
    pos_y = y;
    width = w;
    height = h;

    ResetGlobalVariables();
    if (!CreateBrowserWindow(parent)) {
        SaveLastError();
        return 0;
    }

    if (!NavigateRimo()) {
        return 0;
    }

    navigateCompleteEvent = CreateEvent(
        NULL,              // default security attributes
        FALSE,             // manual-reset event
        FALSE,             // initial state is signaled
        "NavigateComplete" // object name
        );

    DWORD tid;
    waiterThread = CreateThread(
        NULL,              // default security attributes
        0,                 // use default stack size
        WaitForRimo,       // thread function
        0,                 // argument to thread function
        0,                 // use default creation flags
        &tid);             // returns the thread identifier

    D("finish");
    return XyRimoGetWindow();
}

static BOOL
RegisterBrowserWindow()
{
    D(__FUNCTION__);

    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = rimoInstance;
    wc.hIcon = 0;
    wc.hCursor = LoadCursor (0, IDC_ARROW);
    wc.hbrBackground = 0;
    wc.lpszMenuName = 0;
    wc.lpszClassName = windowClassName;

    if (!RegisterClass (&wc)) {
        SaveLastError();
        return FALSE;
    }

    return TRUE;
}

static HWND
CreateBrowserWindow(HWND parent)
{
    D(__FUNCTION__);

    if (rimoWindow) {
        return rimoWindow;
    }

    if (!initialized) {
        RegisterBrowserWindow();
        initialized = TRUE;
    }

    _Module.Init(ObjectMap, rimoInstance);

    rimoWindow = CreateWindow(windowClassName, "",
                              WS_POPUP | WS_BORDER,
                              pos_x, pos_y, width, height,
                              parent, 0, rimoInstance, 0);

    if (!rimoWindow) {
        SaveLastError();
        return 0;
    }

    XyRimoShowWindow();
    XyRimoUpdateWindow();

    return XyRimoGetWindow();
}

static BOOL
NavigateRimo()
{
    D(__FUNCTION__);

    if (!ie) {
        return FALSE;
    }

    BSTR burl = SysAllocString(L"http://rimo.tv/");
    VARIANT nil;
    VariantInit(&nil);

    ie->Navigate(burl, &nil, &nil, &nil, &nil);

    VariantClear(&nil);
    SysFreeString(burl);

    return TRUE;
}

static DWORD WINAPI
WaitForRimo(LPVOID lpParam)
{
    D(__FUNCTION__);

    __try {
        return WaitForRimoImpl();
    } __except(1) {
        D("failed to WaitForRimoImpl");
        SetEvent(navigateCompleteEvent);
        XyRimoCloseWindow();
    }

    return 2;
}

static DWORD
WaitForRimoImpl()
{
    D(__FUNCTION__);
    ScopedLock lock(mutex);

    D1("shutdown = %p", shutdownRequest);
    if (shutdownRequest || !WaitForNaviagte()) {
        D("fire cancel!");
        SetEvent(navigateCompleteEvent);
        return 0;
    }

    if (shutdownRequest || !DisableScrollBar()) {
        D("fire cancel!");
        SetEvent(navigateCompleteEvent);
        return 0;
    }

    XyRimoUpdateWindow();
    D("fire!");
    SetEvent(navigateCompleteEvent);
    return 1;
}

static BOOL
WaitForNaviagte()
{
    D(__FUNCTION__);

    READYSTATE state;
    while (true) {
        if (shutdownRequest) {
            D("shutdown!");
            return FALSE;
        }
        if (ie->get_ReadyState(&state) != S_OK) {
            D("readstate fail");
            return FALSE;
        }
        if (state == READYSTATE_COMPLETE) {
            D("ready!!!");
            return TRUE;
        }
        D("sleep(10)");
        Sleep(10);
    }

    return TRUE;
}

static BOOL
DisableScrollBar()
{
    D(__FUNCTION__);

    if (shutdownRequest || ie == NULL) {
        return FALSE;
    }

    LPDISPATCH lpDispatch = NULL;
    ie->get_Document(&lpDispatch);
    if (shutdownRequest || lpDispatch == NULL) {
        return FALSE;
    }

    IHTMLDocument2* lpHtmlDocument = NULL;
    lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&lpHtmlDocument);
    lpDispatch->Release();
    if (shutdownRequest || lpHtmlDocument == NULL) {
        return FALSE;
    }

    IHTMLElement* lpBodyElem = NULL;
    lpHtmlDocument->get_body(&lpBodyElem);
    lpHtmlDocument->Release();
    if (shutdownRequest || lpBodyElem == NULL) {
        return FALSE;
    }

    IHTMLBodyElement* lpBody = NULL;
    lpBodyElem->QueryInterface(IID_IHTMLBodyElement,(void**)&lpBody);
    lpBodyElem->Release();
    if (shutdownRequest || lpBody == NULL) {
        return FALSE;
    }

    lpBody->put_scroll(L"no");
    lpBody->Release();

    return TRUE;
}

static LRESULT CALLBACK
WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
    case WM_CREATE:
        {
            AtlAxWinInit();
            browserWindow = CreateWindow("AtlAxWin",
                                         "Shell.Explorer.2",
                                         WS_CHILD|WS_VISIBLE,
                                         0, 0, width, height,
                                         hwnd, (HMENU)0, GetModuleHandle(NULL), NULL);
            CComPtr<IUnknown> unknown;
            if (AtlAxGetControl(browserWindow, &unknown) == S_OK) {
                ie = unknown;
            }
            return 0;
        }

    case WM_DESTROY:
        ReleaseWebBrowser();
        break;
    }

    return DefWindowProc (hwnd, msg, wparam, lparam);
}
