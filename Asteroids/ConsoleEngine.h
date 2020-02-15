#ifndef INTERFACE_H
#define INTERFACE_H


#pragma warning(disable:4996)

#define _CRT_SECURE_NO_WARNINGS

#include <stdbool.h>
#include <Windows.h>
#include <stdarg.h>
#include <stdint.h>

typedef struct console_t *console_t;

struct keystate_t
{
	bool bPressed;
	bool bReleased;
	bool bHeld;
};

extern struct keystate_t keys[256];
extern struct keystate_t mouse[5];

enum ConsoleEngine_keys
{
	BACKSPACE = VK_BACK,
	ENTER = VK_RETURN,
	TAB = VK_TAB,
	CTRL = VK_CONTROL,
	LSHIFT = VK_LSHIFT,
	RSHIFT = VK_RSHIFT,
	CLEAR = VK_CLEAR,
	ESCAPE = VK_ESCAPE,
	ALT = VK_MENU,
	PAUSE = VK_PAUSE,
	CAPSLOCK = VK_CAPITAL,
	SPACE = VK_SPACE,
	END = VK_END,
	HOME = VK_HOME,
	UPARROW = VK_UP,
	RIGHTARROW = VK_RIGHT,
	DOWNARROW = VK_DOWN,
	LEFTARROW = VK_LEFT,
	SELECT = VK_SELECT,
	PRINT = VK_PRINT,
	PRINTSCREEN = VK_SNAPSHOT,
	INSERT = VK_INSERT,
	DEL = VK_DELETE
};

enum ConsoleEngine_color
{
	BLACK = 0x0000,
	DARK_BLUE = 0x0010,
	DARK_GREEN = 0x0020,
	DARK_CYAN = 0x0030,
	DARK_RED = 0x0040,
	DARK_MAGENTA = 0x0050,
	DARK_YELLOW = 0x0060,
	GREY = 0x0070,
	DARK_GREY = 0x0080,
	BLUE = 0x0090,
	GREEN = 0x00A0,
	CYAN = 0x00B0,
	RED = 0x00C0,
	MAGENTA = 0x00D0,
	YELLOW = 0x00E0,
	WHITE = 0x00F0
};

extern console_t	ConsoleEngine_init(console_t console, uint16_t width, uint16_t height, const char *name);
extern void			ConsoleEngine_update(console_t console);
extern void			ConsoleEngine_clear(console_t console, uint16_t col);
extern void			ConsoleEngine_drawpixel(console_t console, int16_t x, int16_t y, uint16_t col);
extern void			ConsoleEngine_drawcircle(console_t console, int16_t x, int16_t y, int16_t r, uint16_t col);
extern void			ConsoleEngine_drawline(console_t console, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t col);
extern void 		ConsoleEngine_drawtriangle(console_t console, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t col);
extern console_t	ConsoleEngine_new(void);
extern uint16_t		ConsoleEngine_get_width(console_t console);
extern uint16_t		ConsoleEngine_get_height(console_t console);
extern void			ConsoleEngine_free(console_t console);
extern void			ConsoleEngine_set_title(console_t console, const char *name);
extern void			ConsoleEngine_drawstring(console_t console, int16_t x, int16_t y, int16_t scale, const char *str, uint16_t col);
extern void			ConsoleEngine_readinput(console_t console);
extern int16_t		ConsoleEngine_get_mouse_x(void);
extern int16_t		ConsoleEngine_get_mouse_y(void);
extern bool			ConsoleEngine_in_focus(void);
extern int64_t		ConsoleEngine_get_time_ms(void);
extern int32_t		ConsoleEngine_printf(console_t console, int16_t x, int16_t y, int16_t scale, uint16_t col, const char *format, ...);
// Added this for asteroid game
extern void 		ConsoleEngine_wrap_coordinates(console_t console, float ix, float iy, float *ox, float *oy);
#endif