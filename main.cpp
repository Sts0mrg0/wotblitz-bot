#include <iostream>
#include <fstream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/X.h>

#include <string.h> 
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

const int SKIP = 10;
const int COLOR_ZONE = 32*64;
const int PADDING = 148;
int MOUSE_SENX = 1;
int MOUSE_SENY = 1;

const int LOW_SEN = 5;
const int HIGH_SEN = 32;
const int HIGH_ZONE = 90;

int needR = 57311;
int needG = 5911;
int needB = 6425;

void click(Display *display, int button) {
  // Create and setting up the event
  XEvent event;
  memset (&event, 0, sizeof (event));
  event.xbutton.button = button;
  event.xbutton.same_screen = True;
  event.xbutton.subwindow = DefaultRootWindow (display);
  while (event.xbutton.subwindow)
    {
      event.xbutton.window = event.xbutton.subwindow;
      XQueryPointer (display, event.xbutton.window,
		     &event.xbutton.root, &event.xbutton.subwindow,
		     &event.xbutton.x_root, &event.xbutton.y_root,
		     &event.xbutton.x, &event.xbutton.y,
		     &event.xbutton.state);
    }
  // Press
  event.type = ButtonPress;
  if (XSendEvent (display, PointerWindow, True, ButtonPressMask, &event) == 0)
    fprintf (stderr, "Error to send the event!\n");
  XFlush (display);
  usleep (1);
  // Release
  event.type = ButtonRelease;
  if (XSendEvent (display, PointerWindow, True, ButtonReleaseMask, &event) == 0)
    fprintf (stderr, "Error to send the event!\n");
  XFlush (display);
  usleep (1);
}


int main() {
    Display* d = XOpenDisplay(NULL);
    Screen*  s = DefaultScreenOfDisplay(d);
    Window root = DefaultRootWindow(d);

    XColor c;    
    int finish;
    while(true) {
        XImage *image = XGetImage(d, root, 0, 0, s->width, s->height, AllPlanes, ZPixmap);

        finish = 0;
        for(int x = PADDING; x < s->width - PADDING; x += SKIP) {
            if(finish == 1) break;
            for(int y = PADDING; y < s->height - PADDING; y += SKIP) {
                if(finish == 1) break;

                c.pixel = XGetPixel(image, x, y);
                //printf("%d, %d, %d\n", c.red, c.green, c.blue);
                XQueryColor(d, DefaultColormap(d, DefaultScreen (d)), &c);

                if(c.red - COLOR_ZONE < needR && c.red + COLOR_ZONE > needR)
                if(c.green - COLOR_ZONE < needG && c.green + COLOR_ZONE > needG)
                if(c.blue - COLOR_ZONE < needB && c.blue + COLOR_ZONE > needB) {
                    int mX = 0;
                    int mY = 0;

                    if(x > s->width / 2) mX += x - (s->width / 2);
                    if(x < s->width / 2) mX -= (s->width / 2) - x;
                    if(y > s->height / 2) mY += y - (s->height / 2);
                    if(y < s->height / 2) mY -= (s->height / 2) - y;

                    if(mX > HIGH_ZONE) MOUSE_SENX = HIGH_SEN;
                    else MOUSE_SENX = LOW_SEN;
                    if(mY > HIGH_ZONE) MOUSE_SENY = HIGH_SEN;
                    else MOUSE_SENY = LOW_SEN;

                    mX *= MOUSE_SENX;
                    mY *= MOUSE_SENY;

                    XWarpPointer(d, None, None, 0, 0, 0, 0, mX, mY + 100);
                    click(d, Button1);

                    finish = 1;
                }
            }
        }
    }
}