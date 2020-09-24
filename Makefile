all: 
	gcc x11copy.c -o x11copy -lX11
	gcc x11paste.c -o x11paste -lX11

clean:
	rm -f x11copy x11paste
