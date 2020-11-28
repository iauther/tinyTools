#ifndef __PLOT_Hx__
#define __PLOT_Hx__

#include <windows.h>


class CPlot{
    
public:
    CPlot();
    ~CPlot();
    
    void DrawCurve(CDC *pDC);
    void DrawTimeValue(CDC *pDC);
    void ShowtimeValue(int scale,CDC *pDC);
    void AddPoint(CTime t,float d);
    void CPlot::DrawBasic(CDC *pDC);
    void CPlot::DrawGrids(CDC *pDC);
    void CPlot::DrawXGrids(CDC *pDC);
    void CPlot::DrawYGrids(CDC *pDC);
        
private:
    int leftmargin;
    int rightmargin;
    int topmargin;
    int bottommargin;

    COLORm_crBrgndColor;
    COLOR m_crGridPen;
    COLOR m_crCurve;
    int m_pValue;
    int m_dLen;
    int m_LastValue;
        
        
};


#endif











