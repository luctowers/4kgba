#include <stdint.h>
#include <stdbool.h>

// TYPES

typedef uint8_t         u8;
typedef uint16_t        u16;
typedef uint32_t        u32;
typedef int8_t          s8;
typedef int16_t         s16;
typedef int32_t         s32;

typedef volatile u8     vu8;
typedef volatile u16    vu16;
typedef volatile u32    vu32;
typedef volatile u8     vs8;
typedef volatile u16    vs16;
typedef volatile u32    vs32;

typedef u16             Color;
typedef u32             Tile[8];
typedef u16             ScreenEntry;
typedef Color           Palbank[16];
typedef Tile            Charblock[512];
typedef ScreenEntry     Screenblock[1024];

typedef struct
{
  u16 attr0;
  u16 attr1;
  u16 attr2;
  s16 fill;
} ObjectAttributes;

typedef struct
{
  s16 x;
  s16 y;
} BackgroundScroll;

typedef volatile struct
{
  const volatile void* source;
  volatile void* destination;
  vu32 control;
} DmaChannel;

// MEMORY MAP

#define MEM_EWRAM       0x02000000
#define MEM_IWRAM       0x03000000
#define MEM_IO          0x04000000
#define MEM_PAL         0x05000000
#define MEM_VRAM        0x06000000
#define MEM_OAM         0x07000000
#define MEM_ROM         0x08000000
#define MEM_SRAM        0x0E000000

#define MEM_PAL_OBJ     (MEM_PAL  + 0x00000200)
#define MEM_VRAM_OBJ    (MEM_VRAM + 0x00010000)

// MEMORY POINTER CASTS

#define BG_PALETTE      ((Color*)MEM_PAL)
#define BG_PALBANKS     ((Palbank*)MEM_PAL)
#define BG_CHARBLOCKS   ((Charblock*)MEM_VRAM)
#define BG_SCREENBLOCKS ((Screenblock*)MEM_VRAM)
#define OBJ_PALETTE     ((Color*)MEM_PAL_OBJ)
#define OBJ_PALBANKS    ((Palbank*)MEM_PAL_OBJ)
#define OBJ_CHARBLOCKS  ((Charblock*)MEM_VRAM_OBJ)
#define OBJ_ATTRIBUTES  ((ObjectAttributes*)MEM_OAM)

// REGISTER POINTER CASTS

#define REG_DISPCNT      (*(vu32*)(MEM_IO+0x0000))
#define REG_VCOUNT       (*(vu16*)(MEM_IO+0x0006))
#define REG_BGCNT        ((vu16*)(MEM_IO+0x0008))
#define REG_BGOFS        ((BackgroundScroll*)(MEM_IO+0x0010))
#define REG_DMA          ((DmaChannel*)(MEM_IO+0x00B0))
#define REG_KEYINPUT     (*(vu16*)(MEM_IO+0x0130))
#define  REG_SOUNDCNT_L  (*(vu16*)(MEM_IO+0x0080))
#define  REG_SOUNDCNT_H  (*(vu16*)(MEM_IO+0x0082))
#define  REG_SOUNDCNT_X  (*(vu16*)(MEM_IO+0x0084))
#define  REG_SOUND1CNT_L (*(vu16*)(MEM_IO+0x0060))
#define  REG_SOUND1CNT_H (*(vu16*)(MEM_IO+0x0062))
#define  REG_SOUND1CNT_X (*(vu16*)(MEM_IO+0x0064))
#define  REG_SOUND4CNT_L (*(vu16*)(MEM_IO+0x0078))
#define  REG_SOUND4CNT_H (*(vu16*)(MEM_IO+0x007c))
#define  REG_SOUNDBIAS   (*(vu16*)(MEM_IO+0x0088))

// REGISTER VALUES DEFINITIONS

#define DISPCNT_MODE(n)             ((n)<<0)
#define DISPCNT_BG(n)               (1<<(8+(n)))
#define DISPCNT_OBJ_2D              0x0000
#define DISPCNT_OBJ_1D              0x0040
#define DISPCNT_BLANK               0x0080
#define DISPCNT_OBJ                 0x1000

#define BGCNT_PRIORITY(n)           ((n)<<0)
#define BGCNT_CHARBLOCK(n)          ((n)<<2)
#define BGCNT_SCREENBLOCK(n)        ((n)<<8)
#define BGCNT_REG_32x32             0x0000
#define BGCNT_REG_64x32             0x4000
#define BGCNT_REG_32x64             0x8000
#define BGCNT_REG_64x64             0xC000

#define DMA_COUNT(n)                ((n)<<0)
#define DMA_DST_INC                 0x00000000
#define DMA_DST_DEC                 0x00200000
#define DMA_DST_FIXED               0x00400000
#define DMA_DST_RELOAD              0x00600000
#define DMA_SRC_INC                 0x00000000
#define DMA_SRC_DEC                 0x00800000
#define DMA_SRC_FIXED               0x01000000
#define DMA_REPEAT                  0x02000000
#define DMA_16                      0x00000000
#define DMA_32                      0x04000000
#define DMA_ENABLE                  0x80000000

#define KEYINPUT_A                  0x0001
#define KEYINPUT_B                  0x0002
#define KEYINPUT_SELECT             0x0004
#define KEYINPUT_START              0x0008
#define KEYINPUT_RIGHT              0x0010
#define KEYINPUT_LEFT               0x0020
#define KEYINPUT_UP                 0x0040
#define KEYINPUT_DOWN               0x0080
#define KEYINPUT_R                  0x0100
#define KEYINPUT_L                  0x0200

#define SOUNDCNT_L_SOUND1_LEFT      0x0100
#define SOUNDCNT_L_SOUND2_LEFT      0x0200
#define SOUNDCNT_L_SOUND3_LEFT      0x0400
#define SOUNDCNT_L_SOUND4_LEFT      0x0800
#define SOUNDCNT_L_SOUND1_RIGHT     0x1000
#define SOUNDCNT_L_SOUND2_RIGHT     0x2000
#define SOUNDCNT_L_SOUND3_RIGHT     0x4000
#define SOUNDCNT_L_SOUND4_RIGHT     0x8000
#define SOUNDCNT_L_VOL_LEFT(n)      ((n)<<0)
#define SOUNDCNT_L_VOL_RIGHT(n)     ((n)<<4)
#define SOUNDCNT_H_DMG25            0x0000
#define SOUNDCNT_H_DMG50            0x0001
#define SOUNDCNT_H_DMG100           0x0002
#define SOUNDCNT_X_SOUND1           0x0001
#define SOUNDCNT_X_SOUND2           0x0002
#define SOUNDCNT_X_SOUND3           0x0004
#define SOUNDCNT_X_SOUND4           0x0008
#define SOUNDCNT_X_DISABLE          0x0000
#define SOUNDCNT_X_ENABLE           0x0080

#define SOUND1CNT_L_SWEEP_INC       0x0000
#define SOUND1CNT_L_SWEEP_DEC       0x0008
#define SOUND1CNT_L_SWEEP_SHIFT(n)  ((n)<<0)
#define SOUND1CNT_L_SWEEP_TIME(n)   ((n)<<4)
#define SOUND1CNT_H_DUTY1_8         0x0000
#define SOUND1CNT_H_DUTY1_4         0x0040
#define SOUND1CNT_H_DUTY1_2         0x0080
#define SOUND1CNT_H_DUTY3_4         0x00C0
#define SOUND1CNT_H_ENV_DEC         0x0000
#define SOUND1CNT_H_ENV_INC         0x0800
#define SOUND1CNT_H_LENGTH(n)       ((n)<<0)
#define SOUND1CNT_H_ENV_TIME(n)     ((n)<<8)
#define SOUND1CNT_H_ENV_INIT(n)     ((n)<<12)
#define SOUND1CNT_X_HOLD            0x0000
#define SOUND1CNT_X_TIMED           0x4000
#define SOUND1CNT_X_RESET           0x8000
#define SOUND1CNT_X_RATE(n)         ((n)<<0)

#define SOUND4CNT_L_ENV_DEC         0x0000
#define SOUND4CNT_L_ENV_INC         0x0800
#define SOUND4CNT_L_LENGTH(n)       ((n)<<0)
#define SOUND4CNT_L_ENV_TIME(n)     ((n)<<8)
#define SOUND4CNT_L_ENV_INIT(n)     ((n)<<12)
#define SOUND4CNT_H_CLK_DIV(n)      ((n)<<0)
#define SOUND4CNT_H_STAGES_7        0x0008
#define SOUND4CNT_H_STAGES_15       0x0000
#define SOUND4CNT_H_PRESTEP(n)      ((n)<<4)
#define SOUND4CNT_H_HOLD            0x0000
#define SOUND4CNT_H_TIMED           0x4000
#define SOUND4CNT_H_RESET           0x8000

// TYPE VALUE DEFINITIONS

#define SCREEN_ENTRY_HFLIP          0x0400
#define SCREEN_ENTRY_VFLIP          0x0800
#define SCREEN_ENTRY_ID(n)          ((n)<<0)
#define SCREEN_ENTRY_PALBANK(n)     ((n)<<12)

#define OBJ_ATTR0_HIDE              0x0200
#define OBJ_ATTR0_SQUARE            0x0000
#define OBJ_ATTR0_WIDE              0x4000
#define OBJ_ATTR0_TALL              0x8000
#define OBJ_ATTR0_Y(n)              (((n)<<0)&0x00FF)

#define OBJ_ATTR1_HFLIP             0x1000
#define OBJ_ATTR1_VFLIP             0x2000
#define OBJ_ATTR1_SIZE_8            0x0000
#define OBJ_ATTR1_SIZE_16           0x4000
#define OBJ_ATTR1_SIZE_32           0x8000
#define OBJ_ATTR1_SIZE_64           0xC000
#define OBJ_ATTR1_X(n)              (((n)<<0)&0x01FF)

#define OBJ_ATTR2_TILE_ID(n)        ((n)<<0)
#define OBJ_ATTR2_PRIO(n)           ((n)<<10)
#define OBJ_ATTR2_PALBANK(n)        ((n)<<12)

// UTILITY MACROS

#define RGB5(r,g,b) (((r)<<0)|((g)<<5)|((b)<<10))
#define RGB8(r,g,b) (((r)>>3<<0)|((g)>>3<<5)|((b)>>3<<10))

// BIOS CALLS

int Mod(s32 num, s32 den);
int Div(s32 num, s32 den);
