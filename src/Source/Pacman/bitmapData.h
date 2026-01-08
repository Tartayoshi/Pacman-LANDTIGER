#include "LPC17xx.h"

#define BLINKYSIZE 14 //workaround for blinky prec frame

//Pacman
extern const uint16_t pac0[]; 
extern const uint16_t pac1[];
extern const uint16_t pac2[];
extern const uint16_t pacSize;

extern const uint16_t  pac_death10[];
extern const uint16_t  pac_death9[];
extern const uint16_t  pac_death8[];
extern const uint16_t  pac_death7[];
extern const uint16_t  pac_death5[];
extern const uint16_t  pac_death6[];
extern const uint16_t  pac_death4[];
extern const uint16_t  pac_death3[];
extern const uint16_t  pac_death2[];
extern const uint16_t  pac_death1[];
extern const uint16_t  pac_death0[];

extern const uint16_t deathsize;

//Blinky
extern const uint16_t  blinky_l1 [];
extern const uint16_t  blinky_l2 [];
extern const uint16_t  blinky_u1 [];
extern const uint16_t  blinky_u2 [];
extern const uint16_t  blinky_d1 [];
extern const uint16_t  blinky_d2 [];
extern const uint16_t  blinky_f1 [];
extern const uint16_t  blinky_f2 [];
extern const uint16_t  blinky_f3 [];
extern const uint16_t  blinky_f4 [];
extern const uint16_t  blinky_le [];
extern const uint16_t  blinky_ue [];
extern const uint16_t  blinky_de[];
extern const uint16_t  blinky_n[];
extern const uint16_t blinkySize;

//Map Tails

extern const uint16_t mapOuterCorner[];
extern const uint16_t mapOuterEdge[];
extern const uint16_t mapInnerEdge[];
extern const uint16_t mapOuterTJoint[];
extern const uint16_t  mapOuterTJoint2[];

extern const uint16_t obstacleCorner[];
extern const uint16_t obstacleEdge[];
extern const uint16_t obstacleInnerCorner[];
extern const uint16_t superPill[];


extern const uint16_t tailSize;
//extern const int pillScore;
//extern const int superPillScore;

extern const uint16_t victoryScreen[];
extern const uint16_t victoryScreenX;
extern const uint16_t victoryScreenY;

extern const uint16_t lazy_spawn[];
extern const uint16_t spawnX;
extern const uint16_t spawnY;
