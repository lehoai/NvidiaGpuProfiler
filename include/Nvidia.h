#include <string>
#include "GpuData.h"
#include <vector>

class Nvidia {
    private:
        Gpu* gpu;
        std::string execQuery(const char* query);
        void getGpuData();
        void getProcessData();
    public:
        Nvidia();
        ~Nvidia();
        Gpu* getData();
        std::vector<Gpu> getAllGpuData();
};