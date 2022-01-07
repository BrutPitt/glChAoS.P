//------------------------------------------------------------------------------
//  Copyright (c) 2018-2020 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://BrutPitt.com
//
//  twitter: https://twitter.com/BrutPitt - github: https://github.com/BrutPitt
//
//  mailto:brutpitt@gmail.com - mailto:me@michelemorrone.eu
//  
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#include "../appDefines.h"
#define GLAPP_HELP_ABOUT\
    "   glChAoS.P - glChAoSP / wglChAoS.P - wglChAoSP\n"\
    "openGL/WebGL CHaotic Attractor Of Slight (dot) Particles\n"\
    "                   Version: " GLCHAOSP_VERSION "\n\n"\
    "open source - multiplatform - 3D real time strange attractors scout\n"\
    "         and Hyper-complex Fractals via Stochastic IIM\n\n"\
    "Copyright (c) 2018/2019 Michele Morrone\n"\
    "All rights reserved.\n"\
    "\n"\
    "    https://michelemorrone.eu - https://BrutPitt.com \n"\
    "\n"\
    "    twitter: https://twitter.com/BrutPitt \n"\
    "    github:  https://github.com/BrutPitt \n"\
    "    brutpitt@gmail.com - me@michelemorrone.eu\n"\
    "\n"\
    "Software distributed under BSD 2-Clause license\n"\
    "\n"
#define GLAPP_HELP_AXES_COR\
    "View/Modify Center of Rotation\n\n"\
    "Show Cartesian 3D axes as reference to view/change rotation center\n"\
    "Blending/Transparency is auto-activated to prevent the object from hiding the axes\n"\
    "You can modify also axes thickness and length"

#define GLAPP_HELP_GLOW_TREE\
    "Glow effect\n\n"\
    "Enable/Disable effect via check-box: some options are enabled in base to selected glow effect\n\n"\
    "* Gaussian Blur: typical Gaussian 1D 2pass filter (fastest)\n"\
    "* Bilateral Threshold: spatial 2D low noise reduction filter, with threshold regulation (very expensive)\n"\
    "* Gaussian + bilateral: Combine Gaussian blur + bilateral filter with a .25 of sigma\n"\
    "     gauss <-> bilat: mix results of this combination\n\n"\
    "* sigma: Gaussian standard deviation\n"\
    "* 2x/3x: compute the distribution on 2x/3x sigma\n"\
    "* rendrI: color Intensity of original particles (rendered)\n"\
    "* gaussI: color Intensity of Gaussian blur component\n"\
    "* bilatI: color Intensity of bilateral component\n"\
    "* render <-> glow: mix results from original rendering and the selected glow effect (recommended for gauss)\n"
#define GLAPP_HELP_EXPORT_PLY\
    "Export PLY file format\n\n"\
    " - Binary fileType\n"\
    "     Select Binary or ASCII file type\n"\
    " - Colors\n"\
    "     Add Color attribute per vertex\n"\
    " - Use CoR\n"\
    "     Use actual Center of Rotation as axes origin\n"\
    " - Normals\n"\
    "     Add Normal attribute per vertex\n"\
    " - Normalized\n"\
    "     Normal are normalized\n"\
    " - Normals type\n"\
    "     To build pointNormal need other point to define plane:\n"\
    "        *  actual pt + Center of Rotation\n"\
    "        *  actual pt + previous pt\n"\
    "        * (actual pt + previous pt) + Center of Rotation\n"
#define GLAPP_HELP_IMPORT_PLY\
    "Import PLY file format (binary or ASCII auto detected)\n\n"\
    " - Colors\n"\
    "     * Import original PLY color vertex attribute:\n"\
    "       can't use more palettes\n"\
    "     * Use false colors based on points speed or radial dist\n"\
    " - radial dist\n"\
    "     Select type of false color to use:\n"\
    "         * use point speed (distance between points) to get\n"\
    "           index palette color\n"\
    "         * use distance from origin to get index color\n"\
    " - continue DLA\n"\
    "     Continue/Start Diffusion Limited Aggregation on object:\n"\
    "         *  need more seconds to build binaryTree index\n"\
    "         *  this is a special case: after PLY loaded\n"\
    "            automatically starts DLA creation over object"
#define GLAPP_HELP_EXP_RENDR_CFG\
    "Export ALL rendering settings\n\n"\
    "Export ALL rendering settings in a configuration file\n"\
    "It save: Rendering, Light, Palette and Colors, PointSize, View/Model position, Shadow/AO, DisplayAdjust, etc...\n"\
    "It don't save (only) the attractor parameters\n"\
    "It can be associated to saved PLY object (to reload own settings) or to save preferred settings and load they over a saved attractors"
#define GLAPP_HELP_IMP_RENDR_CFG\
    "Import SELECTED rendering settings\n\n"\
    "Import only selected settings from configuration file or from an Attractor file: you can exclude what is not of interest\n"\
    "If an Attractor file is selected, it don't load (anyway) the attractor parameters (only rendering settings)"
#define GLAPP_HELP_COMMON_TREE\
    "Common settings\n\n"\
    "* Points/Billboard: select a particular technique of rendering: same results on both, but one can be faster than the other on a different hw\n"\
    "     settings are different, for two different rendering\n"\
    "* Both: mix both rendering results\n"\
    "* Motion blur on/off: motion effect, with adjustable persistence\n"\
    "* Emitter on/off: start/stop generation of new particles\n"\
    "* fullStop/Continue: auto-stop when buffer is full or using a circular buffer\n"\
    "* Restart/Endless: auto-restart from begin (attractor values) when buffer\n"\
    "     same functionality of \"Restart\" button in attractor window\n"\
    "     it is useful in cases where you modify parameters and the attractor is instable\n"\
    "* Buffer: num of particles to emit, in M (millions); and progressive point, in K (thousands)"
#define GLAPP_HELP_TRACK_IDLE\
    "Enable/Disable vGizmo idle rotation\n\n"\
    "   use \"G\" key to quickly toggle on/off"
#define GLAPP_HELP_LIGHT_TREE\
    "Light settings\n\n"\
    "All parameters have effect ONLY if/when light button (Rendering) is active\n\n"\
    "* Model: Light model for specular component\n"\
    "* Dist: Light distance from origin\n"\
    "* Diff: Diffuse Intensity\n"\
    "* Ambt: Ambient Intensity\n"\
    "* Spec: Specular Intensity\n"\
    "* sExp: Shininess Exponent (Phong/Blinn-Phong)\n"\
    "     0.0 <- Opaque - sExp - Glossy -> XXX\n"\
    "* R: Roughness value (GGX model)\n"\
    "     0.0 <- Smooth - R - Rough -> 1.0\n"\
    "* F: Fresnel factor Schlick's approximation (GGX model)\n"\
    "     FresnelTerm = F + (1-F) * (1-HdotL)^5\n"\
    "     0.0 <- Dielectric - F - Metal -> 1.0\n"\
    "* Modulate: curve color Correction\n"\
    "     form standard [0,1] interval to new [min,max]\n"

#if !defined(GLCHAOSP_LIGHTVER)
#define GLAPP_HELP_MOUSE_KEY_SCRSHT\
    "Keys - Screen capture\n"\
    "   SHIFT+PrtSc    : Screenshot silently (no GUI)\n"\
    "   CTRL+PrtSc     : Screenshot with prompt filename (no GUI)\n"\
    "     *left  SHIFT|CTRL save solid background\n"\
    "     *right SHIFT|CTRL save transparent background\n"\
    "   ALT|SUPER+PrtSc: Capture screen also with GUI\n"
#else
#define GLAPP_HELP_MOUSE_KEY_SCRSHT
#endif

#define GLAPP_HELP_MOUSE_KEY\
    "Mouse\n"\
    "   Left button:\n"\
    "      Double click: toggle fullscreen/windowed (desktop version)\n"\
    "      Drag        : rotate object\n"\
    "      Shift & Drag: rotate on X\n"\
    "      Ctrl  & Drag: rotate on Y\n"\
    "      Alt   & Drag: rotate on Z\n"\
    "      Super & Drag: rotate on Z\n"\
    "   Right button:\n"\
    "      Drag vertical: Dolly camera (zoom)\n"\
    "      Ctrl  & Drag : Pan camera\n"\
    "      Shift & Drag : Pan camera\n\n"\
    "Keys\n"\
    "   Esc  : show/hide GUI\n"\
    "   F11  : toggle fullscreen/windowed (desktop version)\n"\
    "   Space: Restart attractor (like button)\n"\
    "   I|i            : invert parameters Billboard/Pointsprite\n"\
    "                    (WGL: toggle alternative settings)\n"\
    "   G|g            : toggle idle rotation ON/OFF\n"\
    "   D|d|DownArrow  : next attractor\n"\
    "   U|u|UpArrow    : previous attractor\n"\
    "   R|r|RightArrow : next attractor from QuickViewDir\n"\
    "   L|l|LeftArrow  : previous attractor from QuickViewDir\n"\
    GLAPP_HELP_MOUSE_KEY_SCRSHT\
    "Keys - SlowMotion->CockPit feature\n"\
    "   Home - Enable PiP in TopLeft corner\n"\
    "   PgUp - Enable PiP in TopRight corner\n"\
    "   End  - Enable PiP in Bottom corner\n"\
    "   PgDn - Enable PiP in BottomRight corner\n"\
    "   Ins|Del  - Disable PIP\n"\
    "   P    - Invert PIP view\n\n"\
    "GUI interface\n"\
    "   The values in many controls can be changed via mouse drag,\n"\
    "   in this case you can associate also key modifier as helper:\n"\
    "       ALT   & Drag : value increment/decrement slower\n"\
    "       Shift & Drag : value increment/decrement faster\n"\
    "       Ctrl + Click or Double Click: enter to edit value\n"\

#define GLAPP_HELP_EMIT_TYPE\
    "Emitter type:\n"\
    "Select emitter features:\n"\
    "   Single thread & Aux buffer - compatibility mode\n"\
    "   Aux thread & Aux buffer    - best performance\n"\
    "   Aux thread & Mapped buffer - low mem use\n"\
    "   TransformFeedback          - particles effects\n"\
    "Default: Aux thread & Aux buffer\n\n"\
    "Aux Buffer (when available):\n"\
    "   Size (in bytes) of auxiliary buffer:\n"\
    "   Bigger values avoid bottleneck between calculus/rendering\n   especially on slow GPUs\n"\
    "Default: 100K (100 * 1024 bytes)\n\n"\
    "Max particles\n"\
    "Preallocated buffer for MAX circular buffer:\n"\
    "   Increase - if you want use more\n"\
    "   Decrease - if your GPU have low memory\n"\
    "Default allocation: 30M of particles\n\n"\
    "*NOTE: to apply changes need to press APPLY EMITTER CHANGES"
#define GLAPP_HELP_SYNCOGL\
    "Toggle OpenGL Synchronous mode \n"\
    "   ON  - Wait until all particles are rendered before continue\n"\
    "   OFF - Immediately returns control to the CPU\n\n"\
    "Default: OFF\n"\
    "   Enable in case of stuttering, or on slow GPU"
#define GLAPP_HELP_PART_SZ_CONST\
    "Particles size behavior changing window size:\n"\
    "     Checked - Particles have fixed size when resizing window\n"\
    "   unChecked - Particles are proportional to window height\n\n"\
    "Default: unChecked"
#define GLAPP_HELP_EMIT_SM\
    "Emitter settings:\n"\
    "   Emit # - Number of dots for single step\n"\
    "   EmitVel - Starting max random vel. for dot\n"\
    "   airFriction - Air resistance\n\n"\
    "   pointSize - pointSize used only in TF full view\n"\
    "   lifeTime - time of particle with full bright\n"\
    "   lifeTimeAtten - Attenuation factor to hide particle after\n"\
    "                   lifeTime\n"\
    "   coreMag - core Magnitude: increases core size\n"\
    "   coreInt - core Intensity: increases core whiteness\n"\
    "   coreAtten - core intensity attenuation, in sec\n\n"\
    "   wind - wind speed (x,y,z) and start time (w)\n"\
    "   gravity - gravity speed (x,y,z) and start time (w)"
#define GLAPP_HELP_COCKPIT_PARTICLES\
    "Cockpit Particles settings:\n"\
    "   pointSize - pointSize used only in TF cockpit view\n"\
    "   lifeTime - time of particle with full bright\n"\
    "   lifeTimeAtten - Attenuation factor to hide particle after\n"\
    "                   lifeTime\n"\
    "   coreMag - core Magnitude: increases core size\n"\
    "   coreInt - core Intensity: increases core whiteness\n"\
    "   coreAtten - core intensity attenuation, in sec\n\n"\
    "   smoothDistance - smoothing distance for near particles\n"\
    "   clipDistance - hihe nearest particles"
#define GLAPP_HELP_COCKPIT_PIP\
    "Cockpit Picture in Picture:\n"\
    "   Corner - Activate PiP in that corner\n"\
    "   Center - DeSelect PiP\n"\
    "   Invert PiP - switch Cockpit/staticView in PiP\n"\
    "   size PiP - trim size from 1/32 to up to 1/4 of screen\n\n"\
    "Keys:\n"\
    "   Home - Enable PiP in TopLeft corner\n"\
    "   PgUp - Enable PiP in TopRight corner\n"\
    "   End  - Enable PiP in Bottom corner\n"\
    "   PgDn - Enable PiP in BottomRight corner\n"\
    "   Ins/Del  - Disable PIP\n"\
    "   P    - Invert PIP view"
#define GLAPP_HELP_COCKPIT_VIEW\
    "Cockpit view settings:\n\n"\
    "-The target view is placed on emitter head\n"\
    "-The POV view is placed on emitter tail\n\n"\
    "   fov angle - focal angle\n"\
    "   tail POV - move POV far from head follow attractor wake\n"\
    "   move POV - move POV long POV-TGT vector\n"\
    "   fixedDist - fixed distance from nozzle\n"\
    "            or fixed time (distance change with speed)\n"\
    "   Invert view - POV <-> TGT inversion\n"\
    "   GIZMO trackball - rotate POV around TGT\n"\
    "   Reset - Reset GIZMO trackball\n\n"\
    "Keys:\n"\
    "   V or C - Enable/Disable CockPit View"
#define GLAPP_HELP_FORCE_PNTSPRT_SZ\
    "Force minimum PointSprite size to 1.0\n"\
    "Currently NVidia and Intel supports PoininSprite with values >= 1.0, below this size the point disappear\n"\
    "Look at Info dialog for PointSprites range size and granularity supported from your GPU\n"

#define GLAPP_HELP_FONTS\
     "GUI\nSelect preferred GUI style and/or adjust color/alpha\n\n"\
     "FONTS\n"\
     "In settings phase the fonts can appear slightly 'blurred', after you have selected and saved a preferred"\
     " dimension, restart the program for a clear font appearance:\n"\
     " - Prefer always \"FontSize\" that is the real dimension\n   of the characters\n"\
     " - FontScale is an antialiased zoom of FontSize"\

#define GLAPP_HELP_PRECISION\
    "Internal precision of FBO, palettes, textures\n"\
    "      Checked - use 16bit floating point\n"\
    "    unChecked - use 32bit floating point (default)\n\n"\
    "  *is necessary to save to apply the changes (autoRestart)"\

#define GLAPP_HELP_START_ATTRACTOR\
    "With which attractor glChAoSP starts\n"\
    "      Checked - always starts with current attractor\n"\
    "    unChecked - starts with random selection (default)\n\n"\
    "  *is necessary to save to apply the changes (autoRestart)"\

#define GLAPP_HELP_DETAILED_SHADOW\
    "More detailed shadows\n"\
    "      Checked - Wider texture for shadows (w*2 x h*2)\n"\
    "    unChecked - Standard window texture (w x h => default)"\

#define GLAPP_HELP_SLOW_GPU\
    "Start any attractor in PointSprite mode and with GLOW OFF\n\n"\
    "After you can select whatever you want: BillBoard/GLOW\n"\
    "(pointSprite are faster and GLOW is critical mostly in macOS)"\

#define GLAPP_HELP_SHADOWS_TREE\
    "Shadow settings\n\n"\
    "   Rad - Smoothing shadow radius in pix (0 = noSmooth)\n"\
    "   Ratio - expand smooth radius with subPixels\n"\
    "      real RADIUS = Rad/Ratio\n"\
    "      computed RADIUS pixels = Rad*Ratio\n"\
    "   AutoLightDist - auto light distance estimation\n"\
    "   Bias - Shadow adaptation\n"\
    "   Dark - Shadow intensity"\

#define GLAPP_HELP_AO_TREE\
    "AO settings\n\n"\
    "   Mul  - Magnitudo\n"\
    "   Diff - Differential factor\n"\
    "   More - Influence in DIFFUSE Light/Color component\n"\
    "   Rad  - Radius calculation\n"\
    "   Bias - Adaptation\n"\
    "   Dark - Intensity"\

#define GLAPP_HELP_CAPTURE\
    "Select path where to save the screenShots\n"\
    "   Default: ./imgsCapture\n\n"\
    "ScreenShots keys:\n"\
    "   SHIFT+PrtSc    : Screenshot silently (w/o GUI) \n"\
    "   CTRL+PrtSc     : Screenshot with prompt filename (w/o GUI)\n"\
    "   ALT|SUPER+PrtSc: Capture screen with GUI\n"\

#define GLAPP_HELP_LOWRES1 \
    "\n"\
    "     Detected Chromium-based browser using ANGLE in Windows OS\n"\
    "         wglChAoS.P disables Full Rendering Mode features\n"

/*
#define GLAPP_HELP_LOWRES2\
    "Firefox:\n"\
    "    1) Using/typing \"about:config\" in the url bar\n"\
    "    2) Search for \"angle\" and set \"webgl.disable-angle\" -> true\n\n"\
    "Chrome and Chromium based browsers (Opera/Edge/Vivaldi/etc.):\n"\
    "    1) Using/typing \"chrome://flags\" in the url bar\n"\
    "    2) Search for \"angle\" and chose \"OpenGL\" from combobox\n"\
 */
#define GLAPP_HELP_LOWRES2\
    "Chrome and Chromium based browsers (Opera/Edge/Vivaldi/etc.):\n"\
    "    1) Using/typing \"chrome://flags\" in the url bar\n"\
    "    2) Search for \"angle\" and chose \"OpenGL\" from combobox\n"\

#define GLAPP_HELP_LOWRES3\
    "For more information visit the site: \"glchaosp.com\" and go to in wglChAoS.P section"
