#------------------------------------------------------------------------------
#  Copyright (c) 2018-2021 Michele Morrone
#  All rights reserved.
#
#  https://michelemorrone.eu - https://brutpitt.com - https://glchaosp.com
#
#  twitter: https://twitter.com/BrutPitt - github: https://github.com/BrutPitt
#
#  mail me to: brutpitt at gmail.com - me at michelemorrone.eu
#  
#  This software is distributed under the terms of the BSD 2-Clause license
#------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.15)

# To build glChAoS.P in Linux/MacOS/Windows (with MinGW) uses follow command:
#
# cmake -DBuildTarget:String=<BuildVer> -G <MakeTool> -B<FolderToBuild>
#   where:
#       <BuildVer> must be one of follow strings:
#           OpenGL_45
#           OpenGL_41
#           OpenGL_41_LowRes
#           OpenGL_ES
#           OpenGL_ES_LowRes
#       <MakeTool> is your preferred generator like "Unix Makefiles" or "ninja"
#       <FolderToBuild> is the folder where will be generated Makefile, move in it and run your generator
#       - Default build is "Release" but it can be changed via CMAKE_BUILD_TYPE definition:
#           command line: -DCMAKE_BUILD_TYPE:STRING=<Debug|Release|MinSizeRel|RelWithDebInfo>
#           cmake-gui: from combo associated to CMAKE_BUILD_TYPE var
#       - Executable will be generated directly in glChAoS.P root folder
#
# for Windows users, Visual Studio "solution" is also provided (msBuilds folder)
#
#
# To build wlChAoS.P with EMSCRIPTEN uses follow command:
# cmake cmake -DCMAKE_TOOLCHAIN_FILE:STRING=<EMSDK_PATH>/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DBuildTarget:String=<BuildVer> -G "Unix Makefiles"|"ninja" -B<FolderToBuild>
#   where:
#       <EMSDK_PATH> is where was installed EMSCRIPTEN: you need to have it in EMSDK environment variable
#       <BuildVer> must be one of follow strings:
#           WebGL
#           WebGL_LowRes
#       <MakeTool> is your preferred generator like "Unix Makefiles" or "ninja"
#           Windows users need to use MinGW-make utility (by EMSCRIPTEN specification): ninja or othe can not work.
#       <FolderToBuild> is the folder where will be generated Makefile, move in it and run your generator
#       - Default build is "MinSizeRel" but it can be changed via CMAKE_BUILD_TYPE definition:
#           command line: -DCMAKE_BUILD_TYPE:STRING=<Debug|Release|MinSizeRel|RelWithDebInfo>
#           cmake-gui: from combo associated to CMAKE_BUILD_TYPE var
#       - HTML and WASM files will be generated in "<glChAoSP_root>/emsout/<CMAKE_BUILD_TYPE>" folder
#
# More information, necessary dependencies, required packages are described: https://github.com/BrutPitt/glChAoS.P


# SET(BUILD_wglChAoSP FALSE BOOL "wglChAoS.P: need EMSCRIPTEN toolchain")
# SET(BUILD_wglChAoSP_lowres FALSE BOOL "wglChAoS.P: need EMSCRIPTEN toolchain")
# SET(BUILD_prng_test FALSE BOOL)

set(OGL_45      "OpenGL_45")
set(OGL_41      "OpenGL_41")
set(OGL_41_LOW  "OpenGL_41_LowRes")
set(OGL_ES      "OpenGL_ES")
set(OGL_ES_LOW  "OpenGL_ES_LowRes")
set(WGL_FULL    "WebGL")
set(WGL_LOW     "WebGL_LowRes")

set(BuildTarget ${OGL_45} CACHE STRING "Select the target to build:\n    glChAoS.P compliant with OpenGL 4.5/4.1/ES 3.x\n    wglChAoS.P compliant with WebGL 2.0\n    \"LowRes\" stands for GPU with LowResources (mem/cores\)")
set(BuildTargetValues "${OGL_45};${OGL_41};${OGL_41_LOW};${OGL_ES};${OGL_ES_LOW};${WGL_FULL};${WGL_LOW}" CACHE INTERNAL "Select one")

set_property(CACHE BuildTarget PROPERTY STRINGS ${BuildTargetValues} )
# define_property(CACHED_VARIABLE PROPERTY BuildTargetValues BRIEF_DOCS "aaaaaa" "bbb" "ccccc" FULL_DOCS "uuuu")

###########################################################

#   OpenGL 4.5 - Zero Driver Overhead
#
set(OGL45 "-DGLAPP_REQUIRE_OGL45")

#   OpenGL 4.1 - Olden GPU and MacOS X
#
set(OGL41 "-DGLCHAOSP_NO_USES_GLSL_SUBS")   #-DGLCHAOSP_NO_USES_GLSL_SUBS like webGL version: doesn't uses OGL subroutines
set(OGL41_LOW " -DGLCHAOSP_NO_USES_GLSL_SUBS
                -DGLAPP_NO_GLSL_PIPELINE
                -DGLCHAOSP_NO_AX
                -DGLCHAOSP_NO_BB
                -DGLCHAOSP_NO_MB
                -DGLCHAOSP_NO_TH
                ") #  -DGLCHAOSP_NO_AO_SHDW # add for lower resources

#   OpenGL ES 3.0
#
set(OGLES " -DGLAPP_USES_ES3
            -DIMGUI_IMPL_OPENGL_ES3
            -DGLFW_INCLUDE_NONE
            -DGLCHAOSP_NO_USES_GLSL_SUBS
            -DGLAPP_NO_GLSL_PIPELINE
            -DGLCHAOSP_NO_AX
            -DGLCHAOSP_NO_BB
            -DGLCHAOSP_NO_MB")

set(OGLES_LOW "${OGLES} -DGLCHAOSP_NO_AO_SHDW -DGLCHAOSP_NO_BLUR -DGLCHAOSP_NO_TH")


set(defaultBuildType "Release")

if(${BuildTarget} STREQUAL ${OGL_45})
    set(OGL_VER_USED ${OGL45})
    message(STATUS "selected ${BuildTarget} ...")
    elseif(${BuildTarget} STREQUAL ${OGL_41})
        set(OGL_VER_USED ${OGL41})
        message(STATUS "selected ${BuildTarget} ...")
    elseif(${BuildTarget} STREQUAL ${OGL_41_LOW})
            set(OGL_VER_USED ${OGL41_LOW})
            message(STATUS "selected ${BuildTarget} ...")
    elseif(${BuildTarget} STREQUAL ${OGL_ES})
                set(OGL_VER_USED ${OGLES})
                message(STATUS "selected ${BuildTarget} ...")
    elseif(${BuildTarget} STREQUAL ${OGL_ES_LOW})
                    set(OGL_VER_USED ${OGLES_LOW})
                    message(STATUS "selected ${BuildTarget} ...")
    elseif(${BuildTarget} STREQUAL ${WGL_FULL})
                        set(BUILD_wglChAoSP TRUE BOOL)
                        set(defaultBuildType "MinSizeRel")
                        message(STATUS "selected ${BuildTarget} ...")
    else(${BuildTarget} STREQUAL ${WGL_LOW})
                            set(BUILD_wglChAoSP_lowres TRUE BOOL)
                            set(defaultBuildType "MinSizeRel")
                            message(STATUS "selected ${BuildTarget} ...")
endif()

#change default build type from cmake-gui
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "'CMAKE_BUILD_TYPE' not specified, now is set to: '${defaultBuildType}' ...")
  set(CMAKE_BUILD_TYPE "${defaultBuildType}" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;MinSizeRel;RelWithDebInfo")
endif()

set(SRC "${CMAKE_SOURCE_DIR}/src")

    set(SOURCE_FILES
        ${SRC}/libs/IconsFontAwesome/IconsFontAwesome.h
        ${SRC}/libs/imgui/imconfig.h
        ${SRC}/libs/imgui/imgui.cpp
        ${SRC}/libs/imgui/imgui_widgets.cpp
        ${SRC}/libs/imgui/imgui_tables.cpp
        ${SRC}/libs/imgui/imgui.h
        ${SRC}/libs/imgui/imgui_draw.cpp
        ${SRC}/libs/imgui/imgui_impl_glfw.cpp
        ${SRC}/libs/imgui/imgui_impl_glfw.h
        ${SRC}/libs/imgui/imgui_impl_opengl3.cpp
        ${SRC}/libs/imgui/imgui_impl_opengl3.h
        ${SRC}/libs/imgui/imgui_internal.h
        ${SRC}/libs/imgui/imstb_rectpack.h
        ${SRC}/libs/imgui/imstb_textedit.h
        ${SRC}/libs/imgui/imstb_truetype.h
        ${SRC}/libs/configuru/configuru.hpp
        ${SRC}/tools/fastPRNG.cpp
        ${SRC}/tools/fastPRNG.h
        ${SRC}/tools/imGuIZMOquat.cpp
        ${SRC}/tools/imGuIZMOquat.h
        ${SRC}/tools/imguiControls.cpp
        ${SRC}/tools/imguiControls.h
        ${SRC}/tools/glslProgramObject.cpp
        ${SRC}/tools/glslProgramObject.h
        ${SRC}/tools/glslShaderObject.cpp
        ${SRC}/tools/glslShaderObject.h
        ${SRC}/tools/transforms.h
        ${SRC}/tools/vgMath.h
        ${SRC}/tools/vgConfig.h
        ${SRC}/tools/vGizmo.h
        ${SRC}/ui/uiAttractorsDlg.cpp
        ${SRC}/ui/uiMainDlg.cpp
        ${SRC}/ui/uiMainDlg.h
        ${SRC}/ui/uiSettings.cpp
        ${SRC}/appDefines.h
        ${SRC}/attractorsBase.cpp
        ${SRC}/attractorsBase.h
        ${SRC}/attractorsDLA3D.cpp
        ${SRC}/attractorsDLA3D.h
        ${SRC}/attractorsFractalsIIM.cpp
        ${SRC}/attractorsFractalsIIM.h
        ${SRC}/attractorsDiffEq.cpp
        ${SRC}/attractorsDiffEq.h
        ${SRC}/attractorsMagnetic.cpp
        ${SRC}/attractorsMagnetic.h
        ${SRC}/attractorsPolynomial.cpp
        ${SRC}/attractorsPolynomial.h
        ${SRC}/attractorsTrigonom.cpp
        ${SRC}/attractorsTrigonom.h
        ${SRC}/attractors2DTransf.cpp
        ${SRC}/attractors2DTransf.h
        ${SRC}/attractorsRampe.cpp
        ${SRC}/attractorsRampe.h
        ${SRC}/attractorsFiles.cpp
        ${SRC}/attractorsStartVals.cpp
        ${SRC}/configFile.cpp
        ${SRC}/glApp.cpp
        ${SRC}/glApp.h
        ${SRC}/glWindow.cpp
        ${SRC}/glWindow.h
        ${SRC}/IFS.cpp
        ${SRC}/IFS.h
        ${SRC}/partSystem.cpp
        ${SRC}/partSystem.h
        ${SRC}/emitter.cpp
        ${SRC}/emitter.h
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
#   wglChAoS.P - EMSCRIPTEN build
###########################################################
if(BUILD_wglChAoSP OR BUILD_wglChAoSP_lowres)
    set(WGL_DEFINES "-DGLCHAOSP_WEBGL
                     -DGLCHAOSP_LIGHTVER
                     -DGLCHAOSP_NO_TF_QUERY
                     -DGLCHAOSP_NO_AX
                     -DGLCHAOSP_NO_TH
                     -DGLCHAOSP_NO_MB
                     -DGLCHAOSP_NO_BB
                     -DGLCHAOSP_NO_USES_GLSL_SUBS
                     -DGLCHAOSP_WGL_REDUCE_TEXTURES
                     -DGLAPP_NO_GLSL_PIPELINE")
    if(BUILD_wglChAoSP_lowres)
        set(WGL_DEFINES "${WGL_DEFINES} -DGLCHAOSP_USE_LOWPRECISION -DGLCHAOSP_NO_AO_SHDW")
        project(wglChAoSP_lowres)
    else()
        set(WGL_DEFINES "${WGL_DEFINES} -DGLCHAOSP_WEBGL_W_SHADOWS -DGLCHAOSP_LIGHTVER_EXPERIMENTAL")
        project(wglChAoSP)
    endif()

    #set(CMAKE_CXX_STANDARD 14)
    # find_package(OpenGL)
    if(DEFINED ENV{EMSDK})
        set(EMS_CMAKE_FILE $ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake)
        if(EXISTS "${EMS_CMAKE_FILE}")
            include_directories(BEFORE SYSTEM $ENV{EMSDK}/upstream/emscripten/system/include)
        else()
            message(FATAL_ERROR "Can't find the file: ${EMS_CMAKE_FILE}")
        endif()
    else()
        message(FATAL_ERROR "Can't find EMSCRIPTEN path!\nHave you installed it?\nPlease set the installation path in \"EMSDK\" environment variable or first run \"source emsdk_env.sh\" or \"emsdk_env.bat\" to set it")
    endif()

    #add EMSCRIPTEN PATH with preference to command-line define
    #if(EXISTS ${EMSCRIPTEN_ROOT_PATH})
    #    include_directories(BEFORE SYSTEM ${EMSCRIPTEN_ROOT_PATH}/system/include)
    #else()
    #    if(EXISTS $ENV{EMSCRIPTEN})
    #        include_directories(BEFORE SYSTEM $ENV{EMSCRIPTEN}/system/include)
    #    else()
    #        message(FATAL_ERROR "Can't find EMSCRIPTEN path: please set it in \"EMSCRIPTEN\" environment variable or pass it to EMSCRIPTEN_ROOT_PATH via command line/gui")
    #    endif()
    #endif()
    include_directories(${SRC})
    include_directories(${SRC}/tools)
    include_directories(${SRC}/libs)
    include_directories(${SRC}/libs)
    #add also environment include
    #include_directories(AFTER $ENV{INCLUDE})

# -o ${PROJECT_NAME}.html ->  CMAKE_EXECUTABLE_SUFFIX  .html .bc, .js (default), etc
set(CMAKE_EXECUTABLE_SUFFIX ".html")

# emcc compiler options
    add_definitions(${WGL_DEFINES} -DVGM_USES_TEMPLATE -DIMGUI_IMPL_OPENGL_ES3 -DGLFW_INCLUDE_ES3 -DGLM_ENABLE_EXPERIMENTAL) # -DGLFW_INCLUDE_NONE
    add_compile_options(-Wno-parentheses -Wno-format-security -Wno-logical-op-parentheses -Wno-unused-value -Wdelete-non-virtual-dtor -Wno-unused-command-line-argument -fpermissive)

# BUILD TYPE: [Debug|RelWithDebInfo|Release|MinSizeRel]    
    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(M_EMCC_BINARYEN "-s WASM=0 -s DEMANGLE_SUPPORT=1 -s LZ4=1")
        set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/shell_minimal.html\"")
    elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
            set(M_EMCC_BINARYEN "-s WASM=1")
            set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/shell_minimal.html\"")
        elseif(${CMAKE_BUILD_TYPE} MATCHES "MinSizeRel")
                # one file no memory option: -s SINGLE_FILE=1 --memory-init-file 0
                # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
                set(M_EMCC_BINARYEN "--no-heap-copy --closure 1 -s WASM=1 -s LZ4=1")
                set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/mVeryMinimal.html\"")
        else() # Release
                # -s SINGLE_FILE=1 --memory-init-file 0 -s \"BINARYEN_METHOD='interpret-asm2wasm'\"
                set(M_EMCC_BINARYEN "-s WASM=1 ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/mVeryMinimal.html\"")
    endif()

    set(M_EMCC_EMBED_ALL_ATTRACTORS "${CMAKE_SOURCE_DIR}/../ChaoticAttractors@ChaoticAttractors/")
    set(M_EMCC_EMBED_FEW_ATTRACTORS "${CMAKE_SOURCE_DIR}/../selectedAttractors@ChaoticAttractors/")

# grouped EMCC_OPTIONS
#     --embed-file \"${CMAKE_SOURCE_DIR}/../ChaoticAttractors@ChaoticAttractors/\"\

    if(BUILD_prng_test)
        set(M_EMCC_EMBED_FILES "--preload-file \"${CMAKE_SOURCE_DIR}/../Shaders@Shaders/\"\
         --preload-file \"${CMAKE_SOURCE_DIR}/../colorMaps/palettes.json@colorMaps/\"\
         --preload-file \"${CMAKE_SOURCE_DIR}/../Fonts@Fonts/\"\
         --preload-file \"${CMAKE_SOURCE_DIR}/../rndData@rndData/\"")
        add_definitions(-DGLCHAOSP_TEST_RANDOM_DISTRIBUTION)
    else()
        set(M_EMCC_EMBED_FILES "--preload-file \"${CMAKE_SOURCE_DIR}/../Shaders@Shaders/\"\
         --preload-file \"${M_EMCC_EMBED_FEW_ATTRACTORS}\"\
         --preload-file \"${CMAKE_SOURCE_DIR}/../colorMaps/palettes.json@colorMaps/\"\
         --preload-file \"${CMAKE_SOURCE_DIR}/../Fonts@Fonts/\"\
         --preload-file \"${CMAKE_SOURCE_DIR}/../startData@startData/\"")
    endif()
     
#     --embed-file \"${CMAKE_SOURCE_DIR}/../Fonts/Cousine-Regular.ttf@Fonts/\"\
#     --embed-file \"${CMAKE_SOURCE_DIR}/../Fonts/fontawesome-webfont.ttf@Fonts/\"\

    # set(M_EMCC_EMBED_FILES "--preload-file \"${CMAKE_SOURCE_DIR}/Shaders@/Shaders\"")
    # -s FULL_ES3=1 -s OFFSCREENCANVAS_SUPPORT=1
    set(M_EMCC_OGL_OPTIONS "-s USE_WEBGL2=1 -s USE_GLFW=3 -s LEGACY_GL_EMULATION=0 -s OFFSCREENCANVAS_SUPPORT=1 ")
    set(M_EMCC_OPTIONS "-s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s ALLOW_MEMORY_GROWTH=1 ${M_EMCC_SHELL_FILE} ${M_EMCC_OGL_OPTIONS} ${M_EMCC_BINARYEN} ${M_EMCC_EMBED_FILES}")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OGL_EMITTER_TYPE} ${M_EMCC_OPTIONS} ${GLAPP_DISABLE_DLA}")

    add_executable(${PROJECT_NAME} 
                    ${SRC}/emsTouch.cpp
                    ${SRC}/emsTouch.h
                    ${SOURCE_FILES})

# relative emscripten output files (EMSOUT_PATH for my RAMDISK, if defined)
    if(DEFINED $ENV{EMSOUT_PATH})
        set(EMSOUT "$ENV{EMSOUT_PATH}")
    else()
        set(EMSOUT "${CMAKE_SOURCE_DIR}/..")
    endif()
    target_compile_features(${PROJECT_NAME}  PRIVATE cxx_std_14)

    if(EXISTS $ENV{RAMDISK}) #my RAMDISK env
        set(BUILD_DIR $ENV{RAMDISK}/emsOut/${CMAKE_BUILD_TYPE})
    else ()
        set(BUILD_DIR ${EMSOUT}/emsOut/${CMAKE_BUILD_TYPE})
    endif()
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

#strip "_LowRes" from .js file to load just one wglChAoSP.data file, common for both version
    if(NOT DEFINED BuildForProduction)
        set(BuildForProduction FALSE BOOL)
    endif()
    if(BUILD_wglChAoSP_lowres AND BuildForProduction)
        if(WIN32)
            add_custom_command(TARGET wglChAoSP_lowres POST_BUILD
                               COMMAND powershell -Command "(gc ${BUILD_DIR}\\wglChAoSP_lowres.js) -replace 'wglChAoSP_lowres.data', 'wglChAoSP.data' | Out-File -encoding ASCII ${BUILD_DIR}\\wglChAoSP_lowres.js")
            add_custom_command(TARGET wglChAoSP_lowres POST_BUILD
                               COMMAND del ${BUILD_DIR}/wglChAoSP_lowres.data)
        else()
            add_custom_command(TARGET wglChAoSP_lowres POST_BUILD
                               COMMAND sed -i 's/wglChAoSP_lowres.data/wglChAoSP.data/g' ${BUILD_DIR}/wglChAoSP_lowres.js)
            add_custom_command(TARGET wglChAoSP_lowres POST_BUILD
                               COMMAND rm ${BUILD_DIR}/wglChAoSP_lowres.data)
        endif()
    endif()
else()
###########################################################
#   glChAoS.P - Windows/Linux/MacOS build
###########################################################

    if(APPLE)
        project(glChAoSP_OSX)
    else()
        if(WIN32)
            project(glChAoSP)
        else()
            project(glChAoSP_Linux)
        endif()
    endif()

    #set(CMAKE_CXX_STANDARD 14)

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

# GLFW package - default search for already installed
#
#       to use internal GLFW set TRUE
set(GLAPP_USE_INTERNAL_GLFW FALSE)
if(${GLAPP_USE_INTERNAL_GLFW})
    target_include_directories(${PROJECT_NAME} PUBLIC ${SRC}/libs/glfw/include)

    if(APPLE)
        target_link_directories(${PROJECT_NAME} PUBLIC ${SRC}/libs/glfw/buildOSX/lib)
        set(LIB_GLFW "-lglfw3")
    else()
        if(WIN32)
            if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU") # MinGW
                # need to have in the LIB env var
                set(LIB_GLFW "-lglfw3")
            else()
                target_link_directories(${PROJECT_NAME} PRIVATE ${SRC}/libs/glfw/buildWin/lib/${CMAKE_BUILD_TYPE})
                set(LIB_GLFW "../../src/libs/glfw/buildWin/lib/${CMAKE_BUILD_TYPE}/glfw3")
            endif()
        else()
        # both MinGW and Linux
            target_link_directories(${PROJECT_NAME} PRIVATE ${SRC}/libs/glfw/buildLinux/lib)
            target_link_directories(${PROJECT_NAME} PRIVATE ${SRC}/libs/glfw/buildLinux/lib64)
            set(LIB_GLFW "glfw3")
        endif(WIN32)
    endif(APPLE)
else()
    find_package(glfw3)
    if(glfw3_FOUND)
        set(LIB_GLFW "glfw")
    else()
        message (FATAL_ERROR "GLFW not found... REQUIRED!!!!")
    endif()
endif()

# OpenGL
#
    find_package(OpenGL)
if(OPENGL_FOUND)
    message(STATUS "OPENGL_INCLUDE_DIRS: ${OPENGL_INCLUDE_DIRS}")
    message(STATUS "OPENGL_LIBRARY: ${OPENGL_LIBRARY}")
    #include_directories(${OPENGL_INCLUDE_DIRS}) # no more necessary: modern OGL uses loaders ==> glad
else ()
    message (FATAL_ERROR "OPENGL not found... REQUIRED!!!!")
endif()

# Debug - list CMake vars
#
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

target_include_directories(${PROJECT_NAME} PUBLIC ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${SRC}/tools)
target_include_directories(${PROJECT_NAME} PUBLIC ${SRC}/libs)
#target_include_directories(${PROJECT_NAME} PUBLIC $ENV{INCLUDE})

add_definitions(-DVGM_USES_TEMPLATE -DGLFW_INCLUDE_NONE -DIMGUI_IMPL_OPENGL_LOADER_GLAD -DGLM_ENABLE_EXPERIMENTAL)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(WIN32)
        set($ENV{CL},"")
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    endif()
    target_compile_options(${PROJECT_NAME} PRIVATE -Ofast -Wno-format-security -Wno-format-nonliteral -Wdeprecated-declarations -Wno-reorder -Wno-logical-op-parentheses -fexceptions -Wno-unused-command-line-argument -Wno-parentheses)
    message("Clang...")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -fopenmp -Wno-format-security -fpermissive)
    #add_compile_options(--verbose -Wno-format-security -std=c++14 -fpermissive)  -O3 -ftree-vectorize -mavx2
    message("GNU...")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # add_compile_options("/W1 /Qpar /Gm- /O2 /Ob2 /Zc:inline /fp:precise /WX- /Zc:forScope /Gd /FC /Fa /Ot /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W1 /Qpar /Gm- /O2 /Ob2 /Zc:inline /fp:precise /WX- /Zc:forScope /EHsc /Gd /FC /Fa /Ot")
    # Force to always compile with W1 (before CMake 3.15)
    # if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    #     string(REGEX REPLACE "/W[0-4]" "/W1" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    # else()
    #     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W1")
    # endif()
    # MSVC_RUNTIME_LIBRARY (from CMake 3.15)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT.lib " )
    message("MSVC...")
endif()

#add_compile_options("${OGL_EMITTER_TYPE} ${COMMON_DEFINES} ${GLAPP_DISABLE_DLA}")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${OGL_EMITTER_TYPE} ${COMMON_DEFINES} ${GLAPP_DISABLE_DLA}")

if(EXISTS $ENV{RAMDISK}) #my RAMDISK env var
    message("RamDisk...")
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY $ENV{RAMDISK}/glChAoSP_release)
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/..)
endif()


    if(APPLE)
        add_definitions(${OGL41} -DGLCHAOSP_NO_USES_GLSL_SUBS -DGLAPP_NO_GLSL_PIPELINE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-logical-op-parentheses -Wno-unused-command-line-argument -Wno-parentheses -framework Cocoa -framework IOKit -framework CoreVideo")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security -Wno-format-nonliteral -Wdeprecated-declarations -Wno-reorder -Wno-logical-op-parentheses -fexceptions -Wno-unused-command-line-argument -Wno-parentheses")
        set(TARGET_LIBS ${CMAKE_DL_LIBS} "-lpthread")
        message("AppleClang...")
    else()
       add_definitions(${OGL_VER_USED})

        if(WIN32)
            set(TARGET_LIBS ${CMAKE_DL_LIBS})
        else()
            set(TARGET_LIBS ${CMAKE_DL_LIBS} "-fopenmp -lpthread -lX11")
        endif(WIN32)
    endif(APPLE)

    target_compile_features(${PROJECT_NAME}  PRIVATE cxx_std_14)
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY} ${TARGET_LIBS} ${LIB_GLFW})
endif()
