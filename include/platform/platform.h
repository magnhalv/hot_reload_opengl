#ifndef HOT_RELOAD_OPENGL_PLATFORM_H
#define HOT_RELOAD_OPENGL_PLATFORM_H

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

#include <glad/gl.h>

#include "types.h"
#include "user_input.h"

struct GLFunctions {
    PFNGLATTACHSHADERPROC attach_shader;
    PFNGLBINDBUFFERBASEPROC bind_buffer_base;
    PFNGLBINDVERTEXARRAYPROC bind_vertex_array;
    PFNGLCLEARCOLORPROC clear_color;
    PFNGLCLEARPROC clear;
    PFNGLCOMPILESHADERPROC compile_shader;
    PFNGLCREATEBUFFERSPROC create_buffers;
    PFNGLCREATEPROGRAMPROC create_program;
    PFNGLCREATESHADERPROC create_shader;
    PFNGLCREATEVERTEXARRAYSPROC create_vertex_arrays;
    PFNGLDELETEBUFFERSPROC delete_buffers;
    PFNGLDELETEPROGRAMPROC delete_program;
    PFNGLDELETESHADERPROC delete_shader;
    PFNGLDELETEVERTEXARRAYSPROC delete_vertex_array;
    PFNGLDRAWARRAYSPROC draw_arrays;
    PFNGLENABLEPROC enable;
    PFNGLENABLEVERTEXARRAYATTRIBPROC enable_vertex_array_attrib;
    PFNGLFINISHPROC finish;
    PFNGLGETERRORPROC get_error;
    PFNGLGETPROGRAMINFOLOGPROC get_program_info_log;
    PFNGLGETSHADERINFOLOGPROC get_shader_info_log;
    PFNGLGETUNIFORMLOCATIONPROC get_uniform_location;
    PFNGLLINKPROGRAMPROC link_program;
    PFNGLNAMEDBUFFERSTORAGEPROC named_buffer_storage;
    PFNGLNAMEDBUFFERSUBDATAPROC named_buffer_sub_data;
    PFNGLPOLYGONMODEPROC polygon_mode;
    PFNGLSHADERSOURCEPROC shader_source;
    PFNGLUNIFORM4FPROC uniform_4f;
    PFNGLUSEPROGRAMPROC use_program;
    PFNGLVERTEXARRAYATTRIBBINDINGPROC vertex_array_attrib_binding;
    PFNGLVERTEXARRAYATTRIBFORMATPROC vertex_array_attrib_format;
    PFNGLVERTEXARRAYVERTEXBUFFERPROC vertex_array_vertex_buffer;
    PFNGLVIEWPORTPROC viewport;
};

const u32 Gl_Invalid_Id = 0;

// Functions platform MUST support
typedef u64 (*GET_FILE_LAST_MODIFIED_PROC)(const char *file_path);
typedef bool (*READ_FILE_PROC)(const char *, char *, const u64);
typedef u64 (*GET_FILE_SIZE_PROC)(const char *);
typedef void (*DEBUG_PRINT_READABLE_TIMESTAMP_PROC) (u64);

struct Platform {
    GET_FILE_LAST_MODIFIED_PROC get_file_last_modified;
    READ_FILE_PROC read_file;
    GET_FILE_SIZE_PROC get_file_size;
    DEBUG_PRINT_READABLE_TIMESTAMP_PROC debug_print_readable_timestamp;
};

struct ApplicationMemory {
    size_t permanent_storage_size = 0;
    void *permanent_storage = nullptr; // NOTE: Must be cleared to zero
    size_t transient_storage_size = 0;
    void *transient_storage = nullptr; // NOTE: Must be cleared to zero
};

struct ApplicationInput {
    i32 client_width;
    i32 client_height;
    f32 dt;
    UserInput *input;
};

// Functions application MUST support
typedef void (__cdecl *UPDATE_AND_RENDER_PROC)(ApplicationMemory *, ApplicationInput *);
typedef void (__cdecl *LOAD_GL_FUNCTIONS_PROC)(GLFunctions *);
typedef void (__cdecl *LOAD_PLATFORM_FUNCTIONS_PROC)(Platform *);

const u64 Permanent_Storage_Size = MegaBytes(10);
const u64 Transient_Storage_Size = MegaBytes(1);

#endif //HOT_RELOAD_OPENGL_PLATFORM_H
