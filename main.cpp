#include "./headers/render.hpp"

void display() {
    render::display();
}

void reshape(int width, int height) {
    render::reshape(width, height);
}

void specialKeyDown(int key, int x, int y) {
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

void specialKeyUp(int key, int x, int y) {
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

void timer(int value) {
    render::move(0, false);
    render::rotate(0, false);
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
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeyDown);
    glutSpecialUpFunc(specialKeyUp);
    timer(0);
    glutMainLoop();
    return 0;
}