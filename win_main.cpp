#include <Windows.h>
#include <cstdio>

#include "src/types.h"
#include "src/application.h"

static bool is_running = true;


bool win32_should_reload_dll(FILETIME prev_dll_last_write_time) {
    HANDLE h_file = CreateFile(TEXT("Application.dll"), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                               nullptr);

    if (h_file == INVALID_HANDLE_VALUE) {
        printf("Unable to open Application.dll\n");
        return false;
    }

    FILETIME creation_time, last_access_time, last_write_time;
    if (!GetFileTime(h_file, &creation_time, &last_access_time, &last_write_time)) {
        printf("Unable to open read time of Application.dll\n");
        return false;
    }

    return CompareFileTime(&last_write_time, &prev_dll_last_write_time) < 0;
}

void win32_process_pending_messages(HWND hwnd) {
    MSG message;
    while (PeekMessage(&message, hwnd, 0, 0, PM_REMOVE)) {
        switch (message.message) {
            case WM_QUIT: {
                is_running = false;
            }
                break;
            case WM_INPUT: {
                break;
                case WM_LBUTTONDOWN: {
                }
                break;
                case WM_LBUTTONUP: {
                    break;
                }
            }
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                u32 vk_code = (u32) message.wParam;
                bool was_down = ((message.lParam & (1 << 30)) != 0);
                bool is_down = ((message.lParam & (1 << 31)) == 0);
                if (was_down != is_down) {
                    if (vk_code == VK_ESCAPE) {
                        is_running = false;
                    }
                }
            }
                break;
            default:
                TranslateMessage(&message);
                DispatchMessageA(&message);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    switch (iMsg) {
        case WM_CLOSE:
        case WM_DESTROY: {
            is_running = false;
        }
            break;
        case WM_PAINT:
        case WM_ERASEBKGND:
        default:
            return DefWindowProc(hwnd, iMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

#include <errhandlingapi.h>

#if _DEBUG
#pragma comment(linker, "/subsystem:console")

int main() {
    return WinMain(GetModuleHandle(nullptr), nullptr, GetCommandLineA(), SW_SHOWDEFAULT);
}

#else
#pragma comment(linker, "/subsystem:windows")
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    WNDCLASSEX wndclass;
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.lpfnWndProc = WndProc; // Needed, otherwise it crashes
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndclass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
    wndclass.lpszMenuName = nullptr;
    wndclass.lpszClassName = "Win32 Game Window";
    RegisterClassEx(&wndclass);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int client_width = 1280;
    int client_height = 720;
    RECT windowRect;
    SetRect(&windowRect, (screenWidth / 2) - (client_width / 2), (screenHeight / 2) - (client_height / 2),
            (screenWidth / 2) + (client_width / 2), (screenHeight / 2) + (client_height / 2));

    DWORD style = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX |
                   WS_MAXIMIZEBOX); // WS_THICKFRAME to resize
    AdjustWindowRectEx(&windowRect, style, FALSE, 0);
    HWND hwnd = CreateWindowEx(0, wndclass.lpszClassName, "Game Window", style, windowRect.left, windowRect.top,
                               windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr,
                               hInstance, szCmdLine);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    FILETIME last_loaded_dll_write_time = { 0, 0};

    while (is_running) { // NOLINT
        auto should_reload_dll = win32_should_reload_dll(last_loaded_dll_write_time);
        if (should_reload_dll) {
            printf("Should reload\n");
        }
        else {
            printf("Should NOT reload\n");
        }
        win32_process_pending_messages(hwnd);
        update_and_render();
    }


    return 0;
}