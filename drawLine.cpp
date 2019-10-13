
// #include <math.h>
// using namespace std;
// //Using structs to store my coordinates.
// struct Coordinate
// {
//     int x, y;
//     Coordinate(int inputX, int inputY)
//     {
//         x = inputX;
//         y = inputY;
//     }
// };

// void swapCor(Coordinate start, Coordinate end)
// {
//     int tempx = start.x;
//     int tempy = start.y;
//     start.x = end.x;
//     start.y = end.y;
//     end.x = tempx;
//     end.y = tempy;
// }
// void drawLineDDA(Coordinate start, Coordinate end)
// {
//     // Coordinate pixels[] = {start, end};
//     //First figure out the direction
//     int dx = -(start.x - end.x);
//     int dy = -(start.y - end.y);
//     float m = (float)dy / dx;
//     float j = 0.0;
//     float i = 0.0;

//     if ((fabs(m)) < 1)
//     { //Shallow, calculating Y
//         if (m < 0)
//         {
//             if (start.y < end.y)
//             {
//                 int tempx = start.x;
//                 int tempy = start.y;
//                 start.x = end.x;
//                 start.y = end.y;
//                 end.x = tempx;
//                 end.y = tempy;
//             }
//             for (int i = start.x; i > end.x; i--)
//             {
//                 j = (i - start.x) * m + start.y;
//                 draw_pix(i, (int)j);
//             }
//         }
//         else if (m == 0 || dy == 0)
//         {
//             if (start.x > end.y)
//             {
//                 int tempx = start.x;
//                 int tempy = start.y;
//                 start.x = end.x;
//                 start.y = end.y;
//                 end.x = tempx;
//                 end.y = tempy;
//             }
//             for (int i = start.x; i < end.x; i++)
//             {
//                 //j = (i-start.x)*m + start.y;
//                 draw_pix(i, end.y);
//             }
//         }
//         else
//         {
//             if (start.y > end.y)
//             {
//                 int tempx = start.x;
//                 int tempy = start.y;
//                 start.x = end.x;
//                 start.y = end.y;
//                 end.x = tempx;
//                 end.y = tempy;
//             }
//             for (int i = start.x; i < end.x; i++)
//             {
//                 j = (i - start.x) * m + start.y;
//                 draw_pix(i, (int)j);
//             }
//         }
//     }
//     else
//     { //Steep, calculating X
//         m = (float)dx / dy;
//         if (m < 0)
//         {
//             if (start.y < end.y)
//             {
//                 int tempx = start.x;
//                 int tempy = start.y;
//                 start.x = end.x;
//                 start.y = end.y;
//                 end.x = tempx;
//                 end.y = tempy;
//             }
//             for (int j = start.y; j >= end.y; j--)
//             {
//                 i = start.x + (j - start.y) * m;
//                 draw_pix((int)i, j);
//             }
//         }
//         else
//         {
//             if (start.y > end.y)
//             {
//                 int tempx = start.x;
//                 int tempy = start.y;
//                 start.x = end.x;
//                 start.y = end.y;
//                 end.x = tempx;
//                 end.y = tempy;
//             }
//             for (int j = start.y; j < end.y; j++)
//             {
//                 i = start.x + (j - start.y) * m;
//                 draw_pix((int)i, j);
//             }
//         }
//     }
// }

// void drawLineBresenham(Coordinate start, Coordinate end)
// {
//     //Coordinate pixels[] = {start, end};
//     if (start.y > end.y)
//     {
//         swapCor(start, end);
//     }
//     int dx = -(start.x - end.x);
//     int dy = -(start.y - end.y);
//     int m = dy / dx;

//     int x = start.x;
//     int y = start.y;
//     //If the case is steep

//     if (m > 0 && m < 1)
//     {
//         int p = 2 * dy - dx;
//         while (y <= end.y)
//         {
//             if (p > 0)
//             {
//                 x += 1;
//                 p = p + 2 * dy - 2 * dx;
//                 draw_pix(x, y);
//             }
//             else
//             {
//                 x += 0;
//                 p = p + 2 * dy;
//                 draw_pix(x, y);
//             }
//             y += 1;
//         }
//     }
//     else if (m > 1)
//     { /* WORKS!!!*/
//         int p = 2 * dx - dy;
//         while (y <= end.y)
//         {
//             if (p > 0)
//             {
//                 x += 1;
//                 p = p + 2 * dx - 2 * dy;
//                 draw_pix(x, y);
//             }
//             else
//             {
//                 x += 0;
//                 p = p + 2 * dx;
//                 draw_pix(x, y);
//             }
//             y += 1;
//         }
//     }
//     else if (m < 0 && m > -1)
//     {
//         int p = 2 * dx - dy;
//         while (y <= end.y)
//         {
//             if (p > 0)
//             {
//                 x -= 1;
//                 p = p + 2 * dx - 2 * dy;
//                 draw_pix(x, y);
//             }
//             else
//             {
//                 x += 0;
//                 p = p + 2 * dx;
//                 draw_pix(x, y);
//             }
//             y += 1;
//         }
//     }
// }