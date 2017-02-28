//
// This is a demo of vertex arrays (and normal arrays) using VBOs.
// It uses glDrawArrays to zap the entire image to the FB in one call.
//

#include <stdio.h>
#include <GL/gl.h>
#include <GL/glut.h>

struct point {
    float x;
    float y;
    float z;
};

void setup_the_viewvol() {
    struct point eye;
    struct point view;
    struct point up;

    glEnable(GL_DEPTH_TEST);

    // Specify the size and shape of the view volume.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,1.0,0.1,20.0);

    // Specify the position for the view volume.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    eye.x = 0.05;
    eye.y = 0.3;
    eye.z = 0.3;
    view.x = -0.02;
    view.y = 0.1;
    view.z = 0.0;
    up.x = 0.0;
    up.y = 1.0;
    up.z = 0.0;

    gluLookAt(eye.x,eye.y,eye.z,view.x,view.y,view.z,up.x,up.y,up.z);
}

void draw_stuff() {
    FILE *fp = fopen("bunnyN.ply","r");

    char* line = NULL;
    size_t len =  0;
    char* word = NULL;
    int vertex = 0;
    int face = 0;   
    do {
        getline(&line, &len, fp);
        word = strtok(line, " ");
        if (!strcmp(word, "element")) {
            word = strtok(NULL, " ");
            if (!strcmp(word, "vertex")) {
                word = strtok(NULL, " ");
                vertex = atoi(word);
            } else if (!strcmp(word, "face")) {
                word = strtok(NULL, " ");
                face = atoi(word);
            }
        }
    } while (strncmp(line, "end_header", 10) != 0);

    int i;
    GLfloat vertex_array[vertex*6];
    for (i = 0; i < vertex; i++) {
        getline(&line, &len, fp);

        word = strtok(line, " ");
        vertex_array[i*6 + 0] = atof(word);
        word = strtok(NULL, " ");
        vertex_array[i*6 + 1] = atof(word);
        word = strtok(NULL, " ");
        vertex_array[i*6 + 2] = atof(word);
        word = strtok(NULL, " ");
        vertex_array[i*6 + 3] = atof(word);
        word = strtok(NULL, " ");
        vertex_array[i*6 + 4] = atof(word);
        word = strtok(NULL, " ");
        vertex_array[i*6 + 5] = atof(word);
    }

    GLuint face_array[face*3];
    for (i = 0; i < face; i++) {
        getline(&line, &len, fp);
        
        word = strtok(line, " ");
        word = strtok(NULL, " ");
        face_array[i*3 + 0] = atoi(word);
        word = strtok(NULL, " ");
        face_array[i*3 + 1] = atoi(word);
        word = strtok(NULL, " ");
        face_array[i*3 + 2] = atoi(word);
    }

    glBindBuffer(GL_ARRAY_BUFFER,1);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertex_array),vertex_array,GL_STATIC_DRAW);

    glVertexPointer(3,GL_FLOAT,6*sizeof(GLfloat),NULL+0);
    glNormalPointer(GL_FLOAT,6*sizeof(GLfloat),NULL+3*sizeof(GLfloat));
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glClearColor(0.35,0.35,0.35,0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES,face*3,GL_UNSIGNED_INT,face_array);
    glutSwapBuffers();
}

void do_lights() {
    // This is a white light.
    float light0_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
    float light0_diffuse[] = { 1.0, 1.0, 1.0, 0.0 };
    float light0_specular[] = { 1.0, 1.0, 1.0, 0.0 };
    float light0_position[] = { 1.5, 2.0, 2.0, 1.0 };
    float light0_direction[] = { -1.5, -2.0, -2.0, 1.0};

    // Turn off scene default ambient.
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light0_ambient);

    // Make specular correct for spots.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);

    glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);
    glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,1.0);
    glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,180.0);
    glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,0.5);
    glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.1);
    glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.01);
    glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
    glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_direction);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void do_material() {
    float mat_ambient[] = {0.0,0.0,0.0,1.0};
    float mat_diffuse[] = {0.9,0.9,0.1,1.0};
    float mat_specular[] = {1.0,1.0,1.0,1.0};
    float mat_shininess[] = {2.0};

    glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
    glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
    glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
}

void getout(unsigned char key, int x, int y) {
    switch(key) {
    case 'q':
        exit(1);
    default:
        break;
    }
}

int main(int argc, char **argv) {
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
    glutInitWindowSize(768,768);
    glutInitWindowPosition(100,50);
    glutCreateWindow("my_cool_cube");
    setup_the_viewvol();
    do_lights();
    do_material();
    glutDisplayFunc(draw_stuff);
//    glutIdleFunc(update);
    glutKeyboardFunc(getout);
    glutMainLoop();
    return 0;
}
