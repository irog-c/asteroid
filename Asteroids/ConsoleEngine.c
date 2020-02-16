#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#pragma warning(disable:4996)
#include "consoleEngine.h"

struct console_t
{
	uint16_t 	width;
	uint16_t 	height;
	CHAR_INFO 	*buff_screen;
	char 		*title;
	char		*font_sprite;

	HANDLE 		hConsole;
	HANDLE 		hConsoleIn;

	SMALL_RECT 	rectWindow;
};

static uint16_t keyOldState[256] = { 0 };
static uint16_t keyNewState[256] = { 0 };
static bool 	mouseOldState[5] = { 0 };
static bool 	mouseNewState[5] = { 0 };
static bool 	bConsoleInFocus = true;
static int16_t 	mouse_pos_x = 0;
static int16_t 	mouse_pos_y = 0;

struct keystate_t keys[256];
struct keystate_t mouse[5];

static inline void _constructFontSheet(console_t console)
{
#if 1
	char fontdata[] = "?Q`0001oOch0o01o@F40o0<AGD4090LAGD<090@A7ch0?00O7Q`0600>00000000" \
		"O000000nOT0063Qo4d8>?7a14Gno94AA4gno94AaOT0>o3`oO400o7QN00000400" \
		"Of80001oOg<7O7moBGT7O7lABET024@aBEd714AiOdl717a_=TH013Q>00000000" \
		"720D000V?V5oB3Q_HdUoE7a9@DdDE4A9@DmoE4A;Hg]oM4Aj8S4D84@`00000000" \
		"OaPT1000Oa`^13P1@AI[?g`1@A=[OdAoHgljA4Ao?WlBA7l1710007l100000000" \
		"ObM6000oOfMV?3QoBDD`O7a0BDDH@5A0BDD<@5A0BGeVO5ao@CQR?5Po00000000" \
		"Oc``000?Ogij70PO2D]??0Ph2DUM@7i`2DTg@7lh2GUj?0TO0C1870T?00000000" \
		"70<4001o?P<7?1QoHg43O;`h@GT0@:@LB@d0>:@hN@L0@?aoN@<0O7ao0000?000" \
		"OcH0001SOglLA7mg24TnK7ln24US>0PL24U140PnOgl0>7QgOcH0K71S0000A000" \
		"00H00000@Dm1S007@DUSg00?OdTnH7YhOfTL<7Yh@Cl0700?@Ah0300700000000" \
		"<008001QL00ZA41a@6HnI<1i@FHLM81M@@0LG81?O`0nC?Y7?`0ZA7Y300080000" \
		"O`082000Oh0827mo6>Hn?Wmo?6HnMb11MP08@C11H`08@FP0@@0004@000000000" \
		"00P00001Oab00003OcKP0006@6=PMgl<@440MglH@000000`@000001P00000000" \
		"Ob@8@@00Ob@8@Ga13R@8Mga172@8?PAo3R@827QoOb@820@0O`0007`0000007P0" \
		"O`000P08Od400g`<3V=P0G`673IP0`@3>1`00P@6O`P00g`<O`000GP800000000" \
		"?P9PL020O`<`N3R0@E4HC7b0@ET<ATB0@@l6C4B0O`H3N7b0?P01L3R000000020";
#else
	char fontdata[] = "6000000090`0o00o@Q80:005^R409045ZR409085_Q80900200`0600000000000" \
		"00000000<0006100300092XN2b8092XQ2cl_92XQ3200:1XQ<000o3aN00000000" \
		"00000000?b8003lo9C43?1@59BT022@=9BT312@E6RD012@R028021P000000000" \
		"300000004Q8D00008B4n91PB8BTD:R@U8BTn:R@Y4RDD4R@Y0180000B00000000" \
		"00000000?aP001P18A@T12@18A8Z?R@18Cmo92@o7Q0Z11@1000B03l100000001" \
		"00020000?aL501`O9BDE72XP9BD:82XP9BDD82XP8BDZ42XP01TX?0`O000@0000" \
		"00000001?ah030061BDF40PH1BDY83hP1BDY80XP0BDA400H01P@3006000X0001" \
		"300030004R40<1Po8A408:@@:@T04:@8:@D348P46P<087`80040<00@0000300o" \
		"00000000?aX0900Q12D<:3lB12DB40@<12DQ40@<12D0:3PB?aX0900Q00000000" \
		"0000000000H0Q0018BT0R0028BTQD004?bTB83dh8BT<40048Ah0300200000001" \
		"000000004000080Q800E980a8@0>=8@Y8B@O;7dU7`0>900S000E000Q00000000" \
		"00000000?`000000200833lo3408?a0Q4R@n8BPQ8@088B@00008000000000000" \
		"00000000?`P0000180P0000281A003l481@POb088280000@8280000P00000000" \
		"00000000?a@003`00Q@88@@Q11@88@PQ21@8?`Po11@830@00Q@003`0?`000000" \
		"00000000?b8040000R8023`211@020P121@P40@240P040@0?`P023`000000000" \
		"00000000008P?aPP7P4@?b@P8BT8?b@P8@T4?b@P8@H2?b@P7P01?aPP00000000";
#endif

	int16_t px = 0, py = 0;
	uint16_t b;

	for(b = 0; b < 1024; b += 4)
	{
		uint32_t sym1 = (uint32_t)fontdata[b + 0] - 48;
		uint32_t sym2 = (uint32_t)fontdata[b + 1] - 48;
		uint32_t sym3 = (uint32_t)fontdata[b + 2] - 48;
		uint32_t sym4 = (uint32_t)fontdata[b + 3] - 48;
		uint32_t r = sym1 << 18 | sym2 << 12 | sym3 << 6 | sym4;

		int16_t i;
		for(i = 0; i < 24; i++)
		{
			char k = r & (1 << i) ? 1 : 0;
			console->font_sprite[px + py * 128] = k;
			if(++py == 48)
			{
				px++;
				py = 0;
			}
		}
	}
}

console_t ConsoleEngine_init(console_t console, uint16_t width, uint16_t height, const char *name)
{
	console->width = (width << 1);
	console->height = height;

	console->hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	console->hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

	if(console->hConsole == INVALID_HANDLE_VALUE)
	{
		ConsoleEngine_free(console);
		return NULL;
	}

	ConsoleEngine_set_title(console, name);

	_constructFontSheet(console);

	memset(keyNewState, 0, 256 * sizeof(short));
	memset(keyOldState, 0, 256 * sizeof(short));
	memset(keys, 0, 256 * sizeof(struct keystate_t));

	console->rectWindow.Left = 0;
	console->rectWindow.Top = 0;
	console->rectWindow.Right = 1;
	console->rectWindow.Bottom = 1;

	SetConsoleWindowInfo(console->hConsole, TRUE, &console->rectWindow);

	COORD coord;
	coord.X = console->width;
	coord.Y = console->height;

	if(!SetConsoleScreenBufferSize(console->hConsole, coord))
	{
		ConsoleEngine_free(console);
		return NULL;
	}

	if(!SetConsoleActiveScreenBuffer(console->hConsole))
	{
		ConsoleEngine_free(console);
		return NULL;
	}

	CONSOLE_FONT_INFOEX cfi;

	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 2;
	cfi.dwFontSize.Y = 2;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy(cfi.FaceName, L"Consolas");
	if(!SetCurrentConsoleFontEx(console->hConsole, false, &cfi))
	{
		ConsoleEngine_free(console);
		return NULL;
	}

	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if(!GetConsoleScreenBufferInfo(console->hConsole, &csbi))
	{
		ConsoleEngine_free(console);
		return NULL;
	}
	if(console->height > csbi.dwMaximumWindowSize.Y)
	{
		ConsoleEngine_free(console);
		return NULL;
	}
	if(console->width > csbi.dwMaximumWindowSize.X)
	{
		ConsoleEngine_free(console);
		return NULL;
	}

	console->rectWindow.Left = 0;
	console->rectWindow.Top = 0;
	console->rectWindow.Right = console->width - 1;
	console->rectWindow.Bottom = console->height - 1;

	if(!SetConsoleWindowInfo(console->hConsole, TRUE, &console->rectWindow))
	{
		ConsoleEngine_free(console);
		return NULL;
	}

	if(!SetConsoleMode(console->hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT))
	{
		ConsoleEngine_free(console);
		return NULL;
	}

	console->buff_screen = (CHAR_INFO *)malloc(sizeof(CHAR_INFO) * (console->height * console->width));
	memset(console->buff_screen, 0, sizeof(CHAR_INFO) * console->width * console->height);

	console->width >>= 1;

	return console;
}

console_t ConsoleEngine_new(void)
{
	console_t ret = malloc(sizeof(struct console_t));
	ret->title = malloc(sizeof(char) * 128);
	ret->font_sprite = malloc(sizeof(char) * 128 * 48);
	return ret;
}

void ConsoleEngine_free(console_t console)
{
	free(console->buff_screen);
	free(console->title);
	free(console->font_sprite);
	free(console);
}

void ConsoleEngine_update(console_t console)
{
	COORD buffSize;
	buffSize.X = (console->width << 1);
	buffSize.Y = console->height;

	COORD buffCoord;
	buffCoord.X = 0;
	buffCoord.Y = 0;

	WriteConsoleOutput(console->hConsole, console->buff_screen, buffSize, buffCoord, &console->rectWindow);
}

void ConsoleEngine_clear(console_t console, uint16_t col)
{
	int i;
	for(i = 0; i < console->height*(console->width << 1); ++i)
	{
		console->buff_screen[i].Char.UnicodeChar = L' ';
		console->buff_screen[i].Attributes = col;
	}
}
// Added this for asteroid game [START]
void ConsoleEngine_wrap_coordinates(console_t console, float ix, float iy, float *ox, float *oy)
{
	*ox = ix;
	*oy = iy;
	if(ix < 0.0f)	*ox = ix + (float)ConsoleEngine_get_width(console);
	if(ix >= (float)ConsoleEngine_get_width(console))	*ox = ix - ConsoleEngine_get_width(console);
	if(iy < 0.0f)	*oy = iy + (float)ConsoleEngine_get_height(console);
	if(iy >= (float)ConsoleEngine_get_height(console)) *oy = iy - (float)ConsoleEngine_get_height(console);
}
// Added this for asteroid game [END]

void ConsoleEngine_drawpixel(console_t console, int16_t x, int16_t y, uint16_t col)
{
	// Added this for asteroid game [START]
	float fx, fy;
	ConsoleEngine_wrap_coordinates(console, x, y, &fx, &fy);
	x = fx;
	y = fy;
	// Added this for asteroid game [END]

	if(x >= 0 && (x << 1) < (console->width << 1) && y >= 0 && y < console->height)
	{
		console->buff_screen[y * (console->width << 1) + (x << 1)].Char.UnicodeChar = L' ';
		console->buff_screen[y * (console->width << 1) + ((x << 1) + 1)].Char.UnicodeChar = L' ';
		console->buff_screen[y * (console->width << 1) + (x << 1)].Attributes = col;
		console->buff_screen[y * (console->width << 1) + ((x << 1) + 1)].Attributes = col;
	}
}

void ConsoleEngine_drawline(console_t console, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t col)
{
	int16_t x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2 - x1;
	dy = y2 - y1;
	dx1 = abs(dx);
	dy1 = abs(dy);
	px = 2 * dy1 - dx1;
	py = 2 * dx1 - dy1;

	if(dy1 <= dx1)
	{
		if(dx >= 0)
		{
			x = x1;
			y = y1;
			xe = x2;
		}
		else
		{
			x = x2;
			y = y2;
			xe = x1;
		}

		ConsoleEngine_drawpixel(console, x, y, col);

		for(i = 0; x < xe; i++)
		{
			x++;
			if(px < 0) px = px + (dy1 << 1);
			else
			{
				if((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y++;
				else y--;
				px = px + ((dy1 - dx1) << 1);
			}
			ConsoleEngine_drawpixel(console, x, y, col);
		}
	}
	else
	{
		if(dy >= 0)
		{
			x = x1;
			y = y1;
			ye = y2;
		}
		else
		{
			x = x2;
			y = y2;
			ye = y1;
		}

		ConsoleEngine_drawpixel(console, x, y, col);

		for(i = 0; y < ye; i++)
		{
			y++;
			if(py <= 0) py = py + (dx1 << 1);
			else
			{
				if((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x++;
				else x--;
				py = py + ((dx1 - dy1) << 1);
			}

			ConsoleEngine_drawpixel(console, x, y, col);
		}
	}
}

uint16_t ConsoleEngine_get_width(console_t console)
{
	return console->width;
}

uint16_t ConsoleEngine_get_height(console_t console)
{
	return console->height;
}

void ConsoleEngine_set_title(console_t console, const char *name)
{
	strcpy(console->title, name);
	SetConsoleTitle(console->title);
}

static inline void _drawcircle(console_t console, int16_t xc, int16_t yc, int16_t x, int16_t y, uint16_t col)
{
	ConsoleEngine_drawpixel(console, xc + x, yc + y, col);
	ConsoleEngine_drawpixel(console, xc - x, yc + y, col);
	ConsoleEngine_drawpixel(console, xc + x, yc - y, col);
	ConsoleEngine_drawpixel(console, xc - x, yc - y, col);
	ConsoleEngine_drawpixel(console, xc + y, yc + x, col);
	ConsoleEngine_drawpixel(console, xc - y, yc + x, col);
	ConsoleEngine_drawpixel(console, xc + y, yc - x, col);
	ConsoleEngine_drawpixel(console, xc - y, yc - x, col);
}

void ConsoleEngine_drawstring(console_t console, int16_t x, int16_t y, int16_t scale, const char *str, uint16_t col)
{
	int16_t sx = 0;
	int16_t sy = 0;

	int16_t k;
	for(k = 0; k < strlen(str); k++)
	{
		if(str[k] == '\n')
		{
			sx = 0; sy += 8 * scale;
		}
		else
		{
			int16_t ox = (str[k] - 32) % 16;
			int16_t oy = (str[k] - 32) / 16;
			uint16_t i, j;

			if(scale > 1)
			{
				uint16_t is, js;
				for(i = 0; i < 8; i++)
					for(j = 0; j < 8; j++)
						if(console->font_sprite[i + (ox * 8) + (j + oy * 8) * 128] > 0)
							for(is = 0; is < scale; is++)
								for(js = 0; js < scale; js++)
									ConsoleEngine_drawpixel(console, x + sx + (i*scale) + is, y + sy + (j*scale) + js, col);
			}
			else
			{
				for(i = 0; i < 8; i++)
					for(j = 0; j < 8; j++)
						if(console->font_sprite[i + (ox * 8) + (j + oy * 8) * 128] > 0)
							ConsoleEngine_drawpixel(console, x + sx + i, y + sy + j, col);
			}
			sx += 8 * scale;
		}
	}
}

void ConsoleEngine_drawcircle(console_t console, int16_t xc, int16_t yc, int16_t r, uint16_t col)
{
	int16_t x = 0, y = r;
	int16_t d = 3 - 2 * r;
	_drawcircle(console, xc, yc, x, y, col);
	while(y >= x)
	{
		x++;
		if(d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else d = d + 4 * x + 6;
		_drawcircle(console, xc, yc, x, y, col);
	}
}

static inline void _readkb(void)
{
	int16_t i;

	for(i = 0; i < 256; i++)
	{
		keyNewState[i] = GetAsyncKeyState(i);

		keys[i].bPressed = false;
		keys[i].bReleased = false;

		if(keyNewState[i] != keyOldState[i])
		{
			if(keyNewState[i] & 0x8000)
			{
				keys[i].bPressed = !keys[i].bHeld;
				keys[i].bHeld = true;
			}
			else
			{
				keys[i].bReleased = true;
				keys[i].bHeld = false;
			}
		}

		keyOldState[i] = keyNewState[i];
	}
}

static inline void _readmouse(console_t console)
{
	INPUT_RECORD inBuf[32];
	DWORD events = 0;
	DWORD i;
	int16_t m;
	GetNumberOfConsoleInputEvents(console->hConsoleIn, &events);
	if(events > 0)
		ReadConsoleInput(console->hConsoleIn, inBuf, events, &events);

	for(i = 0; i < events; i++)
	{
		switch(inBuf[i].EventType)
		{
		case FOCUS_EVENT:
		{
			bConsoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
		}
		break;

		case MOUSE_EVENT:
		{
			switch(inBuf[i].Event.MouseEvent.dwEventFlags)
			{
			case MOUSE_MOVED:
			{
				mouse_pos_x = inBuf[i].Event.MouseEvent.dwMousePosition.X;
				mouse_pos_y = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
			}
			break;

			case 0:
			{
				for(m = 0; m < 5; m++)
					mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
			}
			break;

			default:
				break;
			}
		}
		break;

		default:
			break;
		}
	}

	for(m = 0; m < 5; m++)
	{
		mouse[m].bPressed = false;
		mouse[m].bReleased = false;

		if(mouseNewState[m] != mouseOldState[m])
		{
			if(mouseNewState[m])
			{
				mouse[m].bPressed = true;
				mouse[m].bHeld = true;
			}
			else
			{
				mouse[m].bReleased = true;
				mouse[m].bHeld = false;
			}
		}

		mouseOldState[m] = mouseNewState[m];
	}
}

void ConsoleEngine_readinput(console_t console)
{
	_readkb();
	_readmouse(console);
}

int16_t ConsoleEngine_get_mouse_x(void)
{
	return mouse_pos_x >> 1;
}

int16_t ConsoleEngine_get_mouse_y(void)
{
	return mouse_pos_y;
}

bool ConsoleEngine_in_focus(void)
{
	return bConsoleInFocus;
}

static int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}

int64_t ConsoleEngine_get_time_ms(void)
{
	static struct timeval tv;
	gettimeofday(&tv, NULL);


	return ((tv.tv_sec) * 1000.0 + (tv.tv_usec) / 1000.0);
}

void ConsoleEngine_drawtriangle(console_t console, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t col)
{
	ConsoleEngine_drawline(console, x1, y1, x2, y2, col);
	ConsoleEngine_drawline(console, x2, y2, x3, y3, col);
	ConsoleEngine_drawline(console, x3, y3, x1, y1, col);
}

int32_t ConsoleEngine_printf(console_t console, int16_t x, int16_t y, int16_t scale, uint16_t col, const char *format, ...)
{
	static char buff[1024];
	va_list arglist;
	va_start(arglist, format);
	int32_t ret = vsprintf(buff, format, arglist);
	va_end(arglist);

	ConsoleEngine_drawstring(console, x, y, scale, buff, col);

	return ret;
}