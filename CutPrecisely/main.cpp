//WindowsProject2.cpp: 定义应用程序的入口点。
//
//#define debug
#ifdef debug
#pragma comment( linker, "/subsystem:windows /entry:wWinMainCRTStartup" )
#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )
#pragma comment( linker, "/subsystem:console /entry:mainCRTStartup" )
#pragma comment( linker, "/subsystem:console /entry:wWinMainCRTStartup" )
#endif

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Pathcch.lib")
#pragma comment(lib,"Shcore.lib")

#include "Windows.h"
#include "resource.h"
#include "Shlobj.h"
#include "Shlwapi.h"
#include "Pathcch.h"
#include "stdio.h"
#include "ShellScalingAPI.h"
#include "wchar.h"


INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DialogProc2(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID CALLBACK WaitOrTimerCallback(PVOID   lpParameter, BOOLEAN TimerOrWaitFired);
VOID CALLBACK WaitOrTimerCallback2(PVOID   lpParameter, BOOLEAN TimerOrWaitFired);
HWND CreateToolTip(int toolID, HWND hDlg, PTSTR pszText);
void maketime(wchar_t *);
void DeleteFrames();


wchar_t yuanwenjian[MAX_PATH];
wchar_t yuanwenjianming[MAX_PATH];
wchar_t linshimulu[MAX_PATH];
wchar_t linshiwenjian[MAX_PATH];
wchar_t linshiwenjianming[MAX_PATH];
wchar_t shuchumulu[MAX_PATH];
wchar_t shuchuwenjian[MAX_PATH];
wchar_t shuchuwenjianming[MAX_PATH];
wchar_t ModulePath[MAX_PATH];
wchar_t ImagePath[MAX_PATH];
wchar_t DirOfFrames[MAX_PATH];


#define cmdlength 10000
wchar_t cmdline[cmdlength];


HANDLE hNewWaitObject;
HINSTANCE g_hInstance;
HWND g_hwndDlg;
HBITMAP h_checked;
HBITMAP h_indeterminate;
HBITMAP h_unchecked;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					  _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
	GetModuleFileName(NULL, ModulePath, MAX_PATH);
	PathRemoveFileSpec(ModulePath);
	wcscat_s(ModulePath, MAX_PATH, L"\\ffmpeg.exe");
	g_hInstance = hInstance;
	h_checked = LoadBitmap(hInstance, (LPCTSTR)IDB_checked);
	h_indeterminate = LoadBitmap(hInstance, (LPCTSTR)IDB_indeterminate);
	h_unchecked = LoadBitmap(hInstance, (LPCTSTR)IDB_unchecked);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProc);
	return 0;
}


INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OPENFILENAME ofn;
	BROWSEINFO bi;
	static int hw2sta = BST_CHECKED;
	g_hwndDlg = hwndDlg;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hwndDlg, IDC_ICQ1, WM_SETTEXT, 0, (LPARAM)L"85");
		SendDlgItemMessage(hwndDlg, IDC_ICQ2, WM_SETTEXT, 0, (LPARAM)L"25");
		SendDlgItemMessage(hwndDlg, IDC_HWACCEL1, BM_SETCHECK, BST_CHECKED, 0);
		SendDlgItemMessage(hwndDlg, IDC_RADIO1, BM_SETCHECK, BST_CHECKED, 0);
		SendDlgItemMessage(hwndDlg, IDC_SPIN1, UDM_SETRANGE32, 0, 9999999);
		SendDlgItemMessage(hwndDlg, IDC_SPIN2, UDM_SETRANGE32, 0, 9999999);
		CreateToolTip(IDC_EDIT_qishitime, hwndDlg, (PTSTR)L"HHMMSS");
		CreateToolTip(IDC_EDIT_jieshutime, hwndDlg, (PTSTR)L"HHMMSS");
		CreateToolTip(IDC_EDIT_qishitime2, hwndDlg, (PTSTR)L"HHMMSS.mmm");
		CreateToolTip(IDC_EDIT_jieshutime2, hwndDlg, (PTSTR)L"HHMMSS.mmm");
		CreateToolTip(IDC_ICQ1, hwndDlg, (PTSTR)L"The higher the better");
		CreateToolTip(IDC_ICQ2, hwndDlg, (PTSTR)L"The lower the better");
		CreateToolTip(IDC_biansu, hwndDlg, (PTSTR)L"[0.5, 2.0]");
		CreateToolTip(IDC_qishizhen, hwndDlg, (PTSTR)L"Start from zero");

		WINDOWINFO wi;
		ZeroMemory(&wi, sizeof(WINDOWINFO));
		wi.cbSize = sizeof(WINDOWINFO);
		GetWindowInfo(GetDlgItem(hwndDlg, IDC_HWACCEL2), &wi);
		wi.dwStyle = WS_CLIPCHILDREN | wi.dwStyle;
		SetWindowLong(GetDlgItem(hwndDlg, IDC_HWACCEL2), GWL_STYLE, wi.dwStyle);
		static HWND h_check;
		h_check = GetDlgItem(hwndDlg, IDC_check);
		SetParent(h_check, GetDlgItem(hwndDlg, IDC_HWACCEL2));
		return true;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hwndDlg, wParam);
			return TRUE;
		case IDC_HWACCEL1:
			if (IsDlgButtonChecked(hwndDlg, IDC_HWACCEL1))
				EnableWindow(GetDlgItem(hwndDlg, IDC_ICQ1), TRUE);
			else
				EnableWindow(GetDlgItem(hwndDlg, IDC_ICQ1), FALSE);
			return true;
		case IDC_HWACCEL2:
			if ((BN_CLICKED == HIWORD(wParam)) || (BN_DBLCLK == HIWORD(wParam)))
			{
				switch (hw2sta)
				{
				case BST_CHECKED:
					hw2sta = BST_INDETERMINATE;
					SendMessage(h_check, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)h_indeterminate);
					return true;
				case BST_INDETERMINATE:
					hw2sta = BST_UNCHECKED;
					SendMessage(h_check, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)h_unchecked);
					return true;
				case BST_UNCHECKED:
					hw2sta = BST_CHECKED;
					SendMessage(h_check, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)h_checked);
					return true;
				}
			}
			return false;
		case IDC_CHECK1:
			if (IsDlgButtonChecked(hwndDlg, IDC_CHECK1))
			{
				EnableWindow(GetDlgItem(hwndDlg, IDC_RW), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_RH), TRUE);
			}
			else
			{
				EnableWindow(GetDlgItem(hwndDlg, IDC_RW), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_RH), FALSE);
			}
			return true;
		case IDC_RADIO1:
		case IDC_RADIO2:
			if (IsDlgButtonChecked(hwndDlg, IDC_RADIO1))
			{
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT_jieshutime2), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT_qishitime2), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_jieshuzhen), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_qishizhen), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_zhenlv), TRUE);
			}
			else
			{
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT_jieshutime2), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT_qishitime2), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_jieshuzhen), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_qishizhen), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_zhenlv), FALSE);
			}
			return true;
		case IDC_BUTTON_shuru:
		{
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwndDlg;
			ofn.lpstrFile = yuanwenjian;
			ofn.lpstrFile[0] = L'\0';
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = L"All\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT;
			if (GetOpenFileName(&ofn))
				SendDlgItemMessage(hwndDlg, IDC_EDIT_shuru, WM_SETTEXT, 0, (LPARAM)yuanwenjian);
			return TRUE;
		}
		case IDC_BUTTON_dierbushuru:
		{
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwndDlg;
			ofn.lpstrFile = linshiwenjian;
			ofn.lpstrFile[0] = L'\0';
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = L"All\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT;
			if (GetOpenFileName(&ofn))
				SendDlgItemMessage(hwndDlg, IDC_EDIT_dierbushuru, WM_SETTEXT, 0, (LPARAM)linshiwenjian);
			wcscpy_s(linshiwenjianming, MAX_PATH, PathFindFileName(linshiwenjian));
			return TRUE;
		}
		case IDC_BUTTON_linshi:
		{
			ZeroMemory(&bi, sizeof(bi));
			bi.hwndOwner = hwndDlg;
			bi.pidlRoot = NULL;
			bi.pszDisplayName = linshimulu;
			linshimulu[0] = L'\0';
			bi.lpszTitle = L"Temp Directory:";
			bi.ulFlags = 0;
			bi.lpfn = NULL;
			bi.lParam = 0;
			LPITEMIDLIST lpDlist;
			lpDlist = SHBrowseForFolder(&bi);
			SHGetPathFromIDList(lpDlist, linshimulu);
			CoTaskMemFree(lpDlist);
			SendDlgItemMessage(hwndDlg, IDC_EDIT_linshi, WM_SETTEXT, 0, (LPARAM)linshimulu);
			return true;
		}
		case IDC_BUTTON_shuchu:
		{
			ZeroMemory(&bi, sizeof(bi));
			bi.hwndOwner = hwndDlg;
			bi.pidlRoot = NULL;
			bi.pszDisplayName = shuchumulu;
			shuchuwenjian[0] = L'\0';
			bi.lpszTitle = L"Output:";
			bi.ulFlags = 0;
			bi.lpfn = NULL;
			bi.lParam = 0;
			LPITEMIDLIST lpDlist;
			lpDlist = SHBrowseForFolder(&bi);
			SHGetPathFromIDList(lpDlist, shuchumulu);
			CoTaskMemFree(lpDlist);
			wcscpy_s(shuchuwenjian, MAX_PATH, shuchumulu);
			PathCchAddBackslash(shuchuwenjian, MAX_PATH);
			wcscpy_s(shuchuwenjianming, MAX_PATH, linshiwenjianming);
			PathCchRenameExtension(shuchuwenjianming, MAX_PATH, L".mp4");
			wcscat_s(shuchuwenjian, MAX_PATH, shuchuwenjianming);
			SendDlgItemMessage(hwndDlg, IDC_EDIT_shuchu, WM_SETTEXT, 0, (LPARAM)shuchuwenjian);
			return TRUE;
		}
		case IDC_BUTTON_step1:
		{
			yuanwenjian[0] = L'\0';
			SendDlgItemMessage(hwndDlg, IDC_EDIT_shuru, WM_GETTEXT, MAX_PATH, (LPARAM)yuanwenjian);
			if (yuanwenjian[0] == L'\0')
			{
				MessageBox(hwndDlg, L"There is no input.", NULL, MB_OK | MB_ICONERROR);
				return true;
			}
			wcscpy_s(yuanwenjianming, MAX_PATH, PathFindFileName(yuanwenjian));
			PathQuoteSpaces(yuanwenjian);
			/////////////
			SendDlgItemMessage(hwndDlg, IDC_EDIT_linshi, WM_GETTEXT, MAX_PATH, (LPARAM)linshimulu);
			if (linshimulu[0] == L'\0')
			{
				MessageBox(hwndDlg, L"There is no temp directory.", NULL, MB_OK | MB_ICONERROR);
				return true;
			}
			wcscpy_s(linshiwenjian, MAX_PATH, linshimulu);
			PathCchAddBackslash(linshiwenjian, MAX_PATH);
			wcscpy_s(linshiwenjianming, MAX_PATH, yuanwenjianming);
			PathRemoveExtension(linshiwenjianming);
			wcscat_s(linshiwenjianming, MAX_PATH, L"_");

			wchar_t yiqishi[15] = L"\0";
			SendDlgItemMessage(hwndDlg, IDC_EDIT_qishitime, WM_GETTEXT, 15, (LPARAM)yiqishi);
			wcscat_s(linshiwenjianming, MAX_PATH, yiqishi);
			wcscat_s(linshiwenjianming, MAX_PATH, L".mov");
			wcscat_s(linshiwenjian, MAX_PATH, linshiwenjianming);
			PathQuoteSpaces(linshiwenjian);
			//////////////
			//wcscpy_s(cmdline, cmdlength, ModulePath);
			wcscpy_s(cmdline, cmdlength, L" -y ");

			int IsHW1 = IsDlgButtonChecked(hwndDlg, IDC_HWACCEL1);
			if (IsHW1)
				wcscat_s(cmdline, cmdlength, L" -hwaccel dxva2 -hwaccel_output_format dxva2_vld ");


			if (yiqishi[0] != L'\0')
			{
				maketime(yiqishi);
				wcscat_s(cmdline, cmdlength, L" -ss ");
				wcscat_s(cmdline, cmdlength, yiqishi);
			}

			wchar_t yijieshu[15] = L"\0";
			SendDlgItemMessage(hwndDlg, IDC_EDIT_jieshutime, WM_GETTEXT, 15, (LPARAM)yijieshu);
			if (yijieshu[0] != L'\0')
			{
				maketime(yijieshu);
				wcscat_s(cmdline, cmdlength, L" -to ");
				wcscat_s(cmdline, cmdlength, yijieshu);
			}

			wcscat_s(cmdline, cmdlength, L" -i ");
			wcscat_s(cmdline, cmdlength, yuanwenjian);

			if (IsHW1)
			{
				wcscat_s(cmdline, cmdlength, L" -vf hwmap=derive_device=qsv,format=qsv -c:v mjpeg_qsv -global_quality ");
				wchar_t icq[8] = L"\0";
				SendDlgItemMessage(hwndDlg, IDC_ICQ1, WM_GETTEXT, 8, (LPARAM)icq);
				if (icq[0] != L'\0')
					wcscat_s(cmdline, cmdlength, icq);
				else
					wcscat_s(cmdline, cmdlength, L" 85 ");
			}
			else
				wcscat_s(cmdline, cmdlength, L" -c:v mjpeg -q:v 1 ");

			wcscat_s(cmdline, cmdlength, L" -c:a pcm_s16le ");
			wcscat_s(cmdline, cmdlength, linshiwenjian);
			////////////
			if ((GetKeyState(VK_SHIFT) < 0) && (GetKeyState(VK_CONTROL) < 0) &&
				(GetKeyState(VK_MENU) < 0))
				DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), hwndDlg, DialogProc2);
			else
			{
				wchar_t ErrMsg[200] = L"\0";
				DWORD errID;
				STARTUPINFO si;
				PROCESS_INFORMATION pi;
				ZeroMemory(&si, sizeof(si));
				ZeroMemory(&pi, sizeof(pi));
				GetStartupInfo(&si);
				si.cb = sizeof(si);
				if (!CreateProcess(ModulePath, cmdline, NULL, NULL, FALSE, ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
				{
					errID = GetLastError();
					swprintf_s(ErrMsg, 200, L"ErrCode:%d", errID);
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								  NULL, errID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
								  ErrMsg + wcslen(ErrMsg), 200 - wcslen(ErrMsg), NULL);
					MessageBox(hwndDlg, ErrMsg, NULL, MB_OK | MB_ICONERROR);
					return true;
				}
				EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON_step1), FALSE);
				RegisterWaitForSingleObject(&hNewWaitObject, pi.hProcess, WaitOrTimerCallback, hwndDlg, INFINITE, WT_EXECUTEDEFAULT | WT_EXECUTEONLYONCE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
			return true;
		}
		case IDC_BUTTON_step2:
		{
			linshiwenjian[0] = L'\0';
			SendDlgItemMessage(hwndDlg, IDC_EDIT_dierbushuru, WM_GETTEXT, MAX_PATH,
							   (LPARAM)linshiwenjian);
			if (linshiwenjian[0] == L'\0')
			{
				MessageBox(hwndDlg, L"There is no temp file.", NULL, MB_OK | MB_ICONERROR);
				return false;
			}

			int IsMOV = !wcscmp(PathFindExtension(linshiwenjian), L".mov");

			PathQuoteSpaces(linshiwenjian);
			///////////////
			shuchuwenjian[0] = L'\0';
			SendDlgItemMessage(hwndDlg, IDC_EDIT_shuchu, WM_GETTEXT, MAX_PATH, (LPARAM)shuchuwenjian);
			if (shuchuwenjian[0] == L'\0')
			{
				MessageBox(hwndDlg, L"There is no output.", NULL, MB_OK | MB_ICONERROR);
				return false;
			}
			PathQuoteSpaces(shuchuwenjian);
			////////////////////////////
			//wcscpy_s(cmdline, cmdlength, ModulePath);
			wcscpy_s(cmdline, cmdlength, L" -y ");

			if (BST_CHECKED == hw2sta)
				if (IsMOV)
					wcscat_s(cmdline, cmdlength, L" -hwaccel qsv -c:v mjpeg_qsv ");
				else
					wcscat_s(cmdline, cmdlength, L" -hwaccel dxva2 -hwaccel_output_format dxva2_vld ");
			else if (BST_INDETERMINATE == hw2sta)
				wcscat_s(cmdline, cmdlength, L" -init_hw_device qsv=hw -filter_hw_device hw ");

			wchar_t strzhenlv[9] = L"\0", erqishi[15] = L"\0", erjieshu[15] = L"\0";
			int qishizhen, jieshuzhen;
			double zhenlv = 0.0;
			BOOL b;
			if (IsDlgButtonChecked(hwndDlg, IDC_RADIO1))
			{
				SendDlgItemMessage(hwndDlg, IDC_zhenlv, WM_GETTEXT, 9, (LPARAM)strzhenlv);
				if (strzhenlv[0] != L'\0')
				{
					zhenlv = _wtof(strzhenlv);
					qishizhen = GetDlgItemInt(hwndDlg, IDC_qishizhen, &b, FALSE);
					if ((b == TRUE) && (qishizhen != 0))
						swprintf_s(cmdline + wcslen(cmdline), cmdlength - wcslen(cmdline),
								   L" -ss %02d:%02d:%02d.%03d ",
								   (int)(1000.0 / zhenlv * qishizhen / 1000 / 3600),
								   ((int)(1000.0 / zhenlv * qishizhen / 1000) % 3600) / 60,
								   (int)(1000.0 / zhenlv * qishizhen / 1000) % 60,
								   (int)(1000.0 / zhenlv * qishizhen) % 1000);
					jieshuzhen = GetDlgItemInt(hwndDlg, IDC_jieshuzhen, &b, FALSE);
					if ((b == TRUE) && (jieshuzhen != 0))
						swprintf_s(cmdline + wcslen(cmdline), cmdlength - wcslen(cmdline),
								   L" -to %02d:%02d:%02d.%03d ",
								   (int)(1000.0 / zhenlv * jieshuzhen / 1000 / 3600),
								   ((int)(1000.0 / zhenlv * jieshuzhen / 1000) % 3600) / 60,
								   (int)(1000.0 / zhenlv * jieshuzhen / 1000) % 60,
								   (int)(1000.0 / zhenlv * jieshuzhen + 1) % 1000);
				}
			}
			else
			{
				SendDlgItemMessage(hwndDlg, IDC_EDIT_qishitime2, WM_GETTEXT, 15, (LPARAM)erqishi);
				if (erqishi[0] != L'\0')
				{
					maketime(erqishi);
					wcscat_s(cmdline, cmdlength, L" -ss ");
					wcscat_s(cmdline, cmdlength, erqishi);
				}
				SendDlgItemMessage(hwndDlg, IDC_EDIT_jieshutime2, WM_GETTEXT, 15, (LPARAM)erjieshu);
				if (erjieshu[0] != L'\0')
				{
					maketime(erjieshu);
					wcscat_s(cmdline, cmdlength, L" -to ");
					wcscat_s(cmdline, cmdlength, erjieshu);
				}
			}

			wcscat_s(cmdline, cmdlength, L" -i ");
			wcscat_s(cmdline, cmdlength, linshiwenjian);

			int IsVF = 0;

			if ((BST_CHECKED == hw2sta) && (!IsMOV))
			{
				wcscat_s(cmdline, cmdlength, L" -vf hwmap=derive_device=qsv,format=qsv");
				IsVF = 1;
			}
			else if (BST_INDETERMINATE == hw2sta)
			{
				wcscat_s(cmdline, cmdlength, L" -vf hwupload=extra_hw_frames=64,format=qsv");
				IsVF = 1;
			}

			wchar_t rw[8], rh[8];
			if (IsDlgButtonChecked(hwndDlg, IDC_CHECK1))
			{
				SendDlgItemMessage(hwndDlg, IDC_RW, WM_GETTEXT, 8, (LPARAM)rw);
				SendDlgItemMessage(hwndDlg, IDC_RH, WM_GETTEXT, 8, (LPARAM)rh);
				if (!(rw[0] && rh[0]))
				{
					MessageBox(hwndDlg, L"There is no resolution.", NULL, MB_OK | MB_ICONERROR);
					return false;
				}
				if (BST_CHECKED == hw2sta)
					if (IsMOV)
					{
						swprintf_s(cmdline + wcslen(cmdline), cmdlength - wcslen(cmdline),
								   L" -vf scale_qsv=mode=hq:w=%ls:h=%ls", rw, rh);
						IsVF = 1;
					}
					else
						swprintf_s(cmdline + wcslen(cmdline), cmdlength - wcslen(cmdline),
								   L",scale_qsv=mode=hq:w=%ls:h=%ls", rw, rh);
				else if (BST_INDETERMINATE == hw2sta)
					swprintf_s(cmdline + wcslen(cmdline), cmdlength - wcslen(cmdline),
							   L",scale_qsv=mode=hq:w=%ls:h=%ls", rw, rh);
				else
				{
					swprintf_s(cmdline + wcslen(cmdline), cmdlength - wcslen(cmdline),
							   L" -vf scale=w=%ls:h=%ls", rw, rh);
					IsVF = 1;
				}
			}

			wchar_t speed[9] = L"\0";
			SendDlgItemMessage(hwndDlg, IDC_biansu, WM_GETTEXT, 8, (LPARAM)speed);
			if (speed[0] != L'\0')
			{
				if (IsVF)
					swprintf_s(cmdline + wcslen(cmdline), cmdlength - wcslen(cmdline),
							   L",setpts=%.5f*PTS -af atempo=%ls ", 1.0 / _wtof(speed), speed);
				else
					swprintf_s(cmdline + wcslen(cmdline), cmdlength - wcslen(cmdline),
							   L" -vf setpts=%.5f*PTS -af atempo=%ls ", 1.0 / _wtof(speed), speed);
			}

			if (hw2sta)
			{
				wcscat_s(cmdline, cmdlength, L" -c:v h264_qsv -global_quality ");
				wchar_t icq2[8] = L"\0";
				SendDlgItemMessage(hwndDlg, IDC_ICQ2, WM_GETTEXT, 8, (LPARAM)icq2);
				if (icq2[0] != L'\0')
					wcscat_s(cmdline, cmdlength, icq2);
				else
					wcscat_s(cmdline, cmdlength, L" 25 ");
				wcscat_s(cmdline, cmdlength, L" -look_ahead 1 -preset veryslow ");
			}
			else
				wcscat_s(cmdline, cmdlength, L" -c:v libx264 -crf 18 ");

			wcscat_s(cmdline, cmdlength, L" -c:a aac -ar 48000 -ac 2 -ab 128k ");
			wcscat_s(cmdline, cmdlength, shuchuwenjian);

			if ((GetKeyState(VK_SHIFT) < 0) && (GetKeyState(VK_CONTROL) < 0) &&
				(GetKeyState(VK_MENU) < 0))
				DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), hwndDlg, DialogProc2);
			else
			{
				wchar_t ErrMsg[200] = L"\0";
				DWORD errID;
				STARTUPINFO si;
				PROCESS_INFORMATION pi;
				ZeroMemory(&si, sizeof(si));
				ZeroMemory(&pi, sizeof(pi));
				GetStartupInfo(&si);
				si.cb = sizeof(si);
				if (!CreateProcess(ModulePath, cmdline, NULL, NULL, FALSE, ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
				{
					errID = GetLastError();
					swprintf_s(ErrMsg, 200, L"ErrCode:%d", errID);
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								  NULL, errID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
								  ErrMsg + wcslen(ErrMsg), 200 - wcslen(ErrMsg), NULL);
					MessageBox(hwndDlg, ErrMsg, NULL, MB_OK | MB_ICONERROR);
					return true;
				}
				EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON_step2), FALSE);
				RegisterWaitForSingleObject(&hNewWaitObject, pi.hProcess, WaitOrTimerCallback2,
											(PVOID)(long)zhenlv,
											INFINITE, WT_EXECUTEDEFAULT | WT_EXECUTEONLYONCE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
			return true;
		}
		case IDC_BUTTON_shanchulinshi:
		{
			if (IDYES == MessageBox(hwndDlg, L"Delete temp files?", L"Confirmation",
									MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2))
			{
				PathUnquoteSpaces(linshiwenjian);
				DeleteFile(linshiwenjian);
				PathUnquoteSpaces(ImagePath);
				DeleteFile(ImagePath);
				if (DirOfFrames[0] != L'\0')
				{
					DeleteFrames();
					RemoveDirectory(DirOfFrames);
				}
			}
			return true;
		}
		case IDC_BUTTON_gaiming:
		{
			static int filecounter = 0;
			wchar_t oldname[MAX_PATH] = L"\0", newname[MAX_PATH] = L"\0", fileextension[10] = L"\0";
			SendDlgItemMessage(hwndDlg, IDC_EDIT_shuchu, WM_GETTEXT, MAX_PATH, (LPARAM)oldname);
			wcscpy_s(newname, MAX_PATH, oldname);
			wcscpy_s(fileextension, 10, PathFindExtension(newname));
			PathRemoveFileSpec(newname);
			swprintf_s(newname, MAX_PATH, L"%ls%03d%ls", newname, filecounter, fileextension);
			if (_wrename(oldname, newname))
				MessageBox(hwndDlg, L"Rename failed.", NULL, MB_OK | MB_ICONERROR);
			else
			{
				filecounter++;
				SendDlgItemMessage(hwndDlg, IDC_BUTTON_gaiming, WM_SETTEXT, 0, (LPARAM)L"Rename");
				MessageBox(hwndDlg, L"Rename successfully", L"Done", MB_OK);
			}
			return true;
		}
		case IDC_qishizhen: //stay with IDC_jieshuzhen
		case IDC_jieshuzhen:
		{
			if (HIWORD(wParam) == EN_CHANGE)
				SendDlgItemMessage(hwndDlg, IDC_BUTTON_gaiming, WM_SETTEXT, 0, (LPARAM)L"Rename*");
			return true;
		}
		default:return false;
		}
	case WM_DROPFILES:
		POINT pt, ptmouse;
		RECT rc;
		DragQueryPoint((HDROP)wParam, &ptmouse);
		GetWindowRect(GetDlgItem(hwndDlg, IDC_EDIT_shuru), &rc);
		pt.x = rc.left;
		pt.y = rc.top;
		ScreenToClient(hwndDlg, &pt);
		if ((ptmouse.x > pt.x) && (ptmouse.y > pt.y) && (ptmouse.x < pt.x + rc.right - rc.left) && (ptmouse.y < pt.y + rc.bottom - rc.top))
		{
			DragQueryFile((HDROP)wParam, 0, yuanwenjian, MAX_PATH);
			DragFinish((HDROP)wParam);
			SendDlgItemMessage(hwndDlg, IDC_EDIT_shuru, WM_SETTEXT, 0, (LPARAM)yuanwenjian);
		}
		else
		{
			GetWindowRect(GetDlgItem(hwndDlg, IDC_EDIT_dierbushuru), &rc);
			pt.x = rc.left;
			pt.y = rc.top;
			ScreenToClient(hwndDlg, &pt);
			if ((ptmouse.x > pt.x) && (ptmouse.y > pt.y) && (ptmouse.x < pt.x + rc.right - rc.left) && (ptmouse.y < pt.y + rc.bottom - rc.top))
			{
				DragQueryFile((HDROP)wParam, 0, linshiwenjian, MAX_PATH);
				DragFinish((HDROP)wParam);
				SendDlgItemMessage(hwndDlg, IDC_EDIT_dierbushuru, WM_SETTEXT, 0, (LPARAM)linshiwenjian);
				wcscpy_s(linshiwenjianming, MAX_PATH, PathFindFileName(linshiwenjian));
			}
		}
		return true;
	}
	return FALSE;
}

VOID CALLBACK WaitOrTimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	UnregisterWait(hNewWaitObject);
	
	DirOfFrames[0] = L'\0';
	if (IsDlgButtonChecked((HWND)lpParameter, IDC_FRAMES))
	{
		wcscpy_s(DirOfFrames, MAX_PATH, linshiwenjian);
		PathUnquoteSpaces(DirOfFrames);
		PathCchRemoveExtension(DirOfFrames, MAX_PATH);
		CreateDirectory(DirOfFrames, NULL);
		swprintf_s(cmdline, cmdlength, L" -y -i %ls -an -vsync 0 -f image2 -start_number 0 \"%ls\\%%09d.jpg\"", linshiwenjian, DirOfFrames);
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		GetStartupInfo(&si);
		si.cb = sizeof(si);
		CreateProcess(ModulePath, cmdline, NULL, NULL, FALSE, ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL,
					  &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		ShellExecute(NULL, L"open", DirOfFrames, NULL, NULL, SW_SHOWNORMAL);
	}

	EnableWindow(GetDlgItem((HWND)lpParameter, IDC_BUTTON_step1), true);
	PathUnquoteSpaces(linshiwenjian);
	SendDlgItemMessage((HWND)lpParameter, IDC_EDIT_dierbushuru, WM_SETTEXT, 0, (LPARAM)linshiwenjian);

	SendDlgItemMessage((HWND)lpParameter, IDC_EDIT_shuchu, WM_GETTEXT, 0, (LPARAM)shuchuwenjian);
	if (shuchuwenjian[0] == L'\0')
		return;
	wcscpy_s(shuchuwenjianming, MAX_PATH, linshiwenjianming);
	PathCchRenameExtension(shuchuwenjianming, MAX_PATH, L".mp4");
	PathUnquoteSpaces(shuchuwenjian);
	PathRemoveFileSpec(shuchuwenjian);
	PathCchAddBackslash(shuchuwenjian, MAX_PATH);
	wcscat_s(shuchuwenjian, MAX_PATH, shuchuwenjianming);
	SendDlgItemMessage((HWND)lpParameter, IDC_EDIT_shuchu, WM_SETTEXT, 0, (LPARAM)shuchuwenjian);
	return;
}

VOID CALLBACK WaitOrTimerCallback2(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	UnregisterWait(hNewWaitObject);

	int lines = (((long)lpParameter) == 0) ? 30 : ((long)lpParameter);
	lines = lines / 6 + 1;
	wcscpy_s(ImagePath, MAX_PATH, shuchuwenjian);
	PathUnquoteSpaces(ImagePath);
	PathCchRenameExtension(ImagePath, MAX_PATH, L".jpg");
	swprintf_s(cmdline, cmdlength, L" -y -t 0.5 -i %ls -sseof -0.5 -i %ls -filter_complex  concat=n=2,tile=6x%d:padding=20:margin=20 -an -vsync 0 -f image2 \"%ls\"", shuchuwenjian, shuchuwenjian, lines, ImagePath);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	GetStartupInfo(&si);
	si.cb = sizeof(si);
	CreateProcess(ModulePath, cmdline, NULL, NULL, FALSE, ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL,
				  &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	EnableWindow(GetDlgItem(g_hwndDlg, IDC_BUTTON_step2), true);
	ShellExecute(NULL, L"open", ImagePath, NULL, NULL, SW_SHOWNORMAL);
	return;
}

void maketime(wchar_t *time)
{
	wchar_t wcmd[15] = L"\0";
	wchar_t *t = time, *p = t + wcslen(time);

	wcsncat_s(wcmd, 15, t, 2);
	t = t + 2;
	wcscat_s(wcmd, 15, L":");
	wcsncat_s(wcmd, 15, t, 2);
	t = t + 2;
	wcscat_s(wcmd, 15, L":");
	wcsncat_s(wcmd, 15, t, 2);
	t = t + 2;
	while ((*t) != L'\0')
	{
		wcsncat_s(wcmd, 15, t, 1);
		t++;
	}
	wcscpy_s(time, 15, wcmd);
	return;
}

INT_PTR CALLBACK DialogProc2(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const wchar_t *url = L"https://trac.ffmpeg.org/wiki/Hardware/QuickSync";
	const wchar_t *url2 = L"http://blog.dword1511.info/?p=2913";
	const wchar_t *url3 = L"https://trac.ffmpeg.org/wiki/Encode/H.264";
	const wchar_t *url4 = L"https://superuser.com/questions/1259059/ffmpeg-h264-qsv-encoder-and-crf-issues";

	const wchar_t *opts = L"STEP1\r\nSW+SW\r\nffmpeg -y -ss HH:MM:SS -to HH:MM:SS -i \"input\" -c:v mjpeg -q:v 1 -c:a pcm_s16le \"output.mov\"\r\n\r\nHW+HW\r\nffmpeg -y -hwaccel dxva2 -hwaccel_output_format dxva2_vld -ss HH:MM:SS -to HH:MM:SS -i \"input\" -vf hwmap=derive_device=qsv,format=qsv -c:v mjpeg_qsv -global_quality 85 -c:a pcm_s16le \"output.mov\"\r\n\r\nSTEP2\r\nSW+SW\r\nffmpeg -y -ss HH:MM:SS.mmm -to HH:MM:SS.mmm -i \"input\" -vf scale=w=200:h=100,setpts=1.429*PTS -af atempo=0.7 -c:v libx264 -crf 18 -c:a aac -ar 48000 -ac 2 -ab 128k \"output\"\r\n\r\nHW+HW\r\nffmpeg -y -hwaccel qsv -c:v mjpeg_qsv -ss HH:MM:SS.mmm -to HH:MM:SS.mmm -i \"input\" -vf scale_qsv=mode=hq:w=1920:h=1080,setpts=1.429*PTS -af atempo=0.7 -c:v h264_qsv -global_quality 25 -look_ahead 1 -preset veryslow -c:a aac -ar 48000 -ac 2 -ab 128k \"output\"\r\n\r\nSW+HW\r\nffmpeg -init_hw_device qsv=hw -filter_hw_device hw -i \"input\" -vf hwupload=extra_hw_frames=64,format=qsv,scale_qsv=mode=hq:w=1920:h=1080,setpts=1.429*PTS -af atempo=0.7 -c:v h264_qsv -global_quality 25 -look_ahead 1 -preset veryslow -c:a aac -ar 48000 -ac 2 -ab 128k \"output\"";

	wchar_t buf[cmdlength] = L"\0";

	switch (uMsg)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hwndDlg, IDC_EDIT2, WM_SETTEXT, 0, (LPARAM)opts);
		swprintf_s(buf, cmdlength, L"\"%ls\" %ls", ModulePath, cmdline);
		SendDlgItemMessage(hwndDlg, IDC_EDIT1, WM_SETTEXT, 0, (LPARAM)buf);
		return true;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hwndDlg, wParam);
			return TRUE;
		case IDC_BUTTON1:
			ShellExecute(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL);
			ShellExecute(NULL, L"open", url2, NULL, NULL, SW_SHOWNORMAL);
			ShellExecute(NULL, L"open", url3, NULL, NULL, SW_SHOWNORMAL);
			ShellExecute(NULL, L"open", url4, NULL, NULL, SW_SHOWNORMAL);
			return true;
		}
	}
	return false;
}

HWND CreateToolTip(int toolID, HWND hDlg, PTSTR pszText)
{
	if (!toolID || !hDlg || !pszText)
	{
		return FALSE;
	}
	// Get the window of the tool.
	HWND hwndTool = GetDlgItem(hDlg, toolID);
	// Create the tooltip. g_hInst is the global instance handle.
	HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
								  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
								  hDlg, NULL, g_hInstance, NULL);
	if (!hwndTool || !hwndTip)
	{
		return (HWND)NULL;
	}
	 // Associate the tooltip with the tool.
	TOOLINFO toolInfo = { 0 };
	toolInfo.cbSize = sizeof(toolInfo) - sizeof(void *);
	toolInfo.hwnd = hDlg;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)hwndTool;
	toolInfo.lpszText = pszText;
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
	return hwndTip;
}

void DeleteFrames()
{
	wchar_t p[MAX_PATH + 1];
	swprintf_s(p, MAX_PATH, L"%ls\\*", DirOfFrames);
	p[wcslen(p) + 1] = L'\0';
	SHFILEOPSTRUCTW FileOpS = { 0 };
	FileOpS.wFunc = FO_DELETE;
	FileOpS.pFrom = p;
	FileOpS.fFlags = FOF_NOCONFIRMATION;
	SHFileOperation(&FileOpS);
	return;
}
