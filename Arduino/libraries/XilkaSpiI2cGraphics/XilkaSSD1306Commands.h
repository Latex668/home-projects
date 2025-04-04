#pragma once

#define CMD_SETCONTRAST (0x81)
#define CMD_DISPLAYALLON_RESUME (0xA4)
#define CMD_DISPLAYALLON (0xA5)
#define CMD_NORMALDISPLAY (0xA6)
#define CMD_INVERTDISPLAY (0xA7)
#define CMD_DISPLAYOFF (0xAE)
#define CMD_DISPLAYON (0xAF)

#define CMD_SETDISPLAYOFFSET (0xD3)
#define CMD_SETCOMPINS (0xDA)

#define CMD_SETVCOMDETECT (0xDB)

#define CMD_SETDISPLAYCLOCKDIV (0xD5)
#define CMD_SETPRECHARGE (0xD9)

#define CMD_SETMULTIPLEX (0xA8)

#define CMD_SETLOWCOLUMN (0x00)
#define CMD_SETHIGHCOLUMN (0x10)

#define CMD_SETSTARTLINE (0x40)

#define CMD_MEMORYMODE (0x20)
#define CMD_COLUMNADDR (0x21)
#define CMD_PAGEADDR (0x22)

#define CMD_COMSCANINC (0xC0)
#define CMD_COMSCANDEC (0xC8)

#define CMD_SEGREMAP (0xA0)

#define CMD_CHARGEPUMP (0x8D)

// Scrolling
#define CMD_ACTIVATE_SCROLL (0x2F)
#define CMD_DEACTIVATE_SCROLL (0x2E)
#define CMD_SET_VERTICAL_SCROLL_AREA (0xA3)
#define CMD_RIGHT_HORIZONTAL_SCROLL (0x26)
#define CMD_LEFT_HORIZONTAL_SCROLL (0x27)
#define CMD_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL (0x29)
#define CMD_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL (0x2A)

