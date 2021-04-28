//
// Copyright (C) 2021 Sergii Pylypenko
//
// ZLIB license.
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//


#ifndef __SYS_KAIOS__
#define __SYS_KAIOS__

#ifdef KAIOS_SWAP_NAVIGATION_KEYS
// Navigation keys are rotated in the landscape orientation,
// but SDL keycodes are not changed, so SDL_SCANCODE_UP points to the left.
// Remap them so you don't need to replace all SDL_SCANCODE_UP/DOWN/LEFT/RIGHT
// inside the app code.
#include <SDL2/SDL_scancode.h>

#define SDL_SCANCODE_UP    79 // RIGHT
#define SDL_SCANCODE_DOWN  80 // LEFT
#define SDL_SCANCODE_LEFT  82 // UP
#define SDL_SCANCODE_RIGHT 81 // DOWN

#endif // KAIOS_SWAP_NAVIGATION_KEYS

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#ifdef EMSCRIPTEN
#define FS_WRITE_MOUNT_POINT "/save"
#else
#define FS_WRITE_MOUNT_POINT "./save"
#endif

// On KaiOS IndexedDB refuses to save files bigger than 16 Mb
// You must split all files that you write to storage into 8 Mb chunks
#define FS_MAX_FILE_SIZE (8 * 1024 * 1024)

// To write data to files that will not be deleted after you close the app,
// you have to mount a writable file system, and sync it after writing to file.
// Files should be saved to directory defined in FS_WRITE_MOUNT_POINT.
// First call sys_fs_init() from your init code.
// Then check for sys_fs_init_get_done() to return 1 in a loop,
// before reading or writing any files inside FS_WRITE_MOUNT_POINT.
// Call sys_fs_sync() after writing any files, to push data to filesystem database.
// Then check for sys_fs_sync_get_done() to return 1 when FS sync is finished.

extern void sys_fs_init(void);
extern int sys_fs_init_get_done(void);
extern void sys_fs_sync(void);
extern int sys_fs_sync_get_done(void);

// To prevent the screen from sleeping, call sys_take_wake_lock()
extern void sys_take_wake_lock(void);

// Call sys_free_wake_lock() to enable sleep after sys_take_wake_lock()
extern void sys_free_wake_lock(void);

// Use sys_exit_app() to close the app. If you simply call exit(0),
// the app won't clear it's state and will show black screen on the next launch.
// This function does not return.
extern void sys_exit_app(void);

// After calling SDL_CreateWindow(), call sys_hide_splash_image() to hide splash image.
// If you don't call it, the splash image is hidden automatically in 10 seconds.
extern void sys_hide_splash_image(void);

// To show a fullscreen advertisement, call sys_show_fullscreen_advertisement(),
// it should be accessible from somewhere in the app, like settings dialog.
extern void sys_show_fullscreen_advertisement(void);

// Return device RAM size in megabytes.
// Your manifest.webapp must include following declaration,
// otherwise this function returns 0:
// "type": "privileged", "permissions": { "feature-detection": {} }
extern int sys_get_device_ram_size_megabytes(void);

#endif
