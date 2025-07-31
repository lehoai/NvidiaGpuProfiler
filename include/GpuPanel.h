
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "GpuService.h"
#include "TimelineChart.h"

class GpuPanel : public wxPanel
{
public:
    GpuPanel(wxWindow *parent);
    ~GpuPanel();
    void changeSelectedGpu(int sel);
    std::vector<GpuData> getAllGpuData();

private:
    void CreateControls();
    void OnGpuTimer(wxTimerEvent &event);
    void GetGpuData();

    wxListView *processList;
    wxStaticText *lblGpuName;
    wxStaticText *lblGpuUtilization;
    wxStaticText *lblGpuMemoryUsage;
    wxStaticText *lblGpuTemperature;
    wxStaticText *lblGpuPowerUsage;
    TimelineChart *memoryChart;
    TimelineChart *utilizationChart;
    TimelineChart *temperatureChart;

    std::vector<GpuData> gpus;
    int selectedGpuIndex;
    // Timer
    wxTimer *timer;
    GpuService *nvidia;
};