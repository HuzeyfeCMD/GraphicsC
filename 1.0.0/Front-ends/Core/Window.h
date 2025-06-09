#pragma once

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
    #include "Front-ends/Platforms/Windows/Core/WindowWin32_Win64.h"
#elif defined(__APPLE__) || defined(__MACH__)
    #include "Front-ends/Platforms/Mac/Core/WindowMac.h"
#elif defined(__linux__)
    #include "Front-ends/Platforms/Linux/Core/WindowLinux.h"
#endif

typedef struct Window {
    int width;
    int height;
    const char* title;
    void* nativeHandle;
    int isOpen;
} Window;

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
void OpenWinWindow(const char* title, int width, int height);
void CloseWinWindow(void);
void RunWindowsEventLoop(void);
#elif defined(__APPLE__) || defined(__MACH__)
void OpenMacWindow(const char* title, int width, int height);
void CloseMacWindow(void);
void RunMacEventLoop(void);
#elif defined(__linux__)
void OpenLinuxWindow(const char* title, int width, int height);
void CloseLinuxWindow(void);
void RunLinuxEventLoop(void);
#endif

static inline Window* CreateWindow(int width, int height, const char* title)
{
    Window* window = malloc(sizeof(Window));
    if (!window) {
        printf("Pencere oluşturulamadı.\n");
        return NULL;
    }

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
    OpenWinWindow(title, width, height);
#elif defined(__APPLE__) || defined(__MACH__)
    OpenMacWindow(title, width, height);
#elif defined(__linux__)
    OpenLinuxWindow(title, width, height);
    window->nativeHandle = (void*)display;
#endif

    window->width = width;
    window->height = height;
    window->title = title;

    printf("Pencere oluşturuldu: %s (%dx%d)\n", title, width, height);

    return window;
}

static inline void CloseWindow(Window* window)
{
    if (!window) return;

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
    CloseWinWindow();
#elif defined(__APPLE__) || defined(__MACH__)
    CloseMacWindow();
#elif defined(__linux__)
    CloseLinuxWindow();
#endif

    printf("Pencere kapatıldı: %s\n", window->title);
    free(window);
}

static inline void RunEventLoop(void)
{
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
    RunWindowsEventLoop();
#elif defined(__APPLE__) || defined(__MACH__)
    RunMacEventLoop();
#elif defined(__linux__)
    RunLinuxEventLoop();
#endif
}