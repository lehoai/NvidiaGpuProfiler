#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "TimelineChart.h"
#include "GpuPanel.h"
#include "CpuPanel.h"

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title);

private:
    void CreateControls();

    // Event handlers
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnGpuSelected(wxCommandEvent& event);

    // UI Controls
    wxPanel* mainPanel;
    GpuPanel* gpuPanel;
    CpuPanel* cpuPanel;
    wxListBox* gpuListBox;

    DECLARE_EVENT_TABLE()
}; 