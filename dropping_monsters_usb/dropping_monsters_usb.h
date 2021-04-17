/**
 * Header file for
 * "DROPPING MONSTERS" game is implemented.
 * ATTENTION: You should compile this source with Release option of ARM compiler.
 * Mar.24, 2021 Pa@ART modified from test_ntsc_wall_grph.c
 */

#ifndef __DROPPING_MONSTERS__
#define __DROPPING_MONSTERS__

#define CHAR_W  8       // character width
#define CHAR_H  8       // character height
#define CMOUSE  0xB     // character of space mouse
#define CHEART  0x8     // character of heart
#define CME     0x7     // character of me
#define CWALL   0x1     // character of wall
#define CGND    '*'     // character of ground
#define MAXFLOOR    150 // max floor number
#define NMOUSE  25      // max number of mouse
#define IMRATE  50000   // initial mouse rate
#define MMRATE  10000    // minimum mouse rate
#define IPMOUSE 10      // initial mouse probability
#define NHEART  2       // max number of heart
#define PHEART  5       // heart generation probability
#define HTRATE  40000   // heart rate
#define ORATE   (MYRATE * 40)  // oxygen rate
#define IOXGEN  100     // initial oxygen
#define MYRATE  30000   // my rate
#define ME_X    10      // initial x of me
#define ME_Y    17      // initial y of me
#define ME_HP   5       // HP of me
#define SUPERME 1       // superme mode flag
#define NORMALME    0       // normalme mode flag
#define MYTIMER 15      // superme mode timer value
#define LSCORE  1       // line of score drawing
#define LOXYGEN 2       // line of oxygen drawing
#define STARTW  1       // start x of wall
#define ENDW    36      // end x of wall
#define HP_UP_SCORE 3000    // every HP_UP_SCORE, HP -> HP + 1
#define HEART_BONUS 30  // bonus point for getting heart
#define BASE_SCORE  10  // base score
#define STAGE_BONUS 100 // base stage bonus

#define GX(x)   ((x) * CHAR_W)
#define GY(y)   ((y) * CHAR_H)

typedef struct {
    int x;          // x of entity
    int y;          // y of entity
    int hp;         // HP of entity
    char c;         // character of entity
    int sp;         // special power of entity
    int timer;      // timer of entity
    bool odd;       // true: draw new floor, false: draw empty floor
} entity;

#endif