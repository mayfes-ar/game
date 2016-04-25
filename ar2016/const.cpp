#include "const.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

const int CAP_WIDTH = 640;
const int CAP_HEIGHT = 480;

const int FPS = 30;

const double CAP2IMG_RATE = WIDTH / CAP_WIDTH;
const int CAP2IMG_SHIFT_X = (WIDTH - CAP2IMG_RATE * CAP_WIDTH) / 2;
const int CAP2IMG_SHIFT_Y = (HEIGHT - CAP2IMG_RATE * CAP_HEIGHT) / 2;

const int KEEP_FRAME = 5;