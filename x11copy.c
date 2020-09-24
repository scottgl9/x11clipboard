#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

Display * display;
Window window;
Atom targets_atom, text_atom, UTF8;
unsigned char *text;

static void XCopy() {
	XEvent event;
	Window owner;
	int size, R;
	Atom selection = XInternAtom(display, "CLIPBOARD", 0);
	XSetSelectionOwner (display, selection, window, 0);
	if (XGetSelectionOwner (display, selection) != window) return;
	while (1) {
		XNextEvent (display, &event);
		switch (event.type) {
			case SelectionRequest:
			size = strlen(text);
			if (event.xselectionrequest.selection != selection) break;

			XSelectionEvent ev = {0};
			ev.type = SelectionNotify;
			ev.display = event.xselectionrequest.display;
			ev.requestor = event.xselectionrequest.requestor;
			ev.selection = event.xselectionrequest.selection;
			ev.time = event.xselectionrequest.time;
			ev.target = event.xselectionrequest.target;
			ev.property = event.xselectionrequest.property;

			if (ev.target == targets_atom)
				R = XChangeProperty (ev.display, ev.requestor, ev.property, XA_ATOM, 32,
					PropModeReplace, (unsigned char*)&UTF8, 1);
			else if (ev.target == XA_STRING || ev.target == text_atom) 
				R = XChangeProperty(ev.display, ev.requestor, ev.property, XA_STRING, 8, PropModeReplace, text, size);
			else if (ev.target == UTF8)
				R = XChangeProperty(ev.display, ev.requestor, ev.property, UTF8, 8, PropModeReplace, text, size);
			else {
				ev.property = None;
				R = 0;
			}
			if ((R & 2) == 0) XSendEvent (display, ev.requestor, 0, 0, (XEvent *)&ev);
			break;
			case SelectionClear:
			return;
		}
	}
}

int main(int argc, char *argv[]) {
	display = XOpenDisplay(0);
	int N = DefaultScreen(display);
	window = XCreateSimpleWindow(display, RootWindow(display, N), 0, 0, 1, 1, 0,
		BlackPixel(display, N), WhitePixel(display, N));	
	targets_atom = XInternAtom(display, "TARGETS", 0);
	text_atom = XInternAtom(display, "TEXT", 0);
	UTF8 = XInternAtom(display, "UTF8_STRING", 1);
	if (UTF8 == None) UTF8 = XA_STRING;
	Atom selection = XInternAtom(display, "CLIPBOARD", 0);
	text = argv[1];
	XCopy();
}
