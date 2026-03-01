#include <iostream>

#include <aeMain.hpp>
#include <chrono>

static ThreadSafe<bool> windowShouldClose = false;
static AEFrameBuffer aeFrameBuffer = {0};
static AEWindow aeWindow = {0};
static ThreadSafe<i32> yValue = 100;
static ThreadSafe<bool> cursorVisible = true;

u0 draw() {
    aeSetCursorPosition(aeWindow.windowRect->width / 2, aeWindow.windowRect->height / 2);
    aeFillWindow(aeWindow, 0xff00ffff);
    aeDrawCircle(aeFrameBuffer, 100, aeGetThreadSafeValue(&yValue), 50, 0xffff0000);

    aeInvalidateWindow(aeWindow);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    if (aeGetKeyState('W')) {
        aeSetThreadSafeValue(&yValue, aeGetThreadSafeValue(&yValue) + 1);
    }
    if (aeGetKeyState('S')) {
        aeSetThreadSafeValue(&yValue, aeGetThreadSafeValue(&yValue) - 1);
    } 
    if (aeGetKeyState('D')) {
        aeSetThreadSafeValue(&cursorVisible, false);
    } 
    if (aeGetKeyState('A')) {
        aeSetThreadSafeValue(&cursorVisible, true);
    }
}

int main() {
    AEInstance aeInstance = {0};
    AEWNDPROC proc = [](HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT {
        switch (message) {
            case WM_DESTROY: {
                windowShouldClose = true;
                PostQuitMessage(0);
                return 0;
            } break;
            case WM_PAINT:{
                PAINTSTRUCT Paint;
                HDC DeviceContext = BeginPaint(window_handle, &Paint);

                StretchDIBits(DeviceContext, 0, 0, aeWindow.windowRect->width, aeWindow.windowRect->height,
                              0, 0, aeWindow.windowRect->width, aeWindow.windowRect->height,
                              aeFrameBuffer.pFrameBuffer, &aeFrameBuffer.spFrameBufferInfo, DIB_RGB_COLORS, SRCCOPY);

                EndPaint(window_handle, &Paint);
            } break;
            case WM_INPUT: {
                int x, y;
                aeGetMouseRawDataInput(&x, &y, lParam);
                printf("Mouse movement: %d, %d\n", x, y);
            } break;
            case WM_SETCURSOR: {
                if (aeGetThreadSafeValue(&cursorVisible)) {
                    aeShowCursor();
                } else {
                    aeHideCursor();
                }
                return TRUE;
            } break;
            default: {
                return DefWindowProcW(window_handle, message, wParam, lParam);
            } break;
        }
        return 0;
    };

    aeCreateInstance(&aeInstance, L"My Class", proc);
    aeInstance.winClassW.hCursor = aeLoadCursor(NULL, IDC_ARROW);

    AEGpuPlatforms platforms;
    aeGpuGetPlatforms(&platforms);
    if (platforms.size() == 0) {
        std::cerr << "No OpenCL platforms found." << std::endl;
        return -1;
    }

    AEGpuPlatform platform = aeGpuGetDefaultPlatform();
    AEGpuDevices devices;
    aeGpuGetDevices(&platform, &devices);
    if (devices.size() == 0) {
        std::cerr << "No OpenCL devices found." << std::endl;
        return -1;
    }

    AEGpuDevice device = aeGpuGetDefaultDevice(&platform);
    std::cout << "Using OpenCL platform: " << aeGpuGetPlatformName(&platform) << std::endl;

    AEGpuContext gpuContext;
    aeGpuCreateContext(&gpuContext, &devices);

    AEGpuShader shader;
    shader.loadFromFile("simple_add.cl");

    if (shader.build(gpuContext, devices) != 0) {
        std::cerr << "Failed to build shader." << std::endl;
        return -1;
    }
    AEGpuProgram program = shader.getProgram();

    AEGpuVRamBuffer bufferA;
    aeGpuCreateVRamBuffer(&bufferA, gpuContext, 10 * sizeof(int));
    AEGpuVRamBuffer bufferB;
    aeGpuCreateVRamBuffer(&bufferB, gpuContext, 10 * sizeof(int));
    AEGpuVRamBuffer bufferC;
    aeGpuCreateVRamBuffer(&bufferC, gpuContext, 10 * sizeof(int));

    int A[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int B[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    AEGpuCommandQueue commandQueue(gpuContext, device);
    commandQueue.AddBufferToQueue(bufferA, sizeof(A), A);
    commandQueue.AddBufferToQueue(bufferB, sizeof(B), B);

    AEGpuKernel kernel(program, "simple_add");
    kernel.setArg(0, bufferA);
    kernel.setArg(1, bufferB);
    kernel.setArg(2, bufferC);

    // int C[10] = {0};
    i32 *C = (i32 *)aeGpuCreateNonBlockingHostMappedVRamBuffer(gpuContext, commandQueue, sizeof(A));
    commandQueue.AddNDRangeKernelToQueue(kernel, AEGpuRange(10));
    commandQueue.ExecuteQueuedKernels();
    commandQueue.NonBlockingReadBufferFromQueue(bufferC, sizeof(A), C);
    // commandQueue.finish(); // No need to call finish() since we're using a non-blocking read, but we do need to ensure that the command queue is flushed so that the kernel execution and buffer read are submitted to the GPU.
    commandQueue.flush(); // Flush the command queue to ensure that all commands are sent to the GPU.

    printf("Result of A + B:\n");
    for (int i = 0; i < 10; i++) {
        printf("%d ", C[i]);
    }
    printf("\n");

    AEPRect windowRect = aeCreateRect(FUNCTION_AS_POINTER(malloc), 0, 0, 960, 560);

    aeCreateFrameBuffer(&aeFrameBuffer, windowRect);
    aeCreateWindow(&aeWindow, &aeFrameBuffer, aeInstance, L"Anarcho Game Engine", windowRect);

    aeRegisterRawInputDevices(aeWindow);

    aeFillWindow(aeWindow, 0xff00ffff);
    // AEThread aeThread;
    // aeCreateThread(&aeThread, FUNCTION_AS_POINTER(draw));
    AEPThreadTask task = aeAssignLoopedThread(FUNCTION_AS_POINTER(malloc), FUNCTION_AS_POINTER(draw), &windowShouldClose);
    aeStartLoopedThread(task);

    aeShowWindow(aeWindow);

    BOOL success = aeMoveWindow(aeWindow, 0, 0);
    if (!success) return -1;

    aeEventLoopThreaded(&windowShouldClose);

    return 0;
}