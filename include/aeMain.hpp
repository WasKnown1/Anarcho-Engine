#ifndef anarcho_main_hpp
#define anarcho_main_hpp

#include <aeTypes.hpp>
#include <aeDraw.hpp>
#include <aeThread.hpp>
#include <aeInput.hpp>
#include <aeGpu.hpp>

u0 aeCreateInstance(AEInstance *aeInstance, LPCWSTR lpszClassName, AEWNDPROC lpfnWndProc);
#define aeLoadCursor LoadCursor
AEPRect aeCreateRect(u0 *allocator, u32 x, u32 y, u32 width, u32 height);
u0 aeCreateFrameBuffer(AEFrameBuffer *aeFrameBuffer, AEPRect windowRect);
u0 aeCreateWindow(AEWindow *aeWindow, AEFrameBuffer *aeFrameBuffer, AEInstance aeInstance, LPCWSTR lpWindowName, AEPRect windowRect);
#define aeShowWindow(aeWindow) ShowWindow(aeWindow.windowHandle, SW_SHOW)
u0 aeFillWindow(AEWindow aeWindow, u32 ulColor);
#define aeResizeWindow(aeWindow, newWidth, newHeight) MoveWindow(aeWindow.windowHandle, aeWindow.windowRect->x, aeWindow.windowRect->y, newWidth, newHeight, TRUE)
#define aeMoveWindow(aeWindow, newX, newY) MoveWindow(aeWindow.windowHandle, newX, newY, aeWindow.windowRect->width, aeWindow.windowRect->height, TRUE)
u0 aeEventLoop(bool *eventLoopStop);
u0 aeEventLoopThreaded(ThreadSafe<bool> *eventLoopStop);
#define aeInvalidateWindow(aeWindow) InvalidateRect(aeWindow.windowHandle, NULL, FALSE)
inline u0 aeGetCursorPosition(i32 *x, i32 *y) {
    POINT point;
    GetCursorPos(&point);
    *x = point.x;
    *y = point.y;
}
inline u0 aeGetCursorPositionScreenToClient(AEWindow aeWindow, i32 *x, i32 *y) {
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(aeWindow.windowHandle, &point);
    *x = point.x;
    *y = point.y;
}
#define aeSetCursorPosition(x, y) SetCursorPos(x, y)

inline u0 aeClipCursor(AEPRect clipRect) {
    RECT rect = {(i32)clipRect->x, (i32)clipRect->y, (i32)(clipRect->x + clipRect->width), (i32)(clipRect->y + clipRect->height)};
    ClipCursor(&rect);
}

inline u0 aeClipCursor(i32 x, i32 y) {
    RECT rect = {x, y, x + 1, y + 1};
    ClipCursor(&rect);
}

#define aeReleaseCursor() ClipCursor(NULL)
u0 aeGetClientRect(AEWindow aeWindow, AEPRect rect);
inline u0 aeClientToScreen(AEWindow aeWindow, i32 *x, i32 *y) {
    POINT point = {*x, *y};
    ClientToScreen(aeWindow.windowHandle, &point);
    *x = point.x;
    *y = point.y;
}

#endif // anarcho_main_hpp