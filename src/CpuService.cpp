#include "CpuService.h"
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <unistd.h>
#include <map>
static std::map<int, long> last_proc_time;
static long last_total_cpu = 0;

CpuService::CpuService()
{
    cpuData = new CpuData();
    // get CPU name
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line))
    {
        if (line.find("model name") != std::string::npos)
        {
            this->cpuData->cpuName = line.substr(line.find(":") + 2);
            break;
        }
    }
}

CpuData* CpuService::GetCpuData() const
{
    // Read CPU usage
    std::ifstream statFile("/proc/stat");
    if (statFile.is_open())
    {
        std::string cpuLine;
        std::getline(statFile, cpuLine);
        std::istringstream ss(cpuLine);
        std::string cpu;
        long user, nice, system, idle, iowait, irq, softirq, steal;
        ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
        long idleAll = idle + iowait;
        long nonIdle = user + nice + system + irq + softirq + steal;
        long total = idleAll + nonIdle;

        if (firstRead) {
            lastTotal = total;
            lastIdle = idleAll;
            cpuData->cpuUsage = 0;
            firstRead = false;
        } else {
            long deltaTotal = total - lastTotal;
            long deltaIdle = idleAll - lastIdle;
            cpuData->cpuUsage = (deltaTotal - deltaIdle) * 100 / deltaTotal;
            lastTotal = total;
            lastIdle = idleAll;
        }
    }

    // Read memory usage
    std::ifstream meminfoFile("/proc/meminfo");
    if (meminfoFile.is_open())
    {
        std::string memLine;
        while (std::getline(meminfoFile, memLine))
        {
            if (memLine.find("MemTotal:") != std::string::npos)
            {
                cpuData->memoryTotal = std::stoi(memLine.substr(memLine.find(":") + 1)) / 1024; // Convert to MiB
            }
            else if (memLine.find("MemAvailable:") != std::string::npos)
            {
                auto available = std::stoi(memLine.substr(memLine.find(":") + 1)) / 1024; // Convert to MiB
                cpuData->memoryUsage = cpuData->memoryTotal - available;
            }
        }
    }

    // get list of processes using CPU with memory usage and %cpu
    static std::map<int, long> last_proc_time; // pid -> last utime+stime
    static long last_total_cpu = 0;
    cpuData->processes.clear();
    uid_t current_uid = getuid();
    // Lấy tổng CPU time hiện tại (đã đọc ở trên)
    long total_cpu_now = lastTotal;
    long delta_total_cpu = total_cpu_now - last_total_cpu;
    for (const auto& entry : std::filesystem::directory_iterator("/proc"))
    {
        if (entry.is_directory()) {
            std::string pid = entry.path().filename().string();
            if (std::all_of(pid.begin(), pid.end(), ::isdigit)) {
                std::ifstream statusFile(entry.path() / "status");
                if (statusFile.is_open()) {
                    // Check UID of process
                    std::string line;
                    uid_t proc_uid = -1;
                    while (std::getline(statusFile, line)) {
                        if (line.rfind("Uid:", 0) == 0) {
                            std::istringstream iss(line.substr(4));
                            iss >> proc_uid;
                            break;
                        }
                    }
                    if (proc_uid == current_uid) {
                        // Reset file to read again for VmRSS
                        statusFile.clear();
                        statusFile.seekg(0);
                        CpuProcess proc;
                        proc.pid = std::stoi(pid);
                        // Lấy process name từ /proc/[pid]/comm
                        std::ifstream commFile(entry.path() / "comm");
                        if (commFile.is_open()) {
                            std::getline(commFile, proc.name);
                        } else {
                            proc.name = pid;
                        }
                        proc.memory_usage = 0;
                        while (std::getline(statusFile, line)) {
                            if (line.find("VmRSS:") != std::string::npos) {
                                auto memUsage = std::stoi(line.substr(line.find(":") + 1)) / 1024;
                                proc.memory_usage = memUsage;
                                break;
                            }
                        }
                        // Lấy utime+stime từ /proc/[pid]/stat
                        long utime = 0, stime = 0;
                        std::ifstream statFile(entry.path() / "stat");
                        if (statFile.is_open()) {
                            std::string statLine;
                            std::getline(statFile, statLine);
                            std::istringstream iss(statLine);
                            std::string ignore;
                            for (int i = 0; i < 13; ++i) iss >> ignore; // skip to 14th field
                            iss >> utime >> stime;
                        }
                        long proc_time_now = utime + stime;
                        long delta_proc = 0;
                        if (last_proc_time.count(proc.pid)) {
                            delta_proc = proc_time_now - last_proc_time[proc.pid];
                        }
                        // Tính %cpu cho process
                        float percent = 0.0f;
                        if (delta_total_cpu > 0 && delta_proc > 0) {
                            percent = 100.0f * delta_proc / delta_total_cpu;
                        }
                        proc.cpu_usage = percent;
                        last_proc_time[proc.pid] = proc_time_now;
                        cpuData->processes.push_back(proc);
                    }
                }
            }
        }
    }
    last_total_cpu = total_cpu_now;

    return cpuData;
}

std::string CpuService::GetCpuName() const
{
    return this->cpuData->cpuName;
}

CpuService::~CpuService()
{
    delete cpuData;
}