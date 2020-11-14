/*
 * APoV project
 * By m-c/d in 2020
 */

#ifndef OPTIONS_HPP
#define OPTIONS_HPP
    #include <cmath>
    #include <cstdio>
    #include <cstdlib>
    #include <cstring>
    #include <string>
    #include <unistd.h>
    #include <GL/freeglut.h>
    #define u64 uint64_t
    #define u32 unsigned int
    #define u16 short u32
    #define u8 unsigned char
    class Options {
        public:
        static bool CAM_HEMISPHERE;
        static bool CAM_LOCKED;
        static bool SMOOTH_PIXELS;
        static bool FILTER_GAPS;
        static bool FILTER_GAPS_LITE;
        static float MAX_PROJECTION_DEPTH;
        static u8 HEADER_SIZE;
        static u8 SPACE_BLOCK_COUNT;
        static u8 WIDTH_BLOCK_COUNT;
        static u8 DEPTH_BLOCK_COUNT;
        static u16 SPACE_BLOCK_SIZE;
        static u16 RAY_STEP;
        static u16 HORIZONTAL_POV_COUNT;
        static u16 VERTICAL_POV_COUNT;
        static void init(int argc, char **argv);
    };
#endif
