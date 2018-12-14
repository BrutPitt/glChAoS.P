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
#include "imGuIZMO.h"

ImVector<glm::vec3> imguiGizmo::sphereVtx;
ImVector<int>       imguiGizmo::sphereTess;
ImVector<glm::vec3> imguiGizmo::arrowVtx[4];
ImVector<glm::vec3> imguiGizmo::arrowNorm[4];
ImVector<glm::vec3> imguiGizmo::cubeVtx;
ImVector<glm::vec3> imguiGizmo::cubeNorm;
bool imguiGizmo::solidAreBuilded = false;
bool imguiGizmo::dragActivate = false;
//
//  Settings
//
//      axes/arrow are composed of cone (or pyramid) and cylinder 
//      (or parallelepiped): this solid are builded at first instance
//      and will have same slices/radius/length for all controls in your 
//      applications but can be resized proportionally with a reductin
//      factor: solidResizeFactor and  axesResizeFactor.
//      Same thing for the colors of sphere tessellation, while color
//      of axes and cube are fixed
//
//      Solid/axes settings can be set one only one time before your widget
//      while solidResizeFactor and  axesResizeFactor settings must 
//      be call before and always of your widget, every redraw... and
//      restored after use... like push/pop
//      ... I avoided creating a push/pop mechanism
////////////////////////////////////////////////////////////////////////////
 
// arrow/axes components
///////////////////////////////////////
int   imguiGizmo::coneSlices  = 4;
float imguiGizmo::coneRadius = 0.07f;
float imguiGizmo::coneLength = 0.37f;


int   imguiGizmo::cylSlices   = 7;
float imguiGizmo::cylRadius  = 0.02f; // sizeCylLength = defined in base to control size


// Sphere components
///////////////////////////////////////
float imguiGizmo::sphereRadius = .27f;
int imguiGizmo::sphereTessFactor = imguiGizmo::sphereTess4;

// Cube components
///////////////////////////////////////
float imguiGizmo::cubeSide     = .05f;

// Axes resize
///////////////////////////////////////
glm::vec3 imguiGizmo::axesResizeFactor(.95,1.0,1.0);
glm::vec3 imguiGizmo::savedAxesResizeFactor;

// Solid resize
///////////////////////////////////////
float imguiGizmo::solidResizeFactor= 1.0;
float imguiGizmo::savedSolidResizeFactor;

// Direction arrow color
///////////////////////////////////////
ImVec4 imguiGizmo::directionColor(1.0f, 1.0f, 0.0, 1.0f);
ImVec4 imguiGizmo::savedDirectionColor;

// Sphere Colors 
///////////////////////////////////////
ImU32 imguiGizmo::sphereColors[2]= { 0xff401010, 0xffc0a0a0 }; // Tessellation colors
ImU32 imguiGizmo::savedSphereColors[2]; 
//ImU32 spherecolorA=0xff005cc0, spherecolorB=0xffc05c00;

//
//  for all gizmo3D
//
//      input:
//          size:   dimension of the control
//          mode:   visualization mode: axis starting from origin, fullAxis 
//                  (whit or w/o solid at 0,0,0) or only one arrow for direction
//
//      other settings (to call before and always of your control):
//          dimesion solid, axes, and arrow, slice of poligons end over: view 
//          section "settings of class declaration", these these values are valid for 
//          ALL controls in your application, because the lists of triangles/quads, 
//          which compose the solids, are builded one time with the first 
//          instance ... and NOT every redraw
//
//          solidResizeFactor - axesResizeFactor 
//              can resize axes or solid, respectively (helper func)
////////////////////////////////////////////////////////////////////////////

namespace ImGui
{
//
//  Quaternion control 
//
//      input/output: glm::quat (quaternion) for full control
////////////////////////////////////////////////////////////////////////////
bool gizmo3D(const char* label, glm::quat& quat, float size, const int mode)
{
    imguiGizmo g;
    g.modeSettings(mode & ~g.modeDual);

    g.qtV = quat;

    bool ret = g.drawFunc(label, size);
    if(ret) quat = g.qtV;

    return ret;
}

//
//  Angle/Axes control 
//
//      input/output: 
//          glm::Vec4 - X Y Z vector/axes components - W angle of rotation
////////////////////////////////////////////////////////////////////////////
bool gizmo3D(const char* label, glm::vec4& axis_angle, float size, const int mode)
{
    imguiGizmo g;
    g.modeSettings(mode & ~g.modeDual);

    return g.getTransforms(g.qtV, label, axis_angle, size);
}

//
//  Direction control : 
//      only in directional mode! ... for obvious reasons ;-) 
//      
//      input/output: glm::Vec3 - X Y Z vector/axes components
////////////////////////////////////////////////////////////////////////////

bool gizmo3D(const char* label, glm::vec3& dir, float size, const int mode)
{
    imguiGizmo g;
    g.modeSettings(imguiGizmo::modeDirection); 

    return g.getTransforms(g.qtV, label, dir, size);

}

//
//  2 Manipulators -> 2 Quaternions 
//
//      input/output: glm::axes (quaternion) for full control - LeftClick 
//                    glm::spot (quaternion) for full control - RightClick
//
//                    both pressed buttons... rotate together
//                    ctrl-Shift-Alt mods, for X-Y-Z rotations (respectivally)
//                    are abilitated on both ... also together!
////////////////////////////////////////////////////////////////////////////
bool gizmo3D(const char* label, glm::quat& axes, glm::quat& spot, float size, const int mode)
{
    imguiGizmo g;
    g.setDualMode(mode);
    
    g.qtV = axes; g.qtV2 = spot;
    
    bool ret = g.drawFunc(label, size);
    if(ret) { axes = g.qtV; spot = g.qtV2; }

    return ret;
}

//
//  2 Manipulators -> Quaternions and Vec3
//
//      input/output: glm::axes (quaternion) for full control - LeftClick 
//                    glm::spot (vec3)       for full control - RightClick
//
//                    read above...
////////////////////////////////////////////////////////////////////////////
bool gizmo3D(const char* label, glm::quat& axes, glm::vec3& spotDir, float size, const int mode)
{
    imguiGizmo g;
    g.setDualMode(mode);

    g.qtV = axes;

    bool ret = g.getTransforms(g.qtV2, label, spotDir, size);
    if(ret) axes = g.qtV;
    return ret;
}

//
//  2 Manipulators -> Quaternions and Vec4
//
//      input/output: glm::axes (quaternion) for full control - LeftClick 
//                    glm::spot (vec4 -> xyz axes, q angle)   - RightClick
//
//                    read above...
////////////////////////////////////////////////////////////////////////////
bool gizmo3D(const char* label, glm::quat& axes, glm::vec4& axesAngle, float size, const int mode)
{
    imguiGizmo g;
    g.setDualMode(mode);

    g.qtV = axes;

    bool ret = g.getTransforms(g.qtV2, label, axesAngle, size);
    if(ret) axes = g.qtV;
    return ret;

}

} // namespace ImGui

static inline int clamp(int v, int mn, int mx)
{
    return (v < mn) ? mn : (v > mx) ? mx : v; 
}

//
//  LightEffect
//      faster but minus cute/precise.. ok for sphere
////////////////////////////////////////////////////////////////////////////
inline ImU32 addLightEffect(ImU32 color, float light)
{         
    float l = ((light<.6) ? .6 : light) * .8;  
    float lc = light * 80.f;                    // ambient component 
    return   clamp(ImU32((( color      & 0xff)*l + lc)),0,255)        |
            (clamp(ImU32((((color>>8)  & 0xff)*l + lc)),0,255) <<  8) | 
            (clamp(ImU32((((color>>16) & 0xff)*l + lc)),0,255) << 16) |        
            (ImU32(ImGui::GetStyle().Alpha * 0xff000000) & 0xff000000);  
}
//
//  LightEffect
//      with distance attenuatin
////////////////////////////////////////////////////////////////////////////
inline ImU32 addLightEffect(const glm::vec3 &color, float light, float atten)
{                          
    glm::vec3 l((light<.5) ? .5f : light); 
    glm::vec3 a(atten>.25  ? .25f : atten);
    glm::vec3 c(((color + l*.5f) * l) *.75f + a*color*.45f +a*.25f);

    const float alpha = ImGui::GetStyle().Alpha; //ImGui::GetCo(ImGuiCol_FrameBg).w;
    return ImGui::ColorConvertFloat4ToU32(ImVec4(c.x, c.y, c.z, alpha));
}

inline ImU32 addLightEffect(ImU32 color, float light,  float atten)
{                        
    glm::vec3 c(float(color & 0xff)/255.f,float((color>>8) & 0xff)/255.f,float((color>>16) & 0xff)/255.f);
    return addLightEffect(c, light, atten);
}

//  inline helper drawing functions
////////////////////////////////////////////////////////////////////////////
typedef glm::vec3 & (*ptrFunc)(glm::vec3 &);

inline glm::vec3 &adjustDir(glm::vec3 &coord)
{
    coord.x = (coord.x > 0) ? ( 2.5f * coord.x - 1.6f) : coord.x + 0.1f;
    coord *= glm::vec3(1.f, 3.f, 3.f);
    return coord;
}

inline glm::vec3 &adjustSpotCyl(glm::vec3 &coord)
{
    const float halfCylMinusCone = 1. - imguiGizmo::coneLength;
    coord.x = (coord.x*.075 - 2. +( halfCylMinusCone - halfCylMinusCone*.075)); //cyl begin where cone end
    return coord;

}
inline glm::vec3 &adjustSpotCone(glm::vec3 &coord)
{
    coord.x-= 2.00f;
    return coord;
}

inline glm::vec3 fastRotate (int axis, glm::vec3 &v)
{
    return ((axis == imguiGizmo::axisIsY) ? glm::vec3(-v.y, v.x, v.z) : // rotation Z 90'
           ((axis == imguiGizmo::axisIsZ) ? glm::vec3(-v.z, v.y, v.x) : // rotation Y 90'                            
                                          v));
}
////////////////////////////////////////////////////////////////////////////
//
//  Draw imguiGizmo
//      
////////////////////////////////////////////////////////////////////////////
bool imguiGizmo::drawFunc(const char* label, float size)
{

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    const float arrowStartingPoint = (axesOriginType & imguiGizmo::sphereAtOrigin) ? sphereRadius * solidResizeFactor:
                                    ((axesOriginType & imguiGizmo::cubeAtOrigin  ) ? cubeSide     * solidResizeFactor: 
                                                                                   cylRadius * .5);
    // if modeDual... leave space for draw light arrow
    glm::vec3 resizeAxes( ((drawMode&modeDual) && (axesResizeFactor.x>.75)) ? glm::vec3(.75,axesResizeFactor.y, axesResizeFactor.z) : axesResizeFactor);

    //  build solids... once!
    ///////////////////////////////////////
    if (!solidAreBuilded)  {
        const float arrowBgn = -1.0f, arrowEnd = 1.0f;     

        buildCone    (arrowEnd - coneLength, arrowEnd, coneRadius, coneSlices);
        buildCylinder(arrowBgn, arrowEnd - coneLength, cylRadius , cylSlices );
        buildSphere(sphereRadius, sphereTessFactor);
        buildCube(cubeSide);
        solidAreBuilded = true;
    }

    ImGui::PushID(label);
    ImGui::BeginGroup();

    bool value_changed = false;

    if(label[0]!='#' && label[1]!='#') ImGui::Text("%s", label);

    ImVec2 controlPos = ImGui::GetCursorScreenPos();

    const float innerSquareSize = size; //std::min(ImGui::CalcItemWidth(), size);
    const float halfSquareSize = innerSquareSize*.5;
    const ImVec2 innerSize(innerSquareSize,innerSquareSize);

    bool highlighted = false;
    ImGui::InvisibleButton("imguiGizmo", innerSize);

    ////////////////////////////////////////////////////////////////////////////
    //  trackball control (virtualGizmo.h)
    //      Only this 2 lamdas
    //      can be replaced facilly with an personal 3d manipulator which 
    //      returns rotations in a quaternion
    ////////////////////////////////////////////////////////////////////////////
    auto setTrackball = [&] (virtualGizmoClass<tbT> &track, glm::quat &q) {
        track.viewportSize(size, size);
        track.setRotation(q);
        track.setGizmoScale(size/std::max(io.DisplaySize.x,io.DisplaySize.y));
    };

    //  getTrackball
    //      in : q -> quaternion to which applay rotations
    //      out: q -> quaternion with rotations
    ////////////////////////////////////////////////////////////////////////////
    auto getTrackball = [&] (glm::quat &q) {
            virtualGizmoClass<tbT> track;
            setTrackball(track, q);    
            
            ImVec2 mouse = ImGui::GetMousePos() - controlPos;
            vgModifiers mod  = (io.KeyCtrl) ?  evControlModifier : evNoModifier;
                        if(io.KeyAlt)   mod |= evAltModifier;
                        if(io.KeyShift) mod |= evShiftModifier;
                        if(io.KeySuper) mod |= evSuperModifier;

            track.motionImmediateMode(mouse.x, mouse.y, io.MouseDelta.x, io.MouseDelta.y, mod);
            q = track.getRotation();
            value_changed = true;
    };

    //LeftClick
    if (ImGui::IsItemActive()) {
        highlighted = true;
        if(ImGui::IsMouseDragging(0))                       getTrackball(qtV);        
        if(drawMode==modeDual && ImGui::IsMouseDragging(1)) getTrackball(qtV2); // if dual mode... move together
        if(drawMode==modeDual && ImGui::IsMouseDragging(2)) { getTrackball(qtV);  getTrackball(qtV2); } // middle if dual mode... move together

        ImColor col(style.Colors[ImGuiCol_FrameBgActive]);
        col.Value.w*=ImGui::GetStyle().Alpha;
        draw_list->AddRectFilled(controlPos, controlPos + innerSize, col, style.FrameRounding);
    } else {  // eventual right click... only dualmode
        highlighted = ImGui::IsItemHovered();
        if(highlighted && (drawMode==modeDual) && ImGui::IsMouseDragging(1)) getTrackball(qtV2);

        ImColor col(highlighted ? style.Colors[ImGuiCol_FrameBgHovered]: style.Colors[ImGuiCol_FrameBg]);
        col.Value.w*=ImGui::GetStyle().Alpha;
        draw_list->AddRectFilled(controlPos, controlPos + innerSize, col, style.FrameRounding);
    }

    const ImVec2 wpUV = ImGui::GetFontTexUvWhitePixel(); //culling versus
    ImVec2 uv[4]; ImU32 col[4]; //buffers to storetransformed vtx & col for PrimVtx & PrimQuadUV

    glm::quat quat(glm::normalize(qtV)); 

    ////////////////////////////////////////////////////////////////////////////
    //  Just a "few" lambdas... 

    //////////////////////////////////////////////////////////////////
    auto normalizeToControlSize = [&] (float x, float y) {
        return controlPos + ImVec2(x,-y) * halfSquareSize + ImVec2(halfSquareSize,halfSquareSize); //drawing from 0,0 .. no borders
    };

    //////////////////////////////////////////////////////////////////
    auto addTriangle = [&] ()
    {   // test cull dir        
        if(glm::cross(glm::vec2(uv[1].x-uv[0].x, uv[1].y-uv[0].y), 
                      glm::vec2(uv[2].x-uv[0].x, uv[2].y-uv[0].y)) > 0.f)
            { uv[1] = uv[2] = uv[0]; }

        for(int i=0; i<3; i++) draw_list->PrimVtx(uv[i], wpUV, col[i]);
    };

    //////////////////////////////////////////////////////////////////
    auto addQuad = [&] (ImU32 colLight)
    {   // test cull dir
        if(glm::cross(glm::vec2(uv[1].x-uv[0].x, uv[1].y-uv[0].y), 
                      glm::vec2(uv[3].x-uv[0].x, uv[3].y-uv[0].y)) > 0.f)
            { uv[3] = uv[1] = uv[2] = uv[0]; }

        draw_list->PrimQuadUV(uv[0],uv[1],uv[2],uv[3], wpUV, wpUV, wpUV, wpUV, colLight); 
    };

    //////////////////////////////////////////////////////////////////
    auto drawSphere = [&] () 
    {
        draw_list->PrimReserve(sphereVtx.size(), sphereVtx.size()); // num vert/indices 
        auto itTess = sphereTess.begin();
        for(auto itVtx = sphereVtx.begin(); itVtx != sphereVtx.end(); )  {
            for(int h=0; h<3; h++, itTess++) {
                glm::vec3 coord = quat  * (*itVtx++ * solidResizeFactor);        //Rotate

                uv[h] = normalizeToControlSize(coord.x,coord.y);
                const float drawSize = sphereRadius * solidResizeFactor;
                col[h] = addLightEffect(sphereColors[*itTess], (-drawSize*.5f + (coord.z*coord.z) / (drawSize*drawSize))); 
                //col[h] = colorLightedY(sphereCol[i++], (-sizeSphereRadius.5f + (coord.z*coord.z) / (sizeSphereRadius*sizeSphereRadius)), coord.z); 
            }
            addTriangle();
        }
    };

    //////////////////////////////////////////////////////////////////
    auto drawCube = [&] ()  
    {
        draw_list->PrimReserve(cubeNorm.size()*6, cubeNorm.size()*4); // num vert/indices 
        for(auto itNorm = cubeNorm.begin(), itVtx  = cubeVtx.begin() ; itNorm != cubeNorm.end();) {
            glm::vec3 coord;
            glm::vec3 norm = quat * *itNorm;
            for (int i = 0; i<4; ) {
                coord = quat  * (*itVtx++ * solidResizeFactor);                        
                uv[i++] = normalizeToControlSize(coord.x,coord.y);
            }                    
            addQuad(addLightEffect(glm::abs(*itNorm++), norm.z, coord.z));
        }
    };

    //////////////////////////////////////////////////////////////////
    auto drawAxes = [&] (int side) 
    {   
        for(int n = 0; n < 4; n++) { //Arrow: 2 Cone -> (Surface + cap) + 2 Cyl -> (Surface + cap)
            for(int arrowAxis = 0; arrowAxis < 3; arrowAxis++) { // draw 3 axes
                glm::vec3 arrowCoord(0, 0, 0); arrowCoord[arrowAxis] = 1.0f; // rotate on 3 axis (arrow -> X, Y, Z ) in base to current arrowAxis
                const float arrowCoordZ = glm::vec3(quat*arrowCoord).z; //.Rotate

                const int i = (arrowCoordZ > 0) ? 3 - n : n; //painter algorithm: before farthest

                bool skipCone =true;

                if ((side == backSide  && arrowCoordZ > 0) || (side == frontSide && arrowCoordZ <= 0)) {
                    if (!showFullAxes && (i == CYL_CAP)) continue; // skip if cylCap is hidden
                    if (i <= CONE_CAP) continue;  // do not draw cone
                    else skipCone = false;
                }

                auto *ptrVtx = arrowVtx+i;
                draw_list->PrimReserve(ptrVtx->size(), ptrVtx->size()); // // reserve vtx

                for (auto itVtx = ptrVtx->begin(), itNorm = (arrowNorm+i)->begin(); itVtx != ptrVtx->end(); ) { //for all Vtx
#if !defined(imguiGizmo_INTERPOLATE_NORMALS)
                    glm::vec3 norm( quat * fastRotate(arrowAxis, *itNorm++));
#endif                    
                    for(int h=0; h<3; h++) {
                        glm::vec3 coord(*itVtx++ * resizeAxes); //  reduction

                    // reposition starting point...
                        if (!skipCone && coord.x >  0)                         coord.x = -arrowStartingPoint; 
                        if ((skipCone && coord.x <= 0) || 
                           (!showFullAxes && (coord.x < arrowStartingPoint)) ) coord.x =  arrowStartingPoint;
                        //transform
                        coord = quat * fastRotate(arrowAxis, coord); 
                        uv[h] = normalizeToControlSize(coord.x,coord.y);
#ifdef imguiGizmo_INTERPOLATE_NORMALS
                        glm::vec3 norm( quat * fastRotate(arrowAxis, *itNorm++));
#endif                  
                        //col[h] = addLightEffect(ImU32(0xFF) << arrowAxis*8, float(0xa0)*norm.z+.5f);
                        col[h] = addLightEffect(glm::vec3(float(arrowAxis==axisIsX),float(arrowAxis==axisIsY),float(arrowAxis==axisIsZ)), norm.z, coord.z);
                    }
                    addTriangle();
                }
            }
        }
    };

    //////////////////////////////////////////////////////////////////
    auto drawComponent = [&] (const int idx, const glm::quat &q, ptrFunc func)
    {
        auto *ptrVtx = arrowVtx+idx;
        draw_list->PrimReserve(ptrVtx->size(), ptrVtx->size()); // reserve vtx
        for (auto itVtx = ptrVtx->begin(), itNorm = (arrowNorm+idx)->begin(); itVtx != ptrVtx->end(); ) { 
#if !defined(imguiGizmo_INTERPOLATE_NORMALS)
            glm::vec3 norm = (quat * *itNorm++);
#endif
            for(int h=0; h<3; h++) {
                glm::vec3 coord = *itVtx++;
#ifdef imguiGizmo_INTERPOLATE_NORMALS
                glm::vec3 norm = (q * *itNorm++);
#endif
                coord = q * (func(coord) * resizeAxes); // remodelling Directional Arrow (func) and transforms;

                uv[h] = normalizeToControlSize(coord.x,coord.y);
                //col[h] = addLightEffect(color, float(0xa0)*norm.z+.5f);
                col[h] = addLightEffect(glm::vec3(directionColor.x, directionColor.y, directionColor.z), norm.z, coord.z>0 ? coord.z : coord.z*.5);
            }
            addTriangle();
        }

    };

    //////////////////////////////////////////////////////////////////
    auto dirArrow = [&] (const glm::quat &q) 
    {
        glm::vec3 arrowCoord(quat * glm::vec3(1.0f, 0.0f, 0.0f));

        if(arrowCoord.z <= 0) for(int i = 0; i <  4; i++) drawComponent(i, q, adjustDir);
        else                  for(int i = 3; i >= 0; i--) drawComponent(i, q, adjustDir);
            
    };
    
    //////////////////////////////////////////////////////////////////
    auto spotArrow = [&] (const glm::quat &q, const float arrowCoordZ) 
    {
        if(arrowCoordZ > 0) { 
            drawComponent(CONE_SURF, q, adjustSpotCone); drawComponent(CONE_CAP , q, adjustSpotCone);
            drawComponent(CYL_SURF , q, adjustSpotCyl ); drawComponent(CYL_CAP  , q, adjustSpotCyl );
        } else {
            drawComponent(CYL_CAP  , q, adjustSpotCyl ); drawComponent(CYL_SURF , q, adjustSpotCyl );
            drawComponent(CONE_CAP , q, adjustSpotCone); drawComponent(CONE_SURF, q, adjustSpotCone);
        }
            
    };

    //////////////////////////////////////////////////////////////////
    auto draw3DSystem = [&] ()
    {
        drawAxes(backSide);
        if     (axesOriginType & sphereAtOrigin) drawSphere();
        else if(axesOriginType & cubeAtOrigin)   drawCube();
        drawAxes(frontSide);  
    };

    //  ... and now..  draw the widget!!!
    ///////////////////////////////////////
    if(drawMode == modeDirection) dirArrow(quat);
    else { // draw arrows & sphere
        if(drawMode == modeDual) {
            glm::vec3 spot(qtV2 * glm::vec3(-1.0f, 0.0f, .0f)); // versus opposite
            if(spot.z>0) { draw3DSystem(); spotArrow(glm::normalize(qtV2),spot.z); }
            else         { spotArrow(glm::normalize(qtV2),spot.z); draw3DSystem(); }
        } else draw3DSystem();
    }

    ImGui::EndGroup();
    ImGui::PopID();

    return value_changed;
}

//
//  Cube
//
////////////////////////////////////////////////////////////////////////////
void imguiGizmo::buildCube(const float size)
{

    cubeVtx .clear();
    cubeNorm.clear(); 

#define V(x,y,z) cubeVtx.push_back(glm::vec3(x size, y size, z size))
#define N(x,y,z) cubeNorm.push_back(glm::vec3(x, y, z))

    N( 1.0, 0.0, 0.0); V(+,-,+); V(+,-,-); V(+,+,-); V(+,+,+);
    N( 0.0, 1.0, 0.0); V(+,+,+); V(+,+,-); V(-,+,-); V(-,+,+);
    N( 0.0, 0.0, 1.0); V(+,+,+); V(-,+,+); V(-,-,+); V(+,-,+);
    N(-1.0, 0.0, 0.0); V(-,-,+); V(-,+,+); V(-,+,-); V(-,-,-);
    N( 0.0,-1.0, 0.0); V(-,-,+); V(-,-,-); V(+,-,-); V(+,-,+);
    N( 0.0, 0.0,-1.0); V(-,-,-); V(-,+,-); V(+,+,-); V(+,-,-);

#undef V
#undef N
}
//
//  Sphere
//
////////////////////////////////////////////////////////////////////////////
void imguiGizmo::buildSphere(const float radius, const int tessFactor)
{
    const int tess      =  tessFactor;
    const int div       =  tessFactor; //tessellation colors: meridians/div x paralles/div
    const int meridians = 32; //64/2;
    const int parallels = meridians/2;

    sphereVtx .clear();
    sphereTess.clear();

#   define V(x,y,z) sphereVtx.push_back(glm::vec3(x, y, z))
#   define T(t)     sphereTess.push_back(t)
 
    const float incAngle = 2.f*glm::pi<float>()/(float)( meridians );
    float angle = incAngle;

    // Adjust z and radius as stacks are drawn. 
    float z0, z1 = cosf(angle)*radius;
    float r0, r1 = sinf(angle)*radius;
    float x1 = -1.0f;
    float y1 =  0.0f;

    // The first parallel is covered with triangles

    for (int j=0; j<meridians; j++, angle+=incAngle)
    {
        const float x0 = x1; x1 = cosf(glm::pi<float>()-angle);
        const float y0 = y1; y1 = sinf(glm::pi<float>()-angle);

        const int tType = ((j>>div)&1);
        
        V(0.0f,   0.0f, radius); T(tType);        
        V(x0*r1,-y0*r1,     z1); T(tType);
        V(x1*r1,-y1*r1,     z1); T(tType);
    }

    // Cover each stack with a quad divided in 2 triangles, except the top and bottom stacks 
    angle = incAngle+incAngle;
    x1 = 1.f; y1 = 0.f;
    
    for(int i=1; i<parallels-1; i++, angle+=incAngle)  {
        //int div =8;
        z0 = z1; z1 = cosf(angle)*radius;
        r0 = r1; r1 = sinf(angle)*radius;
        float angleJ = incAngle;

            for(int j=0; j<meridians; j++, angleJ+=incAngle)  {
                
                const float x0 = x1; x1 = cosf(angleJ);
                const float y0 = y1; y1 = sinf(angleJ);

                const int tType = ((i>>div)&1) ? ((j>>div)&1) : !((j>>div)&1); 

                V(x0*r1, -y0*r1, z1); T(tType);
                V(x0*r0, -y0*r0, z0); T(tType);
                V(x1*r0, -y1*r0, z0); T(tType);
                V(x0*r1, -y0*r1, z1); T(tType);
                V(x1*r0, -y1*r0, z0); T(tType);
                V(x1*r1, -y1*r1, z1); T(tType);
            }
    }

    // The last parallel is covered with triangls

    z0 = z1; 
    r0 = r1;
    x1 = -1.0f; y1 = 0.f;

    angle = incAngle;
    for (int j=0; j<meridians; j++,angle+=incAngle)
    {
        const float x0 = x1; x1 = cosf(angle+glm::pi<float>());
        const float y0 = y1; y1 = sinf(angle+glm::pi<float>());

        const int tType = ((parallels-1)>>div)&1 ? ((j>>div)&1) : !((j>>div)&1); 
        //color = 0xff0000ff;

        V( 0.0f,   0.0f,-radius); T(tType);
        V(x0*r0, -y0*r0,     z0); T(tType);
        V(x1*r0, -y1*r0,     z0); T(tType);
    }
#   undef V
#   undef C
}
//
//  Cone
//
////////////////////////////////////////////////////////////////////////////
void imguiGizmo::buildCone(const float x0, const float x1, const float radius, const int slices)
{
    const float height = x1-x0 ;
    const float zStep = height;
    const float rStep = radius/(float)slices;

    // Scaling factors for vertex normals 

    const float sq = sqrtf( height * height + radius * radius );
    const float cosn =  height / sq;
    const float sinn =  radius / sq;


    const float incAngle = 2*glm::pi<float>()/(float)( slices );
    float angle = incAngle;

    float yt1 = sinn,  y1 = radius;// cos(0) * sinn ... cos(0) * radius 
    float zt1 = 0.0f,  z1 = 0.0f;  // sin(0) * sinn ... sin(0) * radius 

    const float xt0 = x0 * cosn, xt1 = x1 * cosn; 

    arrowVtx[CONE_CAP ].clear(); arrowNorm[CONE_CAP ].clear();
    arrowVtx[CONE_SURF].clear(); arrowNorm[CONE_SURF].clear();

#   define V(i,x,y,z) arrowVtx [i].push_back(glm::vec3(x, y, z))
#   define N(i,x,y,z) arrowNorm[i].push_back(glm::vec3(x, y, z)) 

    for (int j=0; j<slices; j++, angle+=incAngle)
    {   
        const float yt0 = yt1;  yt1 = cosf(angle);
        const float y0  = y1;   y1  = yt1*radius;   yt1*=sinn;
        const float zt0 = zt1;  zt1 = sinf(angle);
        const float z0  = z1;   z1  = zt1*radius;   zt1*=sinn;   

    // Cover the circular base with a triangle fan... 
        V(CONE_CAP,  x0, 0.f, 0.f);
        V(CONE_CAP,  x0,  y0, -z0);
        V(CONE_CAP,  x0,  y1, -z1);

        N(CONE_CAP,-1.f, 0.f, 0.f);
#                                    ifdef imguiGizmo_INTERPOLATE_NORMALS
        N(CONE_CAP,-1.f, 0.f, 0.f);
        N(CONE_CAP,-1.f, 0.f, 0.f);
#endif
        V(CONE_SURF, x1, 0.f, 0.f);
        V(CONE_SURF, x0,  y0,  z0);
        V(CONE_SURF, x0,  y1,  z1);
#                                    ifdef imguiGizmo_INTERPOLATE_NORMALS
        N(CONE_SURF,xt1, 0.f, 0.f);
        N(CONE_SURF,xt0, yt0, zt0);
        N(CONE_SURF,xt0, yt1, zt1);
#else
        N(CONE_SURF, xt0, yt0, zt0);
#endif
    }
#undef V
#undef N
}
//
//  Cylinder
//
////////////////////////////////////////////////////////////////////////////
void imguiGizmo::buildCylinder(const float x0, const float x1, const float radius, const int slices)
{

    float y1 = 1.0f, yr1 = radius;
    float z1 = 0.0f, zr1 = 0.0f; // * radius

    
    const float incAngle = 2*glm::pi<float>()/(float)( slices );
    float angle = incAngle;

    arrowVtx[CYL_CAP ].clear(); arrowNorm[CYL_CAP ].clear();
    arrowVtx[CYL_SURF].clear(); arrowNorm[CYL_SURF].clear();

#   define V(i,x,y,z) arrowVtx [i].push_back(glm::vec3(x, y, z))
#   define N(i,x,y,z) arrowNorm[i].push_back(glm::vec3(x, y, z)) 

    for (int j=0; j<slices; j++, angle+=incAngle) {
        const float y0  = y1;   y1  = cosf(angle);
        const float z0  = z1;   z1  = sinf(angle);
        const float yr0 = yr1;  yr1 = y1 * radius;
        const float zr0 = zr1;  zr1 = z1 * radius;

    // Cover the base  
        V(CYL_CAP,   x0, 0.f, 0.f);
        V(CYL_CAP,   x0, yr0,-zr0);
        V(CYL_CAP,   x0, yr1,-zr1);

        N(CYL_CAP, -1.f, 0.f, 0.f);
#                                   ifdef imguiGizmo_INTERPOLATE_NORMALS
        N(CYL_CAP, -1.f, 0.f, 0.f);
        N(CYL_CAP, -1.f, 0.f, 0.f);        
#endif
    // Cover surface
        N(CYL_SURF, 0.f,  y0,  z0);
        N(CYL_SURF, 0.f,  y0,  z0);
#                                   ifdef imguiGizmo_INTERPOLATE_NORMALS
        N(CYL_SURF, 0.f,  y1,  z1);
        N(CYL_SURF, 0.f,  y0,  z0);
        N(CYL_SURF, 0.f,  y1,  z1);
        N(CYL_SURF, 0.f,  y1,  z1);
#endif
      
        V(CYL_SURF,  x1, yr0, zr0);
        V(CYL_SURF,  x0, yr0, zr0);
        V(CYL_SURF,  x0, yr1, zr1);
        V(CYL_SURF,  x1, yr0, zr0);
        V(CYL_SURF,  x0, yr1, zr1);
        V(CYL_SURF,  x1, yr1, zr1);

#ifdef SHOW_FULL_CYLINDER 
    // Cover the top ..in the arrow this cap is covered from cone/pyramid
        V(CYL_CAP ,  x1, 0.f, 0.f);
        V(CYL_CAP ,  x1, yr0, zr0);
        V(CYL_CAP ,  x1, yr1, zr1);
        N(CYL_CAP , 1.f, 0.f, 0.f);
    #                               ifdef imguiGizmo_INTERPOLATE_NORMALS
        N(CYL_CAP , 1.f, 0.f, 0.f);
        N(CYL_CAP , 1.f, 0.f, 0.f);        
    #endif
#endif
    }
#undef V
#undef N
}





