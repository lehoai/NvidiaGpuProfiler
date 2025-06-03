#include "Nvidia.h"
#include <array>
#include <memory>
#include <stdexcept>
#include <cstdio>
#include <vector>
#include <sstream>
#include <iostream>
#include <nvml.h>
#include <set>
#include <fstream>
#include <map>

// get full command line (full path) from PID
std::string getProcessCmdlineFromPid(int pid) {
    std::string path = "/proc/" + std::to_string(pid) + "/cmdline";
    std::ifstream ifs(path);
    std::string cmdline;
    if (ifs.is_open()) {
        std::getline(ifs, cmdline, '\0');
        ifs.close();
    }
    return cmdline;
}

Nvidia::Nvidia()
{
    gpu = new Gpu();
    nvmlInit();
}

Nvidia::~Nvidia()
{
    nvmlShutdown();
    delete gpu;
}

void Nvidia::getGpuData()
{
    unsigned int device_count = 0;
    nvmlDeviceGetCount(&device_count);
    if (device_count == 0) return;
    nvmlDevice_t device;
    nvmlDeviceGetHandleByIndex(0, &device); // Get first GPU

    char name[128];
    nvmlDeviceGetName(device, name, sizeof(name));
    gpu->name = name;
    gpu->index = 0;

    nvmlUtilization_t utilization;
    nvmlDeviceGetUtilizationRates(device, &utilization);
    gpu->utilization = utilization.gpu;

    nvmlMemory_t mem_info;
    nvmlDeviceGetMemoryInfo(device, &mem_info);
    gpu->memory_usage = mem_info.used / 1024 / 1024;
    gpu->memory_total = mem_info.total / 1024 / 1024;

    unsigned int temp;
    nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
    gpu->temperature = temp;

    unsigned int power, power_limit;
    nvmlDeviceGetPowerUsage(device, &power);
    nvmlDeviceGetEnforcedPowerLimit(device, &power_limit);
    gpu->power_usage = power / 1000; // W
    gpu->power_capacity = power_limit / 1000; // W
}

void Nvidia::getProcessData()
{
    unsigned int device_count = 0;
    nvmlDeviceGetCount(&device_count);
    if (device_count == 0) return;
    nvmlDevice_t device;
    nvmlDeviceGetHandleByIndex(0, &device); // Get first GPU

    gpu->processes.clear();
    std::map<int, Process> pidMap;

    // process compute (CUDA)
    unsigned int infoCount = 100;
    nvmlProcessInfo_t infos[100];
    nvmlReturn_t result = nvmlDeviceGetComputeRunningProcesses(device, &infoCount, infos);
    if (result == NVML_SUCCESS) {
        for (unsigned int j = 0; j < infoCount; ++j) {
            int pid = infos[j].pid;
            Process process;
            process.pid = pid;
            process.path = getProcessCmdlineFromPid(pid);
            process.gpu_usage = infos[j].usedGpuMemory / 1024 / 1024; // MiB
            process.type = "compute";
            pidMap[pid] = process;
        }
    }

    // process graphics (OpenGL, Vulkan, ...)
    infoCount = 100;
    nvmlProcessInfo_t graphicsInfos[100];
    result = nvmlDeviceGetGraphicsRunningProcesses(device, &infoCount, graphicsInfos);
    if (result == NVML_SUCCESS) {
        for (unsigned int j = 0; j < infoCount; ++j) {
            int pid = graphicsInfos[j].pid;
            auto it = pidMap.find(pid);
            if (it != pidMap.end()) {
                it->second.type = "both";
            } else {
                Process process;
                process.pid = pid;
                process.path = getProcessCmdlineFromPid(pid);
                process.gpu_usage = graphicsInfos[j].usedGpuMemory / 1024 / 1024; // MiB
                process.type = "graphics";
                pidMap[pid] = process;
            }
        }
    }
    // Đưa vào vector
    for (const auto& kv : pidMap) {
        gpu->processes.push_back(kv.second);
    }
}

Gpu* Nvidia::getData()
{
    getGpuData();
    getProcessData();
    return gpu;
}

std::vector<Gpu> Nvidia::getAllGpuData() {
    std::vector<Gpu> result;
    unsigned int device_count = 0;
    nvmlDeviceGetCount(&device_count);
    for (unsigned int i = 0; i < device_count; ++i) {
        nvmlDevice_t device;
        nvmlDeviceGetHandleByIndex(i, &device);
        Gpu gpu;
        char name[128];
        nvmlDeviceGetName(device, name, sizeof(name));
        gpu.name = name;
        gpu.index = i;
        nvmlUtilization_t utilization;
        nvmlDeviceGetUtilizationRates(device, &utilization);
        gpu.utilization = utilization.gpu;
        nvmlMemory_t mem_info;
        nvmlDeviceGetMemoryInfo(device, &mem_info);
        gpu.memory_usage = mem_info.used / 1024 / 1024;
        gpu.memory_total = mem_info.total / 1024 / 1024;
        unsigned int temp;
        nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
        gpu.temperature = temp;
        unsigned int power, power_limit;
        nvmlDeviceGetPowerUsage(device, &power);
        nvmlDeviceGetEnforcedPowerLimit(device, &power_limit);
        gpu.power_usage = power / 1000;
        gpu.power_capacity = power_limit / 1000;
        // Process info
        std::map<int, Process> pidMap;
        unsigned int infoCount = 100;
        nvmlProcessInfo_t infos[100];
        nvmlReturn_t result_nvml = nvmlDeviceGetComputeRunningProcesses(device, &infoCount, infos);
        if (result_nvml == NVML_SUCCESS) {
            for (unsigned int j = 0; j < infoCount; ++j) {
                int pid = infos[j].pid;
                Process process;
                process.pid = pid;
                process.path = getProcessCmdlineFromPid(pid);
                process.gpu_usage = infos[j].usedGpuMemory / 1024 / 1024;
                process.type = "compute";
                pidMap[pid] = process;
            }
        }
        infoCount = 100;
        nvmlProcessInfo_t graphicsInfos[100];
        result_nvml = nvmlDeviceGetGraphicsRunningProcesses(device, &infoCount, graphicsInfos);
        if (result_nvml == NVML_SUCCESS) {
            for (unsigned int j = 0; j < infoCount; ++j) {
                int pid = graphicsInfos[j].pid;
                auto it = pidMap.find(pid);
                if (it != pidMap.end()) {
                    it->second.type = "both";
                } else {
                    Process process;
                    process.pid = pid;
                    process.path = getProcessCmdlineFromPid(pid);
                    process.gpu_usage = graphicsInfos[j].usedGpuMemory / 1024 / 1024;
                    process.type = "graphics";
                    pidMap[pid] = process;
                }
            }
        }
        for (const auto& kv : pidMap) {
            gpu.processes.push_back(kv.second);
        }
        result.push_back(gpu);
    }
    return result;
}