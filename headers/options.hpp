/*
 * APoV project
 * By m-c/d in 2020
 */

#ifndef OPTIONS_HPP
#define OPTIONS_HPP
    #include <GL/freeglut.h>
    #include <cstring>
    #include <cstdio>
    #include <string>
    
        // Types
    #define u32 unsigned int
    #define u16 short u32
    #define u8 unsigned char

    class Options {
        public:
        static int SPACE_SIZE;
        static int ATOMIC_POV_COUNT;
        //
        static void init(int argc, char **argv);
    };
#endif
