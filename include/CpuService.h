#include <string>
#include "Data.h"

class CpuService
{
    public:
        CpuService();
        ~CpuService();
        
        std::string GetCpuName() const;
        CpuData* GetCpuData() const;
    private:
        CpuData* cpuData;
        mutable long lastTotal = 0;
        mutable long lastIdle = 0;
        mutable bool firstRead = true;
};