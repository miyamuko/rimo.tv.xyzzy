extern "C" {

    DWORD
    XyRimoGetLastError();

    void
    XyRimoFormatError(char* buffer, size_t size);

    HWND
    XyRimoGetWindow();

    BOOL
    XyRimoUpdateWindow();

    BOOL
    XyRimoShowWindow();

    BOOL
    XyRimoHideWindow();

    BOOL
    XyRimoMoveWindow(int x, int y, int w, int h);

    DWORD
    XyRimoCloseWindow();

    HWND
    XyRimoCreateWindow(HWND parent, int x, int y, int w, int h);

}
