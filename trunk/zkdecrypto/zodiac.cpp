#include <stdio.h>
#include <time.h>
#include <math.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <richedit.h>
#include "resource.h"
#include "zodiac.h"
#include "display.h"
#include "solve.h"
#include "files.h"

/*Edit Functions*/

void SetUndo()
{
	bUndo=true;
	EnableMenuItem(hMainMenu,IDM_EDIT_UNDO,MF_BYCOMMAND | MF_ENABLED);
	undo_message=message;
}

void Undo()
{
	bUndo=false;
	EnableMenuItem(hMainMenu,IDM_EDIT_UNDO,MF_BYCOMMAND | MF_GRAYED);
	message=undo_message;
	SetCipher();
}

//change letter mapped to symbol
void ChangePlain()
{
	SYMBOL symbol;

	if(iCurSymbol<0) return;

	SetUndo();
	
	//get new letter
	GetDlgItemText(hMainWnd,IDC_MAP_VALUE,szText,10);
	
	//get and update symbol
	message.cur_map.GetSymbol(iCurSymbol,&symbol);
	symbol.plain=szText[0];
	message.cur_map.AddSymbol(symbol,0);
	
	//update info		
	UpdateSymbol(iCurSymbol);
	SetPlain();
	SetTable();
	SetFreq();
}

/*Window Functions*/

//about dalog
LRESULT CALLBACK AboutProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch(iMsg)
	{
		case WM_INITDIALOG:
			sprintf(szText,"%s %s",PROG_NAME,PROG_VER);
			SetDlgItemText(hWnd,IDC_PROG,szText);
			strcpy(szText,"Wesley Hopper (hopperw2000@yahoo.com)");
			SetDlgItemText(hWnd,IDC_ABOUT1,szText);
			strcpy(szText,"Brax Sisco (xenex@bardstowncable.net)");
			SetDlgItemText(hWnd,IDC_ABOUT2,szText);
			strcpy(szText,"Michael Eaton");
			SetDlgItemText(hWnd,IDC_ABOUT3,szText);
			return 0;

		case WM_LBUTTONDOWN:
			EndDialog(hWnd,0);
			return 0;
	}

	return 0;
}

//options dialog
LRESULT CALLBACK OptionsProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int iPrevLang;

	switch(iMsg)
	{
		case WM_INITDIALOG: //init values
			//hillclimber parameters
			SetDlgItemInt(hWnd,IDC_MAXFAIL,siSolveInfo.max_fail,0);
			SetDlgItemInt(hWnd,IDC_SWAPS,siSolveInfo.swaps,0);
			SetDlgItemInt(hWnd,IDC_REVERT,siSolveInfo.revert,0);

			//score parameters
			SendMessage(GetDlgItem(hWnd,IDC_USEBI),BM_SETCHECK,iUseGraphs & USE_BI,0);
			SendMessage(GetDlgItem(hWnd,IDC_USETRI),BM_SETCHECK,iUseGraphs & USE_TRI,0);
			SendMessage(GetDlgItem(hWnd,IDC_USETETRA),BM_SETCHECK,iUseGraphs & USE_TETRA,0);
			SendMessage(GetDlgItem(hWnd,IDC_USEPENTA),BM_SETCHECK,iUseGraphs & USE_PENTA,0);

			//display options
			SetDlgItemInt(hWnd,IDC_LINECHARS,iLineChars,0);

			//language		
			SendDlgItemMessage(hWnd,IDC_LANG,CB_ADDSTRING,0,(LPARAM)"English");
			SendDlgItemMessage(hWnd,IDC_LANG,CB_ADDSTRING,0,(LPARAM)"Spanish");
			SendDlgItemMessage(hWnd,IDC_LANG,CB_ADDSTRING,0,(LPARAM)"German");
			SendDlgItemMessage(hWnd,IDC_LANG,CB_SETCURSEL,iLang,0);
			
			return 0;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK: //get new values
					//hillclimber parameters
					siSolveInfo.max_fail=GetDlgItemInt(hWnd,IDC_MAXFAIL,0,0);
					siSolveInfo.swaps=GetDlgItemInt(hWnd,IDC_SWAPS,0,0);
					siSolveInfo.revert=GetDlgItemInt(hWnd,IDC_REVERT,0,0);

					//score parameters
					iUseGraphs=0;
					
					if(SendMessage(GetDlgItem(hWnd,IDC_USEBI),BM_GETCHECK,0,0))
						iUseGraphs+=USE_BI;
						
					if(SendMessage(GetDlgItem(hWnd,IDC_USETRI),BM_GETCHECK,0,0))
						iUseGraphs+=USE_TRI;
						
					if(SendMessage(GetDlgItem(hWnd,IDC_USETETRA),BM_GETCHECK,0,0))
						iUseGraphs+=USE_TETRA;
						
					if(SendMessage(GetDlgItem(hWnd,IDC_USEPENTA),BM_GETCHECK,0,0))
						iUseGraphs+=USE_PENTA;

					//display options
					iLineChars=GetDlgItemInt(hWnd,IDC_LINECHARS,0,0);

					//language
					iPrevLang=iLang;
					iLang=SendDlgItemMessage(hWnd,IDC_LANG,CB_GETCURSEL,0,0);
					if(iLang!=iPrevLang) SetLanguage();

					//update display
					SetScrollBar();					
					SetDlgInfo();
					SetCipher();

				case IDCANCEL:
					EndDialog(hWnd,0);
					return 0;
			}
	}

	return 0;
}

//merge dialog
LRESULT CALLBACK MergeProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int num_symbols, cur_symbol;
	char cipher1, cipher2;
	SYMBOL symbol;
	
	switch(iMsg)
	{
		case WM_INITDIALOG:
			num_symbols=message.cur_map.GetNumSymbols();
			for(cur_symbol=0; cur_symbol<num_symbols; cur_symbol++)
			{
				message.cur_map.GetSymbol(cur_symbol,&symbol);
				sprintf(szText,"%c",symbol.cipher);
				SendDlgItemMessage(hWnd,IDC_MERSYM1,CB_ADDSTRING,0,(LPARAM)szText);
				SendDlgItemMessage(hWnd,IDC_MERSYM2,CB_ADDSTRING,0,(LPARAM)szText);
			}

			SetFocus(GetDlgItem(hWnd,IDC_MERSYM1));
			return 0;

		/*case WM_CHAR:
			SetFocus(GetDlgItem(hWnd,IDC_MERSYM2));
			return 0;*/

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
					//get 1st symbol cipher
					cur_symbol=SendDlgItemMessage(hWnd,IDC_MERSYM1,CB_GETCURSEL,0,0);
					message.cur_map.GetSymbol(cur_symbol,&symbol);
					cipher1=symbol.cipher;
					
					//get 2nd symbol cipher
					cur_symbol=SendDlgItemMessage(hWnd,IDC_MERSYM2,CB_GETCURSEL,0,0);
					message.cur_map.GetSymbol(cur_symbol,&symbol);
					cipher2=symbol.cipher;
					
					message.MergeSymbols(cipher1,cipher2);
					
					SetCipher();
					
					EndDialog(hWnd,0);
					return 1;

				case IDCANCEL:
					EndDialog(hWnd,0);
					return 0;
			}
	}

	return 0;
}

//word plug dialog
LRESULT CALLBACK WordProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	char szWord[128];

	switch(iMsg)
	{
		case WM_INITDIALOG:
			SetFocus(GetDlgItem(hWnd,IDC_WORD));
			return 0;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
					GetDlgItemText(hWnd,IDC_WORD,szWord,128);
					SetUndo();
					iBestScore=WordPlug(message,szWord,iUseGraphs);
					SetDlgInfo();
					EndDialog(hWnd,0);
					return 1;

				case IDCANCEL:
					EndDialog(hWnd,0);
					return 0;
			}
	}

	return 0;
}

//message handler for main window
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int swap, num_symbols;
	SYMBOL symbol;
	char filename[1024];
	PAINTSTRUCT ps;

	switch(iMsg)
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDM_FILE_OPEN_MSG:
					if(!GetFilename(filename,szCipherName,0)) return 0;
					LoadMessage(filename);				
					return 0;
					
				case IDM_EDIT_MSG: if(bMsgLoaded) OpenWith(szCipherName); return 0;

				case IDM_FILE_OPEN_MAP:
					if(!GetFilename(filename,szKeyName,0)) return 0;
					LoadMap(filename);
					return 0;
					
				case IDM_EDIT_MAP: if(bMsgLoaded) OpenWith(szKeyName); return 0;

				case IDM_FILE_SAVE_MAP:
					if(!bMsgLoaded) return 0;
					if(GetFilename(filename,szKeyName,1)!=1) return 0;
					if(message.cur_map.Write(filename))
						strcpy(szKeyName,filename);
					
					bMapLoaded=true;
					MapEnable(true);
					return 0;
					
				//case IDM_EDIT_RELOAD: LoadMap(szKeyName); return 0;
				
				
				case IDM_EDIT_MERGE:
					SetUndo();
					DialogBox(hInst,MAKEINTRESOURCE(IDD_MERGE),hMainWnd,(DLGPROC)MergeProc);
					return 0;
				
				
				case IDM_EDIT_UNDO: Undo(); SetCipher(); return 0;

				case IDM_EDIT_INIT:
					SetUndo();
					message.cur_map.Init();
					SetDlgInfo();
					return 0;

				case IDM_EDIT_SCRAMBLE:
					SetUndo();
					num_symbols=message.cur_map.GetNumSymbols();
					for(swap=0; swap<3000; swap++)
						message.cur_map.SwapSymbols(rand()%num_symbols,rand()%num_symbols);
					SetDlgInfo();
					return 0;

				case IDM_EDIT_CLEAR:
					SetUndo();
					message.cur_map.Clear(CLR_PLAIN);
					SetDlgInfo();
					return 0;

				case IDM_EDIT_LOCK: message.cur_map.SetAllLock(true); SetKey(); return 0;
				case IDM_EDIT_UNLOCK: message.cur_map.SetAllLock(false); SetKey(); return 0;

				
				case IDM_SOLVE_WORD:
					DialogBox(hInst,MAKEINTRESOURCE(IDD_WORD),hMainWnd,(DLGPROC)WordProc);
					return 0;

				case IDM_SOLVE_OPTIONS:
					DialogBox(hInst,MAKEINTRESOURCE(IDD_OPTIONS),hMainWnd,(DLGPROC)OptionsProc);
					return 0;

				case IDM_SOLVE_TP_HIGH: SetPriority(3); return 0;	
				case IDM_SOLVE_TP_NORM: SetPriority(2); return 0;	
				case IDM_SOLVE_TP_LOW: SetPriority(1); return 0;										

				

				case IDM_HELP_ABOUT:
					DialogBox(hInst,MAKEINTRESOURCE(IDD_ABOUT),hMainWnd,(DLGPROC)AboutProc);
					return 0;


				case IDC_CHANGE: ChangePlain(); return 0;

				case IDC_SOLVE:
					SetUndo();
					if(!bMsgLoaded) return 0;

					if(siSolveInfo.running) StopSolve();
					else StartSolve();
					return 0;

				case IDC_RESET: Reset(); return 0;


				case IDC_PATTERNS:
					switch(HIWORD(wParam))
					{
						case LBN_SELCHANGE:
							iCurPat=SendDlgItemMessage(hMainWnd,IDC_PATTERNS,LB_GETCURSEL,0,0);
							SetText();
					}
					return 0;
					
				case IDC_MAP:
					switch(HIWORD(wParam))
					{
						case LBN_SELCHANGE:
							iCurSymbol=SendDlgItemMessage(hMainWnd,IDC_MAP,LB_GETCURSEL,0,0);
							if(iCurSymbol<0) return 0;

							message.cur_map.GetSymbol(iCurSymbol,&symbol);
							sprintf(szText,"%c",symbol.plain);
							SetDlgItemText(hMainWnd,IDC_MAP_VALUE,szText);
							SetText();
							break;

						case LBN_DBLCLK:
							iCurSymbol=SendDlgItemMessage(hMainWnd,IDC_MAP,LB_GETCURSEL,0,0);
							if(iCurSymbol<0) return 0;
							message.cur_map.ToggleLock(iCurSymbol);
							UpdateSymbol(iCurSymbol);
							break;
					}
					return 0;

				case IDC_TS_UP:
					iCharSize+=float(0.1);
					SetCharSize();
					return 0;

				case IDC_TS_DOWN:
					iCharSize-=float(0.1);
					SetCharSize();
					return 0;
					
				case UDM_DISPALL:
					SetDlgInfo();
					return 0;
					
				case UDM_DISPINFO:
					SetSolve();
					return 0;
			}

			return 0;
			
		//scroll text
		case WM_VSCROLL:
			switch(LOWORD(wParam))
			{
				case SB_PAGEDOWN:	iScrollPos+=5; break;
				case SB_LINEDOWN:	iScrollPos++; break;
				case SB_PAGEUP:		iScrollPos-=5; break;
				case SB_LINEUP:		iScrollPos--; break;
				case SB_TOP:		iScrollPos=0; break;
				case SB_BOTTOM:		iScrollPos=iLines-iDispLines; break;
				case SB_THUMBTRACK:	iScrollPos=HIWORD(wParam); break;
			}
			
			if(iScrollPos<0) iScrollPos=0;
			if(iScrollPos>iMaxScroll) iScrollPos=iMaxScroll;
				
			SetScrollPos(hScroll,SB_CTL,iScrollPos,true);
			
			SetText();
			return 0;

		//click on text
		case WM_LBUTTONDOWN:
			TextClick(LOWORD(lParam),HIWORD(lParam));
			return 0;	

		//redraw window
		case WM_PAINT:
			BeginPaint(hWnd,&ps);
			DrawOutlines();
			EndPaint(hWnd,&ps);
			return 0;	
		
		//drag & drop loading of files
        case WM_DROPFILES:
			DragQueryFile((HDROP)wParam,0,szText,sizeof(szText));
			LoadMessage(szText);
			DragFinish((HDROP)wParam);
			return 0;
			
		case WM_CLOSE:
			EndDialog(hMainWnd,0);
			PostMessage(hMainWnd,WM_DESTROY,0,0);
			return 0;

		case WM_DESTROY: 
			PostQuitMessage(0); 
			return 0;
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpszCmdLine, int nCmdShow)
{
	int iTextFlags=WS_CHILD | SS_NOPREFIX | SS_LEFTNOWORDWRAP;
	
	hInst=hInstance;
	MSG Msg;
	
	srand(time(0));

	/*Initialize common controls*/
    /*INITCOMMONCONTROLSEX icInitCC;
    icInitCC.dwSize=sizeof(INITCOMMONCONTROLSEX);
    icInitCC.dwICC=ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icInitCC);

	if(!LoadLibrary(TEXT("riched32.dll")))
		MessageBox(NULL,"Could not load riched32.dll","Error",MB_ICONEXCLAMATION);*/

	/*create main window*/
	hMainWnd=CreateDialog(hInst,MAKEINTRESOURCE(IDD_MAIN_DLG),NULL,(DLGPROC)MainWndProc);
	SendMessage(hMainWnd,WM_SETICON,ICON_BIG,(WPARAM)LoadIcon(hInst,MAKEINTRESOURCE(IDI_ZODIAC)));
	hMainMenu=GetMenu(hMainWnd);

	/*cipher/plain text controls*/
	hScroll=GetDlgItem(hMainWnd,IDC_SCROLL);
	hCipher=CreateWindow("STATIC","",iTextFlags,CIPHER_X,CIPHER_Y,TEXT_WIDTH,TEXT_HEIGHT,hMainWnd,NULL,hInst,NULL);
	hPlain=CreateWindow("STATIC","",iTextFlags,PLAIN_X,PLAIN_Y,TEXT_WIDTH,TEXT_HEIGHT,hMainWnd,NULL,hInst,NULL);
	hCipherDC=GetWindowDC(hCipher);
	hPlainDC=GetWindowDC(hPlain);	
	
	/*drawing*/
	hRedPen=CreatePen(0,0,RGB(255,0,0));
	hGreenPen=CreatePen(0,0,RGB(0,196,0));
	hBluePen=CreatePen(0,0,RGB(0,0,255));
	SelectObject(hCipherDC,(HBRUSH)GetStockObject(NULL_BRUSH));
	SelectObject(hPlainDC,(HBRUSH)GetStockObject(NULL_BRUSH));
	SetCharSize();

	/*setup window*/
	SetTitle();
	MsgEnable(false);
	MapEnable(false);
	EnableMenuItem(hMainMenu,IDM_FILE_OPEN_MSG,MF_BYCOMMAND | MF_ENABLED);
	Button_Enable(GetDlgItem(hMainWnd,IDC_SOLVE),false);
	SendDlgItemMessage(hMainWnd,IDC_MAP_VALUE,EM_LIMITTEXT,1,0);
	SetScrollBar();
	SetPriority(2);
	StopSolve();
	
	/*init solve info*/
	memset(&siSolveInfo,0,sizeof(SOLVEINFO));
	siSolveInfo.max_fail=100;
	siSolveInfo.swaps=5;
	siSolveInfo.revert=120*5-1;
	siSolveInfo.disp_all=disp_all;
	siSolveInfo.disp_info=disp_info;
	siSolveInfo.time_func=GetTime;
	Reset();

	/*setup directories*/
	//executable's directory
	GetModuleFileName(hInst,szExeDir,1024);
	char *szEndDir;
	szEndDir=strrchr(szExeDir,'\\');
	if(szEndDir) *(szEndDir+1)='\0';
	
	//set open/save dir as executable dir
	strcpy(szCipherName,szExeDir);
	strcpy(szKeyName,szExeDir);
	
	//language
	iLang=0;
	SetLanguage();

	//show the windows
	ShowWindow(hMainWnd,SW_SHOWNORMAL);
	ShowWindow(hCipher,SW_SHOWNORMAL);
	ShowWindow(hPlain,SW_SHOWNORMAL);
	
	//Open From Command Line
	if(lpszCmdLine[0]!='\0') 
	{
		strcpy(szCipherName,lpszCmdLine); 
		message.Read(szCipherName);
	}

	//message loop
	while(GetMessage(&Msg,NULL,0,0))
		if(!IsDialogMessage(hMainWnd,&Msg))
		{	
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}

	return(Msg.wParam);
}