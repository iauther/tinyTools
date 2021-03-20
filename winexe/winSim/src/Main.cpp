#include <windows.h>
#include "Serial.h"
#include "win.h"
#include "data.h"
#include "log.h"

#define RETRY_TIMES     5
#define WAIT_TIME       1000


int exit_flag = 0;
int sett_flag = 0;
int ack_flag = 0;
setts_t  curSetts;
CSerial mSerial;
BYTE serial_rx_buf[1000];
BYTE serial_tx_buf[1000];


static int upgrade_proc(pkt_hdr_t* p)
{
    int r = 0;
    static U16 upg_pid = 0;
    upgrade_pkt_t* upg = (upgrade_pkt_t*)p->data;

    if (upg->pid == 0) {
        upg_pid = 0;
    }

    if (upg->pid != upg_pid) {
        return -1;
    }

    if (upg->pid == upg->pnum - 1) {
        //upgrade finished
    }
    else {
        upg_pid++;
    }

    return r;
}
static int stat_proc(pkt_hdr_t* p)
{
    int r = 0;
    static U16 upg_pid = 0;
    stat_t* st = (stat_t*)p->data;

}
static int ack_proc(pkt_hdr_t* p)
{
    int r = 0;
    static U16 upg_pid = 0;
    stat_t* st = (stat_t*)p->data;

}
static U8 get_checksum(U8 *data, U16 len)
{
    U8 sum = 0;
    for (int i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}
static int dev_init(void)
{
    return mSerial.Open(8);
}
static int dev_deinit(void)
{
    return mSerial.Close();
}
static int dev_send(void *data, U16 len)
{
    return mSerial.Write(data, len);
}
static int dev_recv(void* data, U16 len)
{
    return mSerial.Read(data, len, 0);
}
int com_send(U8 type, U8 nAck, void *data, U16 len)
{
    int i,j,r,brk=0;
    U8* buf = serial_tx_buf;
    pkt_hdr_t* p = (pkt_hdr_t*)buf;

    p->magic = PKT_MAGIC;
    p->type  = type;
    p->flag  = 0;
    p->askAck = nAck;
    p->dataLen = len;
    memcpy(p->data, data, len);
    buf[PKT_HDR_LENGTH] = get_checksum(buf, PKT_HDR_LENGTH +len);

    ack_flag = 0;
    r = dev_send(buf, PKT_HDR_LENGTH+len+1);
    if (r == PKT_HDR_LENGTH + len + 1 && nAck) {
        for (i = 0; i < RETRY_TIMES; i++) {
            for (j = 0; j < WAIT_TIME; j++) {
                if (ack_flag) {
                    brk = 1;
                    break;
                }
                Sleep(1);
            }
            if (brk) {
                break;
            }
            dev_send(buf, PKT_HDR_LENGTH + len + 1);
        }
    }

    return r;
}


static  int data_thread_run = 0;
static DWORD WINAPI data_thread(LPVOID lpParam)
{
    bool  r;
    int rlen;
    U8  checksum;
    U8* buf=serial_rx_buf;
    pkt_hdr_t* p = (pkt_hdr_t*)serial_rx_buf;

    dev_init();
    com_send(TYPE_SETT, 0, NULL, 0);

    data_thread_run = 1;
    while (1) {
        rlen = dev_recv(serial_rx_buf, sizeof(serial_rx_buf));
        if (rlen > 0) {

            if (p->magic != PKT_MAGIC) {
                log("___ pkt magic is 0x%04x, magic is wrong!\n", p->magic);
                continue;
            }
            if (p->dataLen + PKT_HDR_LENGTH+1 != rlen) {
                log("___ pkt length is wrong\n");
                continue;
            }

            checksum = get_checksum(buf, p->dataLen+PKT_HDR_LENGTH);
            if (checksum != buf[p->dataLen + PKT_HDR_LENGTH]) {
                log("___ pkt checksum is wrong!\n", checksum);
                continue;
            }

            switch (p->type) {
            case TYPE_STAT:
            {
                stat_t* stat = (stat_t*)p->data;
                win_stat_update(stat);
            }
            break;

            case TYPE_ACK:
            {
                ack_flag = 1;
            }
            break;

            case TYPE_SETT:
            {
                memcpy(&curSetts, p->data, sizeof(curSetts));
            }
            break;

            case TYPE_UPGRADE:
            {

            }
            break;

            case TYPE_FWINFO:
            {

            }
            break;

            default:
                break;
            }
        }

        if (exit_flag) {
            break;
        }
    }

    return 0;
}
//////////////////////////////////////////////////
static DWORD mainThreadId;
#define WM_MYTIMER     WM_USER+1 
#define WM_MYLOOP      WM_USER+2 
static void CALLBACK timer_callback(HWND hwnd, UINT message, UINT_PTR iTimerID, DWORD dwTime)
{
    //PostThreadMessage(mainThreadId, WM_MYLOOP, NULL, NULL);
}

int main(void)
{
    MSG msg;
    BOOL ret;

    mainThreadId = GetCurrentThreadId();
    //SetTimer(NULL, WM_MYTIMER, 100, NULL);
    CreateThread(NULL, 0, data_thread, NULL, 0, NULL);
    while (data_thread_run==0) Sleep(1);

    win_init();
    while (GetMessage(&msg, NULL, 0, 0)) {

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        switch (msg.message) {
            case WM_TIMER:
            {
                //printf("____ my timer\n");



            }
            break;
        }


        if (exit_flag) {
            break;
        }
    }

    return 0;
}

