#pragma once

#include "Window.h"
#include "RendererType.h"

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void Init() = 0;
    virtual void Draw() = 0;
    virtual void Shutdown() = 0;
};



void InitRenderer(RendererType type);
void RenderFrame(void);
void ShutdownRenderer(void);