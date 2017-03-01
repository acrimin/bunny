#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>


char *read_shader_program(char *filename) {
    FILE *fp;
    char *content = NULL;
    int fd, count;
    fd = open(filename,O_RDONLY);
    count = lseek(fd,0,SEEK_END);
    close(fd);
    content = (char *)calloc(1,(count+1));
    fp = fopen(filename,"r");
    count = fread(content,sizeof(char),count,fp);
    content[count] = '\0';
    fclose(fp);
    return content;
}

void set_light() {
    float light0_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    float light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    float light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    float light0_position[] = { 0.22, 0.55, 0.6, 1.0 };
    float light0_direction[] = { -0.22, -0.35, -0.5, 1.0};

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

void set_material() {
    float mat_diffuse[] = {0.5,0.4,0.0,1.0};
    float mat_specular[] = {0.7,0.5,0.0,1.0};
    float mat_shininess[] = {20.0};

    glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
    glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
    glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
}

float eye[] = {0.05,0.5,1.0};
float viewpt[] = {-0.02,0.1,0.0};
float up[] = {0.0,1.0,0.0};

void view_volume() {

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,1.0,1.0,20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[0],eye[1],eye[2],viewpt[0],viewpt[1],viewpt[2],up[0],up[1],up[2]);
}

void draw_bunny() {
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

    glDrawElements(GL_TRIANGLES,face*3,GL_UNSIGNED_INT,face_array);
}

void renderScene(void) {
    glClearColor(0.3,0.3,0.3,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(eye[0],eye[1],eye[2],
        viewpt[0],viewpt[1],viewpt[2],
        up[0],up[1],up[2]);

    draw_bunny();
    glutSwapBuffers();
}

unsigned int set_shaders() {
    GLint vertCompiled, fragCompiled;
    char *vs, *fs;
    GLuint v, f, p;
    int result = -1;

    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
    vs = read_shader_program("bunny.vert");
    fs = read_shader_program("bunny.frag");
    glShaderSource(v,1,(const char **)&vs,NULL);
    glShaderSource(f,1,(const char **)&fs,NULL);
    free(vs);
    free(fs); 
    glCompileShader(v);
    glCompileShader(f);
    glGetShaderiv(f,GL_COMPILE_STATUS,&result);
    fprintf(stderr,"%d\n",result);
    p = glCreateProgram();
    glAttachShader(p,f);
    glAttachShader(p,v);
    glLinkProgram(p);
    glUseProgram(p);
    return(p);
}

void controller(unsigned char key, int x, int y) {
    // angle of rotation for the camera direction
    float scale=0.2;
    float look[] = {eye[0] - viewpt[0],
                    eye[1] - viewpt[1],
                    eye[2] - viewpt[2]
    };
    float dis = sqrt(look[0]*look[0]+
                     look[1]*look[1]+
                     look[2]*look[2]);
    float norm_view[] = {look[0]/dis, look[1]/dis, look[2]/dis};
    printf("%f %f %f\n", norm_view[0], norm_view[1], norm_view[2]);
    switch(key) {
        case 'q':
            exit(1);
        case 'w':
            eye[0] -= norm_view[0] * scale;
            eye[1] -= norm_view[1] * scale;
            eye[2] -= norm_view[2] * scale;
            break;
        case 's':
            eye[0] += norm_view[0] * scale;
            eye[1] += norm_view[1] * scale;
            eye[2] += norm_view[2] * scale;
            break;
        case 'a':
            break;
        case 'd':
            break;
        default:
            break;
    }
    renderScene();
}

int main(int argc, char **argv) {
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(768,768);
    glutCreateWindow("bunny");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE_ARB);
    view_volume();
    set_light();
    set_material();
    set_shaders();
    glutDisplayFunc(renderScene);
    glutKeyboardFunc(controller);
    glutMainLoop();
    return 0;
}