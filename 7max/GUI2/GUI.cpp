// GUI.cpp

#include "stdafx.h"

#include "Common/CommandLineParser.h"
#include "Common/StringConvert.h"
#include "Windows/Menu.h"
#include "Windows/Window.h"
#include "Windows/Security.h"

#include "resource.h"

// #include "ProcessList.h"
#include "RegistryUtils.h"
#include "RunProcess.h"
#include "../Common/7maxInstall.h"
#include "../Common/ProcessStatus.h"
#include "../Common/7maxCommandLine.h"
#include "../Common/InjectDLL3.h"

using namespace NWindows;

HINSTANCE g_hInstance;
// CProcessList g_ProcessList;
HWND g_HWND;
static bool g_Silent = false;

// #define MAX_LOADSTRING 100

void MessageBoxError(const wchar_t *s)
{
  if (!g_Silent)
    MessageBoxW(0, s, L"7-max", MB_ICONERROR);
}

static void SaveWindowInfo(HWND aWnd)
{
  WINDOWPLACEMENT placement;
  placement.length = sizeof(placement);
  if (!::GetWindowPlacement(aWnd, &placement))
    return;
  SaveWindowSize(placement.rcNormalPosition, 
      BOOLToBool(::IsZoomed(aWnd)));
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  TCHAR windowClass[100];
  lstrcpy(windowClass, TEXT("7-max"));
  const int kStringSize = 100;

  CSysString title = TEXT("7-max");

  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX); 
  // wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.style = 0;
  wcex.lpfnWndProc	= (WNDPROC)WndProc;
  wcex.cbClsExtra		= 0;
  wcex.cbWndExtra		= 0;
  wcex.hInstance		= hInstance;
  wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_GUI);
  wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground	= (HBRUSH)(
      // COLOR_WINDOW
      COLOR_BTNFACE
      + 1);
  // wcex.hbrBackground  = GetSysColorBrush(COLOR_3DFACE);

  wcex.lpszMenuName	= (LPCTSTR)IDM_MENU;
  wcex.lpszClassName	= windowClass;
  // wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
  wcex.hIconSm		= 0;
  ATOM myClass = RegisterClassEx(&wcex);

  RECT rect;
  bool maximized = false;
  int x , y, xSize, ySize;
  x = y = xSize = ySize = CW_USEDEFAULT;
  bool windowPosIsRead = ReadWindowSize(rect, maximized);
  if (windowPosIsRead)
  {
    xSize = rect.right - rect.left;
    ySize = rect.bottom - rect.top;
  }

  HWND hWnd;
  g_hInstance = hInstance;
  hWnd = CreateWindow(windowClass, title, WS_OVERLAPPEDWINDOW,
    x, y, xSize, ySize, NULL, NULL, hInstance, NULL);
  if (!hWnd)
  {
    return FALSE;
  }
  g_HWND = hWnd;
  
  CWindow window(hWnd);

  WINDOWPLACEMENT placement;
  placement.length = sizeof(placement);
  if (window.GetPlacement(&placement))
  {
    if (nCmdShow == SW_SHOWNORMAL || nCmdShow == SW_SHOW || 
        nCmdShow == SW_SHOWDEFAULT)
    {
      if (maximized)
        placement.showCmd = SW_SHOWMAXIMIZED;
      else
        placement.showCmd = SW_SHOWNORMAL;
    }
    else
      placement.showCmd = nCmdShow;
    if (windowPosIsRead)
      placement.rcNormalPosition = rect;
    window.SetPlacement(&placement);
    // window.Show(nCmdShow);
  }
  else
    window.Show(nCmdShow);
  
  // UpdateWindow(hWnd);
  return TRUE;
}

bool OnCreate(HWND hWnd)
{
  /*
  if (!g_ProcessList.Create(hWnd))
    return false;
  SetTimer(hWnd, 1, 1000, 0);
  */
  return true;
}

void OnSize(HWND hWnd)
{
  RECT rect;
  ::GetClientRect(hWnd, &rect);
  const int kHeaderSize = 0; // 29;
  const int kHeaderSize2 = 0; // 29;
  int xSize = rect.right;
  int ySize = MyMax(int(rect.bottom - kHeaderSize - kHeaderSize2), 0);
  // g_ProcessList.Move(0, kHeaderSize, xSize, ySize);
}

bool OnNotify(UINT controlID, LPNMHDR header, LRESULT &result) 
{ 
  /*
  if (header->hwndFrom == g_ProcessList._listView)
    return g_ProcessList.OnNotifyList(header, result);
  */
  return false; 
}

extern void OnMenuActivating(HWND hWnd, HMENU hMenu, int position);
extern bool OnMenuCommand(HWND hWnd, int id);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message) 
  {
		case WM_CREATE:
    {
      OnCreate(hWnd);
      break;
    }
		case WM_NOTIFY:
    {
      LRESULT result;
      if (OnNotify(wParam, (LPNMHDR) lParam, result))
        return result;
      break;
    }
		case WM_SIZE:
    {
      OnSize(hWnd);
      return 0;
    }
		case WM_TIMER:
    {
      // MessageBeep(-1);
      // g_ProcessList.RefreshList();
      return 0;
    }
		case WM_COMMAND:
    {
      int wmId = LOWORD(wParam); 
      int wmEvent = HIWORD(wParam); 
      if ((HWND) lParam != NULL && wmEvent != 0)
        break;
      if (wmId == IDM_EXIT)
      {
        DestroyWindow(hWnd);
        return 0;
      }
      if (::OnMenuCommand(hWnd, wmId))
        return 0;
      break;
    }
    case WM_INITMENUPOPUP:
      OnMenuActivating(hWnd, HMENU(wParam), LOWORD(lParam));
      break;
    case WM_DESTROY:
      // g_ProcessList.OnDestroy();
      SaveWindowInfo(hWnd);
      PostQuitMessage(0);
      return 0;
    case WM_SETFOCUS:
      // g_ProcessList.SetFocusToLastItem();
      break;

    /*
    case kRefreshButtons:
      g_ProcessList._dialogButtons.UpdateButtons();
      return 0;
    */
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

int APIENTRY WinMain2(HINSTANCE hInstance, int nCmdShow)
{
  InitCommonControls();

  // if (ChechFirstTime() && args.Length() == 0)
  Check7maxConditions();
  NSecurity::AddLockMemoryPrivilege();


  CCommandLineOptions options;
  {
    CCommandLineParser parser;
    parser.Parse(options);
  }
  g_Silent = options.Silent;

  if (options.HelpMode)
  {
    return 0;
  }

  // Check7maxConditions();
  InitProcessStatus();

  if (options.IsProcessIdDefined)
  {
    if (!InjectToProcess(options.ProcessId))
      throw "can't inject";
    return 0;
  }

  if (options.Defrag.Enabled)
  {
    // Defrag(options.Defrag.SizeIsDefined, options.Defrag.Size);
  }

  if (options.Arguments.Length() != 0)
  {
    if (!StartProcessVia7max(options.Arguments))
      throw L"Can't run";
    return 0;
  }

  StartProcessVia7max(options.Arguments, true);


 	// TODO: Place code here.
  MSG msg;
  HACCEL hAccelTable;

  // Perform application initialization:
  if (!InitInstance (hInstance, nCmdShow)) 
  {
    return FALSE;
  }
  
  hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDR_ACCELERATOR1);
  
  // Main message loop:
  while (GetMessage(&msg, NULL, 0, 0)) 
  {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return msg.wParam;
}

int APIENTRY WinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  try { return WinMain2(hInstance, nCmdShow); }
  catch(const CSysString &s) { MessageBoxError(s);}
  catch(const char *s) { MessageBoxError(GetUnicodeString(s));}
  catch(const wchar_t *s) { MessageBoxError(s);}
  catch (...) { MessageBoxError(L"Unspecified error"); }
  return FALSE;
}

