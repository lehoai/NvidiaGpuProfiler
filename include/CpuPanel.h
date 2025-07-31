#include <wx/wx.h>
#include <wx/listctrl.h>
#include "TimelineChart.h"
#include "CpuService.h"

class CpuPanel : public wxPanel
{
public:
    CpuPanel(wxWindow *parent);
    ~CpuPanel();

private:
    void CreateControls();
    void OnCpuTimer(wxTimerEvent &event);
    void GetCpuData();
    wxStaticText *lblCpuName;
    wxStaticText *lblCpuUsage;
    wxStaticText *lblMemoryUsage;
    TimelineChart *memoryChart;
    TimelineChart *utilizationChart;
    wxListView *processList;

    wxTimer *timer;
    CpuService* cpuService;
};