cmake_minimum_required( VERSION 3.0.0 )
project(wasm-doom)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

include_directories(
    gifenc
    opl
    sdl_mixer
    src
    src/doom
)
file (GLOB_RECURSE SRC_FILES src/*.c gifenc/*.c opl/*.c sdl_mixer/*.c)

set(CMAKE_C_FLAGS " \
    ${CMAKE_C_FLAGS} -Wall \
    -Wno-\#warnings \
    -Wno-macro-redefined \
    -Wno-switch \
    -Wno-invalid-pp-token \
    -Werror=format \
    -Werror=format-security \
    -Werror=format-nonliteral \
    -s WASM=0 \
    -s USE_SDL=2 \
    -s USE_LIBPNG=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INITIAL_MEMORY=41943040 \
    -s NO_EXIT_RUNTIME=1 \
    -s EXTRA_EXPORTED_RUNTIME_METHODS=['FS','UTF8ToString'] \
    -s FORCE_FILESYSTEM=1 \
    --no-heap-copy \
    --preload-file ${CMAKE_CURRENT_LIST_DIR}/freedoom1.wad@freedoom1.wad \
    --preload-file ${CMAKE_CURRENT_LIST_DIR}/freedoom1.wad.1@freedoom1.wad.1 \
    --preload-file ${CMAKE_CURRENT_LIST_DIR}/freedoom1.wad.2@freedoom1.wad.2 \
    --preload-file ${CMAKE_CURRENT_LIST_DIR}/freedoom1.wad.3@freedoom1.wad.3 \
")

add_executable(doom ${SRC_FILES})
set_target_properties(doom PROPERTIES SUFFIX .html)
target_link_libraries(doom idbfs.js)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS_DEBUG "-s ASSERTIONS=2 -O2")
    target_link_libraries(doom -O2 --shell-file=${CMAKE_CURRENT_LIST_DIR}/app/shell_minimal_debug.html)
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s ASSERTIONS=0 -flto -Oz")
    target_link_libraries(doom -flto -Oz --shell-file=${CMAKE_CURRENT_LIST_DIR}/app/shell_minimal.html)
endif()

em_link_js_library(doom ${libraryJsFiles})
