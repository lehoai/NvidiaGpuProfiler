#include "MainFrame.h"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
END_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
    CreateControls();

    // Bind GPU selection event
    gpuListBox->Bind(wxEVT_LISTBOX, &MainFrame::OnGpuSelected, this);

    // Create status bar
    CreateStatusBar();
    SetStatusText("Connected to GPU");

    Centre();
}

void MainFrame::CreateControls()
{
    mainPanel = new wxPanel(this);
    // Sizer root
    wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);

    // gpu panel
    gpuPanel = new GpuPanel(mainPanel);

    // cpu panel
    cpuPanel = new CpuPanel(mainPanel);

    // Sidebar GPU
    gpuListBox = new wxListBox(mainPanel, wxID_ANY);
    // Populate GPU list
    auto gpus = gpuPanel->getAllGpuData();
    gpuListBox->Clear();
    for (const auto& gpu : gpus) {
        gpuListBox->Append(wxString::Format("GPU %d: %s", gpu.index, gpu.name));
    }
    gpuListBox->Append("CPU & Memory");

    // Add sidebar and right panel to rootSizer
    rootSizer->Add(gpuListBox, 1, wxEXPAND | wxALL, 5);
    rootSizer->Add(cpuPanel, 4, wxEXPAND | wxALL, 0);
    rootSizer->Add(gpuPanel, 4, wxEXPAND | wxALL, 0);
    cpuPanel->Hide();
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

void MainFrame::OnGpuSelected(wxCommandEvent& /*event*/)
{
    int sel = gpuListBox->GetSelection();
    // if select the last item (CPU & Memory), do nothing
    if (sel == static_cast<int>(gpuListBox->GetCount()) - 1) {
        gpuPanel->Hide();
        cpuPanel->Show();
    } else {
        gpuPanel->changeSelectedGpu(sel);
        gpuPanel->Show();
        cpuPanel->Hide();
    }
}

