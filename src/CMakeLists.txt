cmake_minimum_required(VERSION 3.10)

###########################################################
#   OpenGL 4.5 - Zero Driver Overhead
#
# OGL 4.5 Thread and mappedBuffer on GPU (fastest on new GPU)
set(OGL45_THREAD_MAPMEM "-DGLAPP_REQUIRE_OGL45 -DUSE_THREAD_TO_FILL -DUSE_MAPPED_BUFFER")
# OGL 4.5 Thread and CPU buffer (fastest)
set(OGL45_THREAD "-DGLAPP_REQUIRE_OGL45 -DUSE_THREAD_TO_FILL")
# OGL 4.5 NO separate thread, static CPU buffer
set(OGL45_BUFFER "-DGLAPP_REQUIRE_OGL45")

###########################################################
#   OpenGL 4.1 - Olden GPU and MacOS X
#
# OGL 4.1 Thread and CPU buffer (fastest on openGL 4.1)
set(OGL41_THREAD "-DUSE_THREAD_TO_FILL")
# OGL 4.1 without separate thread & static CPU buffer
set(OGL41_BUFFER "")


set(CMAKE_CXX_STANDARD 14)
find_package(OpenGL)

include_directories(src)
include_directories(src/tools)
include_directories(src/libs)

###########################################################
#   EMSCRIPTEN setting
###########################################################
if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    project(ParticlesEMS)
    set(OGL_EMITTER_TYPE ${OGL41_BUFFER})


    set(SOURCE_FILES
        src/libs/IconsFontAwesome/IconsFontAwesome.h
        src/libs/ImGui/imconfig.h
        src/libs/ImGui/imgui.cpp
        src/libs/ImGui/imgui_widgets.cpp
        src/libs/ImGui/imgui.h
        src/libs/ImGui/imgui_demo.cpp
        src/libs/ImGui/imgui_draw.cpp
        src/libs/ImGui/imgui_impl_glfw.cpp
        src/libs/ImGui/imgui_impl_glfw.h
        src/libs/ImGui/imgui_impl_opengl3.cpp
        src/libs/ImGui/imgui_impl_opengl3.h
        src/libs/ImGui/imgui_internal.h
        src/libs/ImGui/imstb_rectpack.h
        src/libs/ImGui/imstb_textedit.h
        src/libs/ImGui/imstb_truetype.h
        src/libs/configuru/configuru.hpp
        src/tools/fastRandom.cpp
        src/tools/fastRandom.h
        src/tools/imGuIZMO.cpp
        src/tools/imGuIZMO.h
        src/tools/imguiControls.cpp
        src/tools/imguiControls.h
        src/tools/glslProgramObject.cpp
        src/tools/glslProgramObject.h
        src/tools/glslShaderObject.cpp
        src/tools/glslShaderObject.h
        src/tools/transforms.h
        src/tools/virtualGizmo.h
        src/ui/uiAttractorsDlg.cpp
        src/ui/uiMainDlg.cpp
        src/ui/uiMainDlg.h
        src/ui/uiSettings.cpp
        src/ui/uiSettings.h
        src/appDefines.h
        src/attractorsBase.cpp
        src/attractorsBase.h
        src/attractorsFiles.cpp
        src/attractorsStartVals.cpp
        src/attractorsStartVals.h
        src/configFile.cpp
        src/colorMaps.cpp
        src/glApp.cpp
        src/glApp.h
        src/glWindow.cpp
        src/glWindow.h
        src/mmFBO.cpp
        src/mmFBO.h
        src/palettes.cpp
        src/palettes.h
        src/ParticlesUtils.cpp
        src/ParticlesUtils.h
        src/ShadersClasses.cpp
        src/ShadersClasses.h
        src/vertexbuffer.cpp
        src/vertexbuffer.h)

# -o ${PROJECT_NAME}.html ->  CMAKE_EXECUTABLE_SUFFIX  .html .bc, .js (default), etc
set(CMAKE_EXECUTABLE_SUFFIX ".html")

# relative emscripten output files
    set(EMSOUT "emsOut")
    file(MAKE_DIRECTORY ${EMSOUT})
    file(MAKE_DIRECTORY ${EMSOUT}/${CMAKE_BUILD_TYPE})
    set(EMSOUT ${EMSOUT}/${CMAKE_BUILD_TYPE})

# emcc compiler options
    set(M_EMCC_FLAGS "-DGLAPP_NO_OGL_DSA -DGLFW_INCLUDE_ES3")

# BUILD TYPE: [Debug|RelWithDebInfo|Release|MinSizeRel]    
    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(M_EMCC_BINARYEN "-s WASM=0 -s \"BINARYEN_METHOD='interpret-s-expr'\"")
        set(M_EMCC_SHELL_FILE "")
    else()
        #  -s BINARYEN_METHOD='native-wasm,asmjs' 'interpret-asm2wasm' -s BINARYEN_ASYNC_COMPILATION=0
        if(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
            set(M_EMCC_BINARYEN "-s WASM=1")
            set(M_EMCC_SHELL_FILE "--shell-file \"${SRC}/shell_minimal.html\"")
        else()
            if(${CMAKE_BUILD_TYPE} MATCHES "MinSizeRel")
                # one file no memory option: -s SINGLE_FILE=1 --memory-init-file 0 
                set(M_EMCC_BINARYEN "-s WASM=1  -s \"BINARYEN_METHOD='native-wasm'\" ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${SRC}/mVeryMinimal.html\"")
            else() # Release 
                # -s SINGLE_FILE=1 --memory-init-file 0 -s \"BINARYEN_METHOD='interpret-asm2wasm'\"
                set(M_EMCC_BINARYEN "-s WASM=1 -s \"BINARYEN_METHOD='native-wasm'\" ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${SRC}/mVeryMinimal.html\"")
            endif()    
        endif()    
    endif()

# grouped EMCC_OPTIONS
    set(M_EMCC_EMBED_FILES "--embed-file \"${CMAKE_SOURCE_DIR}/../Shaders/PointSpriteVert.glsl@Shaders/\" --embed-file \"${CMAKE_SOURCE_DIR}/../Shaders/PointSpriteFragLight.glsl@Shaders/\"")
    # set(M_EMCC_EMBED_FILES "--preload-file \"${CMAKE_SOURCE_DIR}/Shaders@/Shaders\"")
    # -s FULL_ES3=1 -s OFFSCREENCANVAS_SUPPORT=1
    set(M_EMCC_OGL_OPTIONS "-s USE_WEBGL2=1 -s USE_GLFW=3 -s LEGACY_GL_EMULATION=0")

# merge all EMCC_OPTIONS
    set(M_EMCC_OPTIONS "-s \"BINARYEN_TRAP_MODE='clamp'\" ${M_EMCC_SHELL_FILE} ${M_EMCC_OGL_OPTIONS} ${M_EMCC_BINARYEN} ${M_EMCC_EMBED_FILES}")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${EMSOUT}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${M_GLOBAL_FLAGS} ${M_EMCC_FLAGS} ${M_EMCC_OPTIONS}")

    add_executable(${PROJECT_NAME} ${SOURCE_FILES})


###########################################################
#   PC setting
###########################################################
else()
#   Default best emitter type and OpenGL
    if(NOT OGL_EMITTER_TYPE)
        if(APPLE)
            set(OGL_EMITTER_TYPE ${OGL41_THREAD})
        else()
            set(OGL_EMITTER_TYPE ${OGL45_THREAD_MAPMEM})
        endif()
    endif()    


    if(APPLE)
        project(ParticlesOSX)
    else()
        if(WIN32)
            project(Particles)
        else()
            project(ParticlesLinux)
        endif()
    endif()

if(OPENGL_FOUND)
    message(STATUS "OPENGL_INCLUDE_DIRS: ${OPENGL_INCLUDE_DIRS}")
    message(STATUS "OPENGL_LIBRARY: ${OPENGL_LIBRARY}")

    include_directories(${OPENGL_INCLUDE_DIRS})
else ()
    message (FATAL_ERROR "OPENGL not found... REQUIRED!!!!")
endif(OPENGL_FOUND)

# set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/builds")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/..")

if(APPLE)
    set(COMMON_DEFINES "-DGLFW_INCLUDE_NONE -DIMGUI_IMPL_OPENGL_LOADER_GLAD -DGLM_ENABLE_EXPERIMENTAL -framework Cocoa -framework IOKit -framework CoreVideo")
else()
    set(COMMON_DEFINES "-DGLFW_INCLUDE_NONE -DIMGUI_IMPL_OPENGL_LOADER_GLAD -DGLM_ENABLE_EXPERIMENTAL")
endif(APPLE)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security ${OGL_EMITTER_TYPE} ${COMMON_DEFINES} -std=c++14 -fpermissive")

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
        src/libs/glad/glad.cpp
        src/libs/glad/glad.h
        src/libs/IconsFontAwesome/IconsFontAwesome.h
        src/libs/ImGui/imconfig.h
        src/libs/ImGui/imgui.cpp
        src/libs/ImGui/imgui_widgets.cpp
        src/libs/ImGui/imgui.h
        src/libs/ImGui/imgui_demo.cpp
        src/libs/ImGui/imgui_draw.cpp
        src/libs/ImGui/imgui_impl_glfw.cpp
        src/libs/ImGui/imgui_impl_glfw.h
        src/libs/ImGui/imgui_impl_opengl3.cpp
        src/libs/ImGui/imgui_impl_opengl3.h
        src/libs/ImGui/imgui_internal.h
        src/libs/ImGui/imstb_rectpack.h
        src/libs/ImGui/imstb_textedit.h
        src/libs/ImGui/imstb_truetype.h
        src/libs/lodePNG/lodepng.cpp
        src/libs/lodePNG/lodepng.h
        src/libs/tinyFileDialog/tinyfiledialogs.c
        src/libs/tinyFileDialog/tinyfiledialogs.h
        src/libs/configuru/configuru.hpp
        src/tools/fastRandom.cpp
        src/tools/fastRandom.h
        src/tools/imGuIZMO.cpp
        src/tools/imGuIZMO.h
        src/tools/imguiControls.cpp
        src/tools/imguiControls.h
        src/tools/glslProgramObject.cpp
        src/tools/glslProgramObject.h
        src/tools/glslShaderObject.cpp
        src/tools/glslShaderObject.h
        src/tools/oglAxes.cpp
        src/tools/oglAxes.h
        src/tools/transforms.h
        src/tools/virtualGizmo.h
        src/ui/uiAttractorsDlg.cpp
        src/ui/uiMainDlg.cpp
        src/ui/uiMainDlg.h
        src/ui/uiSettings.cpp
        src/ui/uiSettings.h
        src/appDefines.h
        src/attractorsBase.cpp
        src/attractorsBase.h
        src/attractorsFiles.cpp
        src/attractorsStartVals.cpp
        src/attractorsStartVals.h
        src/configFile.cpp
        src/colorMaps.cpp
        src/glApp.cpp
        src/glApp.h
        src/glWindow.cpp
        src/glWindow.h
        src/mmFBO.cpp
        src/mmFBO.h
        src/palettes.cpp
        src/palettes.h
        src/ParticlesUtils.cpp
        src/ParticlesUtils.h
        src/ShadersClasses.cpp
        src/ShadersClasses.h
        src/vertexbuffer.cpp
        src/vertexbuffer.h)

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
