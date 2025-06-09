#pragma once

#include <objc/objc.h>
#include <objc/message.h>
#include <objc/runtime.h>
#include <CoreGraphics/CoreGraphics.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void OpenMacWindow(const char* title, int width, int height);
void RunMacEventLoop(void);
void CloseMacWindow(void);

#ifdef __cplusplus
}
#endif