// OpenMPTest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "OpenMPTest.h"

#include <time.h>
#include <ppl.h>
#include <array>

#define MAX_LOADSTRING 100
#define SWAP(a,b) { int t;t=a;a=b;b=t; }

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND  hMainWnd;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OPENMPTEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OPENMPTEST));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OPENMPTEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OPENMPTEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


std::array<int, 10000> Items10K;
std::array<int, 7500>  Items7K;
std::array<int, 5000>  Items5K;
std::array<int, 2500>  Items2K;




clock_t startTime;
WCHAR wszElapsedTime[255];
LONG BubbleTime10K, InsertionTime10K, BubbleParTime10K, InsertionParTime10K;
LONG BubbleTime7K, InsertionTime7K, BubbleParTime7K, InsertionParTime7K;
LONG BubbleTime5K, InsertionTime5K, BubbleParTime5K, InsertionParTime5K;
LONG BubbleTime2K, InsertionTime2K, BubbleParTime2K, InsertionParTime2K;
LONG GetElasedTime()
{
	return (clock() - startTime);
}


//Init Items values
template <typename T>
void InitItems(T& Items)
{
	for (int i = 0; i < (int)Items.size(); i++)
		Items[i] = i;
}

//Shuffle Items to sort them. 
template <typename T>
void ShuffleItems(T& Items)
{
	std::random_shuffle(Items.begin(), Items.end());
}




template <typename T, typename TIME>
void BubbleSort(T& Items, TIME & time)
{
	ShuffleItems<T>(Items);
	startTime = clock();
	int minidx, j;

	for (int i = 0; i < (int)Items.size() - 1; i++)
	{

		for (minidx = i, j = i + 1; j<(int)Items.size();j++)
		{
			if (Items[minidx] > Items[j])
				minidx = j;
		}

		if (minidx != i)
			SWAP(Items[minidx], Items[i]);
	}
	time = GetElasedTime();
}

template <typename T, typename TIME>
void InsertionSort(T& Items, TIME & time)
{
	ShuffleItems<T>(Items);
	startTime = clock();
	int j;
	for (int i = 1; i < (int)Items.size(); i++)
	{
		int remember = Items[(j = i)];


		while (--j >= 0 && remember < Items[j]) Items[j + 1] = Items[j];
		Items[j + 1] = remember;
	}
	time = GetElasedTime();
}

template <typename T, typename TIME>
void BubbleParallelSort(T& Items, TIME & time)
{
	ShuffleItems<T>(Items);
	startTime = clock();


	concurrency::parallel_for(0, (int) Items.size() - 1, 
		[&](int i)
		{
			int minidx, j;
			for (minidx = i, j = i + 1; j < (int) Items.size();j++)
			{
				if (Items[minidx] > Items[j])
					minidx = j;
			}

			if (minidx != i)
				SWAP(Items[minidx], Items[i]);
		}
	);

	time = GetElasedTime();
}

template <typename T, typename TIME>
void InsertionParallelSort(T& Items, TIME & time)
{
	ShuffleItems<T>(Items);
	startTime = clock();

	concurrency::parallel_for(1, (int)Items.size(), [&](int i)
	{
		int j = i;
		int remember = Items[j];

		while (--j >= 0 && remember < Items[j]) Items[j + 1] = Items[j];
		Items[j + 1] = remember;

	}
	);

	time = GetElasedTime();
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   hMainWnd = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);


   return TRUE;
}


// Drawing function
const COLORREF clrBubble = RGB(50, 50, 255), clrBubblePar = RGB(255, 0, 111), clrInsertion = RGB(0, 50, 30), clrInsertionPar = RGB(0, 150, 0);
void DrawTimeEllipse(HDC hdc, int x, LONG time, COLORREF clr)
{
	HBRUSH hBrush, OldBrush;


	hBrush = CreateSolidBrush(clr);
	OldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	Ellipse(hdc, x - 5, time / 10 + 5, x + 5, time / 10 - 5);
	DeleteObject(SelectObject(hdc, OldBrush));
}

void DrawComparisonChart(HDC hdc)
{
	SetMapMode(hdc, MM_LOENGLISH);
	SetViewportOrgEx(hdc, 80, 500, NULL);

	MoveToEx(hdc, 0, 0, NULL);
	LineTo(hdc, 1050, 0);
	MoveToEx(hdc, 0, 0, NULL);
	LineTo(hdc, 0, 550);

	static LPWSTR strX = L"Number of Items";
	static LPWSTR strY = L"Elapsed Time(ms)";
	WCHAR  wsz[255];
	TextOutW(hdc, 1100, -10, strX, lstrlenW(strX));
	TextOutW(hdc, -40, 590, strY, lstrlenW(strY));

	lstrcpyW(wsz, L"Bubble");
	TextOutW(hdc, 100, 650, wsz, lstrlenW(wsz));
	DrawTimeEllipse(hdc, 50, 6400, clrBubble);

	lstrcpyW(wsz, L"BubbleParallel");
	TextOutW(hdc, 250, 650, wsz, lstrlenW(wsz));
	DrawTimeEllipse(hdc, 220, 6400, clrBubblePar);

	lstrcpyW(wsz, L"Insertion");
	TextOutW(hdc, 450, 650, wsz, lstrlenW(wsz));
	DrawTimeEllipse(hdc, 420, 6400, clrInsertion);

	lstrcpyW(wsz, L"InsertionParallel");
	TextOutW(hdc, 650, 650, wsz, lstrlenW(wsz));
	DrawTimeEllipse(hdc, 620, 6400, clrInsertionPar);

	for (int i = 1; i <= 4; i++)
	{
		MoveToEx(hdc, 250 * i, 0 , NULL);
		LineTo(hdc, 250 * i, -20);
		wsprintfW(wsz, L"%d", 250 * i * 10);
		TextOutW(hdc, 250 * i - 25, -30, wsz, lstrlenW(wsz));

		MoveToEx(hdc, 0, 100 * i, NULL);
		LineTo(hdc, -10, 100 * i);
		wsprintfW(wsz, L"%d", 100 * i * 10);
		TextOutW(hdc, -70, 100 * i + 10, wsz, lstrlenW(wsz));
	}

	//Draw Elapsed Time 

	//10k Items
	DrawTimeEllipse(hdc, 1000, BubbleTime10K, clrBubble);
	DrawTimeEllipse(hdc, 1000, InsertionTime10K, clrInsertion);
	DrawTimeEllipse(hdc, 1000, BubbleParTime10K, clrBubblePar);
	DrawTimeEllipse(hdc, 1000, InsertionParTime10K, clrInsertionPar);

	DrawTimeEllipse(hdc, 750, BubbleTime7K, clrBubble);
	DrawTimeEllipse(hdc, 750, InsertionTime7K, clrInsertion);
	DrawTimeEllipse(hdc, 750, BubbleParTime7K, clrBubblePar);
	DrawTimeEllipse(hdc, 750, InsertionParTime7K, clrInsertionPar);

	DrawTimeEllipse(hdc, 500, BubbleTime5K, clrBubble);
	DrawTimeEllipse(hdc, 500, InsertionTime5K, clrInsertion);
	DrawTimeEllipse(hdc, 500, BubbleParTime5K, clrBubblePar);
	DrawTimeEllipse(hdc, 500, InsertionParTime5K, clrInsertionPar);


	DrawTimeEllipse(hdc, 250, BubbleTime2K, clrBubble);
	DrawTimeEllipse(hdc, 250, InsertionTime2K, clrInsertion);
	DrawTimeEllipse(hdc, 250, BubbleParTime2K, clrBubblePar);
	DrawTimeEllipse(hdc, 250, InsertionParTime2K, clrInsertionPar);
}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

LPCTSTR str = L"It will take some time";
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
	case WM_CREATE:
		InitItems<std::array<int, 10000>>(Items10K);
		InitItems<std::array<int, 7500>>(Items7K);
		InitItems<std::array<int, 5000>>(Items5K);
		InitItems<std::array<int, 2500>>(Items2K);
		break;
	case WM_MOVE:
		break;
	case WM_ACTIVATE:
		MoveWindow(hMainWnd, 50, 50, 1000, 800, TRUE);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_GETSTARTED_SINGLETHREAD:
				
				SetWindowTextW(hMainWnd, str);
				SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

				BubbleSort<std::array<int, 10000>>(Items10K, BubbleTime10K);
				BubbleSort<std::array<int, 7500>>(Items7K, BubbleTime7K);
				BubbleSort<std::array<int, 5000>>(Items5K, BubbleTime5K);
				BubbleSort<std::array<int, 2500>>(Items2K, BubbleTime2K);

				InsertionSort<std::array<int, 10000>>(Items10K, InsertionTime10K);
				InsertionSort<std::array<int, 7500>>(Items7K, InsertionTime7K);
				InsertionSort<std::array<int, 5000>>(Items5K, InsertionTime5K);
				InsertionSort<std::array<int, 2500>>(Items2K, InsertionTime2K);
				

				BubbleParallelSort<std::array<int, 10000>>(Items10K, BubbleParTime10K);
				BubbleParallelSort<std::array<int, 7500>>(Items7K, BubbleParTime7K);
				BubbleParallelSort<std::array<int, 5000>>(Items5K, BubbleParTime5K);
				BubbleParallelSort<std::array<int, 2500>>(Items2K, BubbleParTime2K);
				

				InsertionParallelSort<std::array<int, 10000>>(Items10K, InsertionParTime10K);
				InsertionParallelSort<std::array<int, 7500>>(Items7K, InsertionParTime7K);
				InsertionParallelSort<std::array<int, 5000>>(Items5K, InsertionParTime5K);
				InsertionParallelSort<std::array<int, 2500>>(Items2K, InsertionParTime2K);

				InvalidateRect(hMainWnd, NULL, TRUE);
				SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
				break;

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;

            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
			//Drawing Graphs
			DrawComparisonChart(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
	//case WM_TIMER:
	//	DrawingFunction();
	//	break;
    case WM_DESTROY:
		KillTimer(hMainWnd, 0);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
