//------------------------------------------------------------------------------
//  Copyright (c) 2018-2019 Michele Morrone
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
#define GLAPP_HELP_ABOUT\
    "   glChAoS.P - glChAoSP / wglChAoS.P - wglChAoSP\n"\
    "openGL/WebGL CHaotic Attractor Of Slight (dot) Particles\n"\
    "                   Version: 1.3\n\n"\
    "open source - multiplatform - 3D real time strange attractors scout\n"\
    "         and Hypercomplex Fractals via Stochastic IIM\n\n"\
    "Copyright (c) 2018/2019 Michele Morrone\n"\
    "All rights reserved.\n"\
    "\n"\
    "    https://michelemorrone.eu\n"\
    "    https://github.com/BrutPitt\n"\
    "\n"\
    "    me@michelemorrone.eu\n"\
    "\n"\
    "Software distributed under BSD 2-Clause license\n"\
    "\n"
#define GLAPP_HELP_AXES_COR\
    "View/Modify Center of Rotation\n\n"\
    "Show Cartesian 3D axes as reference to view/change rotation center\n"\
    "Blending/Transparence is auto-activated to prevent the object from hiding the axes\n"\
    "You can modify also axes thicknes and lenght"

#define GLAPP_HELP_GLOW_TREE\
    "Glow effect\n\n"\
    "Enable/Disable effect via checkbox: some options are enabled in base to selected glow effect\n\n"\
    "* Gaussian Blur: typical gaussian 1D 2pass filter (fastest)\n"\
    "* Bilateral Threshold: spatial 2D low noise reduction filter, with thresold regulation (very expensive)\n"\
    "* Gaussian + bilateral: Combine gaussian blur + bilateral filter with a .25 of sigma\n"\
    "     gauss <-> bilat: mix results of this combination\n\n"\
    "* sigma: gaussian standard deviation\n"\
    "* 2x/3x: compute the distribution on 2x/3x sigma\n"\
    "* rendrI: color Intensity of original particles (rendered)\n"\
    "* gaussI: color Intensity of gaussian blur component\n"\
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
    "        *  actual pt + Ceneter of Rotation\n"\
    "        *  actual pt + previous pt\n"\
    "        * (actual pt + previous pt) + Ceneter of Rotation\n"
#define GLAPP_HELP_IMPORT_PLY\
    "Import PLY file format (binary or ASCII auto detected)\n\n"\
    " - Colors\n"\
    "     * Import original PLY color vertex attribute:\n"\
    "       can't use more pallettes\n"\
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
#define GLAPP_HELP_IMP_EXP_CFG\
    "Import/Export a rendering configuration file\n\n"\
    "Import/Export rendering settings: RenderingMode, Light, Colors, pointsSize, etc.\n"\
    "It don't save (only) the attractor parameters\n"\
    "It can be associated to saved PLY object (to reload own settings) or to save preferred settings and load they over a saved attractors\n"\

#define GLAPP_HELP_COMMON_TREE\
    "Common settings\n\n"\
    "* Points/Billboard: select a particular technic of rendering: same results on both, but one can be faster than the other on a different hw\n"\
    "     settings are different, for two different rendering\n"\
    "* Both: mix both rendering results\n"\
    "* Motion blur on/off: motion effect, with regolabile persistence\n"\
    "* Emitter on/off: start/stop generation of new particles\n"\
    "* fullStop/Continue: auto-stop when buffer is full or using a circular buffer\n"\
    "* Restart/Endless: auto-restart from begin (attractor values) when buffer\n"\
    "     same functionality of \"Restart\" button in attractor window\n"\
    "     it is useful in cases where you modify parameters and the attractor is instable\n"\
    "* Buffer: num of particles to emit, in M (millions); and progressive point, in K (thousands)\n"

    
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
    "   D|d|DownArrow  : next attractor\n"\
    "   U|u|UpArrow    : previous attractor\n"\
    "   R|r|RightArrow : next attractor from QuickViewDir\n"\
    "   L|l|LeftArrow  : previous attractor from QuickViewDir\n"\
    "   SHIFT+PrtSc    : Screenshot silently (w/o GUI) \n"\
    "   CTRL+PrtSc     : Screenshot with prompt filename (w/o GUI)\n"\
    "   ALT|SUPER+PrtSc: Capture screen with GUI\n"\
    "       screenshot is available only in desktop version\n\n"\
    "GUI interface\n"\
    "   The values in many controls can be changed via mouse drag,\n"\
    "   in this case you can associate also key modifier as helper:\n"\
    "       ALT   & Drag : value increment/decrement slower\n"\
    "       Shift & Drag : value increment/decrement faster\n"\
    "       Ctrl + Click or Double Click: enter to edit value\n"\

#ifdef GLCHAOSP_USE_MARKDOWN
#define GLAPP_HELP_MOUSE_KEY_MD\
    "  Left mouse button:\n"\
    "      * Double click: toggle fullscreen/windowed (desktop version)\n"\
    "      * Drag        : rotate object\n"\
    "      * Shift & Drag: rotate on X\n"\
    "      * Ctrl  & Drag: rotate on Y\n"\
    "      * Alt   & Drag: rotate on Z\n"\
    "      * Super & Drag: rotate on Z\n"\
    "  Right mouse button:\n"\
    "    Drag vertical: Dolly camera (zoom)\n"\
    "    Ctrl  & Drag : Pan camera\n"\
    "    Shift & Drag : Pan camera\n\n"\
    "  Keys\n"\
    "    Esc  : show/hide GUI\n"\
    "    F11  : toggle fullscreen/windowed (desktop version)\n"\
    "    Space: Restart attractor (like button)\n"\
    "    I|i            : invert parameters Billboard/Pointsprite\n"\
    "                     (WGL: toggle alternative settings)\n"\
    "    D|d|DownArrow  : next attractor\n"\
    "    U|u|UpArrow    : previous attractor\n"\
    "    R|r|RightArrow : next attractor from QuickViewDir\n"\
    "    L|l|LeftArrow  : previous attractor from QuickViewDir\n"\
    "    SHIFT+PrtSc    : Screenshot silently (w/o GUI) \n"\
    "    CTRL+PrtSc     : Screenshot with prompt filename (w/o GUI)\n"\
    "    ALT|SUPER+PrtSc: Capture screen with GUI\n"\
    "        screenshot is available only in desktop version\n\n"\
    "  GUI interface\n"\
    "    The values in many controls can be changed via mouse drag,\n"\
    "    in this case you can associate also key modifier as helper:\n"\
    "        ALT   & Drag : value increment/decrement slower\n"\
    "        Shift & Drag : value increment/decrement faster\n"\
    "        Ctrl + Click or Double Click: enter to edit value\n"

#define GLAPP_HELP_FONTS_MD\
     "In settings fase the fonts can appear slightly 'blurred', after you have selected and saved a preferred"\
     " dimension, restart the program for a clear font appearance:\n"\
     "  *  Prefer always \"FontSize\" that is the real dimension of the characters\n"\
     "  *  FontScale is an antialiased zoom of FontSize"\

#define GLAPP_HELP_PRECISION_MD\
    "Internal precision of FBO, palettes, textures: NEED RESTART\n"\
    "    *    Checked - use 16bit floating point\n"\
    "    *  unChecked - use 32bit floating point (default)"
#endif

#define GLAPP_HELP_EMIT_TYPE\
    "Emitter type:\n"\
    "Select emitter features:\n"\
    "   Single thread & Aux buffer - compatibility mode\n"\
    "   Aux thread & Aux buffer    - best performance\n"\
    "   Aux thread & Mapped buffer - low mem use\n"\
    "   TransformFeedback          - partilces effects\n"\
    "Default: Aux thread & Aux buffer\n\n"\
    "Aux Buffer (when avilable):\n"\
    "   Size (in bytes) of auxiliary buffer:\n"\
    "   Bigger values avoid bottleneck between calculus/rendering\n   especially on slow GPUs\n"\
    "Default: 100K (100 * 1024 bytes)\n\n"\
    "Max particles\n"\
    "Preallocated buffer for MAX circular buffer:\n"\
    "   Increase - if you want use more\n"\
    "   Decrease - if your GPU have low memory\n"\
    "Default allocation: 30M of partilces\n\n"\
    "*NOTE: to apply changes need to press APPLY EMITTER CHANGES"
#define GLAPP_HELP_PART_SZ_CONST\
    "Particles size behavior changing window size:\n"\
    "     Checked - Particles have fixed size when resizing window\n"\
    "   unChecked - Particles are proportional to window height\n\n"\
    "Default: unChecked"

#define GLAPP_HELP_FORCE_PNTSPRT_SZ\
    "Force minimum PointSprite size to 1.0\n"\
    "Currently NVidia and Intel supports PoininSprite with values >= 1.0, below this size the point disappear\n"\
    "Look at Info dialog for PointSprites range size and granularity supported from your GPU\n"

#define GLAPP_HELP_FONTS\
     "GUI\nSelect preferred GUI style and/or adjust color/alpha\n\n"\
     "FONTS\n"\
     "In settings fase the fonts can appear slightly 'blurred', after you have selected and saved a preferred"\
     " dimension, restart the program for a clear font appearance:\n"\
     " - Prefer always \"FontSize\" that is the real dimension\n   of the characters\n"\
     " - FontScale is an antialiased zoom of FontSize"\

#define GLAPP_HELP_PRECISION\
    "Internal precision of FBO, palettes, textures\n"\
    "      Checked - use 16bit floating point\n"\
    "    unChecked - use 32bit floating point (default)"\

#define GLAPP_HELP_START_ATTRACTOR\
    "With which attractor glChAoSP starts\n"\
    "      Checked - always starts with current attractor\n"\
    "    unChecked - starts with random selection (default)\n\n"\
    " *unCheck and reCheck to select new one"\

#define GLAPP_HELP_SHADOWS_TREE\
    "Shadow settings\n\n"\
    "   Rad - Smoothing shadow radius in pix (0 = noSmooth)\n"\
    "   Ratio - expand smooth radius with subPixels\n"\
    "   AutoLightDist - auto ligt distance extimation\n"\
    "   Bias - Shadow adaptation\n"\
    "   Dark - Shadow intensity"\

#define GLAPP_HELP_AO_TREE\
    "AO settings\n\n"\
    "   Mul  - Magnitudo\n"\
    "   Diff - Differential facyor\n"\
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

