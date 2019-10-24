#------------------------------------------------------------------------------
#  Copyright (c) 2018-2019 Michele Morrone
#  All rights reserved.
#
#  https://michelemorrone.eu - https://BrutPitt.com
#
#  twitter: https://twitter.com/BrutPitt - github: https://github.com/BrutPitt
#
#  mailto:brutpitt@gmail.com - mailto:me@michelemorrone.eu
#  
#  This software is distributed under the terms of the BSD 2-Clause license
#------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)
SET(BUILD_wglChAoSP TRUE CACHE BOOL TRUE)
###########################################################
#
# *** CHANGES NOTE ***
# Now ALL EMITTER TYPES are selectable inside program
# Remain only differentiation on OpenGL type
# for EMSCRIPTEN is FIXED: Single thread & Aux Buffer
#
#   OpenGL 4.5 - Zero Driver Overhead
#
# OGL 4.5 Aux Thread and mappedBuffer on GPU (less GPU memory)
# set(OGL45_THREAD_MAPMEM "-DGLAPP_REQUIRE_OGL45 -DUSE_THREAD_TO_FILL -DUSE_MAPPED_BUFFER")
# OGL 4.5 Aux Thread and Aux buffer (fastest)
# set(OGL45_THREAD "-DGLAPP_REQUIRE_OGL45 -DUSE_THREAD_TO_FILL")
# OGL 4.5 Single thread, Aux buffer
# set(OGL45_BUFFER "-DGLAPP_REQUIRE_OGL45")
set(OGL45 "-DGLAPP_REQUIRE_OGL45")
#
#   OpenGL 4.1 - Olden GPU and MacOS X
#
# OGL 4.1 Aux Thread and Aux buffer  (fastest on openGL 4.1)
# set(OGL41_THREAD "-DUSE_THREAD_TO_FILL")
# OGL 4.1 Single thread, Aux buffer
# set(OGL41_BUFFER "")
set(OGL41 "")

#set(GLAPP_DISABLE_DLA "-DGLAPP_DISABLE_DLA")

set(SRC "${CMAKE_SOURCE_DIR}/src")

    set(SOURCE_FILES
        ${SRC}/libs/IconsFontAwesome/IconsFontAwesome.h
        ${SRC}/libs/ImGui/imconfig.h
        ${SRC}/libs/ImGui/imgui.cpp
        ${SRC}/libs/ImGui/imgui_widgets.cpp
        ${SRC}/libs/ImGui/imgui.h
        ${SRC}/libs/ImGui/imgui_draw.cpp
        ${SRC}/libs/ImGui/imgui_impl_glfw.cpp
        ${SRC}/libs/ImGui/imgui_impl_glfw.h
        ${SRC}/libs/ImGui/imgui_impl_opengl3.cpp
        ${SRC}/libs/ImGui/imgui_impl_opengl3.h
        ${SRC}/libs/ImGui/imgui_internal.h
        ${SRC}/libs/ImGui/imstb_rectpack.h
        ${SRC}/libs/ImGui/imstb_textedit.h
        ${SRC}/libs/ImGui/imstb_truetype.h
        ${SRC}/libs/configuru/configuru.hpp
        ${SRC}/tools/fastRandom.cpp
        ${SRC}/tools/fastRandom.h
        ${SRC}/tools/imGuIZMOquat.cpp
        ${SRC}/tools/imGuIZMOquat.h
        ${SRC}/tools/imguiControls.cpp
        ${SRC}/tools/imguiControls.h
        ${SRC}/tools/glslProgramObject.cpp
        ${SRC}/tools/glslProgramObject.h
        ${SRC}/tools/glslShaderObject.cpp
        ${SRC}/tools/glslShaderObject.h
        ${SRC}/tools/transforms.h
        ${SRC}/tools/vGizmoMath.h
        ${SRC}/tools/vGizmoConfig.h
        ${SRC}/tools/vGizmo.h
        ${SRC}/ui/uiAttractorsDlg.cpp
        ${SRC}/ui/uiMainDlg.cpp
        ${SRC}/ui/uiMainDlg.h
        ${SRC}/ui/uiSettings.cpp
        ${SRC}/appDefines.h
        ${SRC}/attractorsBase.cpp
        ${SRC}/attractorsBase.h
        ${SRC}/attractorsFiles.cpp
        ${SRC}/attractorsStartVals.cpp
        ${SRC}/configFile.cpp
        ${SRC}/glApp.cpp
        ${SRC}/glApp.h
        ${SRC}/glWindow.cpp
        ${SRC}/glWindow.h
        ${SRC}/mmFBO.cpp
        ${SRC}/mmFBO.h
        ${SRC}/palettes.cpp
        ${SRC}/palettes.h
        ${SRC}/ParticlesUtils.cpp
        ${SRC}/ParticlesUtils.h
        ${SRC}/ShadersClasses.cpp
        ${SRC}/ShadersClasses.h
        ${SRC}/vertexbuffer.h)

###########################################################
#   EMSCRIPTEN setting
###########################################################
if (BUILD_wglChAoSP)
    project(wglChAoSP)
    set(OGL_EMITTER_TYPE ${OGL41})
    
    set(CMAKE_CXX_STANDARD 14)
    find_package(OpenGL)

    include_directories(BEFORE SYSTEM ${EMSCRIPTEN_ROOT_PATH}/system/include)
#    include_directories(BEFORE SYSTEM $ENV{EMSCRIPTEN}/system/include)
    include_directories(${SRC})
    include_directories(${SRC}/tools)
    include_directories(${SRC}/libs)
    include_directories(${SRC}/libs)
#    include_directories(AFTER $ENV{INCLUDE})

# -o ${PROJECT_NAME}.html ->  CMAKE_EXECUTABLE_SUFFIX  .html .bc, .js (default), etc
set(CMAKE_EXECUTABLE_SUFFIX ".html")

# relative emscripten output files (EMSOUT_PATH for my RAMDISK, if defined)
    if(EXISTS $ENV{EMSOUT_PATH})
        set(EMSOUT "$ENV{EMSOUT_PATH}/emsOut")
    else()
        set(EMSOUT "${CMAKE_SOURCE_DIR}/emsOut")
    endif()

    file(MAKE_DIRECTORY ${EMSOUT})
    file(MAKE_DIRECTORY ${EMSOUT}/${CMAKE_BUILD_TYPE})
    set(EMSOUT ${EMSOUT}/${CMAKE_BUILD_TYPE})

# emcc compiler options
    set(M_EMCC_FLAGS "-DGLAPP_NO_OGL_DSA -DGLFW_INCLUDE_ES3")
    set(M_GLOBAL_FLAGS "-DGLAPP_NO_GLSL_PIPELINE -DVGIZMO_USES_TEMPLATE -DIMGUI_IMPL_OPENGL_ES3 -DGLCHAOSP_LIGHTVER -DGLCHAOSP_USE_LOWPRECISION -DGLFW_INCLUDE_NONE -DGLM_ENABLE_EXPERIMENTAL -fpermissive")

# BUILD TYPE: [Debug|RelWithDebInfo|Release|MinSizeRel]    
    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(M_EMCC_BINARYEN "-s WASM=0 -s DEMANGLE_SUPPORT=1 ")
        set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/shell_minimal.html\"")
    else()
        #  -s BINARYEN_METHOD='native-wasm,asmjs' 'interpret-asm2wasm' -s BINARYEN_ASYNC_COMPILATION=0
        if(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
            set(M_EMCC_BINARYEN "-s WASM=1")
            set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/shell_minimal.html\"")
        else()
            if(${CMAKE_BUILD_TYPE} MATCHES "MinSizeRel")
                # one file no memory option: -s SINGLE_FILE=1 --memory-init-file 0
                set(M_EMCC_BINARYEN "--no-heap-copy --closure 1 -s WASM=1 ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/mVeryMinimal.html\"")
            else() # Release 
                # -s SINGLE_FILE=1 --memory-init-file 0 -s \"BINARYEN_METHOD='interpret-asm2wasm'\"
                set(M_EMCC_BINARYEN "-s WASM=1 ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/mVeryMinimal.html\"")
            endif()    
        endif()    
    endif()

    set(M_EMCC_EMBED_ALL_ATTRACTORS "${CMAKE_SOURCE_DIR}/../ChaoticAttractors@ChaoticAttractors/")
    set(M_EMCC_EMBED_FEW_ATTRACTORS "${CMAKE_SOURCE_DIR}/../selectedAttractors@ChaoticAttractors/")

# grouped EMCC_OPTIONS
#     --embed-file \"${CMAKE_SOURCE_DIR}/../ChaoticAttractors@ChaoticAttractors/\"\

    set(M_EMCC_EMBED_FILES "--preload-file \"${CMAKE_SOURCE_DIR}/../Shaders@Shaders/\"\
     --preload-file \"${M_EMCC_EMBED_FEW_ATTRACTORS}\"\
     --preload-file \"${CMAKE_SOURCE_DIR}/../colorMaps/palettes.json@colorMaps/\"\
     --preload-file \"${CMAKE_SOURCE_DIR}/../Fonts@Fonts/\"\
     --preload-file \"${CMAKE_SOURCE_DIR}/../startData@startData/\"")
     
#     --embed-file \"${CMAKE_SOURCE_DIR}/../Fonts/Cousine-Regular.ttf@Fonts/\"\
#     --embed-file \"${CMAKE_SOURCE_DIR}/../Fonts/fontawesome-webfont.ttf@Fonts/\"\

    # set(M_EMCC_EMBED_FILES "--preload-file \"${CMAKE_SOURCE_DIR}/Shaders@/Shaders\"")
    # -s FULL_ES3=1 -s OFFSCREENCANVAS_SUPPORT=1
    set(M_EMCC_OGL_OPTIONS "-s USE_WEBGL2=1 -s USE_GLFW=3 -s LEGACY_GL_EMULATION=0 -s OFFSCREENCANVAS_SUPPORT=1 ")

# merge all EMCC_OPTIONS -s BINARYEN_TRAP_MODE=clamp 
    set(M_EMCC_OPTIONS "-s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s ALLOW_MEMORY_GROWTH=1 -s PRECISE_F32=1 ${M_EMCC_SHELL_FILE} ${M_EMCC_OGL_OPTIONS} ${M_EMCC_BINARYEN} ${M_EMCC_EMBED_FILES}")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EMSOUT}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${M_GLOBAL_FLAGS} ${OGL_EMITTER_TYPE} ${M_EMCC_FLAGS} ${M_EMCC_OPTIONS} ${GLAPP_DISABLE_DLA} -Wno-parentheses -Wno-format-security -Wno-logical-op-parentheses -Wno-unused-value -Wdelete-non-virtual-dtor")

    add_executable(${PROJECT_NAME} 
                    ${SRC}/emsTouch.cpp
                    ${SRC}/emsTouch.h
                    ${SOURCE_FILES})


###########################################################
#   PC setting
###########################################################
else()
#   Default best emitter type and OpenGL
    if(NOT OGL_EMITTER_TYPE)
        if(APPLE)
            set(OGL_EMITTER_TYPE ${OGL41})
        else()
            set(OGL_EMITTER_TYPE ${OGL45})
        endif()
    endif()    


    if(APPLE)
        project(glChAoSP_OSX)
    else()
        if(WIN32)
            project(glChAoSP)
        else()
            project(glChAoSP_Linux)
        endif()
    endif()

    set(CMAKE_CXX_STANDARD 14)
    find_package(OpenGL)
    

if(OPENGL_FOUND)
    message(STATUS "OPENGL_INCLUDE_DIRS: ${OPENGL_INCLUDE_DIRS}")
    message(STATUS "OPENGL_LIBRARY: ${OPENGL_LIBRARY}")

    include_directories(${OPENGL_INCLUDE_DIRS})
else ()
    message (FATAL_ERROR "OPENGL not found... REQUIRED!!!!")
endif(OPENGL_FOUND)

# set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/builds")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/..")

set(COMMON_DEFINES "-DVGIZMO_USES_TEMPLATE -DGLFW_INCLUDE_NONE -DIMGUI_IMPL_OPENGL_LOADER_GLAD -DGLM_ENABLE_EXPERIMENTAL")
if(APPLE)
    set(COMMON_DEFINES "${COMMON_DEFINES} -DGLCHAOSP_NO_USES_GLSL_SUBS -DGLAPP_NO_GLSL_PIPELINE -Wno-logical-op-parentheses -Wno-unused-command-line-argument -Wno-parentheses -framework Cocoa -framework IOKit -framework CoreVideo")
else()
    set(COMMON_DEFINES "${COMMON_DEFINES}")
endif(APPLE)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security ${OGL_EMITTER_TYPE} ${COMMON_DEFINES} ${GLAPP_DISABLE_DLA} -std=c++14 -fpermissive")

include_directories(${SRC})
include_directories(${SRC}/tools)
include_directories(${SRC}/libs)
include_directories($ENV{INCLUDE})

#uncomment for GLFW installed dir
if(APPLE)
    include_directories(src/libs/glfw/buildOSX/include)
    link_directories(src/libs/glfw/buildOSX/lib)
else()
    if(WIN32)
        include_directories(src/libs/glfw/buildWin/include)
        link_directories(src/libs/glfw/buildWin/lib)
    else()
        include_directories(src/libs/glfw/buildLinux/include)
        link_directories(src/libs/glfw/buildLinux/lib)
    endif(WIN32)
endif(APPLE)

add_executable(${PROJECT_NAME} 
        ${SRC}/libs/glad/glad.cpp
        ${SRC}/libs/glad/glad.h
        ${SRC}/libs/lodePNG/lodepng.cpp
        ${SRC}/libs/lodePNG/lodepng.h
        ${SRC}/libs/tinyFileDialog/tinyfiledialogs.c
        ${SRC}/libs/tinyFileDialog/tinyfiledialogs.h
        ${SRC}/tools/oglAxes.cpp
        ${SRC}/tools/oglAxes.h
        ${SOURCE_FILES})

if(OPENGL_FOUND)
    if(APPLE)
        set(TARGET_LIBS "-lglfw3 -lpthread -ldl")
    else()
        if(WIN32)
        else()    
        set(TARGET_LIBS "-lglfw3 -lpthread -ldl -lX11")
        endif(WIN32)
    endif(APPLE)

    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY} ${TARGET_LIBS})
endif(OPENGL_FOUND)

endif() 
