#ifndef __PLOT_Hx__
#define __PLOT_Hx__

#include "stdafx.h"


typedef struct {
    CTime           time;
    float           dValue;
}pValue;

class CPlot{
    DECLARE_DYNAMIC(CPlot);
public:
    CPlot(HDC hdc);
    ~CPlot();
    
    void        drawCurve(void);
    void        drawTimeValue(void);
    void        showtimeValue(int scale);
    void        addPoint(CTime t,float d);
    void        drawRect(void);
    void        drawGrids(void);
    void        drawXGrids(void);
    void        drawYGrids(void);
        
private:
    int         leftmargin;
    int         rightmargin;
    int         topmargin;
    int         bottommargin;

    HDC         m_Hdc;
    CRect       m_Rect;
    CRect       m_skeletonRect;
    COLORREF    m_crBrgndColor;
    COLORREF    m_crGridPen;
    COLORREF    m_crCurve;
    int         m_dLen;
    float       m_LastValue;
    pValue*     m_pValue;
        
};


#endif












