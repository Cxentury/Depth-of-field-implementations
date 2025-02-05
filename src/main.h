#ifndef MAIN_H
#define MAIN_H

#define RLIGHTS_IMPLEMENTATION
#include "utils.h"
#include "boxBlurDof.h"
#include "accumulationDoF.h"
#include "singlePassDoF.h"
#include "gatherBasedDoF.h"

#define GLSL_VERSION 330
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

void draw_scene();

#endif