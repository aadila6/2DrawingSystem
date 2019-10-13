/*
    * Simple glut demo that can be used as a template for
    * other projects by sai kopparthi
    */

#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "tk.h"
#endif

#if defined(__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#else //linux
#include <GL/gl.h>
#include <GL/glut.h>
#endif

//other includes
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
//other files
#include "fileIO.cpp"
#include "rasterize.cpp"
//#include "PolygonBorderPixels.cpp"
#include "drawLine.cpp"
#include "transformation.cpp"
#include "clipping.cpp"
#include "vector.hpp"

/****set in main()****/
//the number of pixels in the grid
int grid_width;
int grid_height;

//the size of pixels sets the inital window height and width
//don't make the pixels too large or the screen size will be larger than
//your display size
float pixel_size;

/*Window information*/
int win_height;
int win_width;

void init();
void idle();
void display();
void draw_pix(int x, int y);
void reshape(int width, int height);
void key(unsigned char ch, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void check();

int main(int argc, char **argv)
{
    //the number of pixels in the grid
    grid_width = 100;
    grid_height = 100;

    //the size of pixels sets the inital window height and width
    //don't make the pixels too large or the screen size will be larger than
    //your display size
    pixel_size = 5;

    /*Window information*/
    win_height = grid_height * pixel_size;
    win_width = grid_width * pixel_size;

    /*Set up glut functions*/
    /** See https://www.opengl.org/resources/libraries/glut/spec3/spec3.html ***/

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    /*initialize variables, allocate memory, create buffers, etc. */
    //create window of size (win_width x win_height
    glutInitWindowSize(win_width, win_height);
    //windown title is "glut demo"
    glutCreateWindow("glut demo");

    /*defined glut callback functions*/
    glutDisplayFunc(display); //rendering calls here
    glutReshapeFunc(reshape); //update GL on window size change
    glutMouseFunc(mouse);     //mouse button events
    glutMotionFunc(motion);   //mouse movement events
    glutKeyboardFunc(key);    //Keyboard events
    glutIdleFunc(idle);       //Function called while program is sitting "idle"

    //initialize opengl variables
    init();
    //start glut event loop
    glutMainLoop();
    return 0;
}

/*initialize gl stufff*/
void init()
{
    //set clear color (Default background to white)
    glClearColor(1.0, 1.0, 1.0, 1.0);
    //checks for OpenGL errors
    check();
}

//called repeatedly when glut isn't doing anything else
void idle()
{
    //redraw the scene over and over again
    glutPostRedisplay();
}
struct Coordinate
{
    int x, y;
    Coordinate(int inputX, int inputY)
    {
        x = inputX;
        y = inputY;
    }
};

struct Polygon{
    //Center of mass is 0 respect to polygon itself
    vmml::vector<3, float> position;
    std::vector<vmml::vector<3, float>> vertices;
    Polygon(std::vector<vmml::vector<3, float>> vertices, 
            vmml::vector<3, float> pos){
        position = pos;
        this->vertices = vertices;
    }
    Polygon(std::vector <Coordinate> vert){
        float xtotal,ytotal;
        for(int i = 0; i<vert.size(); i++){
            xtotal +=vert[i].x;
            ytotal +=vert[i].y;
            vertices.push_back(
                vmml::vector<3, float>(vert[i].x,vert[i].y,1));
        }
        position = {xtotal/vert.size(),ytotal/vert.size(),1};
    }
    //Coordinates
};

struct PolygonBorderPixels{
        int count;
        std::vector<Coordinate> Coordinates;
        PolygonBorderPixels(){
            count = 0;
            
        }
        PolygonBorderPixels(int incount, std::vector<Coordinate> input) {
            count = incount;
            Coordinates = input;
        }
    };
void swapCor(Coordinate start, Coordinate end)
{
    int tempx = start.x;
    int tempy = start.y;
    start.x = end.x;
    start.y = end.y;
    end.x = tempx;
    end.y = tempy;
}
void readinput(char *filename){
    std::vector<PolygonBorderPixels> PolygonBorderPixelsList;
    std :: ifstream inputFile(filename);
    std :: string line;
    int count;
    inputFile >> count;
    getline(inputFile, line);
    getline(inputFile, line);
    for (int i=0; i< count; i++){
        int num;
        std::vector<Coordinate> coorList;
        inputFile >> num;
        getline(inputFile, line);
        for (int j=0; i<num; j++){
            float x, y;
            std :: string inputX, inputY;
            getline(inputFile, line);
            std :: istringstream record(line);
            getline(record, inputX, ' ');
            getline(record, inputY);
            x = stof(inputX);
            y = stof(inputX);
            Coordinate point(x,y);
            coorList.push_back(point);
        }
        PolygonBorderPixels PolygonBorderPixels(num,coorList);
        PolygonBorderPixelsList.push_back(PolygonBorderPixels);
        getline(inputFile, line);
    }
}
void drawLineDDA(Coordinate start, Coordinate end, bool* buffer )
{
    // Coordinate pixels[] = {start, end};
    //First figure out the direction
    int dx = -(start.x - end.x);
    int dy = -(start.y - end.y);
    float m = (float)dy / dx;
    float j = 0.0;
    float i = 0.0;

    if ((fabs(m)) < 1)
    { //Shallow, calculating Y
        if (m < 0)
        {
            if (start.y < end.y)
            {
                int tempx = start.x;
                int tempy = start.y;
                start.x = end.x;
                start.y = end.y;
                end.x = tempx;
                end.y = tempy;
            }
            for (int in = start.x; in > end.x; in--)
            {
                j = (in - start.x) * m + start.y;
                //draw_pix(in, (int)j);
                //Coordinate point(in,(int)j);
                buffer[in * grid_width + (int)j]=true;
            }
        }
        else if (m == 0 || dy == 0)
        {
            if (start.x > end.y)
            {
                int tempx = start.x;
                int tempy = start.y;
                start.x = end.x;
                start.y = end.y;
                end.x = tempx;
                end.y = tempy;
            }
            for (int inn = start.x; inn < end.x; inn++)
            {
                //j = (i-start.x)*m + start.y;
                //draw_pix(inn, end.y);
                //Coordinate point(inn,end.y);
                //vertice.push_back(point);
                buffer[inn*grid_width+(int)j]=true;
            }
        }
        else
        {
            if (start.y > end.y)
            {
                int tempx = start.x;
                int tempy = start.y;
                start.x = end.x;
                start.y = end.y;
                end.x = tempx;
                end.y = tempy;
            }
            for (int im = start.x; im < end.x; im++)
            {
                j = (im - start.x) * m + start.y;
                //draw_pix(im, (int)j);
                //Coordinate point(im,(int)j);
                //vertice.push_back(point);
                buffer[im* grid_width+(int)j]=true;
            }
        }
    }
    else
    { //Steep, calculating X
        m = (float)dx / dy;
        if (m < 0)
        {
            if (start.y < end.y)
            {
                int tempx = start.x;
                int tempy = start.y;
                start.x = end.x;
                start.y = end.y;
                end.x = tempx;
                end.y = tempy;
            }
            for (int jn = start.y; jn >= end.y; jn--)
            {
                i = start.x + (jn - start.y) * m;
                //draw_pix((int)i, jn);
                //Coordinate point(i,(int)jn);
                //vertice.push_back(point); 
                buffer[(int)i*grid_width+jn]=true;
            }
        }
        else
        {
            if (start.y > end.y){
                int tempx = start.x;
                int tempy = start.y;
                start.x = end.x;
                start.y = end.y;
                end.x = tempx;
                end.y = tempy;
            }
            for (int jm = start.y; jm < end.y; jm++)
            {
                i = start.x + (jm - start.y) * m;
                //draw_pix((int)i, jm);
                //Coordinate point((int)i, jm);
                buffer[(int)i*grid_width+jm]=true;
                //vertice.push_back(point); 
            }
        }
    }
}

void drawLineBresenham(Coordinate start, Coordinate end)
{
    
    if (start.y > end.y)
    {
        swapCor(start, end);
    }
    int dx = -(start.x - end.x);
    int dy = -(start.y - end.y);
    int m = dy / dx;
    int x = start.x;
    int y = start.y;
    //If the case is steep

    if (m > 0 && m < 1)
    {
        int p = 2 * dy - dx;
        while (y <= end.y)
        {
            if (p > 0)
            {
                x += 1;
                p = p + 2 * dy - 2 * dx;
                draw_pix(x, y);
            }
            else
            {
                x += 0;
                p = p + 2 * dy;
                draw_pix(x, y);
            }
            y += 1;
        }
    }
    else if (m > 1)
    { /* WORKS!!!*/
        int p = 2 * dx - dy;
        while (y <= end.y)
        {
            if (p > 0)
            {
                x += 1;
                p = p + 2 * dx - 2 * dy;
                draw_pix(x, y);
            }
            else
            {
                x += 0;
                p = p + 2 * dx;
                draw_pix(x, y);
            }
            y += 1;
        }
    }
    else if (m < 0 && m > -1)
    {
        int p = 2 * dx - dy;
        while (y <= end.y)
        {
            if (p > 0)
            {
                x -= 1;
                p = p + 2 * dx - 2 * dy;
                draw_pix(x, y);
            }
            else
            {
                x += 0;
                p = p + 2 * dx;
                draw_pix(x, y);
            }
            y += 1;
        }
    }
}

void rasterization(bool* buffer)
{
    for(int i=0;i<grid_width;i++){ //row num
       for(int j=0;j<grid_height;j++){ // col num
           if(buffer[i*grid_width+j] == true){
               draw_pix(i, (int)j);
               int numtrue = 0;
               for(int k=j; k<grid_width; k++){
                   if(buffer[i*grid_width+j] == true){
                       numtrue +=1;
                       draw_pix(i, (int)k);
                   }
               }
           }
       }
    }
}
//this is where we render the screen
void display()
{
    //clears the screen
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
    glLoadIdentity();

    //draws every other pixel on the screen
    /* 
    for (int j = 0; j < grid_height; j+=2){
        for (int i = 0; i < grid_width; i+=2){
            //this is the only "rendering call you should make in project 1"
            draw_pix(i,j);
        }
    }
    */
    //Testing drawing a triangle with user input

    int countVertex = 0;
    Coordinate start = {10, 10};
    Coordinate mid = {30, 80};
    Coordinate end = {90, 20};
    countVertex = 3;
    std:: vector <Coordinate> vertices;
    
    // vertices.push_back(start);
    // vertices.push_back(mid);
    // vertices.push_back(end);
    //Polygon poly(vertices);
    bool buffer[grid_width * grid_height];
    for(int i = 0; i < grid_width; i++){
       for(int j=0; j < grid_height; j++){
           (buffer[i*grid_width + j]) = false;
       }
    }
    drawLineDDA(start, mid, buffer);
    drawLineDDA(mid, end, buffer);
    drawLineDDA(start, end, buffer);
    Coordinate windowSize(win_width,win_height);
    //bordertoBuffer(vertices,&buffer);
    rasterization(buffer);
    // for(int i = 0; i < grid_width; i++){
    //    for(int j=0; j < grid_height; j++){
    //        //printf(buffer[i*grid_width+j]);
    //    }
    // }
    //PolygonBorderPixels polyone(border.size, border);

    // for(int i = 0; i< border.size; i++)
    // {

    //     //cout << border[i].x << " and " << border[i].y << endl;
    // }

    //draw_pix(50,30);
    // drawLineDDA(one,two);
    //drawLineDDA(start,mid);
    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
    check();
}

//Draws a single "pixel" given the current grid size
//don't change anything in this for project 1
void draw_pix(int x, int y)
{
    glBegin(GL_POINTS);
    glColor3f(.2, .2, 1.0);
    glVertex3f(x + .5, y + .5, 0);
    glEnd();
}

/*Gets called when display size changes, including initial craetion of the display*/
void reshape(int width, int height)
{
    /*set up projection matrix to define the view port*/
    //update the ne window width and height
    win_width = width;
    win_height = height;

    //creates a rendering area across the window
    glViewport(0, 0, width, height);
    // up an orthogonal projection matrix so that
    // the pixel space is mapped to the grid space
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, grid_width, 0, grid_height, -10, 10);

    //clear the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //set pixel size based on width, if the aspect ratio
    //changes this hack won't work as well
    pixel_size = width / (float)grid_width;

    //set pixel size relative to the grid cell size
    glPointSize(pixel_size);
    //check for opengl errors
    check();
}

//gets called when a key is pressed on the keyboard
void key(unsigned char ch, int x, int y)
{
    switch (ch)
    {
    default:
        //prints out which key the user hit
        printf("User hit the \"%c\" key\n", ch);
        break;
    }
    //redraw the scene after keyboard input
    glutPostRedisplay();
}


//gets called when a mouse button is pressed
void mouse(int button, int state, int x, int y)
{
    //print the pixel location, and the grid location
    printf("MOUSE AT PIXEL: %d %d, GRID: %d %d\n", x, y, (int)(x / pixel_size), (int)((win_height - y) / pixel_size));
    switch (button)
    {
    case GLUT_LEFT_BUTTON: //left button
        printf("LEFT ");
        break;
    case GLUT_RIGHT_BUTTON: //right button
        printf("RIGHT ");
    default:
        printf("UNKNOWN "); //any other mouse button
        break;
    }
    if (state != GLUT_DOWN) //button released
        printf("BUTTON UP\n");
    else
        printf("BUTTON DOWN\n"); //button clicked

    //redraw the scene after mouse click
    glutPostRedisplay();
}

//gets called when the curser moves accross the scene
void motion(int x, int y)
{
    //redraw the scene after mouse movement
    glutPostRedisplay();
}

//checks for any opengl errors in the previous calls and
//outputs if they are present
void check()
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("GLERROR: There was an error %s\n", gluErrorString(err));
        exit(1);
    }
}
