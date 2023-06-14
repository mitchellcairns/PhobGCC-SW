#ifndef CVIDEO_H
#define CVIDEO_H

#include "pv_includes.h"

#ifndef VWIDTH
#define VWIDTH 512
#endif

#ifndef VWIDTHBYTE
#define VWIDTHBYTE 256
#endif

#ifndef VHEIGHT
#define VHEIGHT 384
#endif

#ifndef BUFFERLEN
#define BUFFERLEN 98304 // 512*384/2
#endif

typedef enum
{
  LINEGRAPH,
  POINTGRAPH
} LineOrPoint_t;

int videoOut(const uint8_t pin_base,
             Buttons_u *btn,
             Buttons_u *hardware,
             RawStick_s *raw,
             ControlConfig_s *config,
             StickParams_s *aStick,
             StickParams_s *cStick,
             DataCapture_s *capture,
             volatile bool *extSync,
             volatile uint8_t *pleaseCommit,
             int *currentCalStep,
             const int version);

uint16_t getImageWidth(const unsigned char image[]);

uint16_t getImageHeight(const unsigned char image[]);

void drawImage(unsigned char bitmap[],
               const unsigned char image[],
               const unsigned char index[],
               const uint16_t x,
               const uint16_t y);

void drawLine(unsigned char bitmap[],
              const uint16_t x0,
              const uint16_t y0,
              const uint16_t x1,
              const uint16_t y1,
              const uint8_t color);

void drawString(unsigned char bitmap[],
                const uint16_t x,
                const uint16_t y,
                const uint8_t color,
                const char string[]
                /*const uint8_t charLimit*/);

void drawString2x(unsigned char bitmap[],
                  const uint16_t x,
                  const uint16_t y,
                  const uint8_t color,
                  const char string[]
                  /*const uint8_t charLimit*/);

void graphStickmap(unsigned char bitmap[],
                   const uint16_t x0,
                   const uint16_t y0,
                   const int x[],
                   const int y[],
                   const uint16_t length,
                   const uint8_t color,
                   const LineOrPoint_t graphType);

// drawFloat and drawInt draw numbers to the screen
// they handle negative numbers so that they don't misalign
// largestPower aligns the decimal point for numbers up to that power of ten;
//  for example, largestPower=1 aligns numbers up to 2 digits to the left of the decimal
// For floats, totalChars only writes up to that number of characters.
//  It truncates instead of rounding.
//  Only does something if it's at least largestPower+2, which should only show whole numbers and the negative sign
//  largestPower+3 makes it stop after writing the decimal point.
// 0123456789
//-x.y for largestPower 0
//-xx.y for largestPower 1
//-xxx.y for largestPower 2
void drawFloat(unsigned char bitmap[],
               const uint16_t x0,
               const uint16_t y0,
               const uint8_t color,
               const uint8_t largestPower,
               const uint8_t totalChars,
               const float number);

void drawFloat2x(unsigned char bitmap[],
                 const uint16_t x0,
                 const uint16_t y0,
                 const uint8_t color,
                 const uint8_t largestPower,
                 const uint8_t totalChars,
                 const float number);

void drawInt(unsigned char bitmap[],
             const uint16_t x0,
             const uint16_t y0,
             const uint8_t color,
             const uint8_t largestPower,
             const int number);

void drawInt2x(unsigned char bitmap[],
               const uint16_t x0,
               const uint16_t y0,
               const uint8_t color,
               const uint8_t largestPower,
               const int number);

void drawMenuFast(unsigned char bitmap[],
                  const unsigned int menu,
                  const int itemIndex,
                  const bool changeMade,
                  const int currentCalStep,
                  const Buttons_u *btn,
                  const Buttons_u *hardware,
                  const RawStick_s *raw,
                  const ControlConfig_s *controls,
                  const StickParams_s *aStick,
                  const StickParams_s *cStick);

void drawMenu(unsigned char bitmap[],
              const unsigned int menu,
              const int itemIndex,
              const bool changeMade,
              const int currentCalStep,
              const int version,
              const Buttons_u *btn,
              const RawStick_s *raw,
              const ControlConfig_s *controls,
              const StickParams_s *aStick,
              const StickParams_s *cStick,
              DataCapture_s *capture);

void handleMenuButtons(unsigned char bitmap[],
                       unsigned int *menu,
                       int *itemIndex,
                       uint8_t *redraw,
                       bool *changeMade,
                       const int currentCalStep,
                       volatile uint8_t *pleaseCommit, // for asking the other core to commit settings
                       const Buttons_u *btn,
                       const Buttons_u *hardware,
                       ControlConfig_s *controls,
                       DataCapture_s *capture);

void eraseCharLine(unsigned char bitmap[],
                   uint16_t y0);

void eraseRows(unsigned char bitmap[],
               uint16_t y0,
               uint16_t rows);

#endif // CVIDEO_H
