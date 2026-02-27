#ifndef anarcho_types_hpp
#define anarcho_types_hpp

#include <windows.h>
#include <stdint.h>

typedef void u0;
typedef uint32_t u32;
typedef int32_t i32;
typedef unsigned char b8;
typedef LRESULT (*AEWNDPROC)(HWND, UINT, WPARAM, LPARAM);

#define FUNCTION_AS_POINTER(x) (u0*)&x

typedef struct {
    HINSTANCE hInstance;
    WNDCLASSW winClassW;
} AEInstance;

typedef struct {
    u32 x;
    u32 y;
    u32 width;
    u32 height;
} AERect, *AEPRect;

typedef struct {
    u0 *pFrameBuffer;
    BITMAPINFO spFrameBufferInfo;
} AEFrameBuffer;

typedef struct {
    HWND windowHandle;
    AEPRect windowRect;
    AEFrameBuffer *aeFrameBuffer;
} AEWindow;

#endif // anarcho_types_hpp