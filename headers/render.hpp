#ifndef APOV_RENDER_HPP
#define APOV_RENDER_HPP
    
    #include "options.hpp"
    
    //#define MAX_RAY_DEPTH 64
    //#define MAX_PROJECTION_DEPTH 300.0f
    #define COLOR_BYTES_COUNT 3
    
    namespace render {
        int _win_width();
        int _win_height();
        
        void init();
        void initGl();
        void display();
        void reshape(int, int);
        //
        void rotate(const int, bool = true);
        void move(const int, bool = true);
    }
#endif
