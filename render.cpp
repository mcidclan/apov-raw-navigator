#include "./headers/render.hpp"

namespace render {
    static u16 WIN_WIDTH;
    static u16 WIN_HEIGHT;
    static u16 WIN_WIDTH_D2;
    static u16 WIN_HEIGHT_D2;
    static u32 WIN_PIXELS_COUNT;
    static u32 WIN_BYTES_COUNT;
    static u32 VIEW_BYTES_COUNT;
    static uint64_t SPACE_BYTES_COUNT;
    
    // Todo
    static int _move = 0;
    static int _rotate = 0;
    static int moveStep = 0;
    static int rotateStep = 0;
    
    static u32* view;
    static u8* pixels;
    static u8* zvalues;
    static GLuint surface;
    static GLuint texture;
    
    int _win_width() {
        return WIN_WIDTH;
    }
    
    int _win_height() {
        return WIN_HEIGHT;
    }
    
    void rotate(const int step, bool set) {
        if(set) {
            rotateStep = step;
        } else {
            _rotate += rotateStep;
            if(_rotate < 0) {
                _rotate = Options::ATOMIC_POV_COUNT - 1;
            } else if(_rotate >= Options::ATOMIC_POV_COUNT) {
                _rotate = 0;
            }
        }
    }
    
    void move(const int step, bool set) {
        if(set) {
            moveStep = step;
        } else {
            _move += moveStep;
            if(_move < 0) {
                _move = 0;
            }
            if(_move > Options::SPACE_SIZE - 1) {
                _move = Options::SPACE_SIZE - 1;
            }
        }
    }
    
    void reshape(int width, int height) {
        const double halfw = width/2.0;
        const double halfh = height/2.0;
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-halfw, halfw, halfh, -halfh, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    static void initSurface() {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        pixels = (u8*)malloc(WIN_BYTES_COUNT);
        memset(pixels, 0x00, WIN_BYTES_COUNT);
        
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT,
        0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        
        surface = glGenLists(1);
        glNewList(surface, GL_COMPILE);
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0); glVertex2i(-WIN_WIDTH_D2, -WIN_HEIGHT_D2);
        glTexCoord2d(0, 1); glVertex2i(-WIN_WIDTH_D2, WIN_HEIGHT_D2);
        glTexCoord2d(1, 1); glVertex2i(WIN_WIDTH_D2, WIN_HEIGHT_D2);
        glTexCoord2d(1, 0); glVertex2i(WIN_WIDTH_D2, -WIN_HEIGHT_D2);
        glEnd();
        glEndList();
    }

    void initGl() {
        glEnable(GL_CULL_FACE);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        initSurface();
    }
    
    static void getView() {
        FILE* f = fopen("atoms.bin", "r");
        fseeko64(f, VIEW_BYTES_COUNT * _move + SPACE_BYTES_COUNT * _rotate, SEEK_SET);
        fread(view, sizeof(u32), WIN_PIXELS_COUNT, f);
        fclose(f);
        
        memset(pixels, 0x00, WIN_BYTES_COUNT);
        memset(zvalues, 0x00, WIN_PIXELS_COUNT);
        
        int x = WIN_WIDTH;
        while(x--) {
            int y = WIN_HEIGHT;
            while(y--) {
                const u32 step = x + y * WIN_WIDTH;
                
                const float depth = (float)((u8)(view[step] & 0x000000FF));
                const float f = (1.0f - depth / MAX_PROJECTION_DEPTH);
                const int _x = (x - WIN_WIDTH_D2) * f;
                const int _y = (y - WIN_HEIGHT_D2) * f;
                
                const u32 pstep = ((_x + WIN_WIDTH_D2) + ((_y + WIN_HEIGHT_D2) * WIN_WIDTH));
                const u32 poffset = pstep * 3;
                
                u8* const px0 = &pixels[poffset + 0];
                u8* const px1 = &pixels[poffset + 1];
                u8* const px2 = &pixels[poffset + 2];
                
                const u8 v0 = (view[step] & 0xFF000000) >> 24;
                const u8 v1 = (view[step] & 0x00FF0000) >> 16;
                const u8 v2 = (view[step] & 0x0000FF00) >> 8;

                if(
                    (*px0 == 0 && *px1 == 0 && *px2 == 0) ||
                    ((depth <= zvalues[pstep]) && (v0 != 0 || v1 != 0 || v2 != 0))
                ) {
                    *px0 = v0;
                    *px1 = v1;
                    *px2 = v2;
                    zvalues[pstep] = depth;
                }
            }
        }
    }
    
    void display() {
        getView();
        glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, texture);
        glEnable(GL_TEXTURE_2D);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIN_WIDTH,
        WIN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glCallList(surface);
        glutSwapBuffers();
        //glutPostRedisplay();
    }
    
    void init() {
        WIN_WIDTH = WIN_HEIGHT = Options::SPACE_SIZE;
        WIN_WIDTH_D2 = WIN_WIDTH / 2;
        WIN_HEIGHT_D2 = WIN_HEIGHT / 2;
        WIN_PIXELS_COUNT = WIN_WIDTH * WIN_HEIGHT;
        WIN_BYTES_COUNT = WIN_PIXELS_COUNT * COLOR_BYTES_COUNT;
        VIEW_BYTES_COUNT = WIN_PIXELS_COUNT * sizeof(u32);
        SPACE_BYTES_COUNT = Options::SPACE_SIZE * VIEW_BYTES_COUNT;        
        
        view = new u32[WIN_PIXELS_COUNT];
        zvalues = new u8[WIN_PIXELS_COUNT];
        memset(view, 0x00, sizeof(u32)*WIN_PIXELS_COUNT);
        memset(zvalues, 0x00, sizeof(u8)*WIN_PIXELS_COUNT);
    }
}