#ifndef APOV_RENDER_HPP
#define APOV_RENDER_HPP
    #include "options.hpp"
    #define COLOR_BYTES_COUNT 4
    namespace render {
        extern int hstep;
        extern int vstep;
        extern int mstep;
        int _win_width();
        int _win_height();
        void init();
        void initGl();
        void display();
        void reshape(int, int);
    }
#endif
