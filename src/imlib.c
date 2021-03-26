/* imlib.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "scrot.h"
#include "options.h"

Display *disp = NULL;
Visual *visual = NULL;
Screen *screen = NULL;
Colormap colormap;
int depth;
Window root = 0;

void init_x_and_imlib(char *dispstr, int screen_num)
{
	int screen_idx;

	disp = XOpenDisplay(dispstr);
	if (!disp) {
		fprintf(stderr, "Can't open X display. It *is* running, yeah?\n");
		exit(EXIT_FAILURE);
	}

	screen_num = (screen_num ? screen_num : DefaultScreen(disp));
	screen     = ScreenOfDisplay(disp, screen_num);
	screen_idx = XScreenNumberOfScreen(screen);
	visual     = DefaultVisual(disp, screen_idx);
	depth      = DefaultDepth(disp, screen_idx);
	colormap   = DefaultColormap(disp, screen_idx);
	root       = RootWindow(disp, screen_idx);

	imlib_context_set_display(disp);
	imlib_context_set_visual(visual);
	imlib_context_set_colormap(colormap);
	imlib_context_set_color_modifier(NULL);
	imlib_context_set_operation(IMLIB_OP_COPY);
}
