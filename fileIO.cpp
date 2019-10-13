#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "polygon.cpp"

// void readinput(char *filename){
//     vector<Polygon> polygonList;
//     ifstream inputFile(filename);
//     string line;
//     int count;
//     inputFile >> count;
//     getline(inputFile, line);
//     getline(inputFile, line);
//     for (int i=0; i< count; i++){
//         int num;
//         vector<Coordinate> coorList;
//         inputFile >> num;
//         getline(inputFile, line);
//         for (int j=0; i<num; j++){
//             float x, y;
//             string inputX, inputY;
//             getline(inputFile, line);
//             istringstream record(line);
//             getline(record, inputX, ' ');
//             getline(record, inputY);
//             x = stof(inputX);
//             y = stof(inputX);
//             Coordinate point(x,y);
//             coorList.push_back(point);
//         }
//         Polygon polygon(num,coorList);
//         polygonList.push_back(polygon);
//         getline(inputFile, line);
//     }
// }