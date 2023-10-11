#include <Windows.h>
#include <cstdio>

#include <glad/gl.h>
#include <glad/wgl.h>

#include <platform/platform.h>
#include <platform/types.h>
#include <cassert>

struct ApplicationFunctions {
    HMODULE handle = nullptr;
    UPDATE_AND_RENDER_PROC update_and_render = nullptr;
    LOAD_GL_FUNCTIONS_PROC load_gl_functions = nullptr;
    LOAD_PLATFORM_FUNCTIONS_PROC load_platform_functions = nullptr;
    FILETIME last_loaded_dll_write_time = {0, 0};
};

const u32 Max_Path_Length = 512;

void path_to_wchar(const char *in, wchar_t destination[Max_Path_Length]) {
    assert(strlen(in) < Max_Path_Length);
    MultiByteToWideChar(CP_ACP, 0, in, -1, destination, Max_Path_Length / sizeof(destination[0]));
}

u64 win32_file_time_to_u64(const FILETIME &ft) {
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return uli.QuadPart;
}

u64 win32_file_size(const char *path) {
    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &file_info)) {
        printf("PLATFORM: (win32_file_size): unable to get file attributes for: '%s'.\n", path);
        return 0;
    }

    ULARGE_INTEGER size;
    size.HighPart = file_info.nFileSizeHigh;
    size.LowPart = file_info.nFileSizeLow;
    return size.QuadPart;
}

/// \param path Path to the file
/// \param read_buffer Buffer to put the content of the file into
/// \param buffer_size Number of bytes one wish to read, but remember to add one for the zero terminating character.
/// \return Success
bool win32_read_file(const char *path, char *read_buffer, const u64 buffer_size) {
    HANDLE file_handle;
    file_handle = CreateFileA(path,         // file to open
                              GENERIC_READ,          // open for reading
                              FILE_SHARE_READ,       // share for reading
                              nullptr,               // default security
                              OPEN_EXISTING,         // existing file only
                              FILE_ATTRIBUTE_NORMAL, // normal file
                              nullptr);                 // no attr. template

    if (file_handle == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        printf("Failed to open file %s. Error code: %lu\n", path, error);
        return false;
    }

    // Read one character less than the buffer size to save room for
    // the terminating NULL character.
    DWORD bytes_read = 0;
    if (!ReadFile(file_handle, read_buffer, buffer_size - 1, &bytes_read, nullptr)) {
        printf("Unable to read from file: %s\nGetLastError=%lu\n", path, GetLastError());
        CloseHandle(file_handle);
        return false;
    }

    assert(bytes_read == buffer_size - 1);
    read_buffer[buffer_size] = '\0';
    CloseHandle(file_handle);
    return true;
}

void win32_debug_print_readable_timestamp(u64 timestamp) {
    FILETIME file_time;
    file_time.dwLowDateTime = timestamp & 0xFFFFFFFF;
    file_time.dwHighDateTime = timestamp >> 32;

    SYSTEMTIME system_time;
    FILETIME local_file_time;

    FileTimeToLocalFileTime(&file_time, &local_file_time);
    FileTimeToSystemTime(&local_file_time, &system_time);

    printf("%02d:%02d %02d/%02d/%04d",
           system_time.wHour, system_time.wMinute,
           system_time.wDay, system_time.wMonth, system_time.wYear);
}

u64 win32_file_last_modified(const char *path) {
    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if (GetFileAttributesExA(path, GetFileExInfoStandard, &file_info)) {
        return win32_file_time_to_u64(file_info.ftLastWriteTime);
    } else {
        printf("PLATFORM (win32_file_last_modified): unable to get file attributes for: '%s'.\n", path);
        return 0;
    }
}

bool win32_should_reload_dll(ApplicationFunctions *app_functions) {
    if (app_functions->update_and_render == nullptr) {
        return true;
    }

    LPCTSTR path = R"(.\bin\Application.dll)";
    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if (GetFileAttributesEx(path, GetFileExInfoStandard, &file_info)) {
        auto result = CompareFileTime(&file_info.ftLastWriteTime, &app_functions->last_loaded_dll_write_time);
        return result > 0;
    } else {
        printf("Unable to open read time of '%s'.\n", path);
        return false;
    }
}

void win32_copy_dll() {
    LPCTSTR source = R"(.\bin\Application.dll)";
    LPCTSTR destination = R"(.\bin\Application_in_use.dll)";

    int num_retries = 0;
    while (!CopyFile(source, destination, FALSE) && num_retries < 20) {
        DWORD error = GetLastError();
        printf("Failed to copy %s to %s. Error code: %lu\n", source, destination, error);
        Sleep(100);
        num_retries++;
    }
}

void win32_load_dll(ApplicationFunctions *functions) {
    if (functions->handle != nullptr) {
        FreeLibrary(functions->handle);
        functions->handle = nullptr;

        int num_retries = 0;
        while (!DeleteFile(".\\bin\\Application_in_use.dll") && num_retries < 20) {
            Sleep(100);
            printf("Failed to delete temp .dll. Retrying...\n");
            num_retries++;
        }
    }

    win32_copy_dll();

    functions->handle = LoadLibrary(TEXT("bin\\Application_in_use.dll"));
    if (functions->handle == nullptr) {
        DWORD error = GetLastError();
        printf("Unable to load Application_in_use.dll. Error: %d\n", error);
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

    functions->load_platform_functions = (LOAD_PLATFORM_FUNCTIONS_PROC) GetProcAddress(functions->handle,
                                                                                       "load_platform_functions");
    if (functions->load_platform_functions == nullptr) {
        printf("Unable to load 'load_platform_functions' function in Application_in_use.dll\n");
        FreeLibrary(functions->handle);
    }

    LPCTSTR path = "bin\\Application.dll";
    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if (GetFileAttributesEx(path, GetFileExInfoStandard, &file_info)) {
        functions->last_loaded_dll_write_time = file_info.ftLastWriteTime;
    }
}

void win32_process_keyboard_message(ButtonState &new_state, bool is_down) {
    if (new_state.ended_down != is_down) {
        new_state.ended_down = is_down;
        new_state.half_transition_count++;
    }
}

void win32_process_pending_messages(HWND hwnd, bool &is_running, UserInput &new_input, UserInput &old_input) {
    MSG message;

    new_input.mouse.dx = 0;
    new_input.mouse.dy = 0;
    while (PeekMessage(&message, hwnd, 0, 0, PM_REMOVE)) {
        switch (message.message) {
            case WM_QUIT: {
                is_running = false;
            }
                break;
            case WM_INPUT: {
                UINT dwSize;

                GetRawInputData((HRAWINPUT) message.lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
                auto lpb = new BYTE[dwSize];
                if (GetRawInputData((HRAWINPUT) message.lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) !=
                    dwSize) {
                    // Error: couldn't get raw input data
                }

                auto *raw = (RAWINPUT *) lpb;

                if (raw->header.dwType == RIM_TYPEMOUSE) {
                    int xPosRelative = raw->data.mouse.lLastX;
                    int yPosRelative = raw->data.mouse.lLastY;
                    new_input.mouse.dx = xPosRelative;
                    new_input.mouse.dy = yPosRelative;
                    // Process the mouse movements...
                }

                delete[] lpb;
                break;
            }
            case WM_LBUTTONDOWN: {
            }
                break;
            case WM_LBUTTONUP: {
                break;
            }

            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                u32 vk_code = (u32) message.wParam;
                bool was_down = ((message.lParam & (1 << 30)) != 0);
                bool is_down = ((message.lParam & (1 << 31)) == 0);
                if (was_down != is_down) {
                    if (vk_code == 'W') {
                        win32_process_keyboard_message(new_input.move_up, is_down);
                    }
                    if (vk_code == 'A') {
                        win32_process_keyboard_message(new_input.move_left, is_down);
                    }
                    if (vk_code == 'S') {
                        win32_process_keyboard_message(new_input.move_down, is_down);
                    }
                    if (vk_code == 'D') {
                        win32_process_keyboard_message(new_input.move_right, is_down);
                    }
                    if (vk_code == VK_UP) {
                    }
                    if (vk_code == VK_DOWN) {
                    }
                    if (vk_code == VK_RIGHT) {
                    }
                    if (vk_code == VK_LEFT) {
                    }
                    if (vk_code == VK_ESCAPE) {
                        is_running = false;
                    }
                    if (vk_code == VK_SPACE) {
                        win32_process_keyboard_message(new_input.space, is_down);
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
    /* CREATE WINDOW */
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

    /*  CREATE OPEN_GL CONTEXT */
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
    } else {
        gl_funcs.attach_shader = glAttachShader;
        gl_funcs.bind_buffer_base = glBindBufferBase;
        gl_funcs.bind_vertex_array = glBindVertexArray;
        gl_funcs.clear = glClear;
        gl_funcs.clear_color = glClearColor;
        gl_funcs.compile_shader = glCompileShader;
        gl_funcs.create_buffers = glCreateBuffers;
        gl_funcs.create_program = glCreateProgram;
        gl_funcs.create_shader = glCreateShader;
        gl_funcs.create_vertex_arrays = glCreateVertexArrays;
        gl_funcs.delete_buffers = glDeleteBuffers;
        gl_funcs.delete_program = glDeleteProgram;
        gl_funcs.delete_shader = glDeleteShader;
        gl_funcs.delete_vertex_array = glDeleteVertexArrays;
        gl_funcs.draw_arrays = glDrawArrays;
        gl_funcs.enable = glEnable;
        gl_funcs.enable_vertex_array_attrib = glEnableVertexArrayAttrib;
        gl_funcs.finish = glFinish;
        gl_funcs.get_error = glGetError;
        gl_funcs.get_program_info_log = glGetProgramInfoLog;
        gl_funcs.get_shader_info_log = glGetShaderInfoLog;
        gl_funcs.get_uniform_location = glGetUniformLocation;
        gl_funcs.link_program = glLinkProgram;
        gl_funcs.named_buffer_storage = glNamedBufferStorage;
        gl_funcs.named_buffer_sub_data = glNamedBufferSubData;
        gl_funcs.polygon_mode = glPolygonMode;
        gl_funcs.shader_source = glShaderSource;
        gl_funcs.uniform_4f = glUniform4f;
        gl_funcs.use_program = glUseProgram;
        gl_funcs.vertex_array_attrib_binding = glVertexArrayAttribBinding;
        gl_funcs.vertex_array_attrib_format = glVertexArrayAttribFormat;
        gl_funcs.vertex_array_vertex_buffer = glVertexArrayVertexBuffer;
        gl_funcs.viewport = glViewport;
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

    /* MEMORY */
    ApplicationMemory memory = {};
    memory.permanent_storage_size = Permanent_Storage_Size;
    memory.transient_storage_size = Transient_Storage_Size;
    void *memory_block = VirtualAlloc(nullptr, // TODO: Might want to set this
                                      (SIZE_T) (memory.permanent_storage_size + memory.transient_storage_size),
                                      MEM_RESERVE | MEM_COMMIT,
                                      PAGE_READWRITE);
    if (memory_block == nullptr) {
        auto error = GetLastError();
        printf("Unable to allocate memory: %lu", error);
        return -1;
    }
    memory.permanent_storage = memory_block;
    memory.transient_storage = (u8 *) memory.permanent_storage + memory.permanent_storage_size;

    ApplicationInput app_input = {};

    ApplicationFunctions app_functions = {};

    /* INPUT */
    RAWINPUTDEVICE mouse;
    mouse.usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    mouse.usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
    mouse.dwFlags = RIDEV_NOLEGACY;    // adds mouse and also ignores legacy mouse messages
    mouse.hwndTarget = hwnd;

    if (RegisterRawInputDevices(&mouse, 1, sizeof(mouse)) == FALSE) {
        exit(1);
    }

    UserInput inputs[2] = {};
    u32 curr_input_idx = 0;
    auto *current_input = &inputs[curr_input_idx];
    auto *previous_input = &inputs[curr_input_idx + 1];


    Platform platform = {};
    platform.get_file_last_modified = &win32_file_last_modified;
    platform.get_file_size = &win32_file_size;
    platform.read_file = &win32_read_file;
    platform.debug_print_readable_timestamp = &win32_debug_print_readable_timestamp;

    /* MAIN LOOP */
    auto is_running = true;

    DWORD last_tick = GetTickCount();
    while (is_running) {
        DWORD this_tick = GetTickCount();
        app_input.dt = float(this_tick - last_tick) * 0.001f; // to seconds
        last_tick = this_tick;

        if (win32_should_reload_dll(&app_functions)) {
            printf("Hot reloading dll...\n");
            win32_load_dll(&app_functions);
            app_functions.load_gl_functions(&gl_funcs);
            app_functions.load_platform_functions(&platform);
        }

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        app_input.client_height = clientRect.bottom - clientRect.top;
        app_input.client_width = clientRect.right - clientRect.left;

        win32_process_pending_messages(hwnd, is_running, *current_input, *previous_input);
        app_input.input = current_input;
        app_functions.update_and_render(&memory, &app_input);

        SwapBuffers(hdc);

        curr_input_idx = curr_input_idx == 0 ? 1 : 0;
        current_input = &inputs[curr_input_idx];
        previous_input = &inputs[curr_input_idx == 0 ? 1 : 0];
        current_input->frame_clear(*previous_input);
    }

    return 0;
}