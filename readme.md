# [**glChAoS.P**](https://michelemorrone.eu/glchaosp)

[**glChAoS.P**](https://michelemorrone.eu/glchaosp) / [**glChAoSP**](https://michelemorrone.eu/glchaosp): Open**gl** **Ch**aotic **A**ttractors **o**f **S**light (**dot**) **P**articles
A real time 3D strange attractor scout.

| Windows | Windows Viewports |
| ----- | ----|
|[![](https://raw.githubusercontent.com/BrutPitt/glChAoS.P/master/imgsCapture/osSS01.jpg)](https://michelemorrone.eu/glchaosp/screenshots.html#osSShots) | [![](https://raw.githubusercontent.com/BrutPitt/glChAoS.P/master/imgsCapture/ss001.jpg)](https://michelemorrone.eu/glchaosp/screenshots.html#osSShots)|

| Linux | Mac OS X |
| --- | --- |
| [![](https://raw.githubusercontent.com/BrutPitt/glChAoS.P/master/imgsCapture/osSS09.jpg)](https://michelemorrone.eu/glchaosp/screenshots.html#osSShots) | [![](https://raw.githubusercontent.com/BrutPitt/glChAoS.P/master/imgsCapture/osSS04.jpg)](https://michelemorrone.eu/glchaosp/screenshots.html#osSShots)|

Yes, another attractor visualizer, but [**glChAoS.P**](https://michelemorrone.eu/glchaosp/screenshots.html#videoSS) allows you to explore interactively any attractor type, modifyng linearly the values and displaying the changes obtained, immediately, in 3D and in realtime.



With several options: 
- 100 Milion of particles in minus of 1.6 GByte of VRAM (**S**light (**dot**) **P**articles)
- Pointsprite and billboard particles type
- 3D blended/solid/lighted particles
- Distance and alpha attenuation
- Full customizable colors, with several color palettes
- Customizable glow effects: gaussian/threshold/gaussian+threshold
- Customizable FXAA filter
- Motion blur
- Post processing image correction:
    - gamma
    - exposure
    - brightness 
    - contrast 
    - toneMapping

... and more

For more usage info: [**glChAoS.P info**](https://michelemorrone.eu/glchaosp/info.html)

### Hardware required:

- GPU with OpenGL 4.1 or higher

Theoretically all graphics card that supports OpenGL 4.0 supports also OpenGL 4.5, depends only from drivers (and O.S.)

About the GPUs that support the OpenGL 4.5 (with appropriate drivers and OS permitting):
- NVidia starting from GT/GTX 4xx series
- AMD starting from HD 5xxx series
- Intel starting from Ivy Bridge/Bay Trail CPUs (with HD 4000/2500 graphics)
  


### Hardware recommended:

- GPU starting from AMD HD 7970 / NVidia GTX 670 or with better performance
- CPU with 2 or more cores

### About performance
Glow effects, mostly with sigma \> 5, requires expensive calculations in tems of performance

If you have slow performance try, in this order:
- Disable *GlowEffects* and/or *FXAA* 
- Prefer *Pointsprite* on AMD and Intel GPU (sensible difference of performance) 
- Decrease number of particles buffer \< 3000
- Decrease point size (if you can)
- try squared 1024x1024 window size (from *Settings panel*)


### Supported Operating systems:

[x] Microsoft Windows
[x] Linux/Unix
[x] Mac OS X

**Tested Operating System Versions:**

- Microsoft Windows 7/8.x/10
- Linux distributions: Ubuntu 16.04/18.04, Fedora 27/28
- Mac OS X 10.14 (Mojave)

### Executables

No installation program is provided: just clone it form github or download the archive and decompress it in a folder whatever: only the internal directories structure must be preserved.

For Windows and Linux glChAoSP uses OpenGL 4.5 with AZDO (Approaching Zero Driver Overhead) and a separate thread (multithread) emitter/generator of particles with memory mapped directly on GPU vRAM.

On Mac OS X, for a limitation of the OS (from Mojave Apple have deprecated OpenGL) there is a downgraded version that use OpenGL 4.1 (higher possible) with a separate thread emitter/generator that use the conventional CPU memory.

Are provided executable for the followings OS:

- **Windows**\
`Particles.exe` and `Particles32.exe`: native executable (64 and 32 bit) are provided for OpenGL \>= 4.5: *Preferably use the 64-bit version*.
(you can recompile it, with appropriate define, to obtain an OpenGL 4.1 compliant)
\
`ParticlesViewports.exe`: beta version of multiple viewports is also provided (floating GUI windows that can go out outside of main Viewport/Window): (only 64 bits).
This is a great version, solid beta and full functional, but based on a *under development* version of  [**ImGui**](https://github.com/ocornut/imgui) library.
\
*Tested on Windows 7, 8 and 10*

- **Linux**\
`ParticlesLinux`: native executable (64 bit) is provided for OpenGL \>= 4.5, 
(you can recompile it, with appropriate define, to obtain an OpenGL 4.1 compliant)
\
After clone, copy or decompression, take care that executable bit of the file `ParticlesLinux` is active (chmod +x).
\
Take care to have installed OpenGL library, whereas `libX11 libXext` should already be installed.
\
It was tested on Fedora 27/28 and Ubuntu 16.04/18.04 LTS, although with subsequent updates and different library versions you may need to rebuild it.
Read Build/CMake sections for further information.
\
*Tested on Fedora 27/28 and Ubuntu 16.04/18.04*

    - **wine**    
    The Windows executable, 32/64 bit, works fine also in wine 3.xx with no evident loss of performance

- **OS X**\
`ParticlesOSX`: native executable (64 bit) is provided for OpenGL 4.1
\
From *“Finder”* click on applescript: `ParticlesOSX.app`, or form command line type directly the command: `./ParticleOSX`
\
This was tested on OS X ver 10.14 (Mojave) only, although with subsequent updates and different library versions you may need to rebuild it.
Read Build/CMake sections for further information.
\
*Tested on 10.14 Mojave*

## To build [**glChAoS.P**](https://michelemorrone.eu/glchaosp)

### Build requirements

- Compilers with full C++14 standard required
- CMake 3.10 or higher


**Tested Compilers**

- Microsoft Visual Studio 2017/2015 (Platform Toolset v.141/140: it does not work with previous versions)
- CLang 5/6/7
- GNU C++ 5/6/7/8

For Windows and Linux glChAoSP uses OpenGL 4.5 with AZDO (Approaching Zero Driver Overhead) and a separate thread (multithread) emitter/generator of particles with memory mapped directly on GPU vRAM.

On Mac OS X, for a limitation of the OS (from Mojave Apple have deprecated OpenGL) there is a downgraded version, that use OpenGL 4.1 (higher possible) with a separate thread emitter/generator that use the conventional CPU memory.

Furthers “build” option are provided:
- Use OpenGL 4.1 also on Windows and Linux.
- Single thread version.

**CMake**

In the folder `./src` there is the `CmakeLists.txt`, use this folder as base directory.

Read below more obout your OS.

**GLFW:**

Enclosed 32/64bit built library for Windows, and 64bit for Linux and OS X 
\
When build glChAoSP, compiler looks for GLFW installed library, before, in the follows path:
- src/src/libs/glfw/buildLinux (Linux)
- src/src/libs/glfw/buildOSX (OSX)
- src/src/libs/glfw/buildWin (Windows)

So, you need to delete it, or modify the CMake file, to use personal ones version.
In this case you need to have installed GLFW Library, ver 3.3 development branch or higher, or re-build the enclosed version in `“./src/src/libs/glfw”` folder: this is the development branch for version 3.3 (cloned form the github)


**Windows**

Windows user needs of Visual Studio 2017 or, in alternative, CMake 3.10 (or higher) for other compilers toolschain (non tested, but it should work).

- **Microsoft Visual Studio**
In the folder `./src/msBuilds` there is the solution project for use with Visual Studio 2017.
\
The current VisualStudio solution refers to my environment variable RAMDISK (`R:`), and subsequent VS intrinsic variables to generate binary output:
`$(RAMDISK)\$(MSBuildProjectDirectoryNoRoot)\$(DefaultPlatformToolset)\$(Platform)\$(Configuration)\` 
Even without a RAMDISK variable, executable and binary files are outputted in base to the values of these VS variables, starting from root of current drive.
\
You can use CMake to generate a VS2015 solution: previous versions do not support C++14 standard.
\
If you want use *LLVM clang* to build glChAoSP, inside Visual Studio, you can use the LLVM plugin (after to have installed clang, in windows) and simply change the toolschain in *“Properties → General → Platform Toolset”*

**Linux**

Lunux users need to install the GCC C/C++ v.5 or higher (or clang v.5 or higher) compilers and associated tools such as *make* and *CMake* (need v3.10 or higher).
To install gcc C/C++:
* Debian, Ubuntu: `sudo apt-get install build-essential cmake cmake-qt-gui`
* Fedora, RedHat: `sudo dnf install make gcc-c++ cmake cmake-gui`


You need also to have installed OpenGL library and relative development package:
`libgl1-mesa libgl1-mesa-dev` (Ubuntu) or `mesa-libGL mesa-libGL-devel` (Fedora), and also of GLFW library 3.3 or higher.

The `buildParticles.sh` script build glChAoSP, with the pre-built GLFW library (included).

The `buildLinux.sh` script is provided as helper, it call `buildGLFW.sh` (to build/erbuild GLFW) and `buildParticles.sh` sequentially.

To build/rebuild GLFW from enclosed sources you must have installed also “development” packages: `libx11-dev libxext-dev` (Ubuntu) or `libX11-devel libXext-devel` (Fedora).

Use `buildGLFW.sh` from `./src` directory, or build GLFW directly from `./src/src/libs/glfw`

(documentation: [https://github.com/glfw/glfw](https://github.com/glfw/glfw))

**OS X**

Mac users must have installed Xcode and the Command Line Tools, CMake 3.10 or higher.

CMake use the enclosed built version of GLFW (actual development branch v.3.3)

To build glChAoSP on OS X, from `./src` folder, call `sh buildParticles.sh OSX` (recommended use).

If you need to build also GLFW the `buildOSX.sh` script is provided as helper: it call `buildGLFW.sh OSX` (to build/erbuild GLFW) and  `buildParticles.sh OSX` sequentially.

Or use `sh buildGLFW.sh OSX` from ./src directory, or build GLFW directly from `./src/src/libs/glfw`

(documentation: [https://github.com/glfw/glfw](https://github.com/glfw/glfw))

Several warnings are visualized, it is normal: I'm not a mac guru (advice and suggestions are appreciated). I tested it only on OS X 10.14 Mojave (build and binary).

**CMake**

To install CMake 3.10 or higher where is not provided (Ubuntu 16.04 LTS distribution have v3.5):

You have first to remove the installed version by typing executing:
`sudo apt purge cmake` Then go to [https://cmake.org/download/](https://cmake.org/download/) 
and download the latest version you need.
If you download a .tar.gz file you have to unpack it using a command like:
`tar -xvf cmake-3.XX.XX-Linux-x86_64.tar.gz`
Then go to the folder of cmake ( `cd cmake-3.XX.XX-Linux-x86_64`) and from there execute the following commands:
```
 sudo cp -r bin /usr/
 sudo cp -r share /usr/
 sudo cp -r doc /usr/share/
 sudo cp -r man /usr/share/ 
 ```

## 3rd party tools and color maps

[**glChAoS.P**](https://michelemorrone.eu/glchaosp) uses 3rd party software tools components, they are located in the `“./src/src/libs”` folder and built with the program.
A copy of each is included in the repository, to avoid incompatibility with future changes.

Structure and description of 3rd parts libraries/tools/palettes, and related copyrights and licenses:

**Libs and Tools**

- libs/glm → OpenGL Mathematics
[https://glm.g-truc.net](https://glm.g-truc.net/)
*(need version \>= 0.9.9)*

- libs/imGui → Dear ImGui
[https://github.com/ocornut/imgui](https://github.com/ocornut/imgui)
*(need version \>= 1.67)*

- libs/configuru → Configuru, an experimental JSON config library for C++
[https://github.com/emilk/Configuru](https://github.com/emilk/Configuru)

- libs/lodePNG → LodePNG a PNG saver
[https://github.com/lvandeve/lodepng](https://github.com/lvandeve/lodepng)

- libs/Random → Random for modern C++
[https://github.com/effolkronium/random](https://github.com/effolkronium/random)

- libs/tinyFileDialog → file dialogs ( cross-platform C C++ )
[https://github.com/native-toolkit/tinyfiledialogs](https://github.com/native-toolkit/tinyfiledialogs)

- libs/IconFontAwesome → IconFontCppHeaders / Font Icon Toolkit
[https://github.com/juliettef/IconFontCppHeaders](https://github.com/juliettef/IconFontCppHeaders)
[https://github.com/FortAwesome/Font-Awesome](https://github.com/FortAwesome/Font-Awesome)

- libs/glad → GL/GLES/EGL/GLX/WGL Loader-Generator
[https://github.com/Dav1dde/glad](https://github.com/Dav1dde/glad)

- libs/glfw → A multi-platform library for OpenGL, OpenGL ES, Vulkan,
window and input
[https://github.com/glfw/glfw](https://github.com/glfw/glfw)
*(need version \>= 3.3, recommended enclosed pre-built)*

**Color Maps/Palettes**

- colorMaps/jjg\_gradient.json
colorMaps/jjg\_step.json → J.J. Green palettes (creative commons noncommercial license)
[http://soliton.vm.bytemark.co.uk/pub/cpt-city/jjg/ccolo/index.html](http://soliton.vm.bytemark.co.uk/pub/cpt-city/jjg/ccolo/index.html)
Conversion from gpf format to json 256 variations.


## License
[**glChAoS.P**](https://michelemorrone.eu/glchaosp) is licensed under the BSD 2-Clause License, see *license.txt* for more information.
