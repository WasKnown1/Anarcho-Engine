#ifndef anarcho_input_hpp
#define anarcho_input_hpp

#include <aeTypes.hpp>

#define aeGetKeyState(key) (GetAsyncKeyState(key) & 0x8000)

#endif // anarcho_input_hpp