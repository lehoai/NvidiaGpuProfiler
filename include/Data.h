#ifndef DATA_H
#define DATA_H
#include <vector>
#include <string>

struct Process
{
    int pid;
    std::string path;
    int gpu_usage;
    std::string type; // "compute", "graphics", hoặc "both"
    bool operator==(const Process& other) const {
        return pid == other.pid && path == other.path && gpu_usage == other.gpu_usage && type == other.type;
    }
};

struct CpuProcess
{
    int pid;
    std::string name;
    int cpu_usage; // in percentage
    int memory_usage; // in MiB
    bool operator==(const CpuProcess& other) const {
        return pid == other.pid && name == other.name && cpu_usage == other.cpu_usage && memory_usage == other.memory_usage;
    }
};

struct GpuData
{
    int index;
    std::string name;
    int utilization;
    int memory_usage;
    int memory_total;
    int temperature;
    int power_usage;
    int power_capacity;
    std::vector<Process> processes;
    bool operator==(const GpuData& other) const {
        return index == other.index && name == other.name && utilization == other.utilization &&
               memory_usage == other.memory_usage && memory_total == other.memory_total &&
               temperature == other.temperature && power_usage == other.power_usage &&
               power_capacity == other.power_capacity && processes == other.processes;
    }
};

struct CpuData
{
    std::string cpuName;
    int cpuUsage; // in percentage
    int memoryUsage; // in MiB
    int memoryTotal; // in MiB
    std::vector<CpuProcess> processes; // list of processes using CPU
};

#endif // DATA_H