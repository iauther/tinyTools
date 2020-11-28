/***********************************************************************************************
*函数名 : OnDraw
*函数功能描述 : 视类的OnDraw虚函数，在窗口重绘时被WM_PAINT的消息响应函数调用
*函数参数 : 无
*函数返回值 : 无
*作者 : nelson
*函数创作日期 : 2016/3/15
*函数修改日期 :
*修改人 :
*修改原因 :
*版本 : 1.0
*历史版本 : 无
***********************************************************************************************/
void CMyView::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();
    
    if(m_bRedraw == TRUE)                   //避免其他情况引起的重绘
    {
        m_bRedraw = FALSE;
        m_plot.DrawCurve(pDC);
    }
}
复制代码
复制代码
/***********************************************************************************************
*函数名 : OnEraseBkgnd
*函数功能描述 : WM_ERASEBKGND消息响应函数
*函数参数 : 无
*函数返回值 : 无
*作者 : nelson
*函数创作日期 : 2016/3/15
*函数修改日期 :
*修改人 :
*修改原因 :
*版本 : 1.0
*历史版本 : 无
***********************************************************************************************/
BOOL CMyView::OnEraseBkgnd(CDC* pDC)
{
    GetClientRect(m_plot.m_Rect);

    CDC MemDC;                            //内存兼容DC
    CBitmap bmp;
    MemDC.CreateCompatibleDC(pDC);
    bmp.CreateCompatibleBitmap(pDC,m_plot.m_Rect.Width(),m_plot.m_Rect.Height());
    MemDC.SelectObject(&bmp);            //将兼容位图选入兼容DC

    m_plot.DrawBasic(&MemDC);            //在兼容位图
    m_plot.DrawGrids(&MemDC);
    m_plot.DrawTimeValue(&MemDC);

    pDC->BitBlt(m_plot.m_Rect.left,m_plot.m_Rect.top,m_plot.m_Rect.Width(),m_plot.m_Rect.Height(),&MemDC,0,0,SRCCOPY);

    MemDC.DeleteDC();
    bmp.DeleteObject(); 

    return TRUE;
}