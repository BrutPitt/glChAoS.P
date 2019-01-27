////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  https://BrutPitt.com
//
//  This software is distributed under the terms of the BSD 2-Clause license:
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//   
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
////////////////////////////////////////////////////////////////////////////////
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
    "* Diff: Diffuse Intensity\n"\
    "* Ambt: Ambient Intensity\n"\
    "* Spec: Specular Intensity\n"\
    "* sExp: Shininess Exponent\n"\
    "* ccCorr: curve color Correction\n"\
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
    "       Ctrl + Click or Double Click: enter to edit value\n"

#define GLAPP_HELP_MAX_PART\
    "After resizing particles buffer, need to restart the program for validate new buffer dimension"
#define GLAPP_HELP_PART_SZ_CONST\
    "Particles size behavior changing window size:\n"\
    "     Checked - Particles have constant size when resizing window\n"\
    "   unChecked - Particles are proportional to window height\n"
#define GLAPP_HELP_FORCE_PNTSPRT_SZ\
    "Force minimum PointSprite size to 1.0\n"\
    "Currently NVidia and Intel supports PoininSprite with values >= 1.0, below this size the point disappear\n"\
    "Look at Info dialog for PointSprites range size and granularity supported from your GPU\n"

#define GLAPP_HELP_FONTS\
     "In settings fase the fonts can appear slightly 'blurred', after you have selected and saved a preferred"\
     " dimension, restart the program for a clear font appearance.\n"\
     "Prefer always \"FontSize\" that is the real dimension of the characters.\n"\
     "\"FontScale\" is an antialiased zoom of \"FontSize\"."


#define GLAPP_HELP_ABOUT\
    "   glChAoS.P - glChAoSP / wglChAoS.P - wglChAoSP\n"\
    "openGL/WebGL CHaotic Attractor Of Slight (dot) Particles\n"\
    "                   Version: 1.1\n\n"\
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

