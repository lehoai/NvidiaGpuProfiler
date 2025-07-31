#ifndef GpuService_H
#define GpuService_H
#include <string>
#include "Data.h"
#include <vector>

class GpuService {
    private:
        GpuData* gpu;
        std::string execQuery(const char* query);
        void getGpuData(int gpuIndex);
        void getProcessData(int gpuIndex);
    public:
        GpuService();
        ~GpuService();
        GpuData* getData(int gpuIndex = 0);
        std::vector<GpuData> getAllGpuData();
};
#endif // GpuService_H