#include "CpuPanel.h"
#include <fstream>

enum
{
    ID_ADD = wxID_HIGHEST + 1,
    ID_DELETE,
    ID_TASK_LIST,
    ID_TASK_INPUT
};

CpuPanel::CpuPanel(wxWindow *parent)
    : wxPanel(parent)
{
    cpuService = new CpuService();
    CreateControls();
    GetCpuData();
    timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &CpuPanel::OnCpuTimer, this, timer->GetId());
    timer->Start(500);
    // get cpu name
    lblCpuName->SetLabel(cpuService->GetCpuName());
}

void CpuPanel::CreateControls()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // label gpu name
    wxBoxSizer* line1Sizer = new wxBoxSizer(wxHORIZONTAL);

    lblCpuName = new wxStaticText(this, ID_TASK_INPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize);
    lblCpuName->SetFont(wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    lblCpuName->SetLabel("CPU Name: Intel Core i7-9700K");

    line1Sizer->Add(lblCpuName, 1, wxEXPAND | wxALL, 0);

    // chart
    wxBoxSizer* lineChartsSizer = new wxBoxSizer(wxHORIZONTAL);

    // =========================== memory chart ===========================
    wxBoxSizer* lineMemorySizer = new wxBoxSizer(wxVERTICAL);

    lblMemoryUsage = new wxStaticText(this, ID_TASK_INPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize);
    lblMemoryUsage->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    memoryChart = new TimelineChart(this, ID_TASK_INPUT, wxDefaultPosition, wxDefaultSize);

    lineMemorySizer->Add(lblMemoryUsage, 0, wxEXPAND | wxBOTTOM, 5);
    lineMemorySizer->Add(memoryChart, 1, wxEXPAND | wxALL, 0);

    // =========================== utilization chart ===========================
    wxBoxSizer* lineUtilizationSizer = new wxBoxSizer(wxVERTICAL);

    lblCpuUsage = new wxStaticText(this, ID_TASK_INPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize);
    lblCpuUsage->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    utilizationChart = new TimelineChart(this, ID_TASK_INPUT, wxDefaultPosition, wxDefaultSize);
    utilizationChart->maxVal = 100;

    lineUtilizationSizer->Add(lblCpuUsage, 0, wxEXPAND | wxBOTTOM, 5);
    lineUtilizationSizer->Add(utilizationChart, 1, wxEXPAND | wxALL, 0);

    lineChartsSizer->Add(lineMemorySizer, 1, wxEXPAND | wxRIGHT, 5);
    lineChartsSizer->Add(lineUtilizationSizer,1, wxEXPAND | wxALL, 0);

    // =========================== process list ===========================
    processList = new wxListView(this, ID_TASK_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);

    processList->SetBackgroundStyle(wxBG_STYLE_PAINT);
    processList->AppendColumn("PID", wxLIST_FORMAT_LEFT, 60);
    processList->AppendColumn("Process Name", wxLIST_FORMAT_LEFT, 300);
    processList->AppendColumn("CPU(%)", wxLIST_FORMAT_LEFT, 80);
    processList->AppendColumn("Memory", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);

    mainSizer->Add(line1Sizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(lineChartsSizer, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(processList, 2, wxEXPAND | wxALL, 5);

    this->SetSizer(mainSizer);
}

void CpuPanel::OnCpuTimer(wxTimerEvent &)
{
    GetCpuData();
}

void CpuPanel::GetCpuData()
{
    auto cpuData = cpuService->GetCpuData();

    lblCpuUsage->SetLabel(wxString::Format("CPU Usage: %d%%", cpuData->cpuUsage));
    lblMemoryUsage->SetLabel(wxString::Format("Memory: %d MiB / %d MiB", cpuData->memoryUsage, cpuData->memoryTotal));

    utilizationChart->AddDataPoint(cpuData->cpuUsage);

    memoryChart->maxVal = cpuData->memoryTotal;
    memoryChart->AddDataPoint(cpuData->memoryUsage);

    static std::vector<CpuProcess> lastProcesses;
    auto& newProcs = cpuData->processes;
    int oldCount = processList->GetItemCount();
    int newCount = newProcs.size();

    processList->Freeze();
    // Update or insert
    for (int i = 0; i < newCount; ++i) {
        const auto& proc = newProcs[i];
        if (i < oldCount) {
            // if PID different, update all cells
            if (processList->GetItemText(i) != wxString::Format("%d", proc.pid)) {
                processList->SetItem(i, 0, wxString::Format("%d", proc.pid));
                processList->SetItem(i, 1, proc.name);
                processList->SetItem(i, 2, wxString::Format("%d%%", proc.cpu_usage));
                processList->SetItem(i, 3, wxString::Format("%d MB", proc.memory_usage));
            } else {
                // if PID same, update different cells
                if (processList->GetItemText(i, 1) != proc.name)
                    processList->SetItem(i, 1, proc.name);
                if (processList->GetItemText(i, 2) != wxString::Format("%d%%", proc.cpu_usage))
                    processList->SetItem(i, 2, wxString::Format("%d%%", proc.cpu_usage));
                if (processList->GetItemText(i, 3) != wxString::Format("%d", proc.memory_usage))
                    processList->SetItem(i, 3, wxString::Format("%d MB", proc.memory_usage));
            }
        } else {
            // add new row
            long idx = processList->InsertItem(i, wxString::Format("%d", proc.pid));
            processList->SetItem(idx, 1, proc.name);
            processList->SetItem(idx, 2, wxString::Format("%d%%", proc.cpu_usage));
            processList->SetItem(idx, 3, wxString::Format("%d MB", proc.memory_usage));
        }
    }
    // delete extra rows if any
    while (processList->GetItemCount() > newCount) {
        processList->DeleteItem(processList->GetItemCount() - 1);
    }
    processList->Thaw();
    lastProcesses = newProcs;
}

CpuPanel::~CpuPanel()
{
    timer->Stop();
    delete timer;
    delete cpuService;
}