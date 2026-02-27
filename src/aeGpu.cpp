#include <aeGpu.hpp>
#include <exception>

AEGpuDevice aeGpuGetDefaultDevice(AEGpuPlatform *platform) {
    AEGpuDevices devices;
    platform->getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if (devices.size() > 0) {
        return devices[0];
    }
    return AEGpuDevice();
}

u0 AEGpuShader::loadFromFile(const char* filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        printf("Failed to open shader file: %s\n", filePath);
        exit(-1);
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    sources.push_back({source.c_str(), source.size()});
}

u32 AEGpuShader::build(AEGpuContext context, AEGpuDevices devices) {
    program = cl::Program(context, sources);
    try {
        program.build(devices);
        wasShaderBuilt = true;
    } catch (const std::exception& err) {
        printf("Error building shader: %s\n", err.what());
        for (const auto& device : devices) {
            printf("Build log for device %s:\n%s\n", device.getInfo<CL_DEVICE_NAME>().c_str(), program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device).c_str());
        }
        return -1;
    }
    return 0;
}