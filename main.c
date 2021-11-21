#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gba.h"
#include "images/start_image.h"
#include "images/blue_cycle.h"
#include "images/orange_cycle.h"
#include "images/red_wins.h"
#include "images/blue_wins.h"
#include "images/red_h_trace.h"
#include "images/red_v_trace.h"
#include "images/red_tl_trace.h"
#include "images/red_tr_trace.h"
#include "images/red_br_trace.h"
#include "images/red_bl_trace.h"
#include "images/blue_h_trace.h"
#include "images/blue_v_trace.h"
#include "images/blue_tl_trace.h"
#include "images/blue_tr_trace.h"
#include "images/blue_br_trace.h"
#include "images/blue_bl_trace.h"

#define maxLength 16


int main(void) {
    while (1) {
        // Manipulate REG_DISPCNT here to set Mode 3. //
        REG_DISPCNT = MODE3 | BG2_ENABLE;

        // Save current and previous state of button input.
        u32 previousButtons;
        u32 currentButtons = BUTTONS;

        int numBlocks[] = {1, 1};

        struct block blocks[2][maxLength] = {
                {[0]={30, 30, 15, 2, BLUE, blue_light_cycle, blue_h_trace, blue_v_trace, blue_tl_trace, blue_tr_trace, blue_br_trace, blue_bl_trace,
                      BUTTON_L, BUTTON_R, BUTTON_A, BUTTON_B}},
                {[0]={30, 30, 15, 2, RED, orange_light_cycle, red_h_trace, red_v_trace, red_tl_trace, red_tr_trace, red_br_trace, red_bl_trace,
                      BUTTON_LEFT, BUTTON_RIGHT, BUTTON_UP, BUTTON_DOWN}}};
        for (int p = 0; p < 2; ++p) {
            for (int i = 0; i < maxLength; i++) {
                blocks[p][i].size = 15;
            }
        }

        for (int i = 1; i < maxLength; i++) {
            blocks[0][i].color = BLUE;
            blocks[1][i].color = RED;
        }

        blocks[0][0].x = 30;
        blocks[0][0].y = 30;
        blocks[0][0].direction = 2;

        blocks[1][0].x = 120;
        blocks[1][0].y = 120;
        blocks[1][0].direction = 0;

        drawFullScreenImageDMA(start_image);
        drawString(60, 50, "Press Enter to Start", MAGENTA);

        for (previousButtons = BUTTONS; !KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons); waitForVBlank()) {
            previousButtons = currentButtons;
            currentButtons = BUTTONS;
        }

        fillScreenDMA(BLACK);
        char gameOver = 0;
        while (!gameOver) {
            char addTraceClicked[2] = {0, 0}, removeTraceClicked[2] = {0, 0}, leftClicked[2] = {0, 0}, rightClicked[2] = {0, 0};
            for (int i = 0; i < 60; ++i) {
                waitForVBlank();
                previousButtons = currentButtons;
                currentButtons = BUTTONS; // Load the current state of the buttons

                for (int p = 0; p < 2; ++p) {
                    if (KEY_JUST_PRESSED(blocks[p][0].leftButton, currentButtons, previousButtons))
                        leftClicked[p] = 1;
                    if (KEY_JUST_PRESSED(blocks[p][0].rightButton, currentButtons, previousButtons))
                        rightClicked[p] = 1;
                    if (KEY_JUST_PRESSED(blocks[p][0].addTraceButton, currentButtons, previousButtons))
                        addTraceClicked[p] = 1;
                    if (KEY_JUST_PRESSED(blocks[p][0].removeTraceButton, currentButtons, previousButtons))
                        removeTraceClicked[p] = 1;
                }
                if (KEY_DOWN(BUTTON_SELECT, currentButtons))
                    gameOver = 1;
            }

            for (int p = 0; p < 2; p++) {
                int size = numBlocks[p];

                for (int i = size - 1; i >= 0; i--) {
                    struct block *currBlock = &blocks[p][i];
                    drawRectDMA(currBlock->x, currBlock->y, currBlock->size, currBlock->size, BLACK);
                    if (i > 0)
                        currBlock->direction = blocks[p][i - 1].direction;
                }

                if (leftClicked[p])
                    blocks[p][0].direction = (blocks[p][0].direction + 3) % 4;
                if (rightClicked[p])
                    blocks[p][0].direction = (blocks[p][0].direction + 1) % 4;

                for (int i = 0; i < size; i++) {
                    struct block *currBlock = &blocks[p][i];
                    switch (currBlock->direction) {
                        case 0:
                            currBlock->x -= currBlock->size;
                            break;
                        case 1:
                            currBlock->y -= currBlock->size;
                            break;
                        case 2:
                            currBlock->x += currBlock->size;
                            break;
                        case 3:
                            currBlock->y += currBlock->size;
                            break;
                    }
                    if (currBlock->x >= WIDTH / 15 * 15)
                        currBlock->x -= WIDTH / 15 * 15;
                    else if (currBlock->x < 0)
                        currBlock->x += WIDTH / 15 * 15;
                    if (currBlock->y >= HEIGHT / 15 * 15)
                        currBlock->y -= HEIGHT / 15 * 15;
                    else if (currBlock->y < 0)
                        currBlock->y += HEIGHT / 15 * 15;

                    if (i == 0) {
                        drawImageDMA(currBlock->x, currBlock->y, currBlock->size, currBlock->size, currBlock->headImage);
                    } else {
                        struct block *prevBlock = &blocks[p][i-1];
                        int maxDirection = currBlock->direction > (prevBlock->direction + 2) % 4 ? currBlock->direction : (prevBlock->direction + 2) % 4;
                        int minDirection = currBlock->direction < (prevBlock->direction + 2) % 4 ? currBlock->direction : (prevBlock->direction + 2) % 4;
                        drawRectDMA(currBlock->x, currBlock->y, currBlock->size, currBlock->size, WHITE);
                        if (currBlock->direction == prevBlock->direction)
                            drawImageDMA(currBlock->x, currBlock->y, currBlock->size, currBlock->size,
                                     currBlock->direction % 2 ? blocks[p][0].vImage : blocks[p][0].hImage);
                        else
                            switch (10*maxDirection + minDirection) {
                                case 32:
                                    drawImageDMA(currBlock->x, currBlock->y, currBlock->size, currBlock->size,
                                                 blocks[p][0].tlImage);
                                    break;
                                case 30:
                                    drawImageDMA(currBlock->x, currBlock->y, currBlock->size, currBlock->size,
                                                 blocks[p][0].trImage);
                                    break;
                                case 21:
                                    drawImageDMA(currBlock->x, currBlock->y, currBlock->size, currBlock->size,
                                                 blocks[p][0].blImage);
                                    break;
                                case 10:
                                    drawImageDMA(currBlock->x, currBlock->y, currBlock->size, currBlock->size,
                                                 blocks[p][0].brImage);
                                    break;
                            }
                    }
                }
            }

            for (int p = 0; p < 2; ++p) {
                drawRectDMA(20 + p * 180, 20, blocks[p][0].size, blocks[p][0].size, BLACK);
                char tempStr[] = {numBlocks[p] + 48};
                drawString(20 + p * 180, 20, tempStr, blocks[p][0].color);
                if (addTraceClicked[p] && numBlocks[p] < maxLength) {
                    switch (blocks[p][numBlocks[p] - 1].direction) {
                        case 0:
                            blocks[p][numBlocks[p]].x =
                                    blocks[p][numBlocks[p] - 1].x + blocks[p][numBlocks[p] - 1].size;
                            blocks[p][numBlocks[p]].y = blocks[p][numBlocks[p] - 1].y;
                            break;
                        case 1:
                            blocks[p][numBlocks[p]].x = blocks[p][numBlocks[p] - 1].x;
                            blocks[p][numBlocks[p]].y =
                                    blocks[p][numBlocks[p] - 1].y + blocks[p][numBlocks[p] - 1].size;
                            break;
                        case 2:
                            blocks[p][numBlocks[p]].x =
                                    blocks[p][numBlocks[p] - 1].x - blocks[p][numBlocks[p] - 1].size;
                            blocks[p][numBlocks[p]].y = blocks[p][numBlocks[p] - 1].y;
                            break;
                        case 3:
                            blocks[p][numBlocks[p]].x = blocks[p][numBlocks[p] - 1].x;
                            blocks[p][numBlocks[p]].y =
                                    blocks[p][numBlocks[p] - 1].y - blocks[p][numBlocks[p] - 1].size;
                            break;
                    }
                    numBlocks[p]++;
                }
                if (removeTraceClicked[p] && numBlocks[p] > 1) {
                    drawRectDMA(blocks[p][numBlocks[p] - 1].x, blocks[p][numBlocks[p] - 1].y,
                                blocks[p][numBlocks[p]].size, blocks[p][numBlocks[p]].size, BLACK);
                    numBlocks[p]--;
                }
            }

            struct block *allBlocks[maxLength * 2];
            for (int i = 1; i < numBlocks[0]; ++i)
                allBlocks[i - 1] = &blocks[0][i];
            for (int i = 1; i < numBlocks[1]; ++i)
                allBlocks[numBlocks[0] + i - 2] = &blocks[1][i];

            int totalBlocks = numBlocks[0] + numBlocks[1] - 2;
            for (int i = 0; i < totalBlocks; i++) {
                for (int p = 0; p < 2; p++) {
                    if (allBlocks[i]->x == blocks[p][0].x && allBlocks[i]->y == blocks[p][0].y) {
                        gameOver = 1;
                        for (int i = 0; i < 100; ++i)
                            waitForVBlank();
                        drawFullScreenImageDMA(p ? blue_wins : red_wins);
                        drawString(60, 140, "Press Select for Main Menu", GREEN);
                        for (previousButtons = BUTTONS; !KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons,
                                                                          previousButtons); waitForVBlank()) {
                            previousButtons = currentButtons;
                            currentButtons = BUTTONS;
                        }
                    }
                }
            }
        }
    }
}

