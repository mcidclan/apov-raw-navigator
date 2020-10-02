#include "./headers/render.hpp"

namespace render {
    static const float RAD_ANGLE = M_PI / 180.0f;
    
    static u16 WIN_WIDTH;
    static u16 WIN_HEIGHT;
    static u16 WIN_WIDTH_D2;
    static u16 WIN_HEIGHT_D2;
    static u16 WIN_WIDTH_D4;
    static u32 WIN_PIXELS_COUNT;
    static u32 WIN_BYTES_COUNT;
    static u32 VIEW_BYTES_COUNT;
    static uint64_t SPACE_BYTES_COUNT;
    static float ATOMIC_POV_STEP;
    // Todo
    static int _move = 0;
    static int _rotate = 0;
    static int moveStep = 0;
    static int rotateStep = 0;
    
    static u32* view;
    static u8* fbuff;
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
            if(_move > (Options::SPACE_SIZE / Options::RAY_STEP) - 1) {
                _move = (Options::SPACE_SIZE / Options::RAY_STEP) - 1;
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
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        pixels = new u8[WIN_BYTES_COUNT];
        fbuff = new u8[WIN_BYTES_COUNT];
        memset(pixels, 0x00, WIN_BYTES_COUNT);
        
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        if(Options::SMOOTH_PIXELS)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIN_WIDTH, WIN_HEIGHT,
        0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        
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
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        initSurface();
    }
    
    static void filterGap(const u32* const p) {
        const u8 pa[3] = {pixels[p[0] + 0], pixels[p[0] + 1], pixels[p[0] + 2]};
        const u8 pb[3] = {pixels[p[1] + 0], pixels[p[1] + 1], pixels[p[1] + 2]};
        const u8 pc[3] = {pixels[p[2] + 0], pixels[p[2] + 1], pixels[p[2] + 2]};
        const u8 pd[3] = {pixels[p[3] + 0], pixels[p[3] + 1], pixels[p[3] + 2]};
        const u8 pe[3] = {pixels[p[4] + 0], pixels[p[4] + 1], pixels[p[4] + 2]};
        const u8 pf[3] = {pixels[p[5] + 0], pixels[p[5] + 1], pixels[p[5] + 2]};
        const u8 pg[3] = {pixels[p[6] + 0], pixels[p[6] + 1], pixels[p[6] + 2]};
        const u8 ph[3] = {pixels[p[7] + 0], pixels[p[7] + 1], pixels[p[7] + 2]};
        const u8 pi[3] = {pixels[p[8] + 0], pixels[p[8] + 1], pixels[p[8] + 2]};
        
        const u8 a = (pa[0] | pa[1] | pa[2]) ? 1 : 0;
        const u8 b = (pb[0] | pb[1] | pb[2]) ? 1 : 0;
        const u8 c = (pc[0] | pc[1] | pc[2]) ? 1 : 0;
        const u8 d = (pd[0] | pd[1] | pd[2]) ? 1 : 0;
        const u8 e = (pe[0] | pe[1] | pe[2]) ? 1 : 0;
        const u8 f = (pf[0] | pf[1] | pf[2]) ? 1 : 0;
        const u8 g = (pg[0] | pg[1] | pg[2]) ? 1 : 0;
        const u8 h = (ph[0] | ph[1] | ph[2]) ? 1 : 0;
        const u8 i = (pi[0] | pi[1] | pi[2]) ? 1 : 0;
                
        if((a || b || c || d) && !e && (f || g || h || i)) {
            const u8 n = a + b + c + d + f  + g + h + i;
            fbuff[p[4] + 0] = (pa[0] + pb[0] + pc[0] + pd[0] + pf[0] + pg[0] + ph[0] + pi[0]) / n;
            fbuff[p[4] + 1] = (pa[1] + pb[1] + pc[1] + pd[1] + pf[1] + pg[1] + ph[1] + pi[1]) / n;
            fbuff[p[4] + 2] = (pa[2] + pb[2] + pc[2] + pd[2] + pf[2] + pg[2] + ph[2] + pi[2]) / n;
            fbuff[p[4] + 3] = 0xFF;
        }
    }

    static void filterGapLite(const u32* const p) {
        const u8 pc[3] = {pixels[p[0] + 0], pixels[p[0] + 1], pixels[p[0] + 2]};
        const u8 pd[3] = {pixels[p[1] + 0], pixels[p[1] + 1], pixels[p[1] + 2]};
        const u8 pe[3] = {pixels[p[2] + 0], pixels[p[2] + 1], pixels[p[2] + 2]};
        const u8 pf[3] = {pixels[p[3] + 0], pixels[p[3] + 1], pixels[p[3] + 2]};
        const u8 pg[3] = {pixels[p[4] + 0], pixels[p[4] + 1], pixels[p[4] + 2]};
        
        const u8 c = (pc[0] | pc[1] | pc[2]) ? 1 : 0;
        const u8 d = (pd[0] | pd[1] | pd[2]) ? 1 : 0;
        const u8 e = (pe[0] | pe[1] | pe[2]) ? 1 : 0;
        const u8 f = (pf[0] | pf[1] | pf[2]) ? 1 : 0;
        const u8 g = (pg[0] | pg[1] | pg[2]) ? 1 : 0;
                
        if((c || d) && !e && (f || g)) {
            const u8 n = c + d + f  + g;
            fbuff[p[2] + 0] = (pc[0] + pd[0] + pf[0] + pg[0]) / n;
            fbuff[p[2] + 1] = (pc[1] + pd[1] + pf[1] + pg[1]) / n;
            fbuff[p[2] + 2] = (pc[2] + pd[2] + pf[2] + pg[2]) / n;
            fbuff[p[2] + 3] = 0xFF;
        }
    }
    
    static void filterGaps(const u8 pass) {
        u8 p = pass;
        while(p--) {
            int x = WIN_WIDTH - 1;
            while(--x > 1) {
                int y = WIN_HEIGHT - 1;
                while(--y > 1) {
                    const u32 _y = y * WIN_WIDTH;

                    if(!Options::FILTER_GAPS_LITE) {
                        const u32 gx[9] = {
                            (x - 1 + _y - WIN_WIDTH) * COLOR_BYTES_COUNT, //a
                            (x - 1 + _y            ) * COLOR_BYTES_COUNT, //b
                            (x - 1 + _y + WIN_WIDTH) * COLOR_BYTES_COUNT, //c
                            
                            (x + _y - WIN_WIDTH) * COLOR_BYTES_COUNT, //d
                            (x + _y            ) * COLOR_BYTES_COUNT, //e
                            (x + _y + WIN_WIDTH) * COLOR_BYTES_COUNT, //f
                            
                            (x + 1 + _y - WIN_WIDTH) * COLOR_BYTES_COUNT, //g
                            (x + 1 + _y            ) * COLOR_BYTES_COUNT, //h
                            (x + 1 + _y + WIN_WIDTH) * COLOR_BYTES_COUNT //i
                        };
                        filterGap(gx);
                    } else {
                        const u32 gx[5] = {
                            (x - 1 + _y        ) * COLOR_BYTES_COUNT, //c                        
                            (x + _y - WIN_WIDTH) * COLOR_BYTES_COUNT, //d
                            (x + _y            ) * COLOR_BYTES_COUNT, //e
                            (x + _y + WIN_WIDTH) * COLOR_BYTES_COUNT, //f
                            (x + 1 + _y        ) * COLOR_BYTES_COUNT //g
                        };
                        filterGapLite(gx);
                    }
                }
            }
        
            u32 i = WIN_BYTES_COUNT;
            while(i--) {
                if(fbuff[i]) {
                    pixels[i] = fbuff[i];
                }
            }
        }
    }
    
    static float translateX(const float x) {
        if(!Options::CAM_LOCKED) {
            float angle = _rotate * ATOMIC_POV_STEP + 90.0f;
            if(Options::CAM_HEMISPHERE && angle > 90.0f) {
                angle += 180.0f;
            }
            angle *= RAD_ANGLE;
            return x - cosf(angle) * WIN_WIDTH_D2;
        }
        return x;
    }
    
    static uint64_t loffset = -1;
    #ifndef PSP
        static FILE* f;
        static void openCloseData(bool open) {
            if(open) {
                f = fopen64("atoms.bin", "r");
            } else fclose(f);
        }
        static void readFrame(const uint64_t offset) {
            if(offset != loffset) {
                fseeko64(f, offset, SEEK_SET);
                fread(view, sizeof(u32), WIN_PIXELS_COUNT, f);
                loffset = offset;
            }
        }
        static void getFrame(const uint64_t offset) {
            openCloseData(1);
            readFrame(offset);
            openCloseData(0);
        }
    #else   
        static SceUID f;
        static void openCloseData(bool open) {
            if(open) {
                f = sceIoOpen("atoms.bin", PSP_O_RDONLY, 0777);
            } else sceIoClose(f);
        }
        static void readFrame(const uint64_t offset) {
            if(offset != loffset) {
                sceIoLseek(f, offset, SEEK_SET);
                sceIoRead(f, view, WIN_PIXELS_COUNT * sizeof(u32));
                loffset = offset;
            }
        }
    #endif
    
    static void getView() {
        // Fake stream
        #ifndef PSP
            getFrame(VIEW_BYTES_COUNT * _move + SPACE_BYTES_COUNT * _rotate);
        #else
            readFrame(VIEW_BYTES_COUNT * _move + SPACE_BYTES_COUNT * _rotate);
        #endif
        
        int x = WIN_WIDTH;
        while(x--) {
            int y = WIN_HEIGHT;
            while(y--) {
                const u32 step = x + y * WIN_WIDTH;
                
                const u8 depth = (u8)(view[step] & 0x000000FF);
                const float s = 1.0f - (float)depth / Options::MAX_PROJECTION_DEPTH;
                const int _x = translateX((x - WIN_WIDTH_D2) * s);
                const int _y = (y - WIN_HEIGHT_D2) * s;
                
                if(_x >= -WIN_WIDTH_D2 && _x < WIN_WIDTH_D2 && _y >= -WIN_HEIGHT_D2 && _y < WIN_HEIGHT_D2) {
                    const u32 pstep = ((_x + WIN_WIDTH_D2) + ((_y + WIN_HEIGHT_D2) * WIN_WIDTH));
                    const u32 poffset = pstep * COLOR_BYTES_COUNT;
                    
                    u8* const px0 = &pixels[poffset + 0];
                    u8* const px1 = &pixels[poffset + 1];
                    u8* const px2 = &pixels[poffset + 2];
                    u8* const px3 = &pixels[poffset + 3];
                    
                    const u8 v0 = (view[step] & 0xFF000000) >> 24;
                    const u8 v1 = (view[step] & 0x00FF0000) >> 16;
                    const u8 v2 = (view[step] & 0x0000FF00) >> 8;

                    if(
                        (v0 != 0 || v1 != 0 || v2 != 0) &&
                        ((*px0 == 0 && *px1 == 0 && *px2 == 0) || (depth < zvalues[pstep]))
                    ) {
                        *px0 = v0;
                        *px1 = v1;
                        *px2 = v2;
                        *px3 = 0xFF;
                        zvalues[pstep] = depth;
                    }
                }
            }
        }
    }
    
    void display() {
        memset(fbuff, 0x00, WIN_BYTES_COUNT);
        memset(pixels, 0x00, WIN_BYTES_COUNT);
        memset(zvalues, 0x00, WIN_PIXELS_COUNT);
        
        getView();
        if(Options::FILTER_GAPS) {
            filterGaps(1);
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, texture);
        glEnable(GL_TEXTURE_2D);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIN_WIDTH,
        WIN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //glScalef(2.0f, 2.0f, 1.0f);
        glCallList(surface);
        glutSwapBuffers();
    }
        
    void init() {
        printf("Init...\n");
        ATOMIC_POV_STEP = 360.0f / Options::ATOMIC_POV_COUNT;
        WIN_WIDTH = WIN_HEIGHT = Options::SPACE_SIZE;
        WIN_WIDTH_D2 = WIN_WIDTH / 2;
        WIN_HEIGHT_D2 = WIN_HEIGHT / 2;
        WIN_WIDTH_D4 = WIN_WIDTH_D2 / 2;
        WIN_PIXELS_COUNT = WIN_WIDTH * WIN_HEIGHT;
        WIN_BYTES_COUNT = WIN_PIXELS_COUNT * COLOR_BYTES_COUNT;
        VIEW_BYTES_COUNT = WIN_PIXELS_COUNT * sizeof(u32);
        SPACE_BYTES_COUNT = (Options::SPACE_SIZE / Options::RAY_STEP) * VIEW_BYTES_COUNT;        
        
        view = new u32[WIN_PIXELS_COUNT];
        zvalues = new u8[WIN_PIXELS_COUNT];
        memset(view, 0x00, sizeof(u32) * WIN_PIXELS_COUNT);
        
        printf("Read first frame...\n");
        openCloseData(true);
        readFrame(0);
        openCloseData(false);
        
        printf("First frame contains...");
        u32 nvoxel = 0;
        u32 i = WIN_PIXELS_COUNT;
        while(i--) {
            nvoxel +=  view[i] ? 1 : 0;
        }
        
        printf(" %u voxels.\n", nvoxel);
        sleep(1);
        
        openCloseData(true);
    }
}