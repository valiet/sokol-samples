
#include <stdio.h>
#include <string.h>

#include <psp2/kernel/clib.h> 
#include <psp2/kernel/processmgr.h>
#include <psp2/gxm.h>

#include <pib.h>

#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// ANGLE functions not supported. error: arm-dolce-eabi/bin/ld.exe: main.c:(.text+0x5e1a): undefined reference to `glDrawArraysInstancedANGLE'
#undef GL_ANGLE_instanced_arrays 
#define SOKOL_IMPL
#define SOKOL_GLES2
#include "sokol_gfx.h"
#include "sokol_time.h"


int main(unsigned int args, void *argp) {
/*     
*   Always initialize PIB before callling any EGL/GLES functions 
*   Enable the ShaccCg Shader Compiler 'PIB_SHACCCG' and Enabled -nostdlib support 'PIB_NOSTDLIB'(No need if you don't use -nostdlib)
*/
    pibInit(PIB_SHACCCG );

    EGLDisplay egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint majorVersion;
    EGLint minorVersion;
    eglInitialize(egl_display, &majorVersion, &minorVersion);  

    eglBindAPI(EGL_OPENGL_ES_API);

    EGLint numConfigs = 0;
    EGLConfig config;
    EGLint configAttribs[] = { EGL_CONFIG_ID, 2, EGL_NONE};

    eglChooseConfig(egl_display, configAttribs, &config, 1, &numConfigs);
    // You can choose your display resoltion, up to 1080p on the PSTV (Vita requires SharpScale)
    EGLSurface egl_surface = eglCreateWindowSurface(egl_display, config, VITA_WINDOW_960X544, NULL);  

    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE };

    EGLContext egl_context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, contextAttribs);

    eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);

    EGLint egl_surface_width = 0, egl_surface_height = 0;
    eglQuerySurface(egl_display, egl_surface, EGL_WIDTH, &egl_surface_width);
    eglQuerySurface(egl_display, egl_surface, EGL_HEIGHT, &egl_surface_height);


    sg_setup(&(sg_desc){0});

    sg_pass_action pass_action = (sg_pass_action) {
        .colors[0] = { .action = SG_ACTION_CLEAR, .val = { 1.0f, 0.0f, 0.0f, 1.0f } }
    };

    while(1)
    {
        float g = pass_action.colors[0].val[1] + 0.01f;
        if (g > 1.0f) g = 0.0f;
        pass_action.colors[0].val[1] = g;
        sg_begin_default_pass(&pass_action, egl_surface_width, egl_surface_height);
        sg_end_pass();
        sg_commit();
        eglSwapBuffers(egl_display, egl_surface);
    }
    eglDestroyContext(egl_display, egl_context); 
    eglDestroySurface(egl_display, egl_surface);
    eglTerminate(egl_display);

    sg_shutdown();
    sceKernelExitProcess(0);
    return 0;

}

