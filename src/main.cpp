#include "MainFrame.h"

class GpuProfilerApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        MainFrame* frame = new MainFrame("GPU profiler");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(GpuProfilerApp);