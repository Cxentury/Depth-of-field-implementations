#ifndef MAIN_H
#define MAIN_H

#define RLIGHTS_IMPLEMENTATION
#include "utils.h"
#include "BoxBlurDof.h"
#include "accumulationDoF.h"

#define GLSL_VERSION 330
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

void draw_scene();

#endif