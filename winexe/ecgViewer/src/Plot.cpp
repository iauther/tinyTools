// Plot.cpp : 实现文件
//

#include "Plot.h"

// CPlot

IMPLEMENT_DYNAMIC(CPlot, CWnd)

CPlot::CPlot(CDC* pDC)
{
    //画图区域与整个显示区域的边界
    leftmargin=30;                                            //左边界
    rightmargin=30;                                            //右边界
    topmargin=25;                                            //上边界
    bottommargin=25;                                        //下边界

    m_pDC = pDC;
    m_crBrgndColor = RGB(255,255,248);                        //背景色
    m_crGridPen = RGB(149,126,226);                            //格子线
    m_crCurve = RGB(255,0,0);
    m_pValue = NULL;
    m_dLen = 0;
    m_LastValue = 0;
}


CPlot::~CPlot()
{
    if (m_pValue != NULL)
    {
        free(m_pValue);
        m_pValue = NULL;
    }
}

void CPlot::drawCurve(void)
{
    long i=0;
    float randValue = (double)abs(rand()%16);                //randValue 范围 0 - 20
    float cy = randValue/4;                                    //刻度线分为4格    
    if(cy <= 1) {                                               //cy <= 1的概率是25%
        cy = 1;
    }
    
    if(cy == 1){
        addPoint(CTime::GetCurrentTime(),m_LastValue);  
    }
    else{
        addPoint(CTime::GetCurrentTime(),cy);  
    }

    CPen *oldpen;
    CPen pen(PS_SOLID,2,m_crCurve);
    oldpen = m_pDC->SelectObject(&pen);

    float smallgridwidth = (float)m_skeletonRect.Width()/100;
    float intervalY = m_skeletonRect.Height()/4;

    int docnumber1 = m_dLen;
    int offset=3;

    if(m_dLen >=2) {
        if(m_dLen < 100){
            for(int i=0;i<m_dLen-1;i++) {                      //画图都是从左向右，左边的点都是先出来的点，但视觉效果是从右向左
                docnumber1--;
                m_pDC->MoveTo(CPoint(m_skeletonRect.right-(float)smallgridwidth*(docnumber1),m_skeletonRect.Height()+topmargin-
                    ((m_pValue[i].dValue-1)*intervalY)));
                m_pDC->LineTo(CPoint(m_skeletonRect.right-(float)smallgridwidth*(docnumber1-1),m_skeletonRect.Height()+topmargin-
                    ((m_pValue[i+1].dValue-1)*intervalY)));
            }
        }
        else{
            for(i=0;i<100;i++){
                for(int i=0;i<100;i++) {                      //同理  这里也是从左向右描点
                    m_pDC->MoveTo(CPoint(m_skeletonRect.right - smallgridwidth*(100-i),m_skeletonRect.Height()+topmargin-
                        ((m_pValue[i+m_dLen-1-100].dValue-1)*intervalY)));
                    m_pDC->LineTo(CPoint(m_skeletonRect.right - smallgridwidth*(100-i-1),m_skeletonRect.Height()+topmargin-
                        ((m_pValue[i+m_dLen-100].dValue-1)*intervalY)));
                }
            }
        }    
    }

    m_pDC->SelectObject(oldpen);
}

/***********************************************************************************************
*函数功能描述 : 画时间刻度 在刷新背景的的时候画刻度线 避免闪烁
***********************************************************************************************/
void CPlot::drawTimeValue(void)
{
    int i=0;

    if((m_dLen<100) && (m_dLen >0)){
        int scale = m_dLen/10;

        for(i=0;i<=scale;i++){
            showtimeValue(i);
        }
    }
    else {
        for(i=0;i<=10;i++){
            showtimeValue(i);
        }
    }
}

/***********************************************************************************************
*函数功能描述 : 显示某一个时间值
***********************************************************************************************/
void CPlot::showtimeValue(int scale)
{
    CString tmp("");
    int x=0,y=0;
    float smallgridwidth = (float)m_skeletonRect.Width()/100;

    tmp = m_pValue[m_dLen-1-9*scale].time.Format("%H:%M:%S");
    x = m_skeletonRect.right - smallgridwidth*scale*10;
    y = m_skeletonRect.bottom;

    m_pDC->DrawText(tmp,CRect(x-30,y+10,x+25,y+30),DT_CENTER);
}

/***********************************************************************************************
*函数功能描述 : 添加点
***********************************************************************************************/
void CPlot::addPoint(CTime t,float d)
{
    if(m_dLen == 0){
        m_pValue = (pValue *)malloc(sizeof(pValue));
    }
    else{
        m_pValue = (pValue *)realloc(m_pValue,(m_dLen+1)*sizeof(pValue));
    }

    m_LastValue = d;                    //记录最近的一个值
    m_pValue[m_dLen].time = t;
    m_pValue[m_dLen].dValue = d;

    m_dLen++;                            //数据量在增加
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


void CPlot::drawXGrids(int x1, int x2, int nx, COLORREF color)
{
    CPen *old,*old1;
    CPen pen1(PS_SOLID,0,RGB(192,192,192));      
    CPen pen2(PS_SOLID,2,RGB(192,192,192));         
    CPen pen3(PS_SOLID,1,RGB(0,0,0));            
    CFont *oldFont;
    CFont ft;

    old = (CPen *)m_pDC->SelectObject(&pen1);
    ft.CreatePointFont(100,_T("Arial"),m_pDC);
    oldFont = (CFont *)m_pDC->SelectObject(&ft);

    int yDiv = 40;
    double yDistance = ((double)m_skeletonRect.bottom - (double)m_skeletonRect.top)/yDiv;  
    int startX = m_skeletonRect.left;            //左边横坐标
    int startY = m_skeletonRect.top;             //左边上坐标
    int endY = m_skeletonRect.bottom;
    int endX = m_skeletonRect.right;
    int offset = 5;
    int scale = 5;

    CString text;
    text.Format(_T("%d.0"),5);
    m_pDC->DrawText(text,CRect(startX-30,startY-8,startX-3,startY+8),DT_CENTER);
    m_pDC->DrawText(text,CRect(endX+3,startY-8,endX+30,startY+8),DT_CENTER);
    text.Format(_T("%d.0"),1);
    m_pDC->DrawText(text,CRect(startX-30,endY-8,startX-3,endY+8),DT_CENTER);
    m_pDC->DrawText(text,CRect(endX+3,endY-8,endX+30,endY+8),DT_CENTER);

    int i=0;
    for(i=1;i<=39;i++) {                                      //画39条格子
    
        if(i%10 == 0) {                                       //每10格画一个大格子
            old1 = (CPen *)m_pDC->SelectObject(&pen2);
            m_pDC->MoveTo(startX+1,(int)(startY+i*yDistance));   
            m_pDC->LineTo(endX-1,(int)(startY+i*yDistance));
            m_pDC->SelectObject(old1);

            scale--;
            text.Format(_T("%d.0"),scale);
            m_pDC->DrawText(text,CRect(startX-30,startY+i*yDistance-8,startX-3,startY+i*yDistance+8),DT_CENTER);
            m_pDC->DrawText(text,CRect(endX+3,startY+i*yDistance-8,endX+30,startY+i*yDistance+8),DT_CENTER);
        }

        m_pDC->MoveTo(startX+1,(int)(startY+i*yDistance));     
        m_pDC->LineTo(endX-1,(int)(startY+i*yDistance));

        old1 = (CPen *)m_pDC->SelectObject(&pen3);             //画刻度
        m_pDC->MoveTo(startX,(int)(startY+i*yDistance));   
        m_pDC->LineTo(startX-offset,(int)(startY+i*yDistance));
        m_pDC->MoveTo(endX,(int)(startY+i*yDistance));   
        m_pDC->LineTo(endX+offset,(int)(startY+i*yDistance));
        m_pDC->SelectObject(old1);
    }

    m_pDC->SelectObject(old);
    m_pDC->SelectObject(oldFont);
}


void CPlot::drawYGrids(int y1, int y2, int ny, COLORREF color)
{
    CPen *old,*old1;
    CPen pen1(PS_SOLID,0,RGB(192,192,192));                        //细线
    CPen pen2(PS_SOLID,2,RGB(154,154,154));
    CPen pen3(PS_SOLID,1,RGB(0,0,0));                            //画刻度
    
    old = (CPen *)m_pDC->SelectObject(&pen1);

    //横向分为100个点
    int xDiv = 100;
    double xDistance = ((double)m_skeletonRect.right - (double)m_skeletonRect.left)/xDiv;  
    int startX = m_skeletonRect.left;                            //左边横坐标
    int startY = m_skeletonRect.top;                            //左边上坐标
    int endY = m_skeletonRect.bottom;
    int endX = m_skeletonRect.right;
    int offset = 5;

    int i=0;
    for(i=1;i<=99;i++) {                                          //画99条格子
    
        if(i%10 == 0) {                                          //每10格画一个大格子
            old1 = (CPen *)m_pDC->SelectObject(&pen2);
            m_pDC->MoveTo(startX+i*xDistance,startY+1);
            m_pDC->LineTo(startX+i*xDistance,endY-1);
            m_pDC->SelectObject(old1);
        }
        m_pDC->MoveTo(startX+i*xDistance,startY+1);
        m_pDC->LineTo(startX+i*xDistance,endY-1);

        old1 = (CPen *)m_pDC->SelectObject(&pen3);              //画刻度
        m_pDC->MoveTo(startX+i*xDistance,startY-offset);   
        m_pDC->LineTo(startX+i*xDistance,startY);
        m_pDC->MoveTo(startX+i*xDistance,endY);
        m_pDC->LineTo(startX+i*xDistance,endY+offset);
        m_pDC->SelectObject(old1);
    }

    m_pDC->SelectObject(old);
}


void CPlot::drawGrids(CRect rect, int nx, int ny, COLORREF color)
{
    drawXGrids(rect, nx, color);
    drawYGrids(rect, ny, color);
}

