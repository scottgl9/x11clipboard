#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

static Display * display;
static Window window;
static Atom UTF8; 

char * XPasteType(Atom atom) {
	XEvent ev;
	int format;
	unsigned long N, size;
	char * data, * s = 0;
	Atom target, clipboard, xsel;
	clipboard = XInternAtom(display, "CLIPBOARD", 0),
	xsel = XInternAtom(display, "XSEL_DATA", 0);
	XConvertSelection(display, clipboard, atom, xsel, window, CurrentTime);
	XSync(display, 0);
	XNextEvent(display, &ev);

	switch(ev.type) {
		case SelectionNotify:
		if(ev.xselection.selection != clipboard) break;
		if(ev.xselection.property) {
			XGetWindowProperty(ev.xselection.display, ev.xselection.requestor,
				ev.xselection.property, 0L,(~0L), 0, AnyPropertyType, &target,
				&format, &size, &N,(unsigned char**)&data);
			if(target == UTF8 || target == XA_STRING) {
				s = strndup(data, size);
				XFree(data);
			}
			XDeleteProperty(ev.xselection.display, ev.xselection.requestor, ev.xselection.property);
		}
	}
  return s;
}

char *XPaste() {
	char * c = 0;
	UTF8 = XInternAtom(display, "UTF8_STRING", True);
	if(UTF8 != None) c = XPasteType(UTF8);
	if(!c) c = XPasteType(XA_STRING);
	return c;
}

int main(int argc, char *argv[]) {
	display = XOpenDisplay(0);
	int N = DefaultScreen(display);
	window = XCreateSimpleWindow(display, RootWindow(display, N), 0, 0, 1, 1, 0,
		BlackPixel(display, N), WhitePixel(display, N)
	);
	char *c = XPaste();
	if (c != NULL) printf("%s\n", c);
}
