/* (c) 2019 Lucas Towers - Licensed under MIT */

#include <advance.h>


/* TYPES */

typedef struct { s16 x, y; } MapPosition;


/* VIDEO */

// copies assets into vram and configures some video related registers
void setupVideo();

// waits until current frame has been drawn fully
void vsync();


/* SOUND */

// configures sound registers, to be called before bleep and boom
void setupSound();

// plays a sweeping square wave pitched proportionaly to n
void bleep(u32 n); 

// plays a white noise explosion sound
void boom();


/* INPUT */

// updates button input, should be run once before using keyHit or keyHeld
void keyPoll();

// returns whether a given button is newly pressed 
u32 keyHit(u32 key); 

// returns whether a given button is being held down
u32 keyHeld(u32 key);


/* RANDOM NUMBER GENERATION */

// returns a pseudo-random unsigned 32-bit number
u32 random();


/* MAP UTILITIES */

// returns a pseudo-random map position, uses the above-declared random function
MapPosition randomMapPosition();

// returns whether a position is within the bounds of the map
u32 mapPositionIsValid(MapPosition position);

// returns a pointer to a given map entry at a given position
// screenblock identifies whether to fetch entry from either cover or mine layer
ScreenEntry* mapEntryPtr(u32 screenblock, MapPosition position);


/* MINE UTILITIES */

// attempts to plant a mine at a given position, returns whether successful
u32 plantMine(MapPosition position);

// randomizes the minefield
// reticle position is needed leave a space for the starting area
void randomizeMines(MapPosition reticle_position);


/* COVER UTILITIES */

// fills the cover screenblock with a checkered pattern
void coverReset();

// reveals a given position as well surrounding positions if necessary
// uses a recursive floodfill algorithm to reveal surrounding positions
void coverReveal(MapPosition position);
void coverRevealRecurse(MapPosition position);


/* RETICLE UTILITIES */

// update reticle position and graphics based on keyHit and keyHeld
void updateReticle();


/* PLAYER ACTIONS */

// if possible, reveals a tile and plays appropriate sounds 
void investigate(MapPosition position);

// if possible, sets or removes a flag 
void toggleFlag(MapPosition position);


/* MACROS */

// video configuration
#define GFX_MODE         0
#define MINE_BG          1
#define MINE_SBB         2
#define COVER_BG         0
#define COVER_SBB        1
#define SHARED_CBB       0
#define RETICLE_OBJ      0

// game configuration
#define MAP_WIDTH        30
#define MAP_HEIGHT       20
#define MINE_COUNT       140

// tile id configuration
#define BLANK_TILE_ID    0
#define NO_MINE_TILE_ID  1
#define MINE_TILE_ID     10
#define FLAG_TILE_ID     11
#define COVER_TILE_ID    12
#define REVEAL_TILE_ID   13
#define RETICLE_TILE_ID  1
#define ENTRY_ID_MASK    0x01FF


/* GLOBAL VARIABLES */

u16 keys_current = 0x0000;
u16 keys_previous = 0x0000;
u32 rng_value = 0;
u32 reticle_move_repeat_delay = 8;
MapPosition reticle_position = { (MAP_WIDTH-1)/2, (MAP_HEIGHT-1)/2 };


/* GLOBAL CONSTANTS */

const Tile BG_TILES[13] =
{
  { 0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000 },
  { 0x00000000,0x00000000,0x00111100,0x00111100,0x00111100,0x00111100,0x00000000,0x00000000 },
  { 0x00000000,0x00022000,0x00020000,0x00020000,0x00020000,0x00020000,0x00222200,0x00000000 },
  { 0x00000000,0x00033000,0x00300300,0x00030000,0x00003000,0x00000300,0x00333300,0x00000000 },
  { 0x00000000,0x00044000,0x00400400,0x00040000,0x00400000,0x00400400,0x00044000,0x00000000 },
  { 0x00000000,0x00050500,0x00050500,0x00050500,0x00555500,0x00050000,0x00050000,0x00000000 },
  { 0x00000000,0x00666600,0x00000600,0x00066600,0x00600000,0x00600000,0x00066600,0x00000000 },
  { 0x00000000,0x00666000,0x00000600,0x00066600,0x00600600,0x00600600,0x00066000,0x00000000 },
  { 0x00000000,0x00666600,0x00600000,0x00060000,0x00060000,0x00006000,0x00006000,0x00000000 },
  { 0x00000000,0x00066000,0x00600600,0x00066000,0x00600600,0x00600600,0x00066000,0x00000000 },
  { 0x00000000,0x00000000,0x00077000,0x00777700,0x00777700,0x00077000,0x00000000,0x00000000 },
  { 0x11111111,0x11111111,0x11155511,0x11155511,0x11151111,0x11151111,0x11555511,0x11111111 },
  { 0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111 }
};

const Tile OBJ_TILES[5] =
{
  { 0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000 },
  { 0x00000000,0x00000000,0x00222000,0x02111200,0x00221200,0x00021200,0x00002000,0x00000000 },
  { 0x00000000,0x00000000,0x00022200,0x00211120,0x00212200,0x00212000,0x00020000,0x00000000 },
  { 0x00000000,0x00002000,0x00021200,0x00221200,0x02111200,0x00222000,0x00000000,0x00000000 },
  { 0x00000000,0x00020000,0x00212000,0x00212200,0x00211120,0x00022200,0x00000000,0x00000000 }
};

const Palbank BG_COLORS[3] =
{
  {
    RGB8(245,245,245),RGB8(224, 224, 224),RGB8(66, 165, 245),RGB8(102, 187, 106),
    RGB8(255, 167, 38),RGB8(239, 83, 80),RGB8(171, 71, 188),RGB8(0, 0, 0)
  },
  { RGB8(189, 189, 189),RGB8(158, 158, 158) },
  { RGB8(158, 158, 158),RGB8(189, 189, 189) }
};

const Palbank OBJ_COLORS = { RGB8(0,0,0),RGB8(255, 238, 88),RGB8(0,0,0) };


/* FUNCTION DEFINITIONS */

void main()
{
  // setup
  coverReset();
  vsync();
  setupVideo();
  setupSound();

  // game loop until user hits A for the first time
  while(true)
  {
    vsync();
    keyPoll();
    updateReticle();

    // use users input to add some variation to the rng
    rng_value += REG_KEYINPUT;

    if (keyHit(KEYINPUT_A))
    {
      randomizeMines(reticle_position);
      investigate(reticle_position);
      break;
    }
  }

  // main game loop
  while(true)
  {
    vsync();
    keyPoll();
    updateReticle();

    if (keyHit(KEYINPUT_A))
      investigate(reticle_position);
    if (keyHit(KEYINPUT_B))
      toggleFlag(reticle_position);
  }
}

void setupVideo()
{
  REG_DMA[3].source = BG_TILES;
  REG_DMA[3].destination = BG_CHARBLOCKS[SHARED_CBB];
  REG_DMA[3].control = DMA_ENABLE | DMA_32 | DMA_COUNT(sizeof(BG_TILES)/4);

  REG_DMA[3].source = OBJ_TILES;
  REG_DMA[3].destination = OBJ_CHARBLOCKS;
  REG_DMA[3].control = DMA_ENABLE | DMA_32 | DMA_COUNT(sizeof(OBJ_TILES)/4);

  REG_DMA[3].source = BG_COLORS;
  REG_DMA[3].destination = BG_PALBANKS;
  REG_DMA[3].control = DMA_ENABLE | DMA_32 | DMA_COUNT(sizeof(BG_COLORS)/4);

  REG_DMA[3].source = OBJ_COLORS;
  REG_DMA[3].destination = OBJ_PALBANKS;
  REG_DMA[3].control = DMA_ENABLE | DMA_32 | DMA_COUNT(sizeof(OBJ_COLORS)/4);

  OBJ_ATTRIBUTES[RETICLE_OBJ].attr0 = OBJ_ATTR0_HIDE;
  OBJ_ATTRIBUTES[RETICLE_OBJ].attr2 = OBJ_ATTR2_TILE_ID(RETICLE_TILE_ID);

  REG_DISPCNT = (
    DISPCNT_MODE(GFX_MODE) | DISPCNT_BG(COVER_BG) |
    DISPCNT_BG(MINE_BG) | DISPCNT_OBJ | DISPCNT_OBJ_1D
  );
  REG_BGCNT[COVER_BG] = (
    BGCNT_REG_32x32 | BGCNT_CHARBLOCK(SHARED_CBB) | BGCNT_SCREENBLOCK(COVER_SBB)
  );
  REG_BGCNT[MINE_BG] = (
    BGCNT_REG_32x32 | BGCNT_CHARBLOCK(SHARED_CBB) | BGCNT_SCREENBLOCK(MINE_SBB)
  );
}

void vsync()
{
  while(REG_VCOUNT >= 160);
  while(REG_VCOUNT < 160);
}

void setupSound()
{
  REG_SOUNDCNT_X = SOUNDCNT_X_ENABLE;
  REG_SOUNDCNT_H = SOUNDCNT_H_DMG100;
  REG_SOUNDCNT_L = 
  (
    SOUNDCNT_L_SOUND1_LEFT | SOUNDCNT_L_SOUND1_RIGHT | SOUNDCNT_L_SOUND4_LEFT |
    SOUNDCNT_L_SOUND4_RIGHT | SOUNDCNT_L_VOL_LEFT(7) | SOUNDCNT_L_VOL_RIGHT(7)
  );
}

void bleep(u32 n)
{
  REG_SOUND1CNT_L =
  (
    SOUND1CNT_L_SWEEP_INC | SOUND1CNT_L_SWEEP_SHIFT(3+n) |
    SOUND1CNT_L_SWEEP_TIME(7)
  );
  REG_SOUND1CNT_H = SOUND1CNT_H_DUTY1_4 | SOUND1CNT_H_ENV_INIT(15);
  REG_SOUND1CNT_X = SOUND1CNT_X_RESET;
  REG_SOUND1CNT_X =
  (
    SOUND1CNT_X_RESET | SOUND1CNT_X_HOLD | SOUND1CNT_X_RATE(2048 - (1280 >> n))
  );
}

void boom()
{
  REG_SOUND4CNT_L =
  (
    SOUND4CNT_H_CLK_DIV(3) | SOUND4CNT_L_ENV_DEC |
    SOUND4CNT_L_ENV_TIME(6) | SOUND4CNT_L_ENV_INIT(15)
  );
  REG_SOUND4CNT_H = SOUND4CNT_H_RESET;
  REG_SOUND4CNT_H = 
  (
    SOUND4CNT_H_RESET | SOUND4CNT_H_HOLD |
    SOUND4CNT_H_PRESTEP(9) | SOUND4CNT_H_STAGES_15
  );
}

void keyPoll()
{
  keys_previous = keys_current;
  keys_current = ~REG_KEYINPUT;
}

u32 keyHit(u32 key)
{
  return (keys_current & ~keys_previous) & key;
}

u32 keyHeld(u32 key)
{
  return keys_current & key;
}

u32 random()
{
  rng_value = 1103515245 * rng_value + 24691;
  return rng_value;
}

MapPosition randomMapPosition()
{
  MapPosition position;
  u32 r = random();
  position.x = Mod(r & 0xFFFF, MAP_WIDTH);
  position.y = Mod(r >> 16, MAP_HEIGHT);
  return position;
}

u32 mapPositionIsValid(MapPosition position)
{
  return (
    0 <= position.x && position.x < MAP_WIDTH &&
    0 <= position.y && position.y < MAP_HEIGHT
  );
}

ScreenEntry* mapEntryPtr(u32 screenblock, MapPosition position)
{
  u32 index = position.x + position.y * 32;
  return &BG_SCREENBLOCKS[screenblock][index];
}

u32 plantMine(MapPosition position)
{
  ScreenEntry* entry = mapEntryPtr(MINE_SBB, position);
  if (*entry == MINE_TILE_ID)
    return false;
  else
    *entry = MINE_TILE_ID;

  for (s32 offset_x = -1; offset_x <= 1; offset_x++)
    for (s32 offset_y = -1; offset_y <= 1; offset_y++)
      {
        if(offset_x == 0 && offset_y == 0)
          continue;
        MapPosition adjacent_position = position;
        adjacent_position.x += offset_x;
        adjacent_position.y += offset_y;
        if (!mapPositionIsValid(adjacent_position))
          continue;
        ScreenEntry* adjacent_entry = mapEntryPtr(MINE_SBB, adjacent_position);
        if (*adjacent_entry != MINE_TILE_ID)
          (*adjacent_entry) += 1;
      }
  
  return true;
}

void randomizeMines(MapPosition reticle_position)
{
  // fill minefield with NO_MINE_TILE_ID
  volatile ScreenEntry source_data[2] = { NO_MINE_TILE_ID, NO_MINE_TILE_ID };
  u32 words = sizeof(Screenblock)/4; 
  REG_DMA[3].source = source_data;
  REG_DMA[3].destination = BG_SCREENBLOCKS[MINE_SBB];
  REG_DISPCNT |= DISPCNT_BLANK;
  REG_DMA[3].control = DMA_ENABLE | DMA_32 | DMA_SRC_FIXED | DMA_COUNT(words);
  REG_DISPCNT &= ~DISPCNT_BLANK;

  // place mines
  for (u32 i = 0; i < MINE_COUNT; i++)
  {
    MapPosition mine_pos;
    u32 plant_succesful = 0;
    do
    {
      mine_pos = randomMapPosition();

      s32 dx = mine_pos.x - reticle_position.x;
      s32 dy = mine_pos.y - reticle_position.y;
      if (!((dx/4 || dy/2) && (dx/2 || dy/4)))
        continue;
      plant_succesful = plantMine(mine_pos);
    } while (!plant_succesful); 
  }
}

void coverReset()
{
  MapPosition pos;
  for (pos.x = 0; pos.x < MAP_WIDTH; pos.x++)
    for (pos.y = 0; pos.y < MAP_HEIGHT; pos.y++)
    {
      ScreenEntry* entry = mapEntryPtr(COVER_SBB, pos);
      u32 palbank = ((pos.x + pos.y) & 1) + 1;
      *entry = COVER_TILE_ID | SCREEN_ENTRY_PALBANK(palbank);
    }
}

ScreenEntry* flood_cover_entries[MAP_WIDTH*MAP_HEIGHT];
u32 flood_cover_entry_count;

void coverReveal(MapPosition position)
{
  Tile* reveal_tile = &BG_CHARBLOCKS[SHARED_CBB][COVER_TILE_ID+1];
  for (u32 i = 0; i < 8; i++)
    (*reveal_tile)[i] = BG_TILES[COVER_TILE_ID][i];

  flood_cover_entry_count = 0;
  coverRevealRecurse(position);

  for (u32 i = 0; i < 4; i++)
  {
    (*reveal_tile)[i] = 0x00000000;
    (*reveal_tile)[7-i] = 0x00000000;
    u32 shift = (i+1) * 4;
    for (u32 u = i+1; u < 7-i; u++)
      (*reveal_tile)[u] = (*reveal_tile)[u] >> shift << (2*shift) >> shift;

    for (u32 u = 0; u < 3; u++)
      vsync();
  }

  for (u32 i = 0; i < flood_cover_entry_count; i++)
    *flood_cover_entries[i] = BLANK_TILE_ID;
}

void coverRevealRecurse(MapPosition position)
{
  if (!mapPositionIsValid(position))
    return;

  ScreenEntry* cover_entry = mapEntryPtr(COVER_SBB, position);
  if ((*cover_entry & ENTRY_ID_MASK) != COVER_TILE_ID)
    return;

  flood_cover_entries[flood_cover_entry_count++] = cover_entry;
  *cover_entry = REVEAL_TILE_ID | (*cover_entry & ~ENTRY_ID_MASK);

  ScreenEntry* mine_entry = mapEntryPtr(MINE_SBB, position);
  if (*mine_entry != NO_MINE_TILE_ID)
    return;

  for (s32 offset_x = -1; offset_x <= 1; offset_x++)
    for (s32 offset_y = -1; offset_y <= 1; offset_y++)
      {
        if(offset_x == 0 && offset_y == 0)
          continue;

        MapPosition neighbour_postion;
        neighbour_postion.x = position.x + offset_x;
        neighbour_postion.y = position.y + offset_y;

        coverRevealRecurse(neighbour_postion);
      }
}

void updateReticle()
{
  if (keyHit(KEYINPUT_LEFT | KEYINPUT_RIGHT | KEYINPUT_UP | KEYINPUT_DOWN))
  {
    reticle_move_repeat_delay = 16;
    if (keyHit(KEYINPUT_LEFT) && reticle_position.x > 0)
      reticle_position.x -= 1;
    if (keyHit(KEYINPUT_RIGHT) && reticle_position.x < MAP_WIDTH-1)
      reticle_position.x += 1;
    if (keyHit(KEYINPUT_UP) && reticle_position.y > 0)
      reticle_position.y -= 1;
    if (keyHit(KEYINPUT_DOWN) && reticle_position.y < MAP_HEIGHT-1)
      reticle_position.y += 1;
  }

  reticle_move_repeat_delay--;

  if (reticle_move_repeat_delay == 0)
  {
    reticle_move_repeat_delay = 4;
    if (keyHeld(KEYINPUT_LEFT) && reticle_position.x > 0)
      reticle_position.x -= 1;
    if (keyHeld(KEYINPUT_RIGHT) && reticle_position.x < MAP_WIDTH-1)
      reticle_position.x += 1;
    if (keyHeld(KEYINPUT_UP) && reticle_position.y > 0)
      reticle_position.y -= 1;
    if (keyHeld(KEYINPUT_DOWN) && reticle_position.y < MAP_HEIGHT-1)
      reticle_position.y += 1;
  }
  
  ObjectAttributes* attributes = &OBJ_ATTRIBUTES[RETICLE_OBJ];

  s32 pixel_x = reticle_position.x * 8 - 4;
  s32 pixel_y = reticle_position.y * 8 - 4;

  attributes->attr0 = OBJ_ATTR0_Y(pixel_y) | OBJ_ATTR0_SQUARE;
  attributes->attr1 = OBJ_ATTR1_X(pixel_x) | OBJ_ATTR1_SIZE_16;
}

void investigate(MapPosition position)
{
  ScreenEntry* cover_entry = mapEntryPtr(COVER_SBB, position);
  ScreenEntry* mine_entry = mapEntryPtr(MINE_SBB, position);

  if (*cover_entry == BLANK_TILE_ID)
    return;

  if ((*cover_entry & ENTRY_ID_MASK) == FLAG_TILE_ID)
    return;

  u32 mines_nearby;

  if (*mine_entry == MINE_TILE_ID)
    boom();
  else {
    mines_nearby = *mine_entry-NO_MINE_TILE_ID;
    bleep(mines_nearby > 3 ? 3 : mines_nearby);
  }

  coverReveal(position);
}

void toggleFlag(MapPosition position)
{
  ScreenEntry* cover_entry = mapEntryPtr(COVER_SBB, position);
  if ((*cover_entry & ENTRY_ID_MASK) == COVER_TILE_ID)
    *cover_entry = *cover_entry & ~ENTRY_ID_MASK | FLAG_TILE_ID;
  else if ((*cover_entry & ENTRY_ID_MASK) == FLAG_TILE_ID)
    *cover_entry = *cover_entry & ~ENTRY_ID_MASK | COVER_TILE_ID;
}