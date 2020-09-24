#include "./headers/render.hpp"

namespace render {
    static u32 move = 0;
    static u32 moveStep = 0;
    //static bool filled = false;
    
    static u8 view[VIEW_BYTES_COUNT] = {0};
    static u8* pixels;
    static GLuint surface;
    static GLuint texture;

    void moveZ(const int step, bool set) {
        if(set) {
            moveStep = step;
        } else {
            move+=moveStep;
            if(move < 0) {
                move = 0;
            }
            if(move > 511) {
                move = 511;
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
        memset(pixels, 0xFF, WIN_BYTES_COUNT);
        
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

    void init() {
        glEnable(GL_CULL_FACE);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        initSurface();
    }

    static void getView() {
        //if(!filled) {
            FILE* f = fopen("atoms.bin", "r");
            fseek(f, VIEW_BYTES_COUNT * move, SEEK_SET);
            fread(view, sizeof(Atom), WIN_PIXELS_COUNT, f);
            fclose(f);
            
            u32 step = WIN_PIXELS_COUNT;
            while(--step) {
                const u32 poffset = step * 3;
                const u32 voffset = step * 4;
                pixels[poffset + 0] = view[voffset + 3];
                pixels[poffset + 1] = view[voffset + 2];
                pixels[poffset + 2] = view[voffset + 1];
            }
        //}
        //memset(pixels, 0x40, WIN_BYTES_COUNT);
        moveZ(0, false);
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
        glutPostRedisplay();
    }
}