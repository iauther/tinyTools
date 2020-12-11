#ifndef __PLOT_Hx__
#define __PLOT_Hx__

#include "stdafx.h"

typedef struct {
    int           left;
    int           right;
    int           top;
    int           bottom;
}margin_t;

typedef struct {
    CTime           time;
    float           dValue;
}pValue;

class CPlot{
    DECLARE_DYNAMIC(CPlot);
public:
    CPlot(CDC *pDC);
    ~CPlot();
    
    void        drawCurve(void);
    void        drawTimeValue(void);
    void        showtimeValue(int scale);
    void        addPoint(CTime t,float d);
    void        drawRect(CRect rect, COLORREF color);
    void        fillRect(CRect rect, COLORREF color);
    void        drawXGrids(int x1, int x2, int nx, COLORREF color);
    void        drawYGrids(int y1, int y2, int ny, COLORREF color);
    void        drawGrids(CRect rect, int nx, int ny, COLORREF color);
        
private:
    int         leftmargin;
    int         rightmargin;
    int         topmargin;
    int         bottommargin;

    CDC         *m_pDC;
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












