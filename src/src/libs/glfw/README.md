# GLFW

[![Build status](https://github.com/glfw/glfw/actions/workflows/build.yml/badge.svg)](https://github.com/glfw/glfw/actions)
[![Build status](https://ci.appveyor.com/api/projects/status/0kf0ct9831i5l6sp/branch/master?svg=true)](https://ci.appveyor.com/project/elmindreda/glfw)
[![Coverity Scan](https://scan.coverity.com/projects/4884/badge.svg)](https://scan.coverity.com/projects/glfw-glfw)

## Introduction

GLFW is an Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan
application development.  It provides a simple, platform-independent API for
creating windows, contexts and surfaces, reading input, handling events, etc.

GLFW natively supports Windows, macOS and Linux and other Unix-like systems.  On
Linux both X11 and Wayland are supported.

GLFW is licensed under the [zlib/libpng
license](https://www.glfw.org/license.html).

You can [download](https://www.glfw.org/download.html) the latest stable release
as source or Windows binaries, or fetch the `latest` branch from GitHub.  Each
release starting with 3.0 also has a corresponding [annotated
tag](https://github.com/glfw/glfw/releases) with source and binary archives.

The [documentation](https://www.glfw.org/docs/latest/) is available online and is
included in all source and binary archives.  See the [release
notes](https://www.glfw.org/docs/latest/news.html) for new features, caveats and
deprecations in the latest release.  For more details see the [version
history](https://www.glfw.org/changelog.html).

The `master` branch is the stable integration branch and _should_ always compile
and run on all supported platforms, although details of newly added features may
change until they have been included in a release.  New features and many bug
fixes live in [other branches](https://github.com/glfw/glfw/branches/all) until
they are stable enough to merge.

If you are new to GLFW, you may find the
[tutorial](https://www.glfw.org/docs/latest/quick.html) for GLFW 3 useful.  If
you have used GLFW 2 in the past, there is a [transition
guide](https://www.glfw.org/docs/latest/moving.html) for moving to the GLFW
3 API.


## Compiling GLFW

GLFW itself requires only the headers and libraries for your OS and window
system.  It does not need the headers for any context creation API (WGL, GLX,
EGL, NSGL, OSMesa) or rendering API (OpenGL, OpenGL ES, Vulkan) to enable
support for them.

GLFW supports compilation on Windows with Visual C++ 2010 and later, MinGW and
MinGW-w64, on macOS with Clang and on Linux and other Unix-like systems with GCC
and Clang.  It will likely compile in other environments as well, but this is
not regularly tested.

There are [pre-compiled Windows binaries](https://www.glfw.org/download.html)
available for all supported compilers.

See the [compilation guide](https://www.glfw.org/docs/latest/compile.html) for
more information about how to compile GLFW yourself.


## Using GLFW

See the [documentation](https://www.glfw.org/docs/latest/) for tutorials, guides
and the API reference.


## Contributing to GLFW

See the [contribution
guide](https://github.com/glfw/glfw/blob/master/docs/CONTRIBUTING.md) for
more information.


## System requirements

GLFW supports Windows XP and later and macOS 10.8 and later.  Linux and other
Unix-like systems running the X Window System are supported even without
a desktop environment or modern extensions, although some features require
a running window or clipboard manager.  The OSMesa backend requires Mesa 6.3.

See the [compatibility guide](https://www.glfw.org/docs/latest/compat.html)
in the documentation for more information.


## Dependencies

GLFW itself needs only CMake 3.1 or later and the headers and libraries for your
OS and window system.

The examples and test programs depend on a number of tiny libraries.  These are
located in the `deps/` directory.

 - [getopt\_port](https://github.com/kimgr/getopt_port/) for examples
   with command-line options
 - [TinyCThread](https://github.com/tinycthread/tinycthread) for threaded
   examples
 - [glad2](https://github.com/Dav1dde/glad) for loading OpenGL and Vulkan
   functions
 - [linmath.h](https://github.com/datenwolf/linmath.h) for linear algebra in
   examples
 - [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear) for test and example UI
 - [stb\_image\_write](https://github.com/nothings/stb) for writing images to disk

The documentation is generated with [Doxygen](https://doxygen.org/) if CMake can
find that tool.


## Reporting bugs

Bugs are reported to our [issue tracker](https://github.com/glfw/glfw/issues).
Please check the [contribution
guide](https://github.com/glfw/glfw/blob/master/docs/CONTRIBUTING.md) for
information on what to include when reporting a bug.


## Changelog

 - Added `glfwInitAllocator` for setting a custom memory allocator (#544,#1628,#1947)
 - Added `GLFWallocator` struct and `GLFWallocatefun`, `GLFWreallocatefun` and
   `GLFWdeallocatefun` types (#544,#1628,#1947)
 - Added `GLFW_RESIZE_NWSE_CURSOR`, `GLFW_RESIZE_NESW_CURSOR`,
   `GLFW_RESIZE_ALL_CURSOR` and `GLFW_NOT_ALLOWED_CURSOR` cursor shapes (#427)
 - Added `GLFW_RESIZE_EW_CURSOR` alias for `GLFW_HRESIZE_CURSOR` (#427)
 - Added `GLFW_RESIZE_NS_CURSOR` alias for `GLFW_VRESIZE_CURSOR` (#427)
 - Added `GLFW_POINTING_HAND_CURSOR` alias for `GLFW_HAND_CURSOR` (#427)
 - Added `GLFW_MOUSE_PASSTHROUGH` window hint for letting mouse input pass
   through the window (#1236,#1568)
 - Added `GLFW_FEATURE_UNAVAILABLE` error for platform limitations (#1692)
 - Added `GLFW_FEATURE_UNIMPLEMENTED` error for incomplete backends (#1692)
 - Added `GLFW_ANGLE_PLATFORM_TYPE` init hint and `GLFW_ANGLE_PLATFORM_TYPE_*`
   values to select ANGLE backend (#1380)
 - Added `GLFW_X11_XCB_VULKAN_SURFACE` init hint for selecting X11 Vulkan
   surface extension (#1793)
 - Added `GLFW_LIBRARY_TYPE` CMake variable for overriding the library type
   (#279,#1307,#1497,#1574,#1928)
 - Added `GLFW_PKG_CONFIG_REQUIRES_PRIVATE` and `GLFW_PKG_CONFIG_LIBS_PRIVATE` CMake
   variables exposing pkg-config dependencies (#1307)
 - Made joystick subsystem initialize at first use (#1284,#1646)
 - Made `GLFW_DOUBLEBUFFER` a read-only window attribute
 - Updated the minimum required CMake version to 3.1
 - Disabled tests and examples by default when built as a CMake subdirectory
 - Bugfix: The CMake config-file package used an absolute path and was not
   relocatable (#1470)
 - Bugfix: Video modes with a duplicate screen area were discarded (#1555,#1556)
 - Bugfix: Compiling with -Wextra-semi caused warnings (#1440)
 - Bugfix: Built-in mappings failed because some OEMs re-used VID/PID (#1583)
 - Bugfix: Some extension loader headers did not prevent default OpenGL header
   inclusion (#1695)
 - Bugfix: Buffers were swapped at creation on single-buffered windows (#1873)
 - Bugfix: Gamepad mapping updates could spam `GLFW_INVALID_VALUE` due to
   incompatible controllers sharing hardware ID (#1763)
 - [Win32] Added the `GLFW_WIN32_KEYBOARD_MENU` window hint for enabling access
           to the window menu
 - [Win32] Added a version info resource to the GLFW DLL
 - [Win32] Disabled framebuffer transparency on Windows 7 when DWM windows are
   opaque (#1512)
 - [Win32] Bugfix: `GLFW_INCLUDE_VULKAN` plus `VK_USE_PLATFORM_WIN32_KHR` caused
   symbol redefinition (#1524)
 - [Win32] Bugfix: The cursor position event was emitted before its cursor enter
   event (#1490)
 - [Win32] Bugfix: The window hint `GLFW_MAXIMIZED` did not move or resize the
   window (#1499)
 - [Win32] Bugfix: Disabled cursor mode interfered with some non-client actions
 - [Win32] Bugfix: Super key was not released after Win+V hotkey (#1622)
 - [Win32] Bugfix: `glfwGetKeyName` could access out of bounds and return an
   invalid pointer
 - [Win32] Bugfix: Some synthetic key events were reported as `GLFW_KEY_UNKNOWN`
   (#1623)
 - [Win32] Bugfix: Non-BMP Unicode codepoint input was reported as UTF-16
 - [Win32] Bugfix: Monitor functions could return invalid values after
   configuration change (#1761)
 - [Win32] Bugfix: Initialization would segfault on Windows 8 (not 8.1) (#1775)
 - [Win32] Bugfix: Duplicate size events were not filtered (#1610)
 - [Win32] Bugfix: Full screen windows were incorrectly resized by DPI changes
   (#1582)
 - [Win32] Bugfix: `GLFW_SCALE_TO_MONITOR` had no effect on systems older than
   Windows 10 version 1703 (#1511)
 - [Win32] Bugfix: `USE_MSVC_RUNTIME_LIBRARY_DLL` had no effect on CMake 3.15 or
   later (#1783,#1796)
 - [Win32] Bugfix: Compilation with LLVM for Windows failed (#1807,#1824,#1874)
 - [Win32] Bugfix: The foreground lock timeout was overridden, ignoring the user
 - [Cocoa] Added support for `VK_EXT_metal_surface` (#1619)
 - [Cocoa] Added locating the Vulkan loader at runtime in an application bundle
 - [Cocoa] Moved main menu creation to GLFW initialization time (#1649)
 - [Cocoa] Changed `EGLNativeWindowType` from `NSView` to `CALayer` (#1169)
 - [Cocoa] Changed F13 key to report Print Screen for cross-platform consistency
   (#1786)
 - [Cocoa] Removed dependency on the CoreVideo framework
 - [Cocoa] Bugfix: `glfwSetWindowSize` used a bottom-left anchor point (#1553)
 - [Cocoa] Bugfix: Window remained on screen after destruction until event poll
   (#1412)
 - [Cocoa] Bugfix: Event processing before window creation would assert (#1543)
 - [Cocoa] Bugfix: Undecorated windows could not be iconified on recent macOS
 - [Cocoa] Bugfix: Touching event queue from secondary thread before main thread
   would abort (#1649)
 - [Cocoa] Bugfix: Non-BMP Unicode codepoint input was reported as UTF-16
   (#1635)
 - [Cocoa] Bugfix: Failing to retrieve the refresh rate of built-in displays
   could leak memory
 - [Cocoa] Bugfix: Objective-C files were compiled as C with CMake 3.19 (#1787)
 - [Cocoa] Bugfix: Duplicate video modes were not filtered out (#1830)
 - [Cocoa] Bugfix: Menubar was not clickable on macOS 10.15+ until it lost and
   regained focus (#1648,#1802)
 - [Cocoa] Bugfix: Monitor name query could segfault on macOS 11 (#1809,#1833)
 - [Cocoa] Bugfix: The install name of the installed dylib was relative (#1504)
 - [Cocoa] Bugfix: The MoltenVK layer contents scale was updated only after
   related events were emitted
 - [Cocoa] Bugfix: Moving the cursor programmatically would freeze it for
   a fraction of a second (#1962)
 - [X11] Bugfix: The CMake files did not check for the XInput headers (#1480)
 - [X11] Bugfix: Key names were not updated when the keyboard layout changed
   (#1462,#1528)
 - [X11] Bugfix: Decorations could not be enabled after window creation (#1566)
 - [X11] Bugfix: Content scale fallback value could be inconsistent (#1578)
 - [X11] Bugfix: `glfwMaximizeWindow` had no effect on hidden windows
 - [X11] Bugfix: Clearing `GLFW_FLOATING` on a hidden window caused invalid read
 - [X11] Bugfix: Changing `GLFW_FLOATING` on a hidden window could silently fail
 - [X11] Bugfix: Disabled cursor mode was interrupted by indicator windows
 - [X11] Bugfix: Monitor physical dimensions could be reported as zero mm
 - [X11] Bugfix: Window position events were not emitted during resizing (#1613)
 - [X11] Bugfix: `glfwFocusWindow` could terminate on older WMs or without a WM
 - [X11] Bugfix: Querying a disconnected monitor could segfault (#1602)
 - [X11] Bugfix: IME input of CJK was broken for "C" locale (#1587,#1636)
 - [X11] Bugfix: Termination would segfault if the IM had been destroyed
 - [X11] Bugfix: Any IM started after initialization would not be detected
 - [X11] Bugfix: Xlib errors caused by other parts of the application could be
   reported as GLFW errors
 - [X11] Bugfix: A handle race condition could cause a `BadWindow` error (#1633)
 - [X11] Bugfix: XKB path used keysyms instead of physical locations for
   non-printable keys (#1598)
 - [X11] Bugfix: Function keys were mapped to `GLFW_KEY_UNKNOWN` for some layout
   combinaitons (#1598)
 - [X11] Bugfix: Keys pressed simultaneously with others were not always
   reported (#1112,#1415,#1472,#1616)
 - [X11] Bugfix: Some window attributes were not applied on leaving fullscreen
   (#1863)
 - [X11] Bugfix: Changing `GLFW_FLOATING` could leak memory
 - [Wayland] Added dynamic loading of all Wayland libraries
 - [Wayland] Removed support for `wl_shell` (#1443)
 - [Wayland] Bugfix: The `GLFW_HAND_CURSOR` shape used the wrong image (#1432)
 - [Wayland] Bugfix: `CLOCK_MONOTONIC` was not correctly enabled
 - [Wayland] Bugfix: Repeated keys could be reported with `NULL` window (#1704)
 - [Wayland] Bugfix: Retrieving partial framebuffer size would segfault
 - [Wayland] Bugfix: Scrolling offsets were inverted compared to other platforms
   (#1463)
 - [Wayland] Bugfix: Client-Side Decorations were destroyed in the wrong worder
   (#1798)
 - [Wayland] Bugfix: Monitors physical size could report zero (#1784,#1792)
 - [Wayland] Bugfix: Some keys were not repeating in Wayland (#1908)
 - [Wayland] Bugfix: Non-arrow cursors are offset from the hotspot (#1706,#1899)
 - [POSIX] Removed use of deprecated function `gettimeofday`
 - [POSIX] Bugfix: `CLOCK_MONOTONIC` was not correctly tested for or enabled
 - [NSGL] Removed enforcement of forward-compatible flag for core contexts
 - [NSGL] Bugfix: `GLFW_COCOA_RETINA_FRAMEBUFFER` had no effect on newer
   macOS versions (#1442)
 - [NSGL] Bugfix: Workaround for swap interval on 10.14 broke on 10.12 (#1483)
 - [NSGL] Bugfix: Defining `GL_SILENCE_DEPRECATION` externally caused
   a duplicate definition warning (#1840)
 - [EGL] Added platform selection via the `EGL_EXT_platform_base` extension
   (#442)
 - [EGL] Added ANGLE backend selection via `EGL_ANGLE_platform_angle` extension
   (#1380)
 - [EGL] Bugfix: The `GLFW_DOUBLEBUFFER` context attribute was ignored (#1843)


## Contact

On [glfw.org](https://www.glfw.org/) you can find the latest version of GLFW, as
well as news, documentation and other information about the project.

If you have questions related to the use of GLFW, we have a
[forum](https://discourse.glfw.org/), and the `#glfw` IRC channel on
[Libera.Chat](https://libera.chat/).

If you have a bug to report, a patch to submit or a feature you'd like to
request, please file it in the
[issue tracker](https://github.com/glfw/glfw/issues) on GitHub.

Finally, if you're interested in helping out with the development of GLFW or
porting it to your favorite platform, join us on the forum, GitHub or IRC.


## Acknowledgements

GLFW exists because people around the world donated their time and lent their
skills.

 - Bobyshev Alexander
 - Laurent Aphecetche
 - Matt Arsenault
 - ashishgamedev
 - David Avedissian
 - Keith Bauer
 - John Bartholomew
 - Coşku Baş
 - Niklas Behrens
 - Andrew Belt
 - Nevyn Bengtsson
 - Niklas Bergström
 - Denis Bernard
 - Doug Binks
 - blanco
 - Waris Boonyasiriwat
 - Kyle Brenneman
 - Rok Breulj
 - Kai Burjack
 - Martin Capitanio
 - Nicolas Caramelli
 - David Carlier
 - Arturo Castro
 - Chi-kwan Chan
 - Joseph Chua
 - Ian Clarkson
 - Michał Cichoń
 - Lambert Clara
 - Anna Clarke
 - Yaron Cohen-Tal
 - Omar Cornut
 - Andrew Corrigan
 - Bailey Cosier
 - Noel Cower
 - CuriouserThing
 - Jason Daly
 - Jarrod Davis
 - Olivier Delannoy
 - Paul R. Deppe
 - Michael Dickens
 - Роман Донченко
 - Mario Dorn
 - Wolfgang Draxinger
 - Jonathan Dummer
 - Ralph Eastwood
 - Fredrik Ehnbom
 - Robin Eklind
 - Siavash Eliasi
 - Ahmad Fatoum
 - Felipe Ferreira
 - Michael Fogleman
 - Gerald Franz
 - Mário Freitas
 - GeO4d
 - Marcus Geelnard
 - Charles Giessen
 - Ryan C. Gordon
 - Stephen Gowen
 - Kovid Goyal
 - Eloi Marín Gratacós
 - Stefan Gustavson
 - Jonathan Hale
 - hdf89shfdfs
 - Sylvain Hellegouarch
 - Matthew Henry
 - heromyth
 - Lucas Hinderberger
 - Paul Holden
 - Warren Hu
 - Charles Huber
 - IntellectualKitty
 - Aaron Jacobs
 - Erik S. V. Jansson
 - Toni Jovanoski
 - Arseny Kapoulkine
 - Cem Karan
 - Osman Keskin
 - Koray Kilinc
 - Josh Kilmer
 - Byunghoon Kim
 - Cameron King
 - Peter Knut
 - Christoph Kubisch
 - Yuri Kunde Schlesner
 - Rokas Kupstys
 - Konstantin Käfer
 - Eric Larson
 - Francis Lecavalier
 - Jong Won Lee
 - Robin Leffmann
 - Glenn Lewis
 - Shane Liesegang
 - Anders Lindqvist
 - Leon Linhart
 - Marco Lizza
 - Eyal Lotem
 - Aaron Loucks
 - Luflosi
 - lukect
 - Tristam MacDonald
 - Hans Mackowiak
 - Дмитри Малышев
 - Zbigniew Mandziejewicz
 - Adam Marcus
 - Célestin Marot
 - Kyle McDonald
 - David V. McKay
 - David Medlock
 - Bryce Mehring
 - Jonathan Mercier
 - Marcel Metz
 - Liam Middlebrook
 - Ave Milia
 - Jonathan Miller
 - Kenneth Miller
 - Bruce Mitchener
 - Jack Moffitt
 - Jeff Molofee
 - Alexander Monakov
 - Pierre Morel
 - Jon Morton
 - Pierre Moulon
 - Martins Mozeiko
 - Julian Møller
 - ndogxj
 - n3rdopolis
 - Kristian Nielsen
 - Kamil Nowakowski
 - onox
 - Denis Ovod
 - Ozzy
 - Andri Pálsson
 - Peoro
 - Braden Pellett
 - Christopher Pelloux
 - Arturo J. Pérez
 - Vladimir Perminov
 - Anthony Pesch
 - Orson Peters
 - Emmanuel Gil Peyrot
 - Cyril Pichard
 - Keith Pitt
 - Stanislav Podgorskiy
 - Konstantin Podsvirov
 - Nathan Poirier
 - Alexandre Pretyman
 - Pablo Prietz
 - przemekmirek
 - pthom
 - Guillaume Racicot
 - Philip Rideout
 - Eddie Ringle
 - Max Risuhin
 - Jorge Rodriguez
 - Luca Rood
 - Ed Ropple
 - Aleksey Rybalkin
 - Mikko Rytkönen
 - Riku Salminen
 - Brandon Schaefer
 - Sebastian Schuberth
 - Christian Sdunek
 - Matt Sealey
 - Steve Sexton
 - Arkady Shapkin
 - Ali Sherief
 - Yoshiki Shibukawa
 - Dmitri Shuralyov
 - Daniel Skorupski
 - Anthony Smith
 - Bradley Smith
 - Cliff Smolinsky
 - Patrick Snape
 - Erlend Sogge Heggen
 - Julian Squires
 - Johannes Stein
 - Pontus Stenetorp
 - Michael Stocker
 - Justin Stoecker
 - Elviss Strazdins
 - Paul Sultana
 - Nathan Sweet
 - TTK-Bandit
 - Jared Tiala
 - Sergey Tikhomirov
 - Arthur Tombs
 - Ioannis Tsakpinis
 - Samuli Tuomola
 - Matthew Turner
 - urraka
 - Elias Vanderstuyft
 - Stef Velzel
 - Jari Vetoniemi
 - Ricardo Vieira
 - Nicholas Vitovitch
 - Simon Voordouw
 - Corentin Wallez
 - Torsten Walluhn
 - Patrick Walton
 - Xo Wang
 - Jay Weisskopf
 - Frank Wille
 - Andy Williams
 - Joel Winarske
 - Richard A. Wilkes
 - Tatsuya Yatagawa
 - Ryogo Yoshimura
 - Lukas Zanner
 - Andrey Zholos
 - Aihui Zhu
 - Santi Zupancic
 - Jonas Ådahl
 - Lasse Öörni
 - Leonard König
 - All the unmentioned and anonymous contributors in the GLFW community, for bug
   reports, patches, feedback, testing and encouragement

