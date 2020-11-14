#include "./headers/render.hpp"
    
static void specialKeyDown(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_DOWN:
            render::vstep = 1;
            break;
        case GLUT_KEY_UP:
            render::vstep = -1;
            break;
        case GLUT_KEY_LEFT:
            render::hstep = 1;
            break;
        case GLUT_KEY_RIGHT:
            render::hstep = -1;
            break;
    }
}

static void specialKeyUp(int key, int x, int y) {
    switch(key) {    
        case GLUT_KEY_DOWN:
            render::vstep = 0;
            break;
        case GLUT_KEY_UP:
            render::vstep = 0;    
            break;
        case GLUT_KEY_LEFT:
            render::hstep = 0;
            break;
        case GLUT_KEY_RIGHT:
            render::hstep = 0;
            break;
    }
}

void keyDown(unsigned char key, int x, int y) {
    switch(key) {
        case 'z':
            render::mstep = 1;
            break;
        case 's':
            render::mstep = -1;
            break;
    }
}

void keyUp(unsigned char key, int x, int y) {
    switch(key) {
        case 'z':
            render::mstep = 0;
            break;
        case 's':
            render::mstep = 0;
            break;
    }
}

static void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(1000/60, timer, value);
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
    glutDisplayFunc(render::display);
    glutReshapeFunc(render::reshape);
    glutSpecialFunc(specialKeyDown);
    glutSpecialUpFunc(specialKeyUp);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    timer(0);
    glutMainLoop();
    return 0;
}