#ifndef APOV_RENDER_HPP
#define APOV_RENDER_HPP
    #include <GL/freeglut.h>
    #include <cstring>
    #include <cstdio>
    
    #define u32 unsigned int
    #define u16 short u32
    #define u8 unsigned char

    #define WIN_WIDTH 512
    #define WIN_HEIGHT 512
    #define COLOR_BYTES_COUNT 3
    
    const u16 WIN_WIDTH_D2 = WIN_WIDTH / 2;
    const u16 WIN_HEIGHT_D2 = WIN_HEIGHT / 2;
    const u32 WIN_PIXELS_COUNT = WIN_WIDTH * WIN_HEIGHT;
    const u32 WIN_BYTES_COUNT = WIN_PIXELS_COUNT * COLOR_BYTES_COUNT;
    //
    const u32 VIEW_BYTES_COUNT = WIN_PIXELS_COUNT * 4;
    
    #define ATOMIC_POV_QUANTITY 1
    struct Atom {
        u32 quanta[ATOMIC_POV_QUANTITY];
    };
    
    namespace render {
        void init();
        void display();
        void reshape(int, int);
        //
        void moveZ(const int, bool = true);
    }
#endif
