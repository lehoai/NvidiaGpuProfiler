#include <wx/panel.h>
#include <wx/dcclient.h>
#include <vector>

class TimelineChart : public wxPanel
{
public:
    TimelineChart(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size);
    ~TimelineChart();

    int maxVal = 100;
    void AddDataPoint(int value);
protected:
    void OnPaint(wxPaintEvent& event);
private:
    std::vector<int> dataPoints;
    int maxPoints = 100;
    wxDECLARE_EVENT_TABLE();
};