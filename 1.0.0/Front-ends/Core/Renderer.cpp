#include "Renderer.hpp"

static RendererType currentType;

extern void InitOpenGL(void);
extern void RenderOpenGL(void);
extern void ShutdownOpenGL(void);

extern void InitVulkan(void);
extern void RenderVulkan(void);
extern void ShutdownVulkan(void);

void InitRenderer(RendererType type) {
    currentType = type;
    if (type == RENDERER_OPENGL)
        InitOpenGL();
    else if (type == RENDERER_VULKAN)
        InitVulkan();
}

void RenderFrame(void) {
    if (currentType == RENDERER_OPENGL)
        RenderOpenGL();
    else if (currentType == RENDERER_VULKAN)
        RenderVulkan();
}

void ShutdownRenderer(void) {
    if (currentType == RENDERER_OPENGL)
        ShutdownOpenGL();
    else if (currentType == RENDERER_VULKAN)
        ShutdownVulkan();
}