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
    ${CMAKE_C_FLAGS} -O3 -Wall \
    -Wno-\#warnings \
    -Wno-macro-redefined \
    -Wno-switch \
    -s WASM=0 \
    -s USE_SDL=2 \
    -s USE_LIBPNG=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s NO_EXIT_RUNTIME=1 \
    -s EXTRA_EXPORTED_RUNTIME_METHODS=['FS','UTF8ToString'] \
    -s FORCE_FILESYSTEM=1 \
    --no-heap-copy \
    --preload-file ${CMAKE_CURRENT_LIST_DIR}/freedoom1.wad@freedoom1.wad \
")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS_DEBUG "-s ASSERTIONS=2 --source-map-base http://localhost:8000/")
    set(HTML_TEMPLATE ${CMAKE_CURRENT_LIST_DIR}/app/shell_minimal_debug.html)
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s ASSERTIONS=0")
    set(HTML_TEMPLATE ${CMAKE_CURRENT_LIST_DIR}/app/shell_minimal.html)
endif()

add_executable(doom ${SRC_FILES})
target_link_libraries(doom
    idbfs.js
    --shell-file=${HTML_TEMPLATE}
)
set_target_properties(doom PROPERTIES SUFFIX .html)

em_link_js_library(doom ${libraryJsFiles})
