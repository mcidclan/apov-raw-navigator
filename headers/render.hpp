#ifndef APOV_RENDER_HPP
#define APOV_RENDER_HPP
    
    #include <cmath>
    #include "options.hpp"
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
