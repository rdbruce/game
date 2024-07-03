#pragma once

// define macros to represent cell types

/* BYTE DECOMPOSITION:

byte 1:
    1-8 = cell info (wall type, etc)

byte 2:
    9-12 = building hp (0-15)

    13: water by default?
    14: barrier?
    15: occupied?
    16: indestructible by player? 

byte 3:
    17-20 = water corner shorelines
        ....LRlr, 0010 = bottom left, 0100 = top right
    21-24 = side shorelines
        lrtb...., 1100 = left&right, 0101 = right&bottom

byte 4:
    25: can be destroyed by enemies
    26: has timer?
    27-32: timer (0-63)
*/
#define EMPTY           0b00000000000000000000000000000000
#define BARRIER         0b00000000000000000010000000000000
#define OCCUPIED        0b00000000000000000100000000000000
#define WATER           0b00000000000000000001000000000000
#define INDESTRUBTIBLE  0b00000000000000001000000000000000
#define CAN_DIE         0b00000001000000000000000000000000
#define HAS_TIMER       0b00000010000000000000000000000000

// macros to easily access certain regions of data
#define TIMER           0b11111100000000000000000000000000
#define HEALTH          0b00000000000000000000111100000000
#define SHORELINE       0b00000000111111110000000000000000

#define LOG             0b00000001000000000110100000000001 // 8  health
#define BRIDGE          0b00000001000000000100001100000010 // 3  health
#define TREE            0b00000001000000000110110000000011 // 12 health
#define STUMP           0b00000001000000000110100000000100 // 8  health
#define SAPLING         0b00000011000000000110001000000101 // 2  health
#define BORDER          0b00000000000000001110000000000110 // not destructible
#define CLOSED_DOOR     0b00000001000000000110011000000111 // 6 health
#define OPEN_DOOR       0b00000001000000000100011000001000 // 6 health

