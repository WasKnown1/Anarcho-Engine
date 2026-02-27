#include <aeThread.hpp>
#include <aeMain.hpp>
#include <stdbool.h>

u0 aeCreateInstance(AEInstance *aeInstance, LPCWSTR lpszClassName, AEWNDPROC lpfnWndProc) {
    HINSTANCE hInstace = GetModuleHandle(NULL);
    aeInstance->hInstance = hInstace;
    WNDCLASSW window_class = {0};
    window_class.lpszClassName = lpszClassName;
    window_class.lpfnWndProc = lpfnWndProc;
    aeInstance->winClassW = window_class;
    aeInstance->winClassW.hInstance = hInstace;
    RegisterClassW(&aeInstance->winClassW);
}

u0 aeCreateFrameBuffer(AEFrameBuffer *aeFrameBuffer, AEPRect windowRect) {
    aeFrameBuffer->spFrameBufferInfo.bmiHeader.biSize = sizeof(aeFrameBuffer->spFrameBufferInfo.bmiHeader);
    aeFrameBuffer->spFrameBufferInfo.bmiHeader.biWidth = windowRect->width;
    aeFrameBuffer->spFrameBufferInfo.bmiHeader.biHeight = -windowRect->height;
    aeFrameBuffer->spFrameBufferInfo.bmiHeader.biPlanes = 1;
    aeFrameBuffer->spFrameBufferInfo.bmiHeader.biBitCount = 32;
    aeFrameBuffer->spFrameBufferInfo.bmiHeader.biCompression = BI_RGB;

    HDC DeviceContext = CreateCompatibleDC(0);
    HBITMAP Bitmap = CreateDIBSection(DeviceContext, &aeFrameBuffer->spFrameBufferInfo, DIB_RGB_COLORS, &aeFrameBuffer->pFrameBuffer, 0, 0);
    DeleteDC(DeviceContext);
}

AEPRect aeCreateRect(u0 *allocator, u32 x, u32 y, u32 width, u32 height) {
    u0 *(*newAllocator)(u32) = ((u0 *(*)(u32))allocator);
    AEPRect rect = (AEPRect)newAllocator(sizeof(AERect));
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    return rect;
}

u0 aeCreateWindow(AEWindow *aeWindow, AEFrameBuffer *aeFrameBuffer, AEInstance aeInstance, LPCWSTR lpWindowName, AEPRect windowRect) {
    HWND window_handle = CreateWindowW(aeInstance.winClassW.lpszClassName,
                                       lpWindowName,
                                       WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // | WS_MAXIMIZEBOX no need for this, i dont want my engine to have resizable windows by default for now.
                                       windowRect->x,
                                       windowRect->y,
                                       windowRect->width,
                                       windowRect->height,
                                       NULL,
                                       NULL,
                                       aeInstance.hInstance,
                                       NULL);
    if (window_handle == NULL) return;

    aeWindow->windowHandle  = window_handle;
    aeWindow->windowRect    = windowRect;
    aeWindow->aeFrameBuffer = aeFrameBuffer;
}

u0 aeFillWindow(AEWindow aeWindow, u32 ulColor) {
    if (!aeWindow.aeFrameBuffer || !aeWindow.aeFrameBuffer->pFrameBuffer) return;
    u32 pixels = aeWindow.windowRect->width * aeWindow.windowRect->height;
    uint32_t *pix = (uint32_t *)aeWindow.aeFrameBuffer->pFrameBuffer;
    for (u32 i = 0; i < pixels; ++i) pix[i] = ulColor;
}

u0 aeEventLoop(bool *eventLoopStop) {
    while (!*eventLoopStop) {
        MSG message;
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
}

u0 aeEventLoopThreaded(ThreadSafe<bool> *eventLoopStop) {
    while (!aeGetThreadSafeValue(eventLoopStop)) {
        MSG message;
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
}

u0 aeGetClientRect(AEWindow aeWindow, AEPRect rect) {
    RECT clientRect;
    GetClientRect(aeWindow.windowHandle, &clientRect);
    rect->x = clientRect.left;
    rect->y = clientRect.top;
    rect->width = clientRect.right - clientRect.left;
    rect->height = clientRect.bottom - clientRect.top;
}