#include <Windows.h>
#include <cstdio>

#include <glad/gl.h>
#include <glad/wgl.h>

#include "platform.h"
#include "src/types.h"

struct ApplicationFunctions {
    HMODULE handle = nullptr;
    UPDATE_AND_RENDER_PROC update_and_render = nullptr;
    LOAD_GL_FUNCTIONS_PROC load_gl_functions = nullptr;
    FILETIME last_loaded_dll_write_time = {0, 0};
};

bool win32_should_reload_dll(ApplicationFunctions *app_functions) {
    if (app_functions->update_and_render == nullptr) {
        return true;
    }

    LPCTSTR path = "Application.dll";
    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if (GetFileAttributesEx(path, GetFileExInfoStandard, &file_info)) {
        auto result = CompareFileTime(&file_info.ftLastWriteTime, &app_functions->last_loaded_dll_write_time);
        return result > 0;
    }
    else {
        printf("Unable to open read time of '%s'.\n", path);
        return false;
    }
}

void win32_copy_dll() {
    LPCTSTR source = "Application.dll";
    LPCTSTR destination = "Application_in_use.dll";

    int num_retries = 0;
    while (!CopyFile(source, destination, FALSE) && num_retries < 20) {
        printf("Failed to copy %s to %s", source, destination);
        Sleep(100);
        num_retries++;
    }
}

void win32_load_dll(ApplicationFunctions *functions) {
    if (functions->handle != nullptr) {
        FreeLibrary(functions->handle);
        functions->handle = nullptr;

        int num_retries = 0;
        while(!DeleteFile("Application_in_use.dll") && num_retries < 20) {
            Sleep(100);
            printf("Failed to delete temp .dll. Retrying...\n");
            num_retries++;
        }
    }

    win32_copy_dll();

    functions->handle = LoadLibrary(TEXT("Application_in_use.dll"));
    if (functions->handle == nullptr) {
        printf("Unable to load Application_in_use.dll\n");
        functions->update_and_render = nullptr;
        return;
    }

    functions->update_and_render = (UPDATE_AND_RENDER_PROC) GetProcAddress(functions->handle, "update_and_render");
    if (functions->update_and_render == nullptr) {
        printf("Unable to load 'update_and_render' function in Application_in_use.dll\n");
        FreeLibrary(functions->handle);
    }

    functions->load_gl_functions = (LOAD_GL_FUNCTIONS_PROC) GetProcAddress(functions->handle, "load_gl_functions");
    if (functions->load_gl_functions == nullptr) {
        printf("Unable to load 'load_gl_functions' function in Application_in_use.dll\n");
        FreeLibrary(functions->handle);
    }

    LPCTSTR path = "Application.dll";
    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if (GetFileAttributesEx(path, GetFileExInfoStandard, &file_info)) {
        functions->last_loaded_dll_write_time = file_info.ftLastWriteTime;
    }
}

void win32_process_pending_messages(HWND hwnd, bool &is_running) {
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


    HDC hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    // Create OpenGL context
    HGLRC tempRC = wglCreateContext(hdc);
    wglMakeCurrent(hdc, tempRC);
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");

    const int attribList[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB,
            3,
            WGL_CONTEXT_MINOR_VERSION_ARB,
            3,
            WGL_CONTEXT_FLAGS_ARB,
            0,
            WGL_CONTEXT_PROFILE_MASK_ARB,
            WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0,
    };
    HGLRC hglrc = wglCreateContextAttribsARB(hdc, nullptr, attribList);

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(tempRC);
    wglMakeCurrent(hdc, hglrc);

    GLFunctions gl_funcs = {};
    if (!gladLoaderLoadGL()) {
        printf("Could not initialize GLAD\n");
        exit(1);
    }
    else {
        gl_funcs.viewport = glViewport;
        gl_funcs.clear_color = glClearColor;
        gl_funcs.clear = glClear;
        gl_funcs.enable = glEnable;
        gl_funcs.get_error = glGetError;
        gl_funcs.finish = glFinish;
    }

    auto _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress(
            "wglGetExtensionsStringEXT");
    bool swapControlSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;

    int vsynch = 0;
    if (swapControlSupported) {
        // TODO: Remove auto?
        auto wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress(
                "wglSwapIntervalEXT");
        auto wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC) wglGetProcAddress(
                "wglGetSwapIntervalEXT");

        if (wglSwapIntervalEXT(1)) {
            vsynch = wglGetSwapIntervalEXT();
        } else {
            printf("Could not enable vsync\n");
        }
    } else { // !swapControlSupported
        printf("WGL_EXT_swap_control not supported\n");
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    ApplicationMemory memory = {};
    memory.permanent_storage_size = Permanent_Storage_Size;
    memory.permanent_storage = VirtualAlloc(nullptr, // TODO: Might want to set this
                                          (SIZE_T)memory.permanent_storage_size,
                                          MEM_RESERVE|MEM_COMMIT,
                                          PAGE_READWRITE);
    if (memory.permanent_storage == nullptr) {
        auto error = GetLastError();
        printf("Unable to allocate memory: %lu", error);
        return -1;
    }

    ApplicationInput app_input = {};

    ApplicationFunctions app_functions = {};

    auto is_running = true;
    while (is_running) {
        if (win32_should_reload_dll(&app_functions)) {
            printf("Hot reloading dll...\n");
            win32_load_dll(&app_functions);
            app_functions.load_gl_functions(&gl_funcs);
        }

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        app_input.client_height = clientRect.bottom - clientRect.top;
        app_input.client_width = clientRect.right - clientRect.left;

        win32_process_pending_messages(hwnd, is_running);
        app_functions.update_and_render(&memory, &app_input);

        SwapBuffers(hdc);
    }

    return 0;
}