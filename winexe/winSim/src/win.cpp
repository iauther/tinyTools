#include <windows.h>
#include "win.h"
#include "log.h"

extern int exit_flag;
static grp_t     grp;
static uiWindow* win;
uiMultilineEntry* logEntry = NULL;
extern int com_send(U8 type, U8 nAck, void* data, U16 len);

static int onClosing(uiWindow* w, void* data)
{
	uiQuit();
	return 1;
}
static int onShouldQuit(void* data)
{
	uiWindow* win = uiWindow(data);

	uiControlDestroy(uiControl(win));
	return 1;
}


////////////////////////////////////////////////////
static label_btn_t* label_btn_new(const char* label, const char* btn_txt, button_clik_t fn)
{
	char buff[50];
	uiButton* btn;
	label_btn_t* lb = (label_btn_t*)malloc(sizeof(label_btn_t));

	if (!lb) {
		return NULL;
	}

	lb->grid = uiNewGrid(); uiGridSetPadded(lb->grid, 1);
	lb->label = uiNewLabel(label);

	lb->button = uiNewButton(btn_txt);
	uiButtonOnClicked(lb->button, fn, NULL);

	uiGridAppend(lb->grid, uiControl(lb->label),  0, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(lb->grid, uiControl(lb->button), 1, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

	return lb;
}
static label_cmb_t* label_cmb_new(const char* label, combo_sel_t fn, void* user_data, const char **item, int dft)
{
	int i = 0;
	label_cmb_t* lc = (label_cmb_t*)malloc(sizeof(label_cmb_t));

	if (!lc) {
		return NULL;
	}

	lc->grid = uiNewGrid(); uiGridSetPadded(lc->grid, 1);
	lc->label = uiNewLabel(label);
	lc->combo = uiNewCombobox();
	while(item[i]) { 
		uiComboboxAppend(lc->combo, item[i]); i++;
	}
	uiComboboxSetSelected(lc->combo, dft);
	uiComboboxOnSelected(lc->combo, fn, NULL);

	uiGridAppend(lc->grid, uiControl(lc->label), 1, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(lc->grid, uiControl(lc->combo), 2, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

	return lc;
}




static label_input_t* label_input_new(const char* label, const char *value, const char *btn_txt, button_clik_t fn)
{
	char buff[50];
	uiButton *btn;
	label_input_t* li = (label_input_t*)malloc(sizeof(label_input_t));

	if (!li) {
		return NULL;
	}

	li->grid = uiNewGrid(); uiGridSetPadded(li->grid, 1);
	li->label = uiNewLabel(label); 
	li->entry = uiNewEntry();			uiEntrySetText(li->entry, value);
	li->button = uiNewButton(btn_txt);	uiButtonOnClicked(li->button, fn, NULL);

	uiGridAppend(li->grid, uiControl(li->label),  0, 0, 1, 1, 1, uiAlignStart, 0, uiAlignFill);
	uiGridAppend(li->grid, uiControl(li->entry),  1, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
	uiGridAppend(li->grid, uiControl(li->button), 2, 0, 1, 1, 1, uiAlignEnd, 0, uiAlignFill);

	return li;
}
//////////////////////////////////////////////////////
static void on_sett_speed_btn_fn(uiButton* b, void* data)
{

}

static void on_sett_vacuum_btn_fn(uiButton* b, void* data)
{

}
static void on_sett_valve_btn_fn(uiCombobox* c, void* data)
{

}
static void on_sett_start_btn_fn(uiButton* b, void* data)
{
	if(strcmp(uiButtonText(b), "Start")==0) {
		uiButtonSetText(b, "Stop");
		log("stop the pump...\n");
	}
	else if (strcmp(uiButtonText(b), "Stop") == 0) {
		uiButtonSetText(b, "Start");
		log("start the pump...\n");
	}
}
static int para_grp_init(uiWindow* win, para_grp_t* para)
{
	const char* valveStr[] = {"ON", "OFF", NULL};

	para->grid         = uiNewGrid(); uiGridSetPadded(para->grid, 1);
	para->sett.grp    = uiNewGroup("Pump Setting"); 
	para->sett.vbox   = uiNewVerticalBox();uiBoxSetPadded(para->sett.vbox, 1);
	para->sett.grid   = uiNewGrid();       uiGridSetPadded(para->sett.grid, 1);

	para->sett.speed  = label_input_new("Speed",   "1000",   "set", on_sett_speed_btn_fn);
	para->sett.vacuum = label_input_new("Vacuum",   "97 KPa", "set", on_sett_vacuum_btn_fn);
	para->sett.start  = uiNewButton("Start");uiButtonOnClicked(para->sett.start, on_sett_start_btn_fn, NULL);

	uiGridAppend(para->sett.grid, uiControl(para->sett.speed->grid),  0, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(para->sett.grid, uiControl(para->sett.vacuum->grid), 0, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(para->sett.grid, uiControl(para->sett.start),        0, 2, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);

	uiBoxAppend(para->sett.vbox, uiControl(para->sett.grid), 1);
	uiGroupSetChild(para->sett.grp, uiControl(para->sett.vbox));

	////////////////////////////////////////////////////////////
	para->stat.grp    = uiNewGroup("Pump State");
	para->stat.vbox   = uiNewHorizontalBox();    uiBoxSetPadded(para->stat.vbox, 1);
	para->stat.grid   = uiNewGrid();             uiGridSetPadded(para->stat.grid, 1);

	para->stat.mode   = uiNewLabel("Mode:");
	para->stat.temp   = uiNewLabel("Temp:");
	para->stat.valve  = uiNewLabel("Valve:");
	para->stat.speed  = uiNewLabel("Speed:");
	para->stat.vacuum = uiNewLabel("Vacuum:");
	para->stat.current = uiNewLabel("Current:");

	uiGridAppend(para->stat.grid, uiControl(para->stat.mode),    0, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(para->stat.grid, uiControl(para->stat.temp),    0, 1, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(para->stat.grid, uiControl(para->stat.valve),   0, 2, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(para->stat.grid, uiControl(para->stat.speed),   0, 3, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(para->stat.grid, uiControl(para->stat.vacuum),  0, 4, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(para->stat.grid, uiControl(para->stat.current), 0, 5, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

	uiBoxAppend(para->stat.vbox, uiControl(para->stat.grid), 1);
	uiGroupSetChild(para->stat.grp, uiControl(para->stat.vbox));
	//					left   top     hspan   vspan   hexpand    halign         vexpand     valign
	uiGridAppend(para->grid, uiControl(para->sett.grp), 0, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(para->grid, uiControl(para->stat.grp), 1, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);

	return 0;
}

static void on_upg_open_btn_fn(uiButton* b, void* data)
{
	char* filename;
	uiWindow* win = uiWindow(data);

	filename = uiOpenFile(win);
	if (filename) {
		//uiEntrySetText(entry, "");
		return;
	}
}
static void on_upg_start_btn_fn(uiButton* b, void* data)
{

}
static int upg_grp_init(uiWindow* win, upg_grp_t* upg)
{
	upg->grid = uiNewGrid();					uiGridSetPadded(upg->grid, 1);
	upg->grp  = uiNewGroup("Firmware Upgrade");
	upg->vbox = uiNewVerticalBox();
	////////////////
	upg->g    = uiNewGrid();					uiGridSetPadded(upg->g, 1);
	upg->path = uiNewEntry();					uiEntrySetReadOnly(upg->path, 1);
	upg->open = uiNewButton("Open");			uiButtonOnClicked(upg->open, on_upg_open_btn_fn, win);
	upg->pgbar = uiNewProgressBar();			uiProgressBarSetValue(upg->pgbar, 0);
	upg->start = uiNewButton("Start");			uiButtonOnClicked(upg->start, on_upg_start_btn_fn, NULL);
	
	uiGridAppend(upg->g, uiControl(upg->path),  0, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(upg->g, uiControl(upg->open),  1, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(upg->g, uiControl(upg->pgbar), 0, 1, 2, 1, 1, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(upg->g, uiControl(upg->start), 0, 2, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);

	uiBoxAppend(upg->vbox, uiControl(upg->g), 1);
	uiGroupSetChild(upg->grp, uiControl(upg->vbox));
	uiGridAppend(upg->grid, uiControl(upg->grp), 0, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);

	return 0;
}


static void on_log_enable_btn_fn(uiButton* b, void* data)
{
	if (log_get()==0) {
		log_en(1);
		uiButtonSetText(b, "Enable");
	}
	else {
		log_en(0);
		uiButtonSetText(b, "Disable");
	}
}
static void on_log_clear_btn_fn(uiButton* b, void* data)
{
	log_clr();
}
static void on_log_save_btn_fn(uiButton* b, void* data)
{
	char* filename;
	uiWindow* win = uiWindow(data);

	filename = uiSaveFile(win);
	if (filename) {
		log_save(filename);
		return;
	}
}

static int log_grp_init(uiWindow* win, log_grp_t* log)
{
	log->grid = uiNewGrid();	 uiGridSetPadded(log->grid, 1);
	log->grp  = uiNewGroup("Log Infomation");
	log->vbox = uiNewVerticalBox();

	log->g = uiNewGrid();		uiGridSetPadded(log->g, 1);
	log->enable = uiNewButton("Disable");uiButtonOnClicked(log->enable, on_log_enable_btn_fn, win);
	log->clear  = uiNewButton("Clear"); uiButtonOnClicked(log->clear,  on_log_clear_btn_fn, win);
	log->save   = uiNewButton("Save");  uiButtonOnClicked(log->save,   on_log_save_btn_fn, win);
	log->entry  = uiNewMultilineEntry();uiMultilineEntrySetReadOnly(log->entry, 1);
	logEntry    = log->entry;

	//                                              x  y  xs ys hex halign     vex  valign
	uiGridAppend(log->g, uiControl(log->enable), 0, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(log->g, uiControl(log->clear),  1, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(log->g, uiControl(log->save),   2, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(log->g, uiControl(log->entry),  0, 1, 3, 1, 1, uiAlignFill, 1, uiAlignFill);

	uiBoxAppend(log->vbox, uiControl(log->g), 1);
	uiGroupSetChild(log->grp, uiControl(log->vbox));
	uiGridAppend(log->grid, uiControl(log->grp), 0, 1, 1, 1, 1, uiAlignFill, 1, uiAlignFill);

	return 0;
}



static void ui_init(void)
{
	uiGrid* grid;
	const char* err;
	uiInitOptions options;

	memset(&options, 0, sizeof(options));
	err = uiInit(&options);
	if (err != NULL) {
		uiFreeInitError(err);
		return;
	}

	win = uiNewWindow("winSim", 800, 600, 1);
	uiWindowOnClosing(win, onClosing, win);
	uiOnShouldQuit(onShouldQuit, win);

	grid = uiNewGrid();	 uiGridSetPadded(grid, 1);
	uiWindowSetChild(win, uiControl(grid));

	para_grp_init(win, &grp.para);
	upg_grp_init(win, &grp.upg);
	log_grp_init(win, &grp.log);
	uiGridAppend(grid, uiControl(grp.para.grid), 0, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(grid, uiControl(grp.upg.grid),  0, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(grid, uiControl(grp.log.grid),  0, 2, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
	

	uiControlShow(uiControl(win));
}


static DWORD WINAPI uiThread(LPVOID lpParam)
{
	ui_init();
	uiMain();
	uiUninit();
	logEntry = NULL;
	exit_flag = 1;

	return 0;
}

void win_init(void)
{
	CreateThread(NULL, 0, uiThread, NULL, 0, NULL);
}


void win_stat_update(stat_t *stat)
{
	char tmp[50];

	sprintf(tmp, "Vacuum: %.1f", stat->dPres);
	uiLabelSetText(grp.para.stat.vacuum, (const char*)tmp);
}