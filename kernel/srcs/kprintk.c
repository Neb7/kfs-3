#include "kernel.h"
#include "vga.h"
#include "kprintk.h"
#include <stdarg.h>

// VGA foreground colors for each log level (black background)
static uint8_t	g_level_colors[8] = {
	0x4F,	// 0 EMERG   - fond rouge, texte blanc
	0x0C,	// 1 ALERT   - rouge vif
	0x04,	// 2 CRIT    - rouge
	0x0C,	// 3 ERR     - rouge vif
	0x0E,	// 4 WARNING - jaune
	0x0B,	// 5 NOTICE  - cyan vif
	0x0F,	// 6 INFO    - blanc
	0x08,	// 7 DEBUG   - gris sombre
};

// Retourne le niveau (0-7) si le format commence par "<N>", -1 sinon.
// Avance *fmt après le préfixe si trouvé.
static int	parse_level(const char **fmt)
{
	if ((*fmt)[0] == '<'
		&& (*fmt)[1] >= '0' && (*fmt)[1] <= '7'
		&& (*fmt)[2] == '>')
	{
		int	level = (*fmt)[1] - '0';
		*fmt += 3;
		return (level);
	}
	return (-1);
}

int	kprintk(const char *fmt, ...)
{
	va_list	args;
	int		level;
	uint8_t	saved_color;
	int		ret;

	level = parse_level(&fmt);
	saved_color = g_screens[g_cur].color;
	if (level >= 0)
		g_screens[g_cur].color = g_level_colors[level];
	va_start(args, fmt);
	ret = vkprintf(fmt, args);
	va_end(args);
	g_screens[g_cur].color = saved_color;
	return (ret);
}
