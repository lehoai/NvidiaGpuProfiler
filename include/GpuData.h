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

struct Gpu
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
    bool operator==(const Gpu& other) const {
        return index == other.index && name == other.name && utilization == other.utilization &&
               memory_usage == other.memory_usage && memory_total == other.memory_total &&
               temperature == other.temperature && power_usage == other.power_usage &&
               power_capacity == other.power_capacity && processes == other.processes;
    }
};