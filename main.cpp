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
            render::moveZ(-1);
            break;
        case GLUT_KEY_LEFT:
            break;
	    case GLUT_KEY_UP:
            render::moveZ(1);
            break;
	    case GLUT_KEY_RIGHT:
            break;
    }
}

void specialKeyUp(int key, int x, int y) {
    switch (key) {    
	    case GLUT_KEY_UP:
            render::moveZ(0);
            break;
	    case GLUT_KEY_DOWN:
            render::moveZ(0);
            break;
	    case GLUT_KEY_LEFT:
            break;
	    case GLUT_KEY_RIGHT:
            break;
    }
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow("apov-cross-reader");
    render::init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeyDown);
    glutSpecialUpFunc(specialKeyUp);
    glutMainLoop();
    return 0;
}