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
}point_t;

class CPlot{
    DECLARE_DYNAMIC(CPlot);
public:
    CPlot(CDC *pDC);
    ~CPlot();
    
    void      drawCurve(CRect rect, COLORREF color);
    void      addPoint(CRect rect, CTime t, float d);
    void      drawRect(CRect rect, COLORREF color);
    void      fillRect(CRect rect, COLORREF color);
    void      drawXGrids(CRect rect, int nx, COLORREF color);
    void      drawYGrids(CRect rect, int ny, COLORREF color);
    void      drawGrids(CRect rect, int nx, int ny, COLORREF color);
             
private:     
    CDC       *m_pDC;
    int       m_Cnt;
    point_t   *m_pPoint;
};


#endif












