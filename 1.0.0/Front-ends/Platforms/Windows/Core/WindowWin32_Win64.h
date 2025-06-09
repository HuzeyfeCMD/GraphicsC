#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

static HWND hwnd = NULL;
static bool running = true;
static HINSTANCE hInstanceGlobal = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            running = false;
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                running = false;
                DestroyWindow(hwnd);
                return 0;
            }
            break;

        case WM_CLOSE:
            running = false;
            DestroyWindow(hwnd);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void OpenWinWindow(const char* title, int width, int height) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    hInstanceGlobal = hInstance;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "MyWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0,
        "MyWindowClass",
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, SW_SHOW);
    SetFocus(hwnd); // ESC yakalayabilmek için pencereye focus veriyoruz
}

void RunWindowsEventLoop(void) {
    MSG msg;
    while (running) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // buraya oyun mantığı veya çizim vs. konabilir
    }
}

void CloseWinWindow(void) {
    if (hwnd) {
        DestroyWindow(hwnd);
        hwnd = NULL;
    }
}


#endif