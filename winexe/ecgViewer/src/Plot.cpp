#include "plot.h"

CPlot::CPlot()
{
    m_pDC = NULL;
    m_Cnt = 0;
    m_pPoint = NULL;
}


CPlot::~CPlot()
{
    if (m_pPoint != NULL) {
        free(m_pPoint);
        m_pPoint = NULL;
    }
}


void CPlot::init(HDC hdc)
{
    m_pDC = CDC::FromHandle(hdc);
}

void CPlot::drawCurve(CRect rect, COLORREF color)
{
    long i=0;
    float randValue = (double)abs(rand()%16);                //randValue 范围 0 - 20
    float cy = randValue/4;                                    //刻度线分为4格    
    if(cy <= 1) {                                               //cy <= 1的概率是25%
        cy = 1;
    }   

    CPen *oldpen;
    CPen pen(PS_SOLID,2, color);
    oldpen = m_pDC->SelectObject(&pen);

    float smallgridwidth = (float)rect.Width()/100;
    float intervalY = rect.Height()/4;

    int docnumber1 = m_Cnt;
    int offset=3;

    if(m_Cnt >=2) {
        if(m_Cnt < 100){
            for(int i=0;i< m_Cnt -1;i++) {                      //画图都是从左向右，左边的点都是先出来的点，但视觉效果是从右向左
                docnumber1--;
                m_pDC->MoveTo(CPoint(rect.right-(float)smallgridwidth*(docnumber1),rect.Height()+100-
                    ((m_pPoint[i].dValue-1)*intervalY)));
                m_pDC->LineTo(CPoint(rect.right-(float)smallgridwidth*(docnumber1-1),rect.Height()+ 100 -
                    ((m_pPoint[i+1].dValue-1)*intervalY)));
            }
        }
        else{
            for(i=0;i<100;i++){
                for(int i=0;i<100;i++) {                      //同理  这里也是从左向右描点
                    m_pDC->MoveTo(CPoint(rect.right - smallgridwidth*(100-i),rect.Height()+ 100 -
                        ((m_pPoint[i+ m_Cnt -1-100].dValue-1)*intervalY)));
                    m_pDC->LineTo(CPoint(rect.right - smallgridwidth*(100-i-1),rect.Height()+ 100 -
                        ((m_pPoint[i+ m_Cnt -100].dValue-1)*intervalY)));
                }
            }
        }    
    }

    m_pDC->SelectObject(oldpen);
}


#define AREA(r) (rect.Width() * rect.Height())
void CPlot::addPoint(CRect rect, CTime t, float d)
{
    if (!m_pPoint) {
        m_pPoint = new point_t[AREA(rect)];
    }

    m_pPoint[m_Cnt].time = t;
    m_pPoint[m_Cnt].dValue = d;

    m_Cnt++;                            //数据量在增加
}


void CPlot::drawRect(CRect rect, COLORREF color)
{
    CBrush m_bkBrush(color);
    m_pDC->SelectObject(&m_bkBrush);
    m_pDC->Rectangle(rect);
}


void CPlot::fillRect(CRect rect, COLORREF color)
{
    CBrush m_bkBrush(color);
    m_pDC->SelectObject(&m_bkBrush);
    m_pDC->FillRect(rect, &m_bkBrush);
}


void CPlot::drawXGrids(CRect rect, int nx, COLORREF color)
{
    CPen pen1(PS_SOLID,0,color);                     

    

    double yDistance = ((double)rect.bottom - (double)rect.top)/(nx+1);  
    int startX = rect.left;            //左边横坐标
    int startY = rect.top;             //左边上坐标
    int endY = rect.bottom;
    int endX = rect.right;

    m_pDC->SelectObject(&pen1);
    for(int i=1;i<=nx;i++) {                                      //画39条格子
    
        if(i%10 == 0) {                                       //每10格画一个大格子
            m_pDC->MoveTo(startX+1,(int)(startY+i*yDistance));   
            m_pDC->LineTo(endX-1,(int)(startY+i*yDistance));
        }

        m_pDC->MoveTo(startX+1,(int)(startY+i*yDistance));     
        m_pDC->LineTo(endX-1,(int)(startY+i*yDistance));
    }
}


void CPlot::drawYGrids(CRect rect, int ny, COLORREF color)
{
    CPen *old,*old1;
    CPen pen1(PS_SOLID,0,color);                        //细线

    double xDistance = (rect.right-rect.left)/(ny+1);
    int startX = rect.left;                            //左边横坐标
    int startY = rect.top;                            //左边上坐标
    int endY = rect.bottom;
    int endX = rect.right;

    m_pDC->SelectObject(&pen1);
    for(int i=1;i<=ny;i++) { 
        if(i%10 == 0) {                                          //每10格画一个大格子
            m_pDC->MoveTo(startX+i*xDistance,startY+1);
            m_pDC->LineTo(startX+i*xDistance,endY-1);
        }
        m_pDC->MoveTo(startX+i*xDistance,startY+1);
        m_pDC->LineTo(startX+i*xDistance,endY-1);
    }
}


void CPlot::drawGrids(CRect rect, int nx, int ny, COLORREF color)
{
    drawXGrids(rect, nx, color);
    drawYGrids(rect, ny, color);
}

