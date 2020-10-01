#include "./headers/render.hpp"

#ifdef PSP
    static int key(unsigned int args, void *argp) {
        while(true) {
            SceCtrlData pad;
            sceCtrlReadBufferPositive(&pad, 1);
            
            if(pad.Buttons & PSP_CTRL_LEFT) {
                render::rotate(-1);
            }
            if(pad.Buttons & PSP_CTRL_RIGHT) {
                render::rotate(1);
            }
            if(pad.Buttons & PSP_CTRL_UP) {
                render::move(1);
            }
            if(pad.Buttons & PSP_CTRL_DOWN) {
                render::move(-1);
            }
            if(!pad.Buttons) {
                render::move(0);
                render::rotate(0);
            }
            sceKernelDelayThread(10000);
        }
    }
#else
    static void specialKeyDown(int key, int x, int y) {
        switch (key) {
            case GLUT_KEY_DOWN:
                render::move(-1);
                break;
            case GLUT_KEY_UP:
                render::move(1);
                break;
            case GLUT_KEY_LEFT:
                render::rotate(-1);
                break;
            case GLUT_KEY_RIGHT:
                render::rotate(1);
                break;
        }
    }

    static void specialKeyUp(int key, int x, int y) {
        switch (key) {    
            case GLUT_KEY_UP:
                render::move(0);
                break;
            case GLUT_KEY_DOWN:
                render::move(0);
                break;
            case GLUT_KEY_LEFT:
                render::rotate(0);
                break;
            case GLUT_KEY_RIGHT:
                render::rotate(0);
                break;
        }
    }
#endif

static void redisplay() {
    render::move(0, false);
    render::rotate(0, false);
	glutPostRedisplay();
}

static void timer(int value) {
    redisplay();
    glutTimerFunc(1000/60, timer, value);
}

static void display() {
    render::display();
    #ifdef PSP
        redisplay();
    #endif
}

static void reshape(int width, int height) {
    render::reshape(width, height);
}

int main(int argc, char** argv) {
    Options::init(argc, argv);
    render::init();
    
    glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(render::_win_width(), render::_win_height());
    glutCreateWindow("apov-raw-navigator");
    render::initGl();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    #ifndef PSP
        glutSpecialFunc(specialKeyDown);
        glutSpecialUpFunc(specialKeyUp);
        timer(0);
    #else
        scePowerSetClockFrequency(333, 333, 167);
        SceUID id = sceKernelCreateThread("apov_key", key, 0x10, 0x1000, 0, 0);
        if (id >= 0){
            sceKernelStartThread(id, 0, 0);
        }
    #endif
    glutMainLoop();
    return 0;
}