/*
 * APoV project
 * By m-c/d in 2020
 */

#ifndef OPTIONS_HPP
#define OPTIONS_HPP
    
    #include <cmath>
    #include <cstdio>
    #include <cstdlib>
    #include <string>
    #include <unistd.h>
    
    #ifdef PSP
        #include <GL/glut.h>
        #include <pspkernel.h>
        #include <pspctrl.h>
        #include <psppower.h>
        #include <pspdisplay.h>     
        #define M_PI 3.14159265358979323846
        #define printf pspDebugScreenPrintf
    #else
        #include <GL/freeglut.h>
    #endif
    
    // Types
    #define u32 unsigned int
    #define u16 short u32
    #define u8 unsigned char

    class Options {
        public:
        static int SPACE_SIZE;
        static int ATOMIC_POV_COUNT;
        static int RAY_STEP;
        static u32 MAX_RAY_DEPTH;
        static bool CAM_LOCKED;
        static bool CAM_HEMISPHERE;
        static bool SMOOTH_PIXELS;
        static bool FILTER_GAPS;
        static bool FILTER_GAPS_LITE;
        static float MAX_PROJECTION_DEPTH;
        //
        static void init(int argc, char **argv);
    };
#endif

