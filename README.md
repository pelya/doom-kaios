# WebAssembly DOOM

DOOM® is a registered trademark of ZeniMax Media Inc. in the US and/or
other countries. Id Software® is a registered trademark of ZeniMax Media
Inc. in the US and/or other countries. WebAssembly DOOM is in no way affiliated
with ZeniMax Media Inc. or id Software LLC and is not approved by ZeniMax
Media Inc. or id Software.

Doom is © 1993-1996 Id Software, Inc.; Boom 2.02 is © 1999 id Software,
Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman; PrBoom+ is
© 1999 id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty
Halderman, © 1999-2000 Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian
Schulze, © 2005-2006 Florian Schulze, Colin Phipps, Neil Stevens, Andrey
Budko; Chocolate Doom is © 1993-1996 Id Software, Inc., © 2005 Simon
Howard; Strawberry Doom is © 1993-1996 Id Software, Inc., © 2005 Simon
Howard, © 2008-2010 GhostlyDeath; Crispy Doom is © 2014-2018 Fabian
Greffrath.

SDL 2.0, SDL_mixer 2.0 and SDL_net 2.0 are © 1997-2016 Sam Lantinga.

Secret Rabbit Code (libsamplerate) is © 2002-2011 Erik de Castro Lopo;
Libpng is © 1998-2014 Glenn Randers-Pehrson, © 1996-1997 Andreas Dilger, ©
1995-1996 Guy Eric Schalnat, Group 42, Inc.; Zlib is © 1995-2013 Jean-loup
Gailly and Mark Adler.

Emscripten is © 2010-2018 Emscripten authors, see [AUTHORS](https://raw.githubusercontent.com/emscripten-core/emscripten/incoming/AUTHORS) file.

=== KaiOS instructions ===

Download and build Emscripten, as described on their page: https://emscripten.org/

Put directory with emsdk script into your $PATH

Launch build-debug.sh or build-release.sh

You should get compiled KaiOs package debug/application.zip or release/application.zip with manifest.webapp and Javascript code inside

Patch SDL2 to enable '*' and '#' keys:

Delete emsdk/upstream/emscripten/cache/sysroot/lib/wasm32-emscripten/libSDL2.a and emsdk/upstream/emscripten/cache/ports-builds/sdl2

Apply patch SDL_emscriptenevents.c.patch to SDL2 sources at emsdk/upstream/emscripten/cache/ports/sdl2

Launch build-debug.sh again, SDL2 should be recompiled automatically

To install application.zip to your device, launch command:

git submodule update --init --recursive

Download and install ADB to path ~/bin/android-sdk/platform-tools/adb

If you already have Android SDK installed, you can symlink it to your ~/bin directory:

ln -s /path/to/android-sdk ~/bin/android-sdk

On Debian, you can install adb from system packages:

sudo apt-get install adb

Enable debug mode in your device by entering secret code on your home screen:

    *#*#33284#*#*

Plug in USB cable, run 'install.sh debug' or 'install.sh release'

You should see your app in the device app list, install script freezes at the end, you can kill it with Ctrl-C

KaiOS devices generate following SDL2 keycodes:

SDL_SCANCODE_BACKSPACE for 'Back' key

SDL_SCANCODE_KP_ENTER for 'Call' key

SDL_SCANCODE_ESCAPE for 'End Call' key, not present on all phones

SDL_SCANCODE_0 .. SDL_SCANCODE_9, SDL_SCANCODE_KP_HASH, SDL_SCANCODE_KP_MULTIPLY for numeric keyboard

SDL_SCANCODE_RETURN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN for directional pad

SDL_SCANCODE_AC_FORWARD and SDL_SCANCODE_AC_BACK for LSK and RSK (left soft key and right soft key)

SDL_SCANCODE_VOLUMEUP and SDL_SCANCODE_VOLUMEDOWN for volume keys

With Emscripten, your main loop should not block anywhere - do not call sleep() or SDL_Delay() or SDL_WaitEvent()

Use emscripten_set_main_loop() to call your main loop function after you initialized video and everything else

Use following code to close the app:

    EM_ASM( window.open('', '_self').close(); );

If you don't call it, splash image is hidden automatically in 10 seconds.
if you simply call exit(0) the app won't clear it's state and will show black screen on the next launch

After calling SDL_CreateWindow(), call following code to hide splash image.

    EM_ASM( sys_hide_splash_image(); );

To write data to files that will not be deleted after you close the app, you have to mount a writable file system,
and sync it after writing to file.
Files should be saved to directory defined in FS_WRITE_MOUNT_POINT.
First call sys_fs_init() from your init code.
Then check for sys_fs_init_get_done() to return 1 in a loop, before reading or writing any files inside FS_WRITE_MOUNT_POINT.
Call sys_fs_sync() after writing any files, to push data to filesystem database.
Then check for sys_fs_sync_get_done() to return 1 when FS sync is finished.

To prevent the screen from sleeping, call sys_take_wake_lock(), and call sys_free_wake_lock() to enable sleep.

The app must mute any music or audio when the phone lid is closed or when the screen is dimmed.
The app will receive SDL_WINDOWEVENT notifications from SDL. Whenever the app receives events

    if (event.type == SDL_WINDOWEVENT && (
        event.window.event == SDL_WINDOWEVENT_FOCUS_LOST ||
        event.window.event == SDL_WINDOWEVENT_HIDDEN))

it must mute the audio using SDL_PauseAudio(1) call.
The app can unmute the audio only after receiving both events
SDL_WINDOWEVENT_SHOWN and SDL_WINDOWEVENT_FOCUS_GAINED.

To debug your code on the device, compile the app using the script build-debug.sh,
then you can use printf() in the code to write debug messages to the text area on the screen

KaiStore manually tests each app submission, and checks that it does not crash on phones with 256 Mb RAM.
You can monitor your app memory usage by using 'top' command and watching RSS memory usage:

adb shell top -m 5 -s rss

QA testing may take from 2 days up to 3 weeks, if there are many other apps to test.

Maximum size of application.zip for uploading to KaiOS Store is currently 20 Mb, and QA will complain if it's bigger than 6 Mb.

The store does accept games in landscape mode. Device screen is 320x240 pixels, and SDL will stretch smaller video output to fullscreen.

KaiOS Store does provide app updates, however it would require user interaction.

The KaiAds is deeply integrated to the KaiStore, the app analytics is part of the KaiAds SDK
and developers need to go to KaiAds page to access the app install and usage statistics.

If apps aren't monetized, KaiStore team would mark it a low priority and the QA might be delayed.

To show a fullscreen advertisement, add following code to some part of your app,
it should be accessible from somewhere in the app, like settings dialog:

    EM_ASM( if (sys_preloaded_advertisement !== false) sys_preloaded_advertisement.call('display'); );

Netplay will likely never be added.
Mormally UDP sockets are only available as WebRTC API in the web browser,
which wraps UDP data into DTLS/SRTP/SCTP protocols,
so the connection between two phones on the same WiFi is easy, but hosting dedicated servers
will require some kind of WebRTC proxy on top of UDP, so existing server cannot be used.

On top of this, Chocolate Doom does not have any online server browser in the game itself,
you launch SETUP.EXE, select the server from the list, and then it launches DOOM.EXE.
So for KaiOS, two apps will need to be distributed in the same package.

There is supposed to be a hidden API to use UDP sockets directly on KaiOS, without WebRTC wrappers:

https://www.w3.org/TR/tcp-udp-sockets/

https://developer.mozilla.org/en-US/docs/Archive/B2G_OS/API/UDPSocket

It requires privileged app permissions.
Privileged app cannot contain Javascript code embedded into HTML directly,
you must use <script src="..."> everywhere, the embedded Javascript won't be executed.
