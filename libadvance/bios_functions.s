.text

.align 2;
.thumb_func;
.global Div;
Div:
  swi  0x06
  bx   lr

.align 2;
.thumb_func;
.global Mod;
Mod:
  swi  0x06
  mov  r0, r1
  bx   lr
