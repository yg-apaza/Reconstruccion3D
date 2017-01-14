#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H
#include "wxfbWindows.h"
#include "header.h"

class MyMainWindow:public FBMainWindow {
protected:
	void execute( wxCommandEvent& event ) ;

public:
	MyMainWindow(wxWindow *parent=NULL);
	void OnButtonClose(wxCommandEvent &evt);
};

#endif
