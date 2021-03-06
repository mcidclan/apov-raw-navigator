#include "./headers/render.hpp"

namespace render {    
    int mstep = 0, hstep = 0, vstep = 0;
    static const float RAD_ANGLE = M_PI / 180.0f;
    static float PROJECTION_FACTOR;
    static int LAST_POSITION;
    
    static u16 WIN_WIDTH;
    static u16 WIN_HEIGHT;
    static u16 WIN_WIDTH_D2;
    static u16 WIN_HEIGHT_D2;
    static u16 WIN_WIDTH_D4;
    static u32 WIN_PIXELS_COUNT;
    static u32 WIN_BYTES_COUNT;
    static u32 VIEW_BYTES_COUNT;
    static u64 SPACE_BYTES_COUNT;
    static float ATOMIC_POV_STEP;
    
    static int _move = 0;
    static int _hrotate = 0;
    static int _vrotate = 0;
    
    static u32* view;
    static u8* fbuff;
    static u8* pixels;
    static u8* zvalues;
    static GLuint surface;
    static GLuint texture;
    
    // Pre-calculation Processes for realtime projection
    typedef struct {
        int x, y;
    } Coords;

    static float _FACTORS[255] = {0.0f};
    static Coords* _COORDINATES;

    void preCalculate() {
        _COORDINATES = new Coords[WIN_PIXELS_COUNT];
        
        u8 depth = 255;
        while(depth--) {
            _FACTORS[depth] = 1.0f - ((float)depth * PROJECTION_FACTOR);
        }
        
        u16 x = WIN_WIDTH;
        while(x--) {
            u16 y = WIN_HEIGHT;
            while(y--) {
                const u32 i = x + y * WIN_WIDTH;
                _COORDINATES[i].x = x - WIN_WIDTH_D2;
                _COORDINATES[i].y = y - WIN_HEIGHT_D2;
            }
        }
    }
    
    int _win_width() {
        return WIN_WIDTH;
    }
    
    int _win_height() {
        return WIN_HEIGHT;
    }
    
    int ajustCursor(const int value, const u8 mode) {
        if(!mode) {
            u16 max;
            if(value < 0) {
                return 0;
            } else if(value >= (max = (Options::SPACE_BLOCK_SIZE *
                Options::DEPTH_BLOCK_COUNT) / Options::RAY_STEP)) {
                return max - 1;
            }
        } else if(mode == 1) {   
            if(value < 0) {
                return Options::HORIZONTAL_POV_COUNT - 1;
            } else if(value >= Options::HORIZONTAL_POV_COUNT) {
                return 0;
            }
        } else if(mode == 2) {   
            if(value < 0) {
                return Options::VERTICAL_POV_COUNT - 1;
            } else if(value >= Options::VERTICAL_POV_COUNT) {
                return 0;
            }
        }
        return value;
    }    
    
    void controls() {
        _move += mstep;
        _hrotate += hstep;
        _vrotate += vstep;
        _move = ajustCursor(_move, 0);
        _hrotate = ajustCursor(_hrotate, 1);
        _vrotate = ajustCursor(_vrotate, 2);
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
        if(Options::FILTER_GAPS) {
            fbuff = new u8[WIN_BYTES_COUNT];
        }
        memset(pixels, 0x00, WIN_BYTES_COUNT);
        
        glGenTextures(4, &texture);
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
            float angle = _hrotate * ATOMIC_POV_STEP + 90.0f;
            if(Options::CAM_HEMISPHERE && angle > 90.0f) {
                angle += 180.0f;
            }
            angle *= RAD_ANGLE;
            return x - cosf(angle) * WIN_WIDTH_D2;
        }
        return x;
    }
    
    static FILE* f;
    static void openCloseData(bool open) {
        if(open) {
            f = fopen64("atoms.apov", "rb");
        } else fclose(f);
    }
    
    static void readFrame(const u64 offset) {
        static u64 loffset = -1;
        if(offset != loffset) {
            fseeko64(f, offset, SEEK_SET);
            fread(view, sizeof(u32), WIN_PIXELS_COUNT, f);
            loffset = offset;
        }
    }
    
    static void drawPixels() {
        u32 i = WIN_PIXELS_COUNT;
        while(i--) {
            const u32 _view = view[i];
            if(_view) {
                const u8 depth = (u8)(_view & 0x000000FF);
                const float s = _FACTORS[depth];
                const int _x = translateX(_COORDINATES[i].x * s);
                const int _y = _COORDINATES[i].y * s;
                
                if(_x >= -WIN_WIDTH_D2 && _x < WIN_WIDTH_D2 && _y >= -WIN_HEIGHT_D2 && _y < WIN_HEIGHT_D2) {
                    const u32 pstep = ((_x + WIN_WIDTH_D2) + ((_y + WIN_HEIGHT_D2) * WIN_WIDTH));
                    const u32 poffset = pstep * COLOR_BYTES_COUNT;
                    
                    u32* const px = (u32*)&pixels[poffset];
                    
                    if(_view && (!*px || (depth < zvalues[pstep]))) {
                        *px = 0xFF000000 | _view;
                        zvalues[pstep] = depth;
                    }
                }
            }
        }
    }
    
    static u64 getOffset(const int move, const int hrotate, const int vrotate) {
        return Options::HEADER_SIZE + WIN_BYTES_COUNT * move +
            (hrotate * Options::VERTICAL_POV_COUNT + vrotate) * SPACE_BYTES_COUNT;
    }
    
    static void getView() {
        readFrame(getOffset(_move, _hrotate, _vrotate));    
        if(Options::MAX_PROJECTION_DEPTH > 0.0f) {
            drawPixels();
        } else {
            memcpy(pixels, view, VIEW_BYTES_COUNT);
        }
    }
    
    void display() {
        controls();
        if(Options::FILTER_GAPS) {
            memset(fbuff, 0x00, WIN_BYTES_COUNT);
        }
        memset(pixels, 0x00, WIN_BYTES_COUNT);
        if(Options::MAX_PROJECTION_DEPTH > 0.0f) {
            memset(zvalues, 0x00, WIN_PIXELS_COUNT);
        }
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
        const u32 FRAME_COUNT = ((Options::SPACE_BLOCK_SIZE *
            Options::SPACE_BLOCK_COUNT) / Options::RAY_STEP);
        LAST_POSITION = FRAME_COUNT - 1;
        if(Options::MAX_PROJECTION_DEPTH > 0.0f) {
            PROJECTION_FACTOR = 1.0f / Options::MAX_PROJECTION_DEPTH;
        }
        ATOMIC_POV_STEP = 360.0f / Options::HORIZONTAL_POV_COUNT;
        WIN_WIDTH = WIN_HEIGHT = Options::SPACE_BLOCK_SIZE;
        WIN_WIDTH_D2 = WIN_WIDTH / 2;
        WIN_HEIGHT_D2 = WIN_HEIGHT / 2;
        WIN_WIDTH_D4 = WIN_WIDTH_D2 / 2;
        WIN_PIXELS_COUNT = WIN_WIDTH * WIN_HEIGHT;
        WIN_BYTES_COUNT = WIN_PIXELS_COUNT * COLOR_BYTES_COUNT;
        VIEW_BYTES_COUNT = WIN_PIXELS_COUNT * sizeof(u32);
        SPACE_BYTES_COUNT = FRAME_COUNT * VIEW_BYTES_COUNT;
    
        
        view = new u32[WIN_PIXELS_COUNT];
        if(Options::MAX_PROJECTION_DEPTH > 0.0f) {
            zvalues = new u8[WIN_PIXELS_COUNT];
        }
        memset(view, 0x00, sizeof(u32) * WIN_PIXELS_COUNT);
        
        if(Options::MAX_PROJECTION_DEPTH > 0.0f) {
            printf("Pre-calculations for projection...\n");
            preCalculate();
        }
        
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

//delete [] view
//delete [] zvalues
//delete [] _COORDINATES
