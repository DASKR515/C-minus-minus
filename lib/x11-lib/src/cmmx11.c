#ifndef CMX11_H
#define CMX11_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void mXOpenDisplay(void* result, const char* name) {
    Display* dpy = XOpenDisplay(name);
    memcpy(result, &dpy, sizeof(dpy));
}

void mXCloseDisplay(void* dpy_ptr) {
    Display* dpy; memcpy(&dpy, dpy_ptr, sizeof(dpy));
    if (dpy) XCloseDisplay(dpy);
}

void mXFlush(void* dpy_ptr) {
    Display* dpy; memcpy(&dpy, dpy_ptr, sizeof(dpy));
    if (dpy) XFlush(dpy);
}

void mXSync(void* dpy_ptr, int discard) {
    Display* dpy; memcpy(&dpy, dpy_ptr, sizeof(dpy));
    if (dpy) XSync(dpy, discard);
}

void mXDefaultRootWindow(void* result, void* dpy_ptr) {
    Display* dpy; memcpy(&dpy, dpy_ptr, sizeof(dpy));
    Window root = DefaultRootWindow(dpy);
    memcpy(result, &root, sizeof(root));
}

void mXDefaultScreen(void* result, void* dpy_ptr) {
    Display* dpy; memcpy(&dpy, dpy_ptr, sizeof(dpy));
    int scr = DefaultScreen(dpy);
    memcpy(result, &scr, sizeof(scr));
}

void mXDisplayWidth(void* result, void* dpy_ptr, int scr) {
    Display* dpy; memcpy(&dpy, dpy_ptr, sizeof(dpy));
    int w = DisplayWidth(dpy, scr);
    memcpy(result, &w, sizeof(w));
}

void mXDisplayHeight(void* result, void* dpy_ptr, int scr) {
    Display* dpy; memcpy(&dpy, dpy_ptr, sizeof(dpy));
    int h = DisplayHeight(dpy, scr);
    memcpy(result, &h, sizeof(h));
}

void mXBlackPixel(void* result, void* dpy_ptr, int scr) {
    Display* dpy; memcpy(&dpy, dpy_ptr, sizeof(dpy));
    unsigned long pixel = BlackPixel(dpy, scr);
    memcpy(result, &pixel, sizeof(pixel));
}

void mXWhitePixel(void* result, void* dpy_ptr, int scr) {
    Display* dpy; memcpy(&dpy, dpy_ptr, sizeof(dpy));
    unsigned long pixel = WhitePixel(dpy, scr);
    memcpy(result, &pixel, sizeof(pixel));
}

void mXCreateSimpleWindow(void* result, void* dpy_ptr, void* parent_ptr, int x, int y, unsigned int w, unsigned int h, unsigned int border_w, unsigned long border_c, unsigned long bg) {
    Display* dpy; Window parent;
    memcpy(&dpy, dpy_ptr, sizeof(dpy));
    memcpy(&parent, parent_ptr, sizeof(parent));
    Window win = XCreateSimpleWindow(dpy, parent, x, y, w, h, border_w, border_c, bg);
    memcpy(result, &win, sizeof(win));
}

void mXCreateWindow(void* result, void* dpy_ptr, void* parent_ptr, int x, int y, unsigned int w, unsigned int h, unsigned int border_w, int depth, unsigned int class_type, void* visual_ptr, unsigned long valuemask, void* attributes_ptr) {
    Display* dpy; Window parent; Visual* vis; XSetWindowAttributes* attr;
    memcpy(&dpy, dpy_ptr, sizeof(dpy));
    memcpy(&parent, parent_ptr, sizeof(parent));
    memcpy(&vis, visual_ptr, sizeof(vis));
    memcpy(&attr, attributes_ptr, sizeof(attr));
    Window win = XCreateWindow(dpy, parent, x, y, w, h, border_w, depth, class_type, vis, valuemask, attr);
    memcpy(result, &win, sizeof(win));
}

void mXMapWindow(void* dpy_ptr, void* win_ptr) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XMapWindow(dpy, win); XFlush(dpy);
}

void mXMapRaised(void* dpy_ptr, void* win_ptr) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XMapRaised(dpy, win); XFlush(dpy);
}

void mXUnmapWindow(void* dpy_ptr, void* win_ptr) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XUnmapWindow(dpy, win); XFlush(dpy);
}

void mXDestroyWindow(void* dpy_ptr, void* win_ptr) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XDestroyWindow(dpy, win); XFlush(dpy);
}

void mXMoveWindow(void* dpy_ptr, void* win_ptr, int x, int y) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XMoveWindow(dpy, win, x, y); XFlush(dpy);
}

void mXResizeWindow(void* dpy_ptr, void* win_ptr, unsigned int w, unsigned int h) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XResizeWindow(dpy, win, w, h); XFlush(dpy);
}

void mXMoveResizeWindow(void* dpy_ptr, void* win_ptr, int x, int y, unsigned int w, unsigned int h) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XMoveResizeWindow(dpy, win, x, y, w, h); XFlush(dpy);
}

void mXSetWindowBorderWidth(void* dpy_ptr, void* win_ptr, unsigned int width) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XSetWindowBorderWidth(dpy, win, width); XFlush(dpy);
}

void mXSetWindowBorder(void* dpy_ptr, void* win_ptr, unsigned long border_pixel) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XSetWindowBorder(dpy, win, border_pixel); XFlush(dpy);
}

void mXSetWindowBackground(void* dpy_ptr, void* win_ptr, unsigned long color) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XSetWindowBackground(dpy, win, color); XClearWindow(dpy, win); XFlush(dpy);
}

void mXClearWindow(void* dpy_ptr, void* win_ptr) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XClearWindow(dpy, win); XFlush(dpy);
}

void mXClearArea(void* dpy_ptr, void* win_ptr, int x, int y, unsigned int w, unsigned int h, int exposures) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XClearArea(dpy, win, x, y, w, h, exposures); XFlush(dpy);
}

void mXRaiseWindow(void* dpy_ptr, void* win_ptr) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XRaiseWindow(dpy, win); XFlush(dpy);
}

void mXLowerWindow(void* dpy_ptr, void* win_ptr) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XLowerWindow(dpy, win); XFlush(dpy);
}

void mXSetWindowTitle(void* dpy_ptr, void* win_ptr, const char* title) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XStoreName(dpy, win, title); XFlush(dpy);
}

void mXSelectInput(void* dpy_ptr, void* win_ptr, long event_mask) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XSelectInput(dpy, win, event_mask);
}

void mXAllocEvent(void* result) {
    XEvent* ev = (XEvent*)malloc(sizeof(XEvent));
    memcpy(result, &ev, sizeof(ev));
}

void mXFreeEvent(void* ev_ptr) {
    XEvent* ev; memcpy(&ev, ev_ptr, sizeof(ev));
    if (ev) free(ev);
}

void mXNextEvent(void* dpy_ptr, void* ev_ptr) {
    Display* dpy; XEvent* ev;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&ev, ev_ptr, sizeof(ev));
    XNextEvent(dpy, ev);
}

void mXPeekEvent(void* dpy_ptr, void* ev_ptr) {
    Display* dpy; XEvent* ev;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&ev, ev_ptr, sizeof(ev));
    XPeekEvent(dpy, ev);
}

void mXPending(void* result, void* dpy_ptr) {
    Display* dpy; memcpy(&dpy, dpy_ptr, sizeof(dpy));
    int count = XPending(dpy);
    memcpy(result, &count, sizeof(count));
}

void mXCheckTypedEvent(void* result, void* dpy_ptr, int event_type, void* ev_ptr) {
    Display* dpy; XEvent* ev;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&ev, ev_ptr, sizeof(ev));
    Bool found = XCheckTypedEvent(dpy, event_type, ev);
    int res = found ? 1 : 0;
    memcpy(result, &res, sizeof(res));
}

void mXGetEventType(void* result, void* ev_ptr) {
    XEvent* ev; memcpy(&ev, ev_ptr, sizeof(ev));
    int type = ev->type;
    memcpy(result, &type, sizeof(type));
}

void mXGetButtonCoordinates(void* x_ptr, void* y_ptr, void* ev_ptr) {
    XEvent* ev; memcpy(&ev, ev_ptr, sizeof(ev));
    int x = 0, y = 0;
    if (ev->type == ButtonPress || ev->type == ButtonRelease || ev->type == MotionNotify) {
        x = ev->xbutton.x; y = ev->xbutton.y;
    }
    memcpy(x_ptr, &x, sizeof(x)); memcpy(y_ptr, &y, sizeof(y));
}

void mXGetPointerButton(void* result, void* ev_ptr) {
    XEvent* ev; memcpy(&ev, ev_ptr, sizeof(ev));
    unsigned int button = 0;
    if (ev->type == ButtonPress || ev->type == ButtonRelease) {
        button = ev->xbutton.button;
    }
    memcpy(result, &button, sizeof(button));
}

void mXGetKeyCode(void* result, void* ev_ptr) {
    XEvent* ev; memcpy(&ev, ev_ptr, sizeof(ev));
    unsigned int keycode = 0;
    if (ev->type == KeyPress || ev->type == KeyRelease) {
        keycode = ev->xkey.keycode;
    }
    memcpy(result, &keycode, sizeof(keycode));
}

void mXLookupString(void* result_str, void* keysym_res, void* ev_ptr, int max_len) {
    XEvent* ev; memcpy(&ev, ev_ptr, sizeof(ev));
    KeySym keysym;
    int count = XLookupString(&ev->xkey, (char*)result_str, max_len, &keysym, NULL);
    ((char*)result_str)[count] = '\0';
    memcpy(keysym_res, &keysym, sizeof(keysym));
}

void mXIsButtonClicked(void* result, void* ev_ptr, int bx, int by, int bw, int bh) {
    XEvent* ev; memcpy(&ev, ev_ptr, sizeof(ev));
    int inside = 0;
    if (ev->type == ButtonPress) {
        int cx = ev->xbutton.x, cy = ev->xbutton.y;
        if (cx >= bx && cx <= (bx + bw) && cy >= by && cy <= (by + bh)) inside = 1;
    }
    memcpy(result, &inside, sizeof(inside));
}

void mXCreateGC(void* result, void* dpy_ptr, void* win_ptr) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    GC gc = XCreateGC(dpy, win, 0, NULL);
    memcpy(result, &gc, sizeof(gc));
}

void mXFreeGC(void* dpy_ptr, void* gc_ptr) {
    Display* dpy; GC gc;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&gc, gc_ptr, sizeof(gc));
    XFreeGC(dpy, gc);
}

void mXSetForeground(void* dpy_ptr, void* gc_ptr, unsigned long color) {
    Display* dpy; GC gc;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&gc, gc_ptr, sizeof(gc));
    XSetForeground(dpy, gc, color);
}

void mXSetBackground(void* dpy_ptr, void* gc_ptr, unsigned long color) {
    Display* dpy; GC gc;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&gc, gc_ptr, sizeof(gc));
    XSetBackground(dpy, gc, color);
}

void mXSetLineWidth(void* dpy_ptr, void* gc_ptr, unsigned int width) {
    Display* dpy; GC gc;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&gc, gc_ptr, sizeof(gc));
    XSetLineAttributes(dpy, gc, width, LineSolid, CapButt, JoinMiter);
}

void mXDrawPoint(void* dpy_ptr, void* win_ptr, void* gc_ptr, int x, int y) {
    Display* dpy; Window win; GC gc;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win)); memcpy(&gc, gc_ptr, sizeof(gc));
    XDrawPoint(dpy, win, gc, x, y);
}

void mXDrawLine(void* dpy_ptr, void* win_ptr, void* gc_ptr, int x1, int y1, int x2, int y2) {
    Display* dpy; Window win; GC gc;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win)); memcpy(&gc, gc_ptr, sizeof(gc));
    XDrawLine(dpy, win, gc, x1, y1, x2, y2);
}

void mXDrawRectangle(void* dpy_ptr, void* win_ptr, void* gc_ptr, int x, int y, unsigned int w, unsigned int h) {
    Display* dpy; Window win; GC gc;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win)); memcpy(&gc, gc_ptr, sizeof(gc));
    XDrawRectangle(dpy, win, gc, x, y, w, h);
}

void mXFillRectangle(void* dpy_ptr, void* win_ptr, void* gc_ptr, int x, int y, unsigned int w, unsigned int h) {
    Display* dpy; Window win; GC gc;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win)); memcpy(&gc, gc_ptr, sizeof(gc));
    XFillRectangle(dpy, win, gc, x, y, w, h);
}

void mXDrawArc(void* dpy_ptr, void* win_ptr, void* gc_ptr, int x, int y, unsigned int w, unsigned int h, int a1, int a2) {
    Display* dpy; Window win; GC gc;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win)); memcpy(&gc, gc_ptr, sizeof(gc));
    XDrawArc(dpy, win, gc, x, y, w, h, a1, a2);
}

void mXFillArc(void* dpy_ptr, void* win_ptr, void* gc_ptr, int x, int y, unsigned int w, unsigned int h, int a1, int a2) {
    Display* dpy; Window win; GC gc;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win)); memcpy(&gc, gc_ptr, sizeof(gc));
    XFillArc(dpy, win, gc, x, y, w, h, a1, a2);
}

void mXDrawString(void* dpy_ptr, void* win_ptr, void* gc_ptr, int x, int y, const char* str) {
    Display* dpy; Window win; GC gc;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win)); memcpy(&gc, gc_ptr, sizeof(gc));
    XDrawString(dpy, win, gc, x, y, str, (int)strlen(str));
}

void mXDrawButtonFrame(void* dpy_ptr, void* win_ptr, int bx, int by, int bw, int bh, unsigned long bg_color, unsigned long border_color) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    GC gc = XCreateGC(dpy, win, 0, NULL);
    XSetForeground(dpy, gc, bg_color);
    XFillRectangle(dpy, win, gc, bx, by, bw, bh);
    XSetForeground(dpy, gc, border_color);
    XDrawRectangle(dpy, win, gc, bx, by, bw, bh);
    XFreeGC(dpy, gc);
}

void mXCreatePixmap(void* result, void* dpy_ptr, void* d_ptr, unsigned int width, unsigned int height, unsigned int depth) {
    Display* dpy; Drawable d;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&d, d_ptr, sizeof(d));
    Pixmap px = XCreatePixmap(dpy, d, width, height, depth);
    memcpy(result, &px, sizeof(px));
}

void mXFreePixmap(void* dpy_ptr, void* pixmap_ptr) {
    Display* dpy; Pixmap px;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&px, pixmap_ptr, sizeof(px));
    XFreePixmap(dpy, px);
}

void mXCopyArea(void* dpy_ptr, void* src_ptr, void* dest_ptr, void* gc_ptr, int src_x, int src_y, unsigned int w, unsigned int h, int dest_x, int dest_y) {
    Display* dpy; Drawable src, dest; GC gc;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&src, src_ptr, sizeof(src));
    memcpy(&dest, dest_ptr, sizeof(dest)); memcpy(&gc, gc_ptr, sizeof(gc));
    XCopyArea(dpy, src, dest, gc, src_x, src_y, w, h, dest_x, dest_y);
}

void mXCreateImage(void* result, void* dpy_ptr, void* visual_ptr, unsigned int depth, int format, int offset, char* data, unsigned int width, unsigned int height, int bitmap_pad, int bytes_per_line) {
    Display* dpy; Visual* vis;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&vis, visual_ptr, sizeof(vis));
    XImage* img = XCreateImage(dpy, vis, depth, format, offset, data, width, height, bitmap_pad, bytes_per_line);
    memcpy(result, &img, sizeof(img));
}

void mXPutImage(void* dpy_ptr, void* d_ptr, void* gc_ptr, void* img_ptr, int src_x, int src_y, int dest_x, int dest_y, unsigned int w, unsigned int h) {
    Display* dpy; Drawable d; GC gc; XImage* img;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&d, d_ptr, sizeof(d));
    memcpy(&gc, gc_ptr, sizeof(gc)); memcpy(&img, img_ptr, sizeof(img));
    XPutImage(dpy, d, gc, img, src_x, src_y, dest_x, dest_y, w, h);
}

void mXDestroyImage(void* img_ptr) {
    XImage* img; memcpy(&img, img_ptr, sizeof(img));
    if (img) XDestroyImage(img);
}

void mXLoadQueryFont(void* result, void* dpy_ptr, const char* font_name) {
    Display* dpy; memcpy(&dpy, dpy_ptr, sizeof(dpy));
    XFontStruct* font = XLoadQueryFont(dpy, font_name);
    memcpy(result, &font, sizeof(font));
}

void mXSetFont(void* dpy_ptr, void* gc_ptr, void* font_ptr) {
    Display* dpy; GC gc; XFontStruct* font;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&gc, gc_ptr, sizeof(gc)); memcpy(&font, font_ptr, sizeof(font));
    if (font) XSetFont(dpy, gc, font->fid);
}

void mXTextWidth(void* result, void* font_ptr, const char* str) {
    XFontStruct* font; memcpy(&font, font_ptr, sizeof(font));
    int w = font ? XTextWidth(font, str, (int)strlen(str)) : 0;
    memcpy(result, &w, sizeof(w));
}

void mXFreeFont(void* dpy_ptr, void* font_ptr) {
    Display* dpy; XFontStruct* font;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&font, font_ptr, sizeof(font));
    if (font) XFreeFont(dpy, font);
}

void mXInternAtom(void* result, void* dpy_ptr, const char* name, int only_if_exists) {
    Display* dpy; memcpy(&dpy, dpy_ptr, sizeof(dpy));
    Atom atom = XInternAtom(dpy, name, only_if_exists);
    memcpy(result, &atom, sizeof(atom));
}

void mXSetWMProtocols(void* dpy_ptr, void* win_ptr, void* atom_ptr, int count) {
    Display* dpy; Window win; Atom atom;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win)); memcpy(&atom, atom_ptr, sizeof(atom));
    XSetWMProtocols(dpy, win, &atom, count);
}

void mXChangeProperty(void* dpy_ptr, void* win_ptr, void* prop_atom, void* type_atom, int format, int mode, const unsigned char* data, int nelements) {
    Display* dpy; Window win; Atom prop, type;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    memcpy(&prop, prop_atom, sizeof(prop)); memcpy(&type, type_atom, sizeof(type));
    XChangeProperty(dpy, win, prop, type, format, mode, data, nelements);
}

void mXCreateFontCursor(void* result, void* dpy_ptr, unsigned int shape) {
    Display* dpy; memcpy(&dpy, dpy_ptr, sizeof(dpy));
    Cursor cursor = XCreateFontCursor(dpy, shape);
    memcpy(result, &cursor, sizeof(cursor));
}

void mXDefineCursor(void* dpy_ptr, void* win_ptr, void* cursor_ptr) {
    Display* dpy; Window win; Cursor cursor;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win)); memcpy(&cursor, cursor_ptr, sizeof(cursor));
    XDefineCursor(dpy, win, cursor); XFlush(dpy);
}

void mXUndefineCursor(void* dpy_ptr, void* win_ptr) {
    Display* dpy; Window win;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&win, win_ptr, sizeof(win));
    XUndefineCursor(dpy, win); XFlush(dpy);
}

void mXFreeCursor(void* dpy_ptr, void* cursor_ptr) {
    Display* dpy; Cursor cursor;
    memcpy(&dpy, dpy_ptr, sizeof(dpy)); memcpy(&cursor, cursor_ptr, sizeof(cursor));
    XFreeCursor(dpy, cursor);
}

void mprint(const char* str) {
    printf("%s", str); fflush(stdout);
}

void mprint_int(int val) {
    printf("%d\n", val);
}

#endif
