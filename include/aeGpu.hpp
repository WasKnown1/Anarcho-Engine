#ifndef anarcho_gpu_hpp
#define anarcho_gpu_hpp

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.hpp>
#include <CL/opencl.h>

#include <aeTypes.hpp>
#include <vector>
#include <fstream>

using AEGpuPlatforms = std::vector<cl::Platform>;
using AEGpuPlatform = cl::Platform;
using AEGpuDevices = std::vector<cl::Device>;
using AEGpuDevice = cl::Device;
using AEGpuContext = cl::Context;
using AEGpuVRamBuffer = cl::Buffer;
using AEGpuProgram = cl::Program;
#define AEGpuRange cl::NDRange

class AEGpuShader {
    private:
        cl::Program::Sources sources;
        cl::Program program;
        b8 wasShaderBuilt = false;

    public:
    AEGpuShader& operator=(const char* source) {
        sources.push_back({source, strlen(source)});
        return *this;
    }

    u0 loadFromFile(const char *filePath);

    u32 build(AEGpuContext context, AEGpuDevices device);

    AEGpuProgram getProgram() {
        if (!wasShaderBuilt) {
            printf("Shader has not been built yet. Call build() before getProgram().\n");
            exit(-1);
            return cl::Program();
        }
        return program;
    }
};

class AEGpuKernel {
    private:
        cl::Kernel kernel;

    public:
        AEGpuKernel(AEGpuProgram program, const char* kernelName) {
            kernel = cl::Kernel(program, kernelName);
        }

        template<typename T>
        u0 setArg(u32 index, T value) {
            kernel.setArg(index, value);
        }

        cl::Kernel getKernel() {
            return kernel;
        }
};

class AEGpuCommandQueue {
    private:
        cl::CommandQueue commandQueue;
        b8 wasCommandQueueFinished = false;
        b8 wereKernelsExecuted = false;
        cl_int err;
        cl_event event;

    public:
        AEGpuCommandQueue(AEGpuContext context, AEGpuDevice device) {
            commandQueue = cl::CommandQueue(context, device);
            event = clCreateUserEvent(context(), &err);
        }

        ~AEGpuCommandQueue() {
            if (wasCommandQueueFinished)
                commandQueue.finish();
            clReleaseEvent(event);
        }

        cl::CommandQueue getCommandQueue() {
            return commandQueue;
        }

        u0 AddBufferToQueue(AEGpuVRamBuffer buffer, u32 size, u0* data) {
            commandQueue.enqueueWriteBuffer(buffer, CL_TRUE, 0, size, data);
        }

        u0 AddNDRangeKernelToQueue(AEGpuKernel kernel, cl::NDRange globalSize) {
            clEnqueueNDRangeKernel(commandQueue(), kernel.getKernel()(), 1, NULL, globalSize, NULL, 1, &event, NULL);
        }

        u0 ExecuteQueuedKernels() {
            clSetUserEventStatus(event, CL_COMPLETE);
            wereKernelsExecuted = true;
        }

        u0 BlockingReadBufferFromQueue(AEGpuVRamBuffer buffer, u32 size, u0* data) {
            if (!wereKernelsExecuted) {
                printf("Kernels have not been executed yet. Call ExecuteQueuedKernels() before ReadBufferFromQueue().\n");
                exit(-1);
            }
            commandQueue.enqueueReadBuffer(buffer, CL_TRUE, 0, size, data);
        }

        u0 NonBlockingReadBufferFromQueue(AEGpuVRamBuffer buffer, u32 size, u0* data) {
            commandQueue.enqueueReadBuffer(buffer, CL_FALSE, 0, size, data);
        }

        u0 finish() {
            commandQueue.finish();
            wasCommandQueueFinished = true;
        }

        u0 flush() {
            commandQueue.flush();
        }
};

inline u32 aeGpuGetPlatforms(AEGpuPlatforms *gpuPlatforms) {
    cl::Platform::get(gpuPlatforms);
    return gpuPlatforms->size();
}

inline AEGpuPlatform aeGpuGetDefaultPlatform() {
    return cl::Platform::getDefault();
}

inline const char *aeGpuGetPlatformName(AEGpuPlatform *platform) {
    return platform->getInfo<CL_PLATFORM_NAME>().c_str();
}

inline u0 aeGpuGetDevices(AEGpuPlatform *platform, AEGpuDevices *gpuDevices) {
    platform->getDevices(CL_DEVICE_TYPE_ALL, gpuDevices);
}

AEGpuDevice aeGpuGetDefaultDevice(AEGpuPlatform *platform);

inline const char *aeGpuGetDeviceName(AEGpuDevice *device) {
    return device->getInfo<CL_DEVICE_NAME>().c_str();
}

inline u0 aeGpuCreateContext(AEGpuContext *context, AEGpuDevices *devices) {
    cl::Context gpuContext(*devices);
    *context = gpuContext;
}

inline u0 aeGpuCreateVRamBuffer(AEGpuVRamBuffer *buffer, AEGpuContext context, u32 size) {
    cl::Buffer gpuBuffer(context, CL_MEM_READ_WRITE, size);
    *buffer = gpuBuffer;
}

inline u0 *aeGpuCreateNonBlockingHostMappedVRamBuffer(AEGpuContext context, AEGpuCommandQueue commandQueue, u32 size) {
    cl_mem hostPtr = clCreateBuffer(context(), CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, size, NULL, NULL);
    return clEnqueueMapBuffer(commandQueue.getCommandQueue()(), hostPtr, CL_FALSE, CL_MAP_READ | CL_MAP_WRITE, 0, size, 0, NULL, NULL, NULL);
}

#endif // anarcho_gpu_hpp