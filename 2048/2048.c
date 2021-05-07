/* (c) 2019 Lucas Towers - Licensed under MIT */

#include <advance.h>

#define GFX_MODE 0
#define MAIN_BG  0
#define MAIN_SBB 1

#define GLYPH_HEIGHT 12
#define GLYPH_SPACING 2

#define GLYPH_0   0
#define GLYPH_1   1
#define GLYPH_2   2
#define GLYPH_3   3
#define GLYPH_4   4
#define GLYPH_5   5
#define GLYPH_6   6
#define GLYPH_7   7
#define GLYPH_8   8
#define GLYPH_9   9
#define GLYPH_END 10

typedef struct {
  u8 width;
  u8 graphic[GLYPH_HEIGHT];
} Glyph;

const Glyph GLYPHS[] = {
  [GLYPH_0] =
  {
    6,
    {
      0b00011110,
      0b00111111,
      0b00110011,
      0b00110011,
      0b00110011,
      0b00110011,
      0b00110011,
      0b00110011,
      0b00110011,
      0b00110011,
      0b00111111,
      0b00011110
    }
  },
  [GLYPH_1] =
  {
    6,
    {
      0b00001100,
      0b00001110,
      0b00001111,
      0b00001111,
      0b00001100,
      0b00001100,
      0b00001100,
      0b00001100,
      0b00001100,
      0b00001100,
      0b00111111,
      0b00111111
    }
  },
  [GLYPH_2] =
  {
    6,
    {
      0b00011110,
      0b00111111,
      0b00110011,
      0b00110000,
      0b00111000,
      0b00011100,
      0b00001110,
      0b00000111,
      0b00000011,
      0b00000011,
      0b00111111,
      0b00111111
    }
  },
  [GLYPH_3] =
  {
    6,
    {
      0b00011111,
      0b00111111,
      0b00110000,
      0b00110000,
      0b00110000,
      0b00111110,
      0b00111110,
      0b00110000,
      0b00110000,
      0b00110000,
      0b00111111,
      0b00011111,
    }
  },
  [GLYPH_4] =
  {
    6,
    {
      0b00110011,
      0b00110011,
      0b00110011,
      0b00110011,
      0b00110011,
      0b00111111,
      0b00111110,
      0b00110000,
      0b00110000,
      0b00110000,
      0b00110000,
      0b00110000
    }
  },
  [GLYPH_5] =
  {
    6,
    {
      0b00111111,
      0b00111111,
      0b00000011,
      0b00000011,
      0b00000011,
      0b00011111,
      0b00111111,
      0b00110000,
      0b00110000,
      0b00110000,
      0b00111111,
      0b00011111,
    }
  },
  [GLYPH_6] =
  {
    6,
    {
      0b00111110,
      0b00111111,
      0b00000011,
      0b00000011,
      0b00000011,
      0b00011111,
      0b00111111,
      0b00110011,
      0b00110011,
      0b00110011,
      0b00111111,
      0b00011110,
    }
  },
  [GLYPH_7] =
  {
    6,
    {
      0b00111111,
      0b00111111,
      0b00110000,
      0b00111000,
      0b00011000,
      0b00011100,
      0b00001100,
      0b00001110,
      0b00000110,
      0b00000111,
      0b00000011,
      0b00000011,
    }
  },
  [GLYPH_8] =
  {
    6,
    {
      0b00011110,
      0b00111111,
      0b00110011,
      0b00110011,
      0b00110011,
      0b00011110,
      0b00111111,
      0b00110011,
      0b00110011,
      0b00110011,
      0b00111111,
      0b00011110
    }
  },
  [GLYPH_9] =
  {
    6,
    {
      0b00011110,
      0b00111111,
      0b00110011,
      0b00110011,
      0b00110011,
      0b00111111,
      0b00111110,
      0b00110000,
      0b00110000,
      0b00110000,
      0b00110000,
      0b00110000
    }
  }
};

const Tile ROUNDED_CORNER =
{
  0xFFFF0000,
  0xFFFFFF00,
  0xFFFFFFF0,
  0xFFFFFFF0,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF
};

u32 createPaletteMask(u32 palette)
{
  u32 palette_mask = 0x00000000;
  for (u32 i = 0; i < 32; i += 4) {
    palette_mask |= palette << i;
  }
  return palette_mask;
}

void horizontalFlipTile(const Tile* tile_src, Tile* tile_dest) {
  for (u32 i = 0; i < 8; i++) {
    u32 row = 0x00000000;
    for (u32 shift = 0; shift < 32; shift += 4) {
      row |= (((*tile_src)[i] >> shift) & 0xF) << (28-shift);
    }
    (*tile_dest)[i] = row;
  }
}

void verticalFlipTile(const Tile* tile_src, Tile* tile_dest) {
  for (u32 i = 0; i < 4; i++) {
    (*tile_dest)[i] = (*tile_src)[7-i];
    (*tile_dest)[7-i] = (*tile_src)[i];
  }
}

u32 renderRoundedRectangle(
  Tile* tile_base, u32 tile_columns,
  u32 tile_x, u32 tile_y,
  u32 tile_width, u32 tile_height,
  u32 palette
)
{
  vu32 palette_mask = createPaletteMask(palette);
  REG_DMA[3].source = &palette_mask;
  for (u32 ty = 0; ty < tile_height; ty++) {
    REG_DMA[3].destination = &tile_base[tile_x+(tile_y+ty)*tile_columns];
    REG_DMA[3].control = DMA_ENABLE | DMA_32 | DMA_SRC_FIXED | DMA_COUNT(tile_width*sizeof(Tile)/4);
  }
  Tile* tile_top_left = &tile_base[tile_x+tile_y*tile_columns];
  Tile* tile_top_right = &tile_base[tile_x+tile_width-1+tile_y*tile_columns];
  Tile* tile_bottom_left = &tile_base[tile_x+(tile_y+tile_height-1)*tile_columns];
  Tile* tile_bottom_right = &tile_base[tile_x+tile_width-1+(tile_y+tile_height-1)*tile_columns];
  for (u32 i = 0; i < 8; i++) {
    (*tile_top_left)[i] = ROUNDED_CORNER[i] & palette_mask;
  }
  horizontalFlipTile(tile_top_left, tile_top_right);
  verticalFlipTile(tile_top_left, tile_bottom_left);
  verticalFlipTile(tile_top_right, tile_bottom_right);
}

void renderGlyphs(
  const u8* glyph_string,
  Tile* tile_base, u32 tile_columns,
  u32 x, u32 y, u32 palette
)
{
  u32 palette_mask = createPaletteMask(palette);
  for (u32 gy = 0; gy < GLYPH_HEIGHT; gy++)
  {
    u32 mask = 0x00000000;
    u32 tile_x = x / 8;
    u32 pixel_x = x % 8;
    u32 tile_y = (y + gy) / 8;
    u32 pixel_y = (y + gy) % 8;
    for (u32 c = 0; glyph_string[c] != GLYPH_END; c++)
    {
      const Glyph* glyph = &GLYPHS[glyph_string[c]];
      for (u32 gx = 0; gx < glyph->width; gx++)
      {
        if ((glyph->graphic[gy] >> gx) & 1)
        {
          mask |= 0xF << (4 * pixel_x);
        }
        pixel_x++;
        if (gx == glyph->width - 1)
        {
          pixel_x += GLYPH_SPACING;
        }
        if (pixel_x >= 8)
        {
          Tile* tile = &tile_base[tile_x+tile_y*tile_columns];
          (*tile)[pixel_y] &= ~mask;
          (*tile)[pixel_y] |= mask & palette_mask;
          mask = 0x00000000;
          tile_x += pixel_x / 8;
          pixel_x %= 8;
        }
      }
    }
    Tile* tile = &tile_base[tile_x+tile_y*tile_columns];
    (*tile)[pixel_y] &= ~mask;
    (*tile)[pixel_y] |= mask & palette_mask;
  }
}

void reverseGlyphString(u8* string_start, u8* string_end)
{
  while (string_start < string_end)
  {
    u8 temp = *string_start;
    *string_start = *string_end;
    *string_end = temp;
    string_start++;
    string_end--;
  }
}

void numberToGlyphString(u32 n, u8* string)
{
  if (n == 0)
  {
    string[0] = GLYPH_0;
    string[1] = GLYPH_END;
    return;
  }
  u8* ptr = string;
  for (; n != 0; ptr++)
  {
    *ptr = GLYPH_0 + Mod(n, 10);
    n = Div(n, 10);
  }
  *ptr-- = GLYPH_END;
  reverseGlyphString(string, ptr);
}

void main()
{
  REG_DISPCNT = (
    DISPCNT_MODE(GFX_MODE) | DISPCNT_BG(MAIN_BG) |
    DISPCNT_OBJ | DISPCNT_OBJ_1D
  );
  BG_PALBANKS[0][0] = RGB8(250, 248, 239);
  OBJ_PALBANKS[0][0] = RGB8(255, 0, 0);
  OBJ_PALBANKS[0][1] = RGB8(0, 255, 0);
  OBJ_PALBANKS[0][2] = RGB8(0, 0, 255);
  for (u32 i = 0; i < 11; i++) {
    u32 tile_number = 2 << i;
    u8 string[5];
    numberToGlyphString(tile_number, string);
    renderRoundedRectangle(&OBJ_CHARBLOCKS[0][i*16], 4, 0, 0, 4, 4, 1);
    renderGlyphs(string, &OBJ_CHARBLOCKS[0][i*16], 4, 1, 10, 2);
  }
  while (true) {}
}