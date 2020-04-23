<h1 align="center"> <a href="https://michelemorrone.eu/glchaosp">glChAoS.P / wglChAoS.P - Ver 1.4.2</a> => v.1.5 WiP</h1>

[**glChAoS.P**](https://michelemorrone.eu/glchaosp) / [**wglChAoS.P**](https://michelemorrone.eu/glchaosp): open**gl** / **w**eb**gl** **Ch**aotic **A**ttractors **o**f **S**light (**dot**) **P**articles
A real time 3D strange attractors scout... and hypercomplex fractals

To view all Attractors / Fractals currently inserted in this version, follow the link: [**Attractors Formulas**](https://www.michelemorrone.eu/glchaosp/index.html#AttractorsFormula2)

**now any single attractor is also explorable interactively via WebGL/WebAssembly directly from site.*


## All available releases &nbsp; &nbsp; ==> [**Release Notes**](https://github.com/BrutPitt/glChAoS.P/releases) (what's new)

### Desktop - v.1.4.2 (stable release)
- **[glChAoS.P](https://github.com/BrutPitt/glChAoS.P/releases)** - **DeskTop** - binaries available **Windows** / **Linux** / **OS X** 
### WebGL - v.1.5.0 WiP (Work in Progress - Preview)
- **[wglChAoS.P](https://michelemorrone.eu/glchaosp/webGL.html)** - **WebG 2** via **webAssembly** - **live** / **online** using your browser - also for mobile devices 
  - You can select **Advanced Mode** check box (*default*) or deselect it (**Standard Mode**) for low resources devices (mobiles/tablet/smartphones)
  - You can also to explore any single attractor, interactively, staring directly from web-page, using **Explore** button near/below any attractor formula: [**Attractors Formulas**](https://www.michelemorrone.eu/glchaosp/index.html#AttractorsFormula2)

<a href="https://www.jetbrains.com/?from=glChAoS.P" target="_blank"><img align="right" width="100" height="100" src="https://michelemorrone.eu/glchaosp/img/jetbrains.svg"/></a>
<p>&nbsp;<br></p>
<p align="right">This project is supported by <b><a  href="https://www.jetbrains.com/?from=glChAoS.P" target="_blank">JetBrains</a></b><br> <i>thanks to <a  href="https://www.jetbrains.com/?from=glChAoS.P" target="_blank">open-source license</a> for all its products</i></p>




### New ver.1.4.2  feature: 11 unpublished attractor types: [ **PopCorn, Mira, Hopalong and**... ](https://www.michelemorrone.eu/glchaosp/PopCorn_and.html)
An absolutely personal and original "transformations" in 3D/4D of famous 2D attractors:

| PopCorn 4D (ssss) | Martin 4D |
| :---: | :---: |
| ![sShot_20191118_173632](https://user-images.githubusercontent.com/16171743/69288769-cbf85300-0bfa-11ea-8b11-1d916e43af89.jpg) | ![sShot_20191113_43629](https://user-images.githubusercontent.com/16171743/69288951-5c369800-0bfb-11ea-8a24-53d4ae9909c6.jpg) |
|  **Mira 4D** | **Mira 3D** |
| ![sShot_20191113_22737](https://user-images.githubusercontent.com/16171743/69289055-a0299d00-0bfb-11ea-98ac-f18e4a62eed6.jpg) | ![sShot_20191113_2269](https://user-images.githubusercontent.com/16171743/69289104-c8b19700-0bfb-11ea-929b-28770887dc46.jpg) |
|  **PopCorn 4D (scsc)** | **Hopalong 4D** |
| ![sShot_20191112_04710](https://user-images.githubusercontent.com/16171743/69289281-4d041a00-0bfc-11ea-91d0-31a9c00caf37.jpg) | ![sShot_20191113_22333](https://user-images.githubusercontent.com/16171743/69289313-65743480-0bfc-11ea-98d8-ed7f203eb623.jpg) |

Full descriptions, math formulas and code in the relative webpage: [ **PopCorn, Mira, Hopalong and**... ](https://www.michelemorrone.eu/glchaosp/PopCorn_and.html)

**now any single attractor is also explorable interactively via WebGL/WebAssembly directly from site.*


<p>&nbsp;<br>&nbsp;<br></p>

## Particle System rendering features
- **100M** of particles in minus of 1.6 GByte of VRAM.. up to **265M** (4G VRAM): **S**light (**dot**) **P**articles
  - only a **vec4** (4-float) per vertex for position and color
- **Rendering**: RealTime Surface Reconstruction / Shadows / Ambient Occlusion 
- **Light Models**: Phong / Blinn-Phong / GGX
- **Glow effects**: Gaussian Blur / bilateral deNoise with threshold / Gaussian + deNoise
- **Anti-aliasing**: FXAA
- **Image adjustment**: Bright / Contrast / Gamma / Exposure / ToneMapping

With **Billboard** and **PointSprite** techniques

### 3D DLA (Diffusion Limited Aggregation)  - [DLA3D Video Example](https://youtu.be/_R7akPh64XU)
[![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/dla3D.jpg)](https://youtu.be/_R7akPh64XU)
Personal [DLAf-optimized](https://github.com/BrutPitt/DLAf-optimized) project, adaptation and optimization from original code of [Michael Fogleman](https://github.com/fogleman), with possibility to export/import to/from PLY format and to continue a previous rendering.

### Rendering models with Aizawa attractor - **[Video Example](https://youtu.be/mkX4fdv-kZU)**

| Alpha Blending | Solid Dots |
| :-----: | :----: |
|![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/sShot_2019818_5533.jpg) | ![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/sShot_2019818_55251.jpg)|

### Further rendering engine improvement (from ver. >= 1.3.x) - **[Rendering Video Example](https://youtu.be/mkX4fdv-kZU)**
| DualPass rendering: Z-buffer surface reconstruction | DualPass + Ambient Occlusion |
| :---: | :---: |
| ![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/sShot_2019818_55315.jpg) | ![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/sShot_2019818_55328.jpg)|
| **DualPass + AO + Shadows** | **DualPass + AO + Shadows + mixed AlphaBlending** |
| ![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/sShot_2019818_55342.jpg) | ![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/sShot_2019818_55450.jpg)|

**features also available in WebGL advanced version*

<p>&nbsp;<br>&nbsp;<br></p>

## wglChAoS.P - WebGL2 online version - v.1.5 WiP (Work in Progress - Preview)

<p align="center"><a href="https://michelemorrone.eu/glchaosp/webGL.html"> 
<img src="https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/ssWGLtitle1024.jpg"></a>
</p>

This is a **WebGL2** / **WebAssembly** lightened **LIVE / ONLINE** version of **glChAoSP** and Supports touch screen for mobile devices, smartphones and tablets

<p align="center"><b align="center">Go to <a href="https://michelemorrone.eu/glchaosp/webGL.html">wglChAoS.P - LIVE / ONLINE</a> starting page</b></p>

<p>&nbsp;<br></p>


## wglChAoS.P Advanced Mode - WebGL2 with new rendering engine
Starting from ver 1.3.2, also WebGL have **new rendering engine**: dual pass accurate rendering, shadows and ambient occlusion



| ![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/wglChAoSP/ssShot1.jpg) | ![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/wglChAoSP/ssShot2.jpg) |
| :---: | :---: |
| ![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/wglChAoSP/ssShot3.jpg) | ![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/wglChAoSP/ssShot4.jpg)|

**Advanced Mode not available/tested still for mobile devices*

<p>&nbsp;<br>&nbsp;<br></p>


## [Hypercomplex fractals](https://michelemorrone.eu/glchaosp/Hypercomplex.html) like attractors - via IIM (Inverse Iterations Method)
| hypercomplex fractals | hypercomplex fractals |
| :-----: | :----: |
|[![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/ssHyp.jpg)](https://www.michelemorrone.eu/glchaosp/Hypercomplex.html) | [![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/biComplex1.gif)](https://www.michelemorrone.eu/glchaosp/Hypercomplex.html)|


<p>&nbsp;<br>&nbsp;<br></p>

## glChAoS.P features
glChAoS.P is a 3D realtime Particle System with some unique features:

- **100M** of particles in minus of 1.6 GByte of VRAM.. until **265M** (4G VRAM): **S**light (**dot**) **P**articles
  - only 4-float (vec4) per vertex: position and color
- PointSprite and Billboard particles types
- 3D blended/solid/lighted particles
- Single dot or DualPass rendering (surface reconstruction from zBuffer)
- Shadows and AmbientOcclusion
- Light models: Phong / Blinn-Phong / GGX 
- Distance attenuation on Size an Alpha channel
- Full customizable colors, with several color palettes
- Customizable glow effects: Gaussian Blur / bilateral deNoise with threshold / Gaussian + deNoise
- Customizable FXAA filter
- Motion blur
- Post processing image correction:
    - gamma
    - exposure
    - brightness 
    - contrast 
    - toneMapping / HDR

... and more

For more usage info: [**glChAoS.P info**](https://michelemorrone.eu/glchaosp/info.html)

<p>&nbsp;<br>&nbsp;<br></p>

## Some screenshots from different operating systems

| Windows | Windows Viewports |
| :-----: | :----: |
|[![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/osSS01.jpg)](https://michelemorrone.eu/glchaosp/screenshots.html#osSShots) | [![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/ss001.jpg)](https://michelemorrone.eu/glchaosp/screenshots.html#osSShots)|
| Linux | Mac OS X |
| [![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/osSS09.jpg)](https://michelemorrone.eu/glchaosp/screenshots.html#osSShots) | [![](https://raw.githubusercontent.com/BrutPitt/myRepos/master/glChAoSP/screenShots/osSS04.jpg)](https://michelemorrone.eu/glchaosp/screenshots.html#osSShots)|

<p>&nbsp;<br>&nbsp;<br></p>

### Hardware required:

- **glChAoS.P** - GPU with OpenGL 4.1 or higher
- **wglChAoS.P** - Browser with WebGL2 capabilities (FireFox, Chrome or Chromium based browsers)

Theoretically all graphics card that supports OpenGL 4.0 supports also OpenGL 4.5, depends only from drivers (and O.S.)

About the GPUs that support the OpenGL 4.5 (with appropriate drivers and OS permitting):
- NVidia starting from GT/GTX 4xx series
- AMD starting from HD 5xxx series
- Intel starting from Ivy Bridge/Bay Trail CPUs (with HD 4000/2500 graphics)
  


### Hardware recommended:

- GPU starting from AMD HD 7970 / NVidia GTX 670 or with better performance
- CPU with 2 or more cores

### About performance
Glow effects, mostly with sigma \> 5, and DualPass/Shadows/AO requires expensive calculations in terms of performance

If you have slow performance try, in this order:
- Disable *DualPass* rendering and/or *AO* and/or *Shadows*
- Disable *GlowEffects* and/or *FXAA* 
- Prefer *Pointsprite*: on AMD and Intel GPU (sensible difference of performance) 
- Decrease number of particles buffer \< 3000
- Decrease point size (if you can)
- try squared 1024x1024 (power of 2) window size (from *Settings panel*)


### Supported Operating systems:

- [x] Microsoft Windows
- [x] Linux/Unix
- [x] Mac OS X
- [x] Android via webBrowser (**wglChAoS.P** WebGL/webAssembly, lightned version)

**Tested Operating System Versions:**

- Microsoft Windows 7/8.x/10
- Linux distributions: Ubuntu 16.04 -> 20.04, Fedora 27 -> 31
- Mac OS 10.14 (Mojave), 10.15 (Catalina)
- Android 5/6/7/8/9 with Firefox and Chrome


### Executables

No installation program is provided: just clone it form github or download the archive and decompress it in a folder whatever: only the internal directories structure must be preserved.

For Windows and Linux glChAoSP uses OpenGL 4.5 with AZDO (Approaching Zero Driver Overhead) and a separate thread (multithread) emitter/generator of particles with memory mapped directly on GPU vRAM.

On Mac OS X, for a limitation of the OS (from Mojave Apple have deprecated OpenGL) there is a downgraded version that use OpenGL 4.1 (higher possible) with a separate thread emitter/generator that uses the conventional CPU memory.

Are provided executable for the followings OS:

- **Windows**\
`glChAoSP.exe` and `glChAoSP_32.exe`: native executable (64 and 32 bit) are provided for OpenGL \>= 4.5: *Preferably use the 64-bit version*.
(you can recompile it, with appropriate define, to obtain an OpenGL 4.1 compliant)
\
`glChAoSP_viewports.exe`: beta version of multiple viewports is also provided (floating GUI windows that can go out outside of main Viewport/Window): (only 64 bits).
This is a great version, solid beta and full functional, but based on a *under development* version of  [**ImGui**](https://github.com/ocornut/imgui) library.
\
*Tested on Windows 7, 8 and 10*

- **Linux**\
`glChAoSP_Linux`: native executable (64 bit) is provided for OpenGL \>= 4.5, 
(you can recompile it, with appropriate define, to obtain an OpenGL 4.1 compliant)
\
After clone, copy or decompression, take care that executable bit of the file `glChAoSP_Linux` is active (chmod +x).
\
Take care to have installed OpenGL library, whereas `libX11 libXext` should already be installed.
\
It was tested on Fedora 27/28 and Ubuntu 16.04/18.04 LTS, although with subsequent updates and different library versions you may need to rebuild it.
Read Build/CMake sections for further information.
\
*Tested on Fedora 27 -> 31 and Ubuntu 16.04 -> 20.04*

    - **wine**    
    The Windows executable, 32/64 bit, works fine also in wine 3.xx with no evident loss of performance

- **Mac OS**\
`glChAoSP_OSX`: native executable (64 bit) is provided for OpenGL 4.1
\
From *Finder* click on applescript: `glChAoSP_OSX.app`, or form command line type directly the command: `./glChAoSP_OSX`
\
It was tested on OS X ver 10.14 (Mojave) only, although with subsequent updates and different library versions you may need to rebuild it.
Read Build/CMake sections for further information.
\
*Tested on 10.14 Mojave and 10.15 Catalina*

**NOTE**:
\
For **Windows** and **Linux** glChAoS.P uses OpenGL 4.5 with AZDO (Approaching Zero Driver Overhead) and a separate thread (multithread) emitter/generator of particles with memory mapped directly on GPU vRAM.

On **Mac OS**, for a limitation of the OS (from Mojave Apple have deprecated OpenGL) there is a downgraded version, that use OpenGL 4.1 (higher possible) with a separate thread emitter/generator that use the conventional CPU memory.

Furthers build option are provided:
- Use OpenGL 4.1 also on Windows and Linux.
- Single thread version.


<p>&nbsp;<br>&nbsp;<br></p>

## To build [**glChAoS.P**](https://michelemorrone.eu/glchaosp)

### Build requirements

- Compilers with full C++14 standard required
- CMake 3.10 or higher
- ~~Boost Library to build DLA3D (Diffusion Limited Aggregation) object exploration, (or uncomment DISABLE_DLA3D in CMake file to disable it)~~ \
it has been replaced with **nanoflann** header-only (enclosed)


**Tested Compilers**

- Microsoft Visual Studio 2019/2017/2015 (Platform Toolset v.142/141/140: it does not work with previous versions)
- MinGW (64bit) v.9
- CLang v.5/6/7/8/9
- GNU C++ v.5/6/7/8/9


**CMake**

In the folder `./src` there is the `CmakeLists.txt`, use this folder as base directory.

Read below more about your OS.

**GLFW Library**

Enclosed 32/64bit built library for Windows, and 64bit for Linux and OS X 
\
When build glChAoSP, compiler looks for GLFW library in the follows path:
- src/src/libs/glfw/buildLinux (Linux)
- src/src/libs/glfw/buildOSX (OSX)
- src/src/libs/glfw/buildWin (Windows)

You need to delete it, and/or modify the CMake file, to use personal ones version.
In this case you need to have installed GLFW Library, ver 3.3 or higher, or re-build the enclosed version in `./src/src/libs/glfw` folder.

### Build glChAoS.P in Windows 

Windows user needs of Visual Studio 2019 (it works also wit VS 2017/2015, but is need to change appropriate *Platform Toolset* and/or *Windows SDK version* that you have installed). In alternative, CMake 3.10 (or higher) for other compilers toolchain (non tested, but it should work).

- **Microsoft Visual Studio**
  - **VS solution**
\
In the folder `./src/msBuilds` there is the solution project for use with Visual Studio 2017/2019.
\
*(check appropriate **Platform Toolset** and/or **Windows SDK version** that you have installed)*
\
You can use also **LLVM CLang** to build **glChAoS.P** from Visual Studio: you can use the LLVM plugin (after to have installed clang, in windows) and simply change the *toolchain* in *Properties -> General -> Platform Toolset*
     - The current VisualStudio solution refers to my environment variable RAMDISK (`R:`), and subsequent VS intrinsic variables to generate binary output:
`$(RAMDISK)\$(MSBuildProjectDirectoryNoRoot)\$(DefaultPlatformToolset)\$(Platform)\$(Configuration)\` 
\
Even without a RAMDISK variable, executable and binary files are outputted in base to the values of these VS variables, starting from root of current drive.

   - **VS with CMakeFile.txt and CMakeSettings.json** (testing fase - VS2019 only)
   \
   Open `./src` folder in **vs2019** you can build both **Emscripten** / **CLang** inside Visual Studio



- **CMake**
    - You can use CMake to compile with CLang / MinGW, using mingw_make or ninja tool.


***NOTE:** To build *viewports* version you need to add `-DGLAPP_IMGUI_VIEWPORT` to compiler flags, or uncomment it in `appDefines.h`*

### Build glChAoS.P in Linux

**Tools required**
\
Linux users need to install the GCC C/C++ v.5 or higher (or clang v.5 or higher) compilers and associated tools such as *make* and *CMake* (need v3.10 or higher).
\
To install gcc C/C++:
  - Debian, Ubuntu: `sudo apt-get install build-essential cmake cmake-qt-gui`
  - Fedora, RedHat: `sudo dnf install make gcc-c++ cmake cmake-gui`

 You need also to have installed OpenGL library and relative development package:
`libgl1-mesa libgl1-mesa-dev` (Ubuntu) or `mesa-libGL mesa-libGL-devel` (Fedora).

**Build**
\
Form a *Terminal* window, just launch `sh build_glChAoSP.sh` script (from `./src` folder) to build **glChAoSP**, it first runs `cmake` with appropriate parameters and then starts `make` to build `glChAoSP_Linux` executable: it will stored in parent folder (`../`).

- the script uses the enclosed built version of GLFW

Another script, `buildLinux.sh`, is provided (as helper) to re-build GLFW: it calls `buildGLFW.sh` (to build/re-build GLFW) and `build_glChAoSP.sh` sequentially.
 - To build/rebuild GLFW from enclosed sources you must have installed also development packages: `libx11-dev libxext-dev` (Ubuntu) or `libX11-devel libXext-devel` (Fedora).
\
**(documentation: [https://github.com/glfw/glfw](https://github.com/glfw/glfw))*

### Build glChAoS.P in Mac OS

**Tools required**
\
Mac users must have installed **Xcode** and the **Command Line Tools**, also **CMake 3.10** or higher is necessary.


**Build**
\
Form a *Terminal* window, just launch `sh build_glChAoSP.sh` script (from `./src` folder) to build **glChAoSP**, it first runs `cmake` with appropriate parameters and then starts `make` to build `glChAoSP_OSX` executable: it will stored in parent folder (`../`)
\
- the script uses the enclosed built version of GLFW

Another script, `buildOSX.sh`, is provided (as helper) to re-build GLFW: it calls `buildGLFW.sh OSX` (to build/re-build GLFW) and `build_glChAoSP.sh` sequentially.
\
**(documentation: [https://github.com/glfw/glfw](https://github.com/glfw/glfw))*


### Build wglChAoS.P with EMSCRIPTEN - WebGL via WebAssembly 

The CMake file is able to build also an [**EMSCRIPTEN**](https://kripken.github.io/emscripten-site/index.html) version, obviously you need to have installed EMSCRIPTEN SDK on your computer (~~1.38.20~~ ~~1.38.28~~ 1.38.40 or higher).
Use `emsCMakeGen.cmd` or `emsCMakeGen.sh` from ./src directory, or look inside it, to pass appropriate defines/parameters to CMake command line.
`emsCMakeGen` need to know the location of EMSDK, and the "build-type" object to create.

For example, run:

`emsCMakeGen.sh /opt/emsdk/emscripten/1.38.20 Debug|Release|RelWithDebInfo|MinSizeRel`


`emsCMakeGen.cmd C:\emsdk\emscripten\1.38.20 Debug|Release|RelWithDebInfo|MinSizeRel`

To build the EMSCRIPTEN version, in Windows, with CMake, need to have **mingw32-make.exe** in your computer and in the search PATH (only the make utility is enough): it is a condition of EMSDK tool to build with CMake in Windows.


<p>&nbsp;<br>&nbsp;<br></p>

## 3rd party tools and color maps

[**glChAoS.P**](https://michelemorrone.eu/glchaosp) uses 3rd party software tools components, they are located in the `./src/src/libs` folder and built with the program.
A copy of each is included in the repository, to avoid incompatibility with future changes.

Structure and description of 3rd parts libraries/tools/palettes, and related copyrights and licenses:

**Libs and Tools**

- Personal tools
    - tools/[**vgMath**](https://github.com/BrutPitt/vgMath) → 3D Mathematics https://github.com/BrutPitt/vgMath (single file header, **glm** compatible)

    - tools/[**vitualGizmo3D**](https://github.com/BrutPitt/virtualGizmo3D) → 3D objects manipulator https://github.com/BrutPitt/virtualGizmo3D (single file header)

    - tools/[**imGuIZMO.quat**](https://github.com/BrutPitt/imGuIZMO.quat) → ImGui widget: visual 3D objects manipulator https://github.com/BrutPitt/imGuIZMO.quat

- Third party tools 
    - libs/imGui → Dear ImGui
    [https://github.com/ocornut/imgui](https://github.com/ocornut/imgui)
    *(need version \>= 1.72)*

    - libs/configuru → Configuru, an experimental JSON config library for C++
    [https://github.com/emilk/Configuru](https://github.com/emilk/Configuru)

    - libs/tinyPLY → C++11 ply 3d mesh format importer & exporter 
    [https://github.com/ddiakopoulos/tinyply](https://github.com/ddiakopoulos/tinyply)

    - libs/lodePNG → LodePNG a PNG saver
    [https://github.com/lvandeve/lodepng](https://github.com/lvandeve/lodepng)

    - libs/tinyFileDialog → file dialogs ( cross-platform C C++ )
    [https://github.com/native-toolkit/tinyfiledialogs](https://github.com/native-toolkit/tinyfiledialogs)

    - libs/IconFontAwesome → IconFontCppHeaders / Font Icon Toolkit
    [https://github.com/juliettef/IconFontCppHeaders](https://github.com/juliettef/IconFontCppHeaders)
    [https://github.com/FortAwesome/Font-Awesome](https://github.com/FortAwesome/Font-Awesome)

    - libs/glad → GL/GLES/EGL/GLX/WGL Loader-Generator
    [https://github.com/Dav1dde/glad](https://github.com/Dav1dde/glad)

    - libs/dirent → "dirent" Unix/Linux filesystem interface port for Windows
    [https://github.com/tronkko/dirent](https://github.com/tronkko/dirent)

    - libs/glfw → A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input
    [https://github.com/glfw/glfw](https://github.com/glfw/glfw)
    *(need version \>= 3.3, recommended enclosed pre-built)*

    - libs/nanoflann -> header-only library for KD-Trees of datasets point clouds [https://github.com/jlblancoc/nanoflann](https://github.com/jlblancoc/nanoflann) (need only for DLA3D, in alternative you can use **Boost Library** compiling with `GLAPP_USE_BOOST_LIBRARY` define)


### Alternatives and optional, not closely necessary

Below there are some used components, which differ from **MIT** / **BSD 2-Clause** / **Zlib** license. 

**External Color Maps/Palettes - (optional - to load)**

- colorMaps/jjg\_gradient.json
colorMaps/jjg\_step.json → J.J. Green palettes (creative commons noncommercial license)
[http://soliton.vm.bytemark.co.uk/pub/cpt-city/jjg/ccolo/index.html](http://soliton.vm.bytemark.co.uk/pub/cpt-city/jjg/ccolo/index.html)
Conversion from gpf format to json 256 variations.
(these are optional separate files to load and to have more color maps)

**Alternative Library**

- **glm Math Library** - [https://glm.g-truc.net/](https://glm.g-truc.net/) 
  - Not more necessary.\
   Now is used my [**vgMath**](https://github.com/BrutPitt/vgMath) single file header, used also in [**vitualGizmo3D**](https://github.com/BrutPitt/virtualGizmo3D) and [**imGuIZMO.quat**](https://github.com/BrutPitt/imGuIZMO.quat) tools: it's a sub-set of **glm**, more compact, with or w/o template classes (selectable via define: `vgConfig.h`), and a studied **interface** that permits to switch between **glm** and **vgMath** only via compiler defines (`vgConfig.h`).

- **Boost Library** in particular: *function_output_iterator* and *geometry* are necessary **as alternative** (to nanoflann) to build DLA3D (Diffusion Limited Aggregation) object exploration in glChAoS.P (can be disabled). It's not included in the repository, but can be downloaded from [https://www.boost.org/](https://www.boost.org/)
It is not necessary to build the library, only headers files is enough. (more information in: how to build)
  - Compile with `-DGLAPP_USE_BOOST_LIBRARY` flag, to use instead of **nanoflann** lib

<p>&nbsp;<br></p>

## License
[**glChAoS.P** / **wglChAoS.P**](https://michelemorrone.eu/glchaosp) are licensed under the BSD 2-Clause License, see *license.txt* for more information.
