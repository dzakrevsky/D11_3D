#include <windows.h>
#include <vfw.h>
//#pragma comment(lib,"vfw32.lib")

HINSTANCE g_hMsvfw32;
typedef HWND(VFWAPIV* MCIWNDCREATE)(HWND hwndParent, HINSTANCE hInstance, DWORD dwStyle, LPCSTR szFile);
MCIWNDCREATE pMCIWndCreate;

void KillDynamicLibraries()
{
    if (g_hMsvfw32) {
        FreeLibrary(g_hMsvfw32);
        g_hMsvfw32 = NULL;
    }
}

BOOL InitDynamicLibraries()
{
    BOOL Result = -1;
    g_hMsvfw32 = LoadLibrary("msvfw32.dll");
    if (!g_hMsvfw32) return Result;
    ++Result;
    pMCIWndCreate = (MCIWNDCREATE)GetProcAddress(g_hMsvfw32, "MCIWndCreateA");
    if (pMCIWndCreate) ++Result;
    return Result;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    if (1 > InitDynamicLibraries()) {
        KillDynamicLibraries();
        ExitProcess(0);
    }

    HWND hWnd = HWND_DESKTOP;
    char svFilename[] = "Videos\\Op.mp4";

    DWORD dwStyle;
    dwStyle = WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_VISIBLE |
        WS_POPUP | WS_THICKFRAME | WS_MAXIMIZE |
        MCIWNDF_NOERRORDLG | MCIWNDF_NOTIFYMODE |
        MCIWNDF_NOPLAYBAR | MCIWNDF_NOOPEN | MCIWNDF_NOMENU;

    HWND hwndMCI;
    hwndMCI = pMCIWndCreate(hWnd, hInstance, dwStyle, svFilename);
    if (!hwndMCI) {
        MessageBox(hWnd, "Cannot MCIWndCreate", "Error", MB_OK);
        KillDynamicLibraries();
        return -1;
    }
    SendMessage(hwndMCI, MCIWNDM_SETZOOM, (WPARAM)0, (LPARAM)(UINT)400);

    MCIERROR err;
    err = SendMessage(hwndMCI, MCI_PLAY, (WPARAM)0, (LPARAM)0);
    if (0 != err) {
        MessageBox(hWnd, "Cannot MCI_PLAY", "Error", MB_OK);
        DestroyWindow(hwndMCI);
        KillDynamicLibraries();
        return -1;
    }

    ShowCursor(FALSE);

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    while (!GetAsyncKeyState(VK_END) &&
        !GetAsyncKeyState(VK_ESCAPE)) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            char ptr[3];
            long lMode = SendMessage(hwndMCI, MCIWNDM_GETMODE, (WPARAM)(UINT)sizeof(ptr), (LPARAM)(LPSTR)ptr);
            if (MCI_MODE_STOP == lMode ||
                MCI_MODE_NOT_READY == lMode)
                break;
        }
    }

    ShowCursor(TRUE);
    KillDynamicLibraries();
    return (int)msg.wParam;
}