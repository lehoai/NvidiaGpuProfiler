#include <string>
#include "GpuData.h"
#include <vector>

class Nvidia {
    private:
        Gpu* gpu;
        std::string execQuery(const char* query);
        void getGpuData(int gpuIndex);
        void getProcessData(int gpuIndex);
    public:
        Nvidia();
        ~Nvidia();
        Gpu* getData(int gpuIndex = 0);
        std::vector<Gpu> getAllGpuData();
};