#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "Nvidia.h"
#include "TimelineChart.h"

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title);

private:
    void CreateControls();

    // Event handlers
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnGpuTimer(wxTimerEvent& event);
    void GetGpuData();
    void OnGpuSelected(wxCommandEvent& event);

    // UI Controls
    wxPanel* mainPanel;
    wxListView* processList;
    wxStaticText* lblGpuName;
    wxStaticText* lblGpuUtilization;
    wxStaticText* lblGpuMemoryUsage;
    wxStaticText* lblGpuTemperature;
    wxStaticText* lblGpuPowerUsage;
    wxListBox* gpuListBox;
    std::vector<Gpu> gpus;
    int selectedGpuIndex;

    TimelineChart* memoryChart;
    TimelineChart* utilizationChart;
    TimelineChart* temperatureChart;

    // Timer
    wxTimer* timer;
    Nvidia* nvidia;

    DECLARE_EVENT_TABLE()
}; 