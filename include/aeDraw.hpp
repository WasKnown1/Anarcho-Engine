#ifndef anarcho_draw_hpp
#define anarcho_draw_hpp

#include <aeTypes.hpp>

u0 aeDrawPixel(AEFrameBuffer aeFrameBuffer, i32 x, i32 y, u32 ulColor);
u0 aeDrawCube(AEFrameBuffer aeFrameBuffer, i32 x, i32 y, u32 size, u32 ulColor);
u0 aeDrawCircle(AEFrameBuffer aeFrameBuffer, i32 circleX, i32 circleY, i32 radius, u32 ulColor);

#endif // anarcho_draw_hpp