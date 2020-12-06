#ifndef __PLOT_Hx__
#define __PLOT_Hx__

#include "stdafx.h"


typedef struct {
    CTime time;
    float dValue;
}pValue;

class CPlot{
    DECLARE_DYNAMIC(CPlot);
public:
    CPlot();
    ~CPlot();
    
    void DrawCurve(CDC *pDC);
    void DrawTimeValue(CDC *pDC);
    void ShowtimeValue(int scale,CDC *pDC);
    void AddPoint(CTime t,float d);
    void DrawBasic(CDC *pDC);
    void DrawGrids(CDC *pDC);
    void DrawXGrids(CDC *pDC);
    void DrawYGrids(CDC *pDC);
        
private:
    int leftmargin;
    int rightmargin;
    int topmargin;
    int bottommargin;

    CRect m_Rect;
    CRect m_skeletonRect;
    COLORREF  m_crBrgndColor;
    COLORREF  m_crGridPen;
    COLORREF  m_crCurve;
    int m_dLen;
    float m_LastValue;
    pValue* m_pValue;
        
};


#endif












