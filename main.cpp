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

#include "vector.hpp"
#include "matrix.hpp"

typedef int OutCode;
constexpr int INSIDE = 0; // 0000
constexpr int LEFT = 1;   // 0001
constexpr int RIGHT = 2;  // 0010
constexpr int BOTTOM = 4; // 0100
constexpr int TOP = 8;    // 1000
/****set in main()****/
//the number of pixels in the grid
int grid_width;
int grid_height;

int xMin;
int xMax;
int yMin;
int yMax;


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


typedef struct Coordinate
{
    float x, y;
} Coordinate;

struct Polygon{
    //Center of mass is 0 respect to polygon itself
    //But position vector is the centroid from the viewPort
    int count;
    vmml::vector<3, float> position;
    Coordinate transVec;
    float angle;
    float scale;
    std::vector<vmml::vector<3, float>> vertices;
    
    // Polygon(std::vector<vmml::vector<3, float>> vertices, 
    //         vmml::vector<3, float> pos){
    //     position = pos;
    //     this->vertices = vertices;
    //     count = vertices.size();
    //     angle = 0.0;
    //     scale = 1.0;
    // }

    Polygon(std::vector<Coordinate> vert)
    {
        float xtotal = 0, ytotal = 0;
        this->count = vert.size();
        
        for(int i = 0; i < vert.size(); i++){
            vertices.push_back(
                vmml::vector<3, float>(vert[i].x, vert[i].y, 1));
        }
        
        updateCentroid();

        // for (int i = 0; i < vert.size(); i++){
        //     //std::cout << vertices[i] << std::endl;
        //     vertices[i] -= position;
        //     vertices[i].z() = 1.0f;
        // }

        transVec.x = 0.0;
        transVec.y = 0.0;
        angle = 0.0;
        scale = 1.0;
    }

    void printPolygon() {
        std::cout<<"numVertices: "<<this->count<<std::endl;
        for(int i = 0; i < this->count; i++) {
            std::cout<<"x: "<<(this->vertices[i])[0]<<" y: "<<(this->vertices[i])[1]<<std::endl;
        }
        std::cout<<std::endl;
    }

    void updateCentroid() {
        float xtotal = 0, ytotal = 0;
        for(int i = 0; i < this->count; i++){
            xtotal += this->vertices[i].x();
            ytotal += this->vertices[i].y();        
        }
        
        this->position = {xtotal/(float)(this->count), ytotal/(float)(this->count), 1};
    }
};

/*
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
    };*/

void readinput(char *filename, std::vector<Polygon> &polygons);
void writeFile(char *filename, std::vector<Polygon> &polygons);
void drawLineDDA(vmml::vector<3, float> start, vmml::vector<3, float> end, bool* buffer);
void drawLineBresenham(Coordinate start, Coordinate end, bool* buffer );
vmml::vector<3, float> ComputeIntersection(vmml::vector<3, float> a, vmml::vector<3, float> b);
OutCode computeoutbound(vmml::vector<3, float> point);
void clipping(Polygon polygon, Polygon poly);
void rasterization(bool* buffer);
bool* buffer;
bool* loadBuffer;
std::vector<Polygon> polygonList;
std::vector<Polygon> cPolygonList;
void translation(Coordinate transl, Polygon &poly);
void rotation(float angle, Polygon &poly);
void scaling(float scal, Polygon &poly);
//void applyTransform(Polygon &polygon);
char lineMode;
bool rasterswitch;
float angle;

float scale;
int iD;
float translationX, translationY,
sFactor, cliponeX,cliponeY, cliptwoX, cliptwoY;

//poly clipping
void polyClip(Polygon &poly);
void polyClipLeft(Polygon &poly);
void polyClipRight(Polygon &poly); 
void polyClipBottom(Polygon &poly);
void polyClipTop(Polygon &poly);
void exchangeV(vmml::vector<3, float> &vA, vmml::vector<3, float> &vB);  
void copyList(std::vector<vmml::vector<3, float>> &s, std::vector<vmml::vector<3, float>> &t, int n);
void copyVertex(vmml::vector<3, float> &s, vmml::vector<3, float> &t);
int main(int argc, char **argv)
{
   

    //the size of pixels sets the inital window height and width
    //don't make the pixels too large or the screen size will be larger than
    //your display size
    pixel_size = 5;

    /*Window information*/
    // win_height = grid_height * pixel_size;
    // win_width = grid_width * pixel_size;

    /*Set up glut functions*/
    /** See https://www.opengl.org/resources/libraries/glut/spec3/spec3.html ***/
    
    grid_width = 100;
    grid_height = 100;

    xMin = 0;
    xMax = grid_width;
    yMin = 0;
    yMax = grid_height;
    int iD=0;
    translationX = 0; 
    translationY=0;
    sFactor=1;
    lineMode = 'd';
    rasterswitch = false;

    // std::cout << "Please enter width of window: " ;
    // std::cin>> grid_width;
    // std::cout << "Please enter height of window: ";
    // std::cin>> grid_height;
    win_height = grid_height * pixel_size;
    win_width = grid_width * pixel_size;
    buffer = new bool[grid_height* grid_width];
    loadBuffer = new bool[grid_height* grid_width];
    for(int i = 0; i < grid_width; i++){
       for(int j=0; j < grid_height; j++){
           (buffer[i*grid_width + j]) = false;
           (loadBuffer[i*grid_width + j]) = false;
       }
    }
    readinput("testScene.txt", polygonList);
    readinput("testScene.txt",  cPolygonList);
    

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    /*initialize variables, allocate memory, create buffers, etc. */
    //create window of size (win_width x win_height
    glutInitWindowSize(win_width, win_height);
    //windown title is "glut demo"
    glutCreateWindow("Two Dimentional Drawing");
   
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
    iD=0;
    translationX = 0; 
    translationY=0;
    sFactor=1;
        int choice;
        char line;
        std::cout << "Please enter 'b' for Bresemham, 'd' for DDA: "; 
        std::cin>>line;
        lineMode = line;
        std::cout << "1. Rotation \n";  
        std::cout << "2. Translation\n";  
        std::cout << "3. Scalling \n";  
        std::cout << "4. Clipping \n";
        std::cout << "5. Exit \n";
        std::cout << "Please select one of options above for your operation: ";
        std::cin>> choice;
        switch (choice) 
        { 
            case 1:  
                std::cout << "Please enter rotation angle: ";
                std::cin>> angle;
                std::cout << "Please enter Polygon ID such as 0,1,2.. for you operation: ";
                std::cin>> iD;
                break;
            case 2:
                std::cout << "Please enter translation in x and y direction: ";
                std::cin>> translationX >> translationY;
                std::cout << "Please enter Polygon ID such as 0,1,2 for you operation: ";
                std::cin>> iD;
                break; 
            case 3:  
                std::cout << "Please enter scalling factor: " ;
                std::cin>> sFactor;
                std::cout << "Please enter Polygon ID such as 0,1,2.. for you operation: ";
                std::cin>> iD;
                break; 
            case 4:  
                std::cout << "Please enter first clipping coordinates seperated by a space: ";
                std::cin>> cliponeX >> cliponeY;
                std::cout << "Please enter the second clipping coordinates seperated by a space: " ;
                std::cin>> cliptwoX >> cliptwoY;
                std::cout << "Please enter Polygon ID such as 0,1,2.. for you operation: ";
                std::cin >> iD;
                break; 
            case 5: 
                writeFile("testScene.txt", polygonList);
                exit(0);
                break; 
            default:  
                break;
        }
}

void swapCor(Coordinate start, Coordinate end)
{
    int tempx = start.x;
    int tempy = start.y;
    start.x = end.x;
    start.y = end.y;
    end.x = tempx;
    end.y = tempy;
}
void readinput(char *filename, std::vector<Polygon> &polygons){
    
    std::ifstream inputFile;
    inputFile.open(filename);
    std::string line;
    int count;
    inputFile >> count;
    getline(inputFile, line);
    getline(inputFile, line);
    for (int i=0; i< count; i++){
        int num;
        std::vector <Coordinate> vertices;
        inputFile >> num;
        getline(inputFile, line);
        for (int j=0; j<num; j++){
            float x, y;
            std :: string inputX, inputY;
            getline(inputFile, line);
            std :: istringstream record(line);
            getline(record, inputX, ' ');
            getline(record, inputY);
            x = std::stof(inputX);
            y = std::stof(inputY);
            Coordinate point = {.x = x, .y = y};
            vertices.push_back(point);
        }
        Polygon polygon(vertices);
        polygons.push_back(polygon);
        getline(inputFile, line);
        }
        inputFile.close();
}
void writeFile(char *filename,std::vector<Polygon> &polygons){
    std::ofstream outputFile(filename);
    outputFile << polygons.size() << "\n\n";
    for (int i = 0; i<polygons.size();i++) {
        outputFile << polygons[i].count << std::endl;
        for(int j = 0; j<polygons[i].count;j++) {
            outputFile << polygons[i].vertices[j].x() << ' ' << polygons[i].vertices[j].y() << std::endl;
        }
        outputFile << std::endl;
    }
}
void drawLineDDA(vmml::vector<3, float> start, vmml::vector<3, float> end, bool* buffer )
{
    // Coordinate pixels[] = {start, end};
    //First figure out the direction
    int dx = -(start.x() - end.x());
    int dy = -(start.y() - end.y());
    float m = (float)dy / dx;
    float j = 0.0;
    float i = 0.0;

    if ((fabs(m)) < 1)
    { //Shallow, calculating Y
        if (m < 0)
        {
            if (start.y() > end.y())
            {
                int tempx = start.x();
                int tempy = start.y();
                start.x() = end.x();
                start.y() = end.y();
                end.x() = tempx;
                end.y() = tempy;
            }
            for (int in = start.x(); in > end.x(); in--)
            {
                j = (in - start.x()) * m + start.y();
                draw_pix(in, (int)j);
                //Coordinate point(in,(int)j);
                buffer[(int)j * grid_width + in]=true;
            }
        }
        else if (m == 0 )
        {
            if (start.x() > end.x())
            {
                int tempx = start.x();
                int tempy = start.y();
                start.x() = end.x();
                start.y() = end.y();
                end.x() = tempx;
                end.y() = tempy;
            }
            for (int inn = start.x(); inn < end.x(); inn++)
            {
                //j = (i-start.x)*m + start.y;
                draw_pix(inn, end.y());
                //Coordinate point(inn,end.y);
                //vertice.push_back(point);
                buffer[(int)end.y()*grid_width+(int)inn]=true;
            }
        }
        else
        {
            if (start.y() > end.y())
            {
                int tempx = start.x();
                int tempy = start.y();
                start.x() = end.x();
                start.y() = end.y();
                end.x() = tempx;
                end.y() = tempy;
            }
            for (int im = start.x(); im < end.x(); im++)
            {
                j = (im - start.x()) * m + start.y();
                draw_pix(im, (int)j);
                //Coordinate point(im,(int)j);
                //vertice.push_back(point);
                buffer[(int)j* grid_width+im]=true;
            }
        }
    }
    else
    { //Steep, calculating X
        m = (float)dx / dy;
        if (m < 0)
        {
            if (start.y() < end.y())
            {
                int tempx = start.x();
                int tempy = start.y();
                start.x() = end.x();
                start.y() = end.y();
                end.x() = tempx;
                end.y() = tempy;
            }
            for (int jn = start.y(); jn >= end.y(); jn--)
            {
                i = start.x() + (jn - start.y()) * m;
                draw_pix((int)i, jn);
                //Coordinate point(i,(int)jn);
                //vertice.push_back(point); 
                buffer[jn*grid_width+(int)i]=true;
            }
        }
        else
        {
            if (start.y() > end.y()){
                int tempx = start.x();
                int tempy = start.y();
                start.x() = end.x();
                start.y() = end.y();
                end.x() = tempx;
                end.y() = tempy;
            }
            for (int jm = start.y(); jm < end.y(); jm++)
            {
                int ii = (int)(start.x() + (jm - start.y()) * m);
                draw_pix(ii, jm);
                //Coordinate point((int)i, jm);
                buffer[jm*grid_width + ii] = true;
                //vertice.push_back(point); 
            }
        }
    }
}



//Algorithm from class notes & textbook 
void drawLineBresenham(vmml::vector<3, float> start, vmml::vector<3, float> end, bool* buffer)
{
    float m = (end.y() - start.y()) / (end.x() - start.x());
    int x = 0, y = 0;
    if(m == 1){
        if(start.x()<end.x()){
            y = start.x();
        }else{
            y = end.x();
        }
        for (int x = fmin(round(start.x()), round(end.x())); x <= fmax(round(start.x()), round(end.x())); x++) {
            //Coordinate point(x, y++);
            buffer[y*grid_width+x]=true;
        }
    }else if(m == -1){
        int y;
        for (int x = fmin(round(start.x()), round(end.x())), y = fmax(round(start.y()), round(end.y())); x <= fmax(round(start.x()), round(end.x())); x++, y--) {
            //Coordinate point(x, y);
            buffer[y*grid_width+x]=true;
        }

    }
    if (fabs(m) < 1) {
        int dx = fabs(end.x() - start.x()),
            dy = fabs(end.y() - start.y()),
            p = 2 * dy - dx;
        if (start.x() > end.x())
        {
            x = end.x();
            y = end.y();
            end.x() = start.x();
        }else{
            x = start.x();
            y = start.y();
        }
        draw_pix(x, y);
        /*if (y*grid_width + x >= grid_width*grid_height)
        {
            std::cout << "oops, (" << start.x() << "," << start.y() << ") - (" << end.x() << "," << end.y() << ")\n" << std::flush;
        }*/
        //std::cerr << "(" << y << "," << x << ")\n";
        //buffer[y*grid_width+x]=true;
        while (x < end.x())
        {
            x++;
            if (p < 0)
            {
                p = p + 2 * dy;
            }
            else
            {
                if (m> 0) {y++;} else {y--;}
                p = p + 2 * dy - 2 * dx;
            }
            draw_pix(x, y);
            buffer[y*grid_width+x]=true;
        }
    } else if (fabs(m) >= 1) {                    
        int dx = fabs(end.x() - start.x()),
            dy = fabs(end.y() - start.y()),
            p = 2 * dx - dy;
        if (start.y() > end.y())
        {
            x = end.x();
            y = end.y();
            end.y() = start.y();
        }
        else
        {
            x = start.x();
            y = start.y();
        }
        while (y < end.y())
        {
            y++;
            if (p < 0)
            {
                p = p + (2 * dx);
            }else{
                if(m>0){
                    x++;
                }else{
                    x--;
                }
                p = p + (2 * dx) - (2 * dy);
            }
             draw_pix(x, y);
             buffer[y*grid_width+x]=true;
        }
    } 
}
OutCode computeoutbound(vmml::vector<3, float> point){
    OutCode code;
    code = INSIDE;
    if (point.x() < 0)           // to the left of clip window
		code |= LEFT;
	else if (point.x() > grid_width)      // to the right of clip window
		code |= RIGHT;
	if (point.y() < 0)           // below the clip window
		code |= BOTTOM;
	else if (point.y() > grid_height)      // above the clip window
		code |= TOP;
	return code;
}
vmml::vector<3, float> ComputeIntersection(vmml::vector<3, float> a, vmml::vector<3, float> b){
    OutCode one = computeoutbound(a);
    OutCode two = computeoutbound(b);
    bool accept = false;
    if(!(one|two)){
        accept = true;
    }else if(one & two){
        ;
    }else{
        double x = 0, y = 0;
        OutCode outcodeOut = one ? one : two;
        if (outcodeOut & TOP) {           // point is above the clip window
            x = a.x() + (b.x() - a.x()) * (grid_height - a.y()) / (b.y() - a.y());
            y = grid_height;
        } else if (outcodeOut & BOTTOM) { // point is below the clip window
            x = a.x() + (b.x() - a.x()) * (0 - a.y()) / (b.y() - a.y());
            y = 0;
        } else if (outcodeOut & RIGHT) {  // point is to the right of clip window
            y = a.y() + (b.y() - a.y()) * (grid_width - a.x()) / (b.x() - a.x());
            x = grid_width;
        } else if (outcodeOut & LEFT) {   // point is to the left of clip window
            y = a.y() + (b.y() - a.y()) * (0 - a.x()) / (b.x() - a.x());
            x = 0;
        }
       return vmml::vector<3, float>(x, y, 1);
    }
}

void clipping(Polygon polygon, Polygon polybase){
    polygon.vertices.clear();
    for(int i=0; i < polybase.vertices.size(); i++){
        vmml::vector<3, float> currentVert = polybase.vertices[i];
        vmml::vector<3, float> prevVert = polybase.vertices[(i+polybase.vertices.size()-1)%polybase.vertices.size()];
        vmml::vector<3, float> intersect = ComputeIntersection(prevVert,currentVert);

        if((computeoutbound(currentVert)) == INSIDE){
            if((computeoutbound(prevVert)) != INSIDE){
                polygon.vertices.push_back(intersect);
            }
            polygon.vertices.push_back(currentVert);

        }else if((computeoutbound(prevVert)) == INSIDE){
            polygon.vertices.push_back(intersect);
        }else{
            polygon.vertices.push_back(currentVert);
        }
    }
}
   
void rasterization(bool* buffer)
{
   
    //Find first intecet & last. And filling between.
    for(int j = 0; j<grid_height;j++){
        int xbegin, xend;
        for(xbegin = 0; xbegin < grid_width; xbegin++ ){
            int index = j*grid_width+xbegin;
            if(buffer[index] == true){
                break;
            }
        }
        for(xend = grid_width-1; xend >= 0; xend--){
            int index = j*grid_width+xend;
            if(buffer[index] == true){
                break;
            }
        }
        bool toggle = false;
        bool nextTrue = false;
        for(int i = xbegin; i <=xend+1; i++){
            if(i==xend+1){
                toggle = false;
                //break;
            }else if(xbegin == xend){
                draw_pix(i, j);
                break;
            }
            if(buffer[j*grid_width+i] == true){
                if(buffer[j*grid_width+i+1]==true){
                    nextTrue = true;
                }else if(!nextTrue){
                    toggle = (!toggle);
                }
            }
            if(toggle){
            draw_pix(i, j);
            }
        }
    }
}
void translation(Coordinate transl, Polygon &poly){
    
    for(int i = 0; i < poly.count; i++) {
        vmml::vector<3, float> temp = poly.vertices[i];
        temp[0] = temp[0] + transl.x;
        temp[0] = temp[0] + transl.y;
        poly.vertices[i] = temp;
    } 

    poly.updateCentroid();

}
void rotation(float angle, Polygon &poly){
    
    Coordinate trans2Ori = {.x = -poly.position[0], .y = -poly.position[1]};
    Coordinate trans2Cen = {.x = poly.position[0], .y = poly.position[1]};
    float rotateAngleInRad = (angle / 180.0) * 3.14;
    float cosA = cos(rotateAngleInRad);
    float sinA = sin(rotateAngleInRad);

    translation(trans2Ori, poly);
    
    for(int i = 0; i < poly.count; i++) {
        float currX = poly.vertices[i].x();
        float currY = poly.vertices[i].y();

        float nextX = currX * cosA + currY * (-sinA);
        float nextY = currX * sinA + currY * cosA;
        
        vmml::vector<3, float> temp = {nextX,nextY,1} ;
        poly.vertices[i] = temp;
        
    }

    

    translation(trans2Cen, poly);    
}
void scaling(float scal, Polygon &poly){

    Coordinate trans2Ori = {.x = -poly.position[0], .y = -poly.position[1]};
    Coordinate trans2Cen = {.x = poly.position[0], .y = poly.position[1]};

    translation(trans2Ori, poly);
    for(int i = 0; i < poly.count; i++) {
        
        float currX = poly.vertices[i].x();
        float currY = poly.vertices[i].y();
        //std::cout<<"currx: "<<currX<<" currY: "<<currY<<std::endl;
        float nextX = currX * scal;
        float nextY = currY * scal;
        //std::cout<<"nextx: "<<nextX<<" nextY: "<<nextY<<std::endl;

        poly.vertices[i][0] = nextX;
        poly.vertices[i][1] = nextY;
    }

    translation(trans2Cen, poly);  
}

// void applyTransform(Polygon &polygon){
//     vmml::matrix<3,3> transl_M;
//     vmml::matrix<3,3> rotate_M;
//     vmml::matrix<3,3> scale_M;
//     transl_M(0,0) = 1.0f;
//     //transl_M(0,1) = 0.0f;
//     transl_M(0,2) = polygon.position.x();
//     //transl_M(1,0) = 0.0f;
//     transl_M(1,1) = 1.0f;
//     transl_M(1,2) = polygon.position.y();
//     transl_M(2,2) = 1.0f;
//     //transl_M(2,1) = 0.0f;
//     //transl_M(2,0) = 0.0f;
//     rotate_M(0,0) = cos(polygon.angle);
//     rotate_M(0,1) = -sin(polygon.angle);
//     //rotate_M(0,2) = 0;
//     rotate_M(1,0) = sin(polygon.angle);
//     rotate_M(1,1) = cos(polygon.angle);
//     //rotate_M(1,2) = 0;
//     //rotate_M(2,0) = 0;
//     //rotate_M(2,1) = 0;
//     rotate_M(2,2) = 1.0f;
//     scale_M(0,0) = polygon.scale;
//     scale_M(1,1) = polygon.scale;
//     scale_M(2,2) = 1;
//     vmml::matrix<3,3> changes = transl_M*scale_M*rotate_M;
//     for(int i = 0; i< polygon.vertices.size(); i++){
//         polygon.vertices[i] = changes * polygon.vertices[i];
//     }
// }

//this is where we render the screen
void display()
{
    //clears the screen
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
    glLoadIdentity();

    for(int u=0;u<polygonList.size();u++){
        cPolygonList[u].vertices.clear();
        for(int v=0;v<polygonList[u].count;v++){
            for(int j = 0; j< polygonList[u].vertices.size();j++){
                cPolygonList[u].vertices.push_back(polygonList[u].vertices[j]);
            }
        }
        cPolygonList[u].count = polygonList[u].count;
    }

    // for(int n = 0; n < polygonList.size(); n++){
    //     for(int j=0; j<polygonList[n].vertices.size(); j++){
    //         //std::cout<<"Copy: "<<cPolygonList[n].vertices[j];
    //         //std::cout<<"Orginal: "<<polygonList[n].vertices[j];
    //     }
    // }

    for(int u=0;u<cPolygonList.size();u++){
        cPolygonList[u].angle = angle;
        cPolygonList[u].scale = sFactor;
        cPolygonList[u].transVec.x = translationX;
        cPolygonList[u].transVec.y = translationY;
        if(!(cPolygonList[u].transVec.x == 0 || cPolygonList[u].transVec.y == 0 )) {
            translation(cPolygonList[u].transVec, cPolygonList[u]);
        }

        if(!(cPolygonList[u].angle == 0)) {
            rotation(cPolygonList[u].angle, cPolygonList[u]);
        }

        if(!(cPolygonList[u].scale == 1)) {
            scaling(cPolygonList[u].scale, cPolygonList[u]);
        }

        cPolygonList[u].transVec = Coordinate{0.0, 0.0};
        cPolygonList[u].angle = 0.0;
        cPolygonList[u].scale = 1.0;
    }
    
    //  for(int a = 0; a<cPolygonList.size(); a++){
    //     clipping(cPolygonList[a], polygonList[a]);
    //}

    // for(int a = 0; a<cPolygonList.size(); a++){
    //      polyClip(cPolygonList[a]);
    // }

    for(auto p : cPolygonList){
        for(int i = 0; i < grid_width; i++){
            for(int j=0; j < grid_height; j++){
                loadBuffer[i*grid_width + j] = false;
            }
        }

        for(int i = 0; i<p.count;i++){
            vmml::vector<3, float> cur = p.vertices[i];
            vmml::vector<3, float> prev = p.vertices[(i + p.count - 1) % p.count];
            if(lineMode == 'd'){
                drawLineDDA(cur, prev, loadBuffer);
            }else{
                drawLineBresenham(cur, prev, loadBuffer);
            }
        }
        if(rasterswitch){
            rasterization(loadBuffer);
        }
    }

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
    case 'b':
        lineMode = 'b';
        break;

    case 'd':
        lineMode = 'd';
        break;

    case 'r':
        rasterswitch = ! rasterswitch;
        break;
    
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


void polyClip(Polygon &poly) {
    std::cout<<"vertex num: "<<poly.count<<std::endl;
    poly.printPolygon();
    for(int dir = 0; dir < 4; dir++) {
        if(dir == 0) {
            std::cout<<"starting Left: "<<std::endl; 
            polyClipLeft(poly);
            std::cout<<"Left: "<<poly.count<<std::endl;
            poly.printPolygon();
        } else if(dir == 1) {
            std::cout<<"starting Right: "<<std::endl; 
            polyClipRight(poly);
            std::cout<<"Right: "<<poly.count<<std::endl;
            poly.printPolygon();
        } else  if(dir == 2) {
            std::cout<<"starting Bottom: "<<std::endl; 
            polyClipBottom(poly);
            std::cout<<"Bottom: "<<poly.count<<std::endl;
            poly.printPolygon();
        } else if(dir == 3) {
            std::cout<<"starting Top: "<<std::endl; 
            polyClipTop(poly);
            std::cout<<"Top: "<<poly.count<<std::endl;
            poly.printPolygon();
        }
    }
    std::cout<<"poly finished"<<std::endl;
}

void polyClipLeft(Polygon &poly) {
    std::vector<vmml::vector<3, float>> vs;
    int n = poly.count;
    int vsLen = 0;

    for(int i = 0; i < n; i++) {
        vmml::vector<3, float> vA;
        vmml::vector<3, float> vB;
        copyVertex(poly.vertices[i], vA);
        copyVertex(poly.vertices[(i + 1) % n], vB);

        //outside?
        if(vA.x() < xMin && vB.x() < xMin) {
            //discard
        } else if(vA.x() >= xMin && vB.x() >= xMin) {
            //inside?
            //keep both
            vs.push_back(vA);
            vs.push_back(vB);
            vsLen += 2;
        } else  {
             //partial
            if(vB.x() < xMin) {
                exchangeV(vA, vB);
            }

            //vs.push_back(vB);  
            
            //find vert
            float k = (vB.y()-vA.y()) / (vB.x() - vA.x());
            float intersectVal = vA.y() + (xMin-vA.x()) * k;
            vmml::vector<3, float> v = {xMin, intersectVal, 1};
            vs.push_back(v);
            vsLen += 1;          
        }
    }

    poly.count = vsLen;
    
    copyList(vs, poly.vertices, vsLen);
    std::cout<<"copied: "<<std::endl; 
}

void polyClipRight(Polygon &poly) {
    std::vector<vmml::vector<3, float>> vs;
    int n = poly.count;
    int vsLen = 0;

    for(int i = 0; i < n; i++) {
        vmml::vector<3, float> vA;
        vmml::vector<3, float> vB;
        copyVertex(poly.vertices[i], vA);
        copyVertex(poly.vertices[(i + 1) % n], vB);

        //outside?
        if(vA.x() > xMax && vB.x() > xMax) {
            //discard
        } else if(vA.x() <= xMax && vB.x() <= xMax) {
            //inside?
            //keep both
            vs.push_back(vA);
            vs.push_back(vB);
            vsLen += 2;
        } else  {
             //partial
            if(vB.x() > xMax) {
                exchangeV(vA, vB);
            }

            //vs.push_back(vB);  
            //find vert
        
            float k = (vB.y()-vA.y()) / (vB.x() - vA.x());
            float intersectVal = vA.y() + (xMax-vA.x()) * k;
            vmml::vector<3, float> v = {xMax, intersectVal, 1};

            vs.push_back(v);
            vsLen += 1;          
        }
    }

    poly.count = vsLen;     
    copyList(vs, poly.vertices, vsLen);
}

void polyClipBottom(Polygon &poly) {
    std::vector<vmml::vector<3, float>> vs;
    int n = poly.count;
    int vsLen = 0;

    for(int i = 0; i < n; i++) {
        vmml::vector<3, float> vA;
        vmml::vector<3, float> vB;
        copyVertex(poly.vertices[i], vA);
        copyVertex(poly.vertices[(i + 1) % n], vB);

        //outside?
        if(vA.y() < yMin && vB.y() < yMin) {
            //discard
        } else if(vA.y() >= yMin && vB.y() >= yMin) {
            //inside?
            //keep both
            vs.push_back(vA);
            vs.push_back(vB);
            vsLen += 2;
        } else  {
             //partial
            if(vB.y() < yMin) {
                exchangeV(vA, vB);
            }

            //vs.push_back(vB);  
            //find vert
        
            float k = (vB.y()-vA.y()) / (vB.x() - vA.x());
            float intersectVal = vA.x() + (yMin-vA.y()) / k;
            vmml::vector<3, float> v = {intersectVal, yMin, 1};

            vs.push_back(v);
            vsLen += 1;          
        }
    }

    poly.count = vsLen;    
    copyList(vs, poly.vertices, vsLen);
}

void polyClipTop(Polygon &poly) {
    std::vector<vmml::vector<3, float>> vs;
    int n = poly.count;
    int vsLen = 0;

    for(int i = 0; i < n; i++) {
        vmml::vector<3, float> vA;
        vmml::vector<3, float> vB;
        copyVertex(poly.vertices[i], vA);
        copyVertex(poly.vertices[(i + 1) % n], vB);

        //outside?
        if(vA.y() > yMax && vB.y() > yMax) {
            //discard
        } else if(vA.y() <= yMax && vB.y() <= yMax) {
            //inside?
            //keep both
            vs.push_back(vA);
            vs.push_back(vB);
            vsLen += 2;
        } else  {
             //partial
            if(vB.y() > yMax) {
                exchangeV(vA, vB);
            }

            //vs.push_back(vB);  
            //find vert
        
            float k = (vB.y()-vA.y()) / (vB.x() - vA.x());
            float intersectVal = vA.x() + (yMax-vA.y()) / k;
            vmml::vector<3, float> v = {intersectVal, yMax, 1};

            vs.push_back(v);
            vsLen += 1;          
        }
    }

    poly.count = vsLen;    
    copyList(vs, poly.vertices, vsLen);
}

void exchangeV(vmml::vector<3, float> &vA, vmml::vector<3, float> &vB) {
    float tempX, tempY;
    tempX = vA.x();
    tempY = vA.y();
    vA.x() = vB.x();
    vA.y() = vB.y();
    vB.x() = tempX;
    vB.y() = tempY;
}

void copyList(std::vector<vmml::vector<3, float>> &s, std::vector<vmml::vector<3, float>> &t, int n) {
    t.clear();
    for(int i = 0; i < n; i++) {
        vmml::vector<3, float> temp;
        temp.x() = s.at(i).x();
        temp.y() = s.at(i).y();
        temp.z() = 1;
        t.push_back(temp);
    }
}

void copyVertex(vmml::vector<3, float> &s, vmml::vector<3, float> &t) {
    t.x() = s.x();
    t.y() = s.y();
    t.z() = 1;
}