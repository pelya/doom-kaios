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


#include <stdio.h>
#include <stdlib.h>

#ifdef EMSCRIPTEN
#include "emscripten.h"
#endif

#include "sys_kaios.h"

#pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#pragma GCC diagnostic ignored "-Wgnu-statement-expression"
#pragma GCC diagnostic ignored "-Wc11-extensions"
#pragma GCC diagnostic ignored "-Winvalid-pp-token"

static int fs_initialized = 0;

void sys_fs_init(void)
{
	if (!fs_initialized)
	{
		fs_initialized = 1;
#ifdef EMSCRIPTEN
		EM_ASM({
			// Make a directory other than '/'
			FS.mkdir(UTF8ToString($0));
			// Then mount with IDBFS type
			FS.mount(IDBFS, {}, UTF8ToString($0));
			// Then sync
			// var sys_fs_init_is_done = 0; // Defined in HTML file
			FS.syncfs(true, function (err) {
				if (err) alert('Error initializing filesystem');
				//Module.print('sys_fs_init done, mount point ' + UTF8ToString($0));
				sys_fs_init_is_done = 1;
				sys_fs_sync_is_done = 1;
			});
		}, FS_WRITE_MOUNT_POINT);
#endif // EMSCRIPTEN
	}
}

int sys_fs_init_get_done(void)
{
	if (!fs_initialized)
	{
		printf("Please call sys_fs_init() first\n");
		exit(1);
	}

	int status = 1;
#ifdef EMSCRIPTEN
	status = EM_ASM_INT( return sys_fs_init_is_done; );
#endif // EMSCRIPTEN
	return status;
}

void sys_fs_sync(void)
{
	if (!fs_initialized)
	{
		printf("Please call sys_fs_init() first\n");
		exit(1);
	}
#ifdef EMSCRIPTEN
	EM_ASM({
		sys_fs_sync_is_done = 0;
		FS.syncfs(false, function (err) {
			if (err) alert('Error writing to filesystem');
			//Module.print('sys_fs_sync done, error ' + err);
			sys_fs_sync_is_done = 1;
		});
	});
#endif // EMSCRIPTEN
}

int sys_fs_sync_get_done(void)
{
	if (!fs_initialized)
	{
		printf("Please call sys_fs_init() first\n");
		exit(1);
	}

	int status = 1;
#ifdef EMSCRIPTEN
	status = EM_ASM_INT( return sys_fs_sync_is_done; );
#endif // EMSCRIPTEN
	return status;
}

void sys_take_wake_lock(void)
{
#ifdef EMSCRIPTEN
	EM_ASM({
		if (sys_screen_wake_lock === null) {
			sys_screen_wake_lock = window.navigator.requestWakeLock('screen');
		}
	});
#endif // EMSCRIPTEN
}

void sys_free_wake_lock(void)
{
#ifdef EMSCRIPTEN
	EM_ASM({
		if (sys_screen_wake_lock !== null) {
			sys_screen_wake_lock.unlock();
			sys_screen_wake_lock = null;
		}
	});
#endif // EMSCRIPTEN
}

void sys_exit_app(void)
{
#ifdef EMSCRIPTEN
	// Do not call main loop anymore, and throw an exception to simulate program termination
	emscripten_cancel_main_loop();
	//EM_ASM( window.open('', '_self').close(); );
	EM_ASM({
		var sys_shutdown_anim = 100;
		setInterval(function() {
			if (sys_fs_sync_is_done) {
				window.open('', '_self').close();
			}
			if (sys_shutdown_anim > 0) {
				sys_shutdown_anim -= 20;
			}
			document.getElementById('canvas').style.height = String(sys_shutdown_anim) + '%';
			document.getElementById('canvas').style.marginTop = String((100 - sys_shutdown_anim) / 2) + '%';
		}, 20);
	});
#endif // EMSCRIPTEN
	exit(0);
}

void sys_hide_splash_image(void)
{
#ifdef EMSCRIPTEN
	EM_ASM( sys_hide_splash_image(); );
#endif // EMSCRIPTEN
}

extern void sys_show_fullscreen_advertisement(void)
{
#ifdef EMSCRIPTEN
	EM_ASM({
		if (sys_preloaded_advertisement !== false) {
			sys_preloaded_advertisement.call('display');
		}
	});
#endif // EMSCRIPTEN
}
