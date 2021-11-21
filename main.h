#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

struct block { // either bike itself, or its trace
    int x, y;
    int size;
    char direction; // 0 for left, 1 for up, 2 for right, 3 for down
    unsigned short color;
    const unsigned short *headImage, *hImage, *vImage, *tlImage, *trImage, *brImage, *blImage;
    int leftButton, rightButton, addTraceButton, removeTraceButton;
};




#endif
