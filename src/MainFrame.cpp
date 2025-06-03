#include "MainFrame.h"

enum
{
    ID_ADD = wxID_HIGHEST + 1,
    ID_DELETE,
    ID_TASK_LIST,
    ID_TASK_INPUT
};

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
END_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
    nvidia = new Nvidia();
    timer = new wxTimer(this, wxID_ANY);
    Bind(wxEVT_TIMER, &MainFrame::OnGpuTimer, this);

    CreateControls();

    // Create status bar
    CreateStatusBar();
    SetStatusText("Connected to GPU");

    Centre();
    GetGpuData();

    timer->Start(500);
}

void MainFrame::CreateControls()
{
    mainPanel = new wxPanel(this);
    // Sizer root
    wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);

    // Sidebar GPU
    gpuListBox = new wxListBox(mainPanel, wxID_ANY);
    // Lấy danh sách nhiều GPU và binding lên list
    gpus = nvidia->getAllGpuData();
    gpuListBox->Clear();
    for (const auto& gpu : gpus) {
        gpuListBox->Append(wxString::Format("GPU %d: %s", gpu.index, gpu.name));
    }

    // Right panel (GPU info)
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* line1Sizer = new wxBoxSizer(wxHORIZONTAL);

    // label gpu name
    lblGpuName = new wxStaticText(mainPanel, ID_TASK_INPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize);
    lblGpuName->SetFont(wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    // label gpu power usage
    lblGpuPowerUsage = new wxStaticText(mainPanel, ID_TASK_INPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    lblGpuPowerUsage->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    line1Sizer->Add(lblGpuName, 1, wxEXPAND | wxALL, 0);
    line1Sizer->Add(lblGpuPowerUsage, 1, wxEXPAND | wxALL, 0);

    // =========================== chart area ===========================
    wxBoxSizer* lineChartsSizer = new wxBoxSizer(wxHORIZONTAL);

    // =========================== memory chart ===========================
    wxBoxSizer* lineMemorySizer = new wxBoxSizer(wxVERTICAL);

    lblGpuMemoryUsage = new wxStaticText(mainPanel, ID_TASK_INPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize);
    lblGpuMemoryUsage->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    memoryChart = new TimelineChart(mainPanel, ID_TASK_INPUT, wxDefaultPosition, wxDefaultSize);

    lineMemorySizer->Add(lblGpuMemoryUsage, 0, wxEXPAND | wxBOTTOM, 5);
    lineMemorySizer->Add(memoryChart, 1, wxEXPAND | wxALL, 0);

    // =========================== utilization chart ===========================
    wxBoxSizer* lineUtilizationSizer = new wxBoxSizer(wxVERTICAL);

    lblGpuUtilization = new wxStaticText(mainPanel, ID_TASK_INPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize);
    lblGpuUtilization->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    utilizationChart = new TimelineChart(mainPanel, ID_TASK_INPUT, wxDefaultPosition, wxDefaultSize);
    utilizationChart->maxVal = 100;

    lineUtilizationSizer->Add(lblGpuUtilization, 0, wxEXPAND | wxBOTTOM, 5);
    lineUtilizationSizer->Add(utilizationChart, 1, wxEXPAND | wxALL, 0);

    lineChartsSizer->Add(lineMemorySizer, 1, wxEXPAND | wxRIGHT, 5);
    lineChartsSizer->Add(lineUtilizationSizer,1, wxEXPAND | wxALL, 0);

    // =========================== Temperature chart ===========================
    lblGpuTemperature = new wxStaticText(mainPanel, ID_TASK_INPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize);
    lblGpuTemperature->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    temperatureChart = new TimelineChart(mainPanel, ID_TASK_INPUT, wxDefaultPosition, wxDefaultSize);
    temperatureChart->maxVal = 100;

    // =========================== process list ===========================
    processList = new wxListView(mainPanel, ID_TASK_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);

    processList->SetBackgroundStyle(wxBG_STYLE_PAINT);
    processList->AppendColumn("PID", wxLIST_FORMAT_LEFT, 60);
    processList->AppendColumn("Process", wxLIST_FORMAT_LEFT, 300);
    processList->AppendColumn("Memory Usage", wxLIST_FORMAT_LEFT, 100);
    processList->AppendColumn("Type", wxLIST_FORMAT_LEFT, 80);

    // =========================== add all sizers to mainSizer ===========================
    mainSizer->Add(line1Sizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(lineChartsSizer, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(lblGpuTemperature, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(temperatureChart, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(processList, 1, wxEXPAND | wxALL, 5);

    // Add sidebar and right panel to rootSizer
    rootSizer->Add(gpuListBox, 1, wxEXPAND | wxALL, 5);
    rootSizer->Add(mainSizer, 4, wxEXPAND | wxALL, 0);
    mainPanel->SetSizer(rootSizer);
}

void MainFrame::OnExit(wxCommandEvent& /*event*/)
{
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& /*event*/)
{
    wxMessageBox("A simple GPU profiler application\nBuilt with wxWidgets\nAuthor: Le Hoai",
                "About App", wxOK | wxICON_INFORMATION);
}

void MainFrame::GetGpuData()
{
    Gpu* gpu = nvidia->getData();
    lblGpuName->SetLabel(wxString::Format("%s", gpu->name.c_str()));
    lblGpuPowerUsage->SetLabel(wxString::Format("Power usage / capacity: %dW / %dW", gpu->power_usage, gpu->power_capacity));

    lblGpuMemoryUsage->SetLabel(wxString::Format("Memory usage: %dMib / %dMib", gpu->memory_usage, gpu->memory_total));
    memoryChart->maxVal = gpu->memory_total;
    memoryChart->AddDataPoint(gpu->memory_usage);

    lblGpuUtilization->SetLabel(wxString::Format("Utilization: %d%%", gpu->utilization));
    utilizationChart->AddDataPoint(gpu->utilization);

    lblGpuTemperature->SetLabel(wxString::Format("Temperature: %dC", gpu->temperature));
    temperatureChart->AddDataPoint(gpu->temperature);

    static std::vector<Process> lastProcesses;
    auto& newProcs = gpu->processes;
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
                processList->SetItem(i, 1, proc.path);
                processList->SetItem(i, 2, wxString::Format("%d", proc.gpu_usage));
                processList->SetItem(i, 3, proc.type);
            } else {
                // if PID same, update different cells
                if (processList->GetItemText(i, 1) != proc.path)
                    processList->SetItem(i, 1, proc.path);
                if (processList->GetItemText(i, 2) != wxString::Format("%d", proc.gpu_usage))
                    processList->SetItem(i, 2, wxString::Format("%d", proc.gpu_usage));
                if (processList->GetItemText(i, 3) != proc.type)
                    processList->SetItem(i, 3, proc.type);
            }
        } else {
            // add new row
            long idx = processList->InsertItem(i, wxString::Format("%d", proc.pid));
            processList->SetItem(idx, 1, proc.path);
            processList->SetItem(idx, 2, wxString::Format("%d", proc.gpu_usage));
            processList->SetItem(idx, 3, proc.type);
        }
    }
    // delete extra rows if any
    while (processList->GetItemCount() > newCount) {
        processList->DeleteItem(processList->GetItemCount() - 1);
    }
    processList->Thaw();
    lastProcesses = newProcs;
}

void MainFrame::OnGpuTimer(wxTimerEvent& /*event*/)
{
    GetGpuData();
}

void MainFrame::OnGpuSelected(wxCommandEvent& /*event*/)
{
    int sel = gpuListBox->GetSelection();
    if (sel == wxNOT_FOUND || sel >= (int)gpus.size()) return;
    const Gpu& gpu = gpus[sel];
    lblGpuName->SetLabel(wxString::Format("%s", gpu.name.c_str()));
    lblGpuPowerUsage->SetLabel(wxString::Format("Power usage / capacity: %dW / %dW", gpu.power_usage, gpu.power_capacity));
    lblGpuMemoryUsage->SetLabel(wxString::Format("Memory usage: %dMib / %dMib", gpu.memory_usage, gpu.memory_total));
    memoryChart->maxVal = gpu.memory_total;
    memoryChart->Clear();
    memoryChart->AddDataPoint(gpu.memory_usage);
    lblGpuUtilization->SetLabel(wxString::Format("Utilization: %d%%", gpu.utilization));
    utilizationChart->Clear();
    utilizationChart->AddDataPoint(gpu.utilization);
    lblGpuTemperature->SetLabel(wxString::Format("Temperature: %dC", gpu.temperature));
    temperatureChart->Clear();
    temperatureChart->AddDataPoint(gpu.temperature);
    // Cập nhật process list
    processList->Freeze();
    processList->DeleteAllItems();
    for (size_t i = 0; i < gpu.processes.size(); ++i) {
        const auto& proc = gpu.processes[i];
        long idx = processList->InsertItem(i, wxString::Format("%d", proc.pid));
        processList->SetItem(idx, 1, proc.path);
        processList->SetItem(idx, 2, wxString::Format("%d", proc.gpu_usage));
        processList->SetItem(idx, 3, proc.type);
    }
    processList->Thaw();
}

