#include "TimelineChart.h"
#include <wx/graphics.h>

wxBEGIN_EVENT_TABLE(TimelineChart, wxPanel)
    EVT_PAINT(TimelineChart::OnPaint)
wxEND_EVENT_TABLE()

TimelineChart::TimelineChart(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
    : wxPanel(parent, id, pos, size)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(*wxWHITE); // Panel white
}

TimelineChart::~TimelineChart()
{
}

void TimelineChart::AddDataPoint(int value) {
    if (dataPoints.size() >= maxPoints)
        dataPoints.erase(dataPoints.begin());
    dataPoints.push_back(value);
    Refresh();
}

void TimelineChart::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    wxSize size = GetClientSize();
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

    // background color
    gc->SetBrush(wxBrush(*wxWHITE));
    gc->SetPen(*wxTRANSPARENT_PEN);
    gc->DrawRectangle(0, 0, size.x, size.y);

    int chartLeft = 0, chartRight = size.x, chartTop = 0, chartBottom = size.y;
    int chartWidth = chartRight - chartLeft;
    int chartHeight = chartBottom - chartTop;
    int n = dataPoints.size();
    if (n < 2) { delete gc; return; }

    int maxValLocal = maxVal;
    if (maxValLocal == 0) maxValLocal = 1;

    // horizontal line
    gc->SetPen(wxPen(wxColour(196, 218, 234), 1));
    for (int i = 0; i <= 10; ++i) {
        int y = chartBottom - i * chartHeight / 10;
        gc->StrokeLine(chartLeft, y, chartRight, y);
    }
    // vertical line
    for (int i = 0; i <= 5; ++i) {
        int x = chartLeft + i * chartWidth / 5;
        gc->StrokeLine(x, chartTop, x, chartBottom);
    }

    // area line chart
    wxGraphicsPath area = gc->CreatePath();
    area.MoveToPoint(chartRight, chartBottom);
    for (int i = 0; i < n; ++i) {
        int idx = n - 1 - i;
        double x = chartRight - i * chartWidth / (maxPoints - 1);
        double y = chartBottom - (double)dataPoints[idx] * chartHeight / maxValLocal;
        area.AddLineToPoint(x, y);
    }
    double xFirst = chartRight - (n - 1) * chartWidth / (maxPoints - 1);
    area.AddLineToPoint(xFirst, chartBottom);
    area.CloseSubpath();
    gc->SetBrush(wxBrush(wxColour(76, 158, 204, 40))); // opacity 0.5
    gc->FillPath(area);

    // line chart
    gc->SetPen(wxPen(wxColour(76, 158, 204), 2));
    for (int i = 1; i < n; ++i) {
        int idx1 = n - i;
        int idx2 = n - i - 1;
        double x1 = chartRight - (i - 1) * chartWidth / (maxPoints - 1);
        double y1 = chartBottom - (double)dataPoints[idx1] * chartHeight / maxValLocal;
        double x2 = chartRight - i * chartWidth / (maxPoints - 1);
        double y2 = chartBottom - (double)dataPoints[idx2] * chartHeight / maxValLocal;
        gc->StrokeLine(x1, y1, x2, y2);
    }

    // border
    gc->SetPen(wxPen(wxColour(76, 158, 204), 2));
    gc->StrokeLine(chartLeft, chartBottom, chartRight, chartBottom);
    gc->StrokeLine(chartLeft, chartBottom, chartLeft, chartTop);
    gc->StrokeLine(chartLeft, chartTop, chartRight, chartTop);
    gc->StrokeLine(chartRight, chartTop, chartRight, chartBottom);

    delete gc;
}