#ifndef UTIL_H_
#define UTIL_H_

/*
    StdIO Color Code
*/

#define RESET       "\033[0m"
#define BLACK       "\033[30m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"


/*
    Voxel Data
*/

// Voxel vertex
const GLfloat vertices[] =
{
    -1, -1, 1,
    1, -1, 1,
    1, 1, 1,
    -1, 1, 1,
    -1, -1, -1,
    1, -1, -1,
    1, 1, -1,
    -1, 1, -1
};

// Voxel index
const GLushort cube_elements[] =
{
    0, 1, 2,  0, 2, 3,
    1, 5, 6,  1, 6, 2,
    2, 6, 7,  2, 7, 3,
    3, 4, 0,  3, 7, 4,
    4, 5, 1,  4, 1, 0,
    5, 7, 6,  5, 4, 7,
};

#endif
