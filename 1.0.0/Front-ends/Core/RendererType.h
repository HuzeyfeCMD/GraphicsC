#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum RendererType {
    RENDERER_TYPE_NONE = 0,
    RENDERER_TYPE_OPENGL,
    RENDERER_TYPE_VULKAN
} RendererType;

#ifdef __cplusplus
}
#endif