#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#include "Mac/Core/WindowMac.h"

static id app = nil;
static id window = nil;
static bool running = true;

// ESC tuşu kontrolü için event monitoring
static void monitorEscKey(void) {
    id eventMonitor = ((id (*)(id, SEL, id, id))objc_msgSend)(
        objc_getClass("NSEvent"),
        sel_registerName("addLocalMonitorForEventsMatchingMask:handler:"),
        (id)(NSUInteger)(1 << 10), // NSEventMaskKeyDown
        imp_implementationWithBlock(^(id event) {
            unsigned short keyCode = ((unsigned short (*)(id, SEL))objc_msgSend)(event, sel_registerName("keyCode"));
            if (keyCode == 53) { // 53 = ESC key
                running = false;
                ((void (*)(id, SEL))objc_msgSend)(window, sel_registerName("close"));
            }
            return event;
        })
    );
}

// Delegate sınıfı: pencere kapanınca running = false
static id createDelegateClass(void) {
    Class delClass = objc_allocateClassPair(objc_getClass("NSObject"), "WindowDelegate", 0);
    class_addProtocol(delClass, objc_getProtocol("NSWindowDelegate"));

    SEL sel = sel_registerName("windowWillClose:");
    class_addMethod(delClass, sel, (IMP)(+[](id self, SEL _cmd, id notif) {
        running = false;
    }), "v@:@");

    objc_registerClassPair(delClass);
    return [[delClass alloc] init];
}

void OpenMacWindow(const char* title, int width, int height) {
    id nsTitle = ((id (*)(id, SEL, const char*))objc_msgSend)(objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), title);

    ((void (*)(id, SEL))objc_msgSend)(objc_getClass("NSApplication"), sel_registerName("sharedApplication"));
    app = objc_msgSend((id)objc_getClass("NSApplication"), sel_registerName("sharedApplication"));

    ((void (*)(id, SEL))objc_msgSend)(app, sel_registerName("setActivationPolicy:"), (NSInteger)0); // NSApplicationActivationPolicyRegular

    CGRect rect = CGRectMake(0, 0, width, height);
    window = ((id (*)(id, SEL, CGRect, NSUInteger, NSUInteger, BOOL))objc_msgSend)(
        objc_getClass("NSWindow"),
        sel_registerName("alloc"),
        rect,
        (NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable),
        NSBackingStoreBuffered,
        false
    );

    window = ((id (*)(id, SEL, CGRect, NSUInteger, NSUInteger, BOOL))objc_msgSend)(
        window,
        sel_registerName("initWithContentRect:styleMask:backing:defer:"),
        rect,
        (NSUInteger)(1 | 2 | 8), // Titled | Closable | Resizable
        2, // Buffered
        false
    );

    ((void (*)(id, SEL, id))objc_msgSend)(window, sel_registerName("setTitle:"), nsTitle);
    ((void (*)(id, SEL, BOOL))objc_msgSend)(window, sel_registerName("makeKeyAndOrderFront:"), nil);

    id delegate = createDelegateClass();
    ((void (*)(id, SEL, id))objc_msgSend)(window, sel_registerName("setDelegate:"), delegate);

    monitorEscKey();

    ((void (*)(id, SEL))objc_msgSend)(app, sel_registerName("activateIgnoringOtherApps:"), YES);
}

void RunMacEventLoop(void) {
    id event;
    while (running) {
        event = ((id (*)(id, SEL, double, id, id, BOOL))objc_msgSend)(
            app,
            sel_registerName("nextEventMatchingMask:untilDate:inMode:dequeue:"),
            (NSUInteger)0xffffffffffffffff, // NSEventMaskAny
            nil,
            objc_msgSend(objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "kCFRunLoopDefaultMode"),
            YES
        );

        if (event) {
            ((void (*)(id, SEL, id))objc_msgSend)(app, sel_registerName("sendEvent:"), event);
            ((void (*)(id, SEL))objc_msgSend)(app, sel_registerName("updateWindows"));
        }
    }
}

void CloseMacWindow(void) {
    if (window) {
        ((void (*)(id, SEL))objc_msgSend)(window, sel_registerName("close"));
        window = nil;
    }
}