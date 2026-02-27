#include <aeDraw.hpp>

u0 aeDrawPixel(AEFrameBuffer aeFrameBuffer, i32 x, i32 y, u32 ulColor) {
    if (!aeFrameBuffer.pFrameBuffer) return;
    i32 width = (i32)aeFrameBuffer.spFrameBufferInfo.bmiHeader.biWidth;
    i32 height = (i32)aeFrameBuffer.spFrameBufferInfo.bmiHeader.biHeight;
    if (height < 0) height = -height;

    if (x >= 0 && x < width && y >= 0 && y < height) {
        ((u32*)(aeFrameBuffer.pFrameBuffer))[width * y + x] = ulColor;
    }
}

u0 aeDrawCube(AEFrameBuffer aeFrameBuffer, i32 x, i32 y, u32 size, u32 ulColor) {
    for (i32 j = 0; j < size; j++)
        for (i32 i = 0; i < size; i++)
            aeDrawPixel(aeFrameBuffer, x + i, y + j, ulColor);
}

u0 aeDrawCircle(AEFrameBuffer aeFrameBuffer, i32 circleX, i32 circleY, i32 radius, u32 ulColor) {
    for (i32 y = -radius; y <= radius; y++) {
        for (i32 x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                aeDrawPixel(aeFrameBuffer, circleX + x, circleY + y, ulColor);
            }
        }
    }
}