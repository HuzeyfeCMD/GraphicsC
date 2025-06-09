#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __linux__

#ifdef USE_WAYLAND
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

static struct wl_display* display = NULL;
static struct wl_registry* registry = NULL;
static struct wl_compositor* compositor = NULL;
static struct wl_surface* surface = NULL;
static struct wl_shell* shell = NULL;
static struct wl_shell_surface* shell_surface = NULL;
static struct wl_seat* seat = NULL;
static struct wl_keyboard* keyboard = NULL;

// xkbcommon context/state
static struct xkb_context* xkb_ctx = NULL;
static struct xkb_keymap* keymap = NULL;
static struct xkb_state* xkb_state = NULL;

static bool running = true;

static void keyboard_keymap(void* data, struct wl_keyboard* kb,
    uint32_t format, int fd, uint32_t size) {
    char* map_shm = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if (map_shm == MAP_FAILED) return;

    keymap = xkb_keymap_new_from_string(xkb_ctx, map_shm, XKB_KEYMAP_FORMAT_TEXT_V1, 0);
    xkb_state = xkb_state_new(keymap);
    munmap(map_shm, size);
    close(fd);
}

static void keyboard_enter(void* data, struct wl_keyboard* kb, uint32_t serial,
    struct wl_surface* surface, struct wl_array* keys) {}

static void keyboard_leave(void* data, struct wl_keyboard* kb, uint32_t serial,
    struct wl_surface* surface) {}

static void keyboard_key(void* data, struct wl_keyboard* kb, uint32_t serial,
    uint32_t time, uint32_t key, uint32_t state) {
    if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
        xkb_keysym_t sym = xkb_state_key_get_one_sym(xkb_state, key + 8);
        if (sym == XKB_KEY_Escape) {
            running = false;
        }
    }
}

static void keyboard_modifiers(void* data, struct wl_keyboard* kb, uint32_t serial,
    uint32_t mods_depressed, uint32_t mods_latched,
    uint32_t mods_locked, uint32_t group) {
    xkb_state_update_mask(xkb_state, mods_depressed, mods_latched,
                          mods_locked, 0, 0, group);
}

static void keyboard_repeat_info(void* data, struct wl_keyboard* kb, int32_t rate, int32_t delay) {}

static const struct wl_keyboard_listener keyboard_listener = {
    .keymap = keyboard_keymap,
    .enter = keyboard_enter,
    .leave = keyboard_leave,
    .key = keyboard_key,
    .modifiers = keyboard_modifiers,
    .repeat_info = keyboard_repeat_info,
};

static void seat_handle_capabilities(void* data, struct wl_seat* seat, uint32_t caps) {
    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
        keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(keyboard, &keyboard_listener, NULL);
    }
}

static void seat_name(void* data, struct wl_seat* seat, const char* name) {}

static const struct wl_seat_listener seat_listener = {
    .capabilities = seat_handle_capabilities,
    .name = seat_name,
};

static void registry_handler(void* data, struct wl_registry* reg,
                             uint32_t id, const char* interface, uint32_t version) {
    if (strcmp(interface, "wl_compositor") == 0)
        compositor = wl_registry_bind(reg, id, &wl_compositor_interface, 1);
    else if (strcmp(interface, "wl_shell") == 0)
        shell = wl_registry_bind(reg, id, &wl_shell_interface, 1);
    else if (strcmp(interface, "wl_seat") == 0)
        seat = wl_registry_bind(reg, id, &wl_seat_interface, 1);
}

static void registry_remover(void* data, struct wl_registry* reg, uint32_t id) {}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handler,
    .global_remove = registry_remover,
};

static void shell_surface_ping(void* data, struct wl_shell_surface* shell_surface, uint32_t serial) {
    wl_shell_surface_pong(shell_surface, serial);
}

static void shell_surface_configure(void* data, struct wl_shell_surface* shell_surface,
                                    uint32_t edges, int32_t width, int32_t height) {}

static void shell_surface_popup_done(void* data, struct wl_shell_surface* shell_surface) {}

static const struct wl_shell_surface_listener shell_surface_listener = {
    .ping = shell_surface_ping,
    .configure = shell_surface_configure,
    .popup_done = shell_surface_popup_done,
};

void OpenLinuxWindow(const char* title, int width, int height) {
    display = wl_display_connect(NULL);
    if (!display) {
        fprintf(stderr, "Cannot connect to Wayland display\n");
        exit(1);
    }

    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);
    wl_display_roundtrip(display);

    if (!compositor || !shell || !seat) {
        fprintf(stderr, "Missing required Wayland interfaces\n");
        exit(1);
    }

    wl_seat_add_listener(seat, &seat_listener, NULL);

    surface = wl_compositor_create_surface(compositor);
    shell_surface = wl_shell_get_shell_surface(shell, surface);
    wl_shell_surface_add_listener(shell_surface, &shell_surface_listener, NULL);
    wl_shell_surface_set_toplevel(shell_surface);

    xkb_ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    printf("Wayland window opened: %s (%dx%d)\n", title, width, height);
}

void RunLinuxEventLoop(void) {
    while (running) {
        wl_display_dispatch(display);
    }
}

void CloseLinuxWindow(void) {
    if (keyboard) wl_keyboard_destroy(keyboard);
    if (seat) wl_seat_destroy(seat);
    if (shell_surface) wl_shell_surface_destroy(shell_surface);
    if (surface) wl_surface_destroy(surface);
    if (shell) wl_shell_destroy(shell);
    if (compositor) wl_compositor_destroy(compositor);
    if (registry) wl_registry_destroy(registry);

    if (xkb_state) xkb_state_unref(xkb_state);
    if (keymap) xkb_keymap_unref(keymap);
    if (xkb_ctx) xkb_context_unref(xkb_ctx);

    if (display) wl_display_disconnect(display);
    printf("Wayland window closed.\n");
}

#else

#include <X11/Xlib.h>
#include <X11/Xutil.h>

static Display* display = NULL;
static Window window;
static int screen;

void OpenLinuxWindow(const char* title, int width, int height) {
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
    screen = DefaultScreen(display);

    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, width, height, 1,
                                BlackPixel(display, screen), WhitePixel(display, screen));

    XSetStandardProperties(display, window, title, title, None, NULL, 0, NULL);
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    
    Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wmDeleteMessage, 1);

    XMapWindow(display, window);
}

void RunLinuxEventLoop(void) {
    XEvent event;
    Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);

    while (1) {
        XNextEvent(display, &event);

        if (event.type == Expose) {
            // redraw
        }
        else if (event.type == KeyPress) {
            // ESC tuşu: keycode 9 (genelde)
            if (event.xkey.keycode == 9) {
                break;
            }
        }
        else if (event.type == ClientMessage) {
            if ((Atom)event.xclient.data.l[0] == wmDeleteMessage) {
                break;
            }
        }
        else if (event.type == DestroyNotify) {
            break;
        }
    }
}

void CloseLinuxWindow(void) {
    if (display) {
        XDestroyWindow(display, window);
        XCloseDisplay(display);
        display = NULL;
    }
}

#endif // USE_WAYLAND

#endif // __linux__