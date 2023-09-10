#ifndef HOT_RELOAD_OPENGL_PLATFORM_H
#define HOT_RELOAD_OPENGL_PLATFORM_H

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

#include <glad/gl.h>

#include "src/types.h"

struct GLFunctions {
    PFNGLVIEWPORTPROC viewport;
    PFNGLCLEARCOLORPROC clear_color;
    PFNGLCLEARPROC clear;
    PFNGLENABLEPROC enable;
    PFNGLGETERRORPROC get_error;
    PFNGLFINISHPROC finish;
};

struct ApplicationMemory {
    size_t permanent_storage_size;
    void *permanent_storage; // NOTE: Must be cleared to zero
};

struct ApplicationInput {
    i32 client_width;
    i32 client_height;
};

typedef void (__cdecl *UPDATE_AND_RENDER_PROC)(ApplicationMemory*, ApplicationInput*);
typedef void (__cdecl *LOAD_GL_FUNCTIONS_PROC)(GLFunctions*);

const u64 Permanent_Storage_Size = KiloBytes(1);

#endif //HOT_RELOAD_OPENGL_PLATFORM_H
