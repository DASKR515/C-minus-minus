#pragma once
#include "Cmm.h"
/* ===== Display ===== */
#define mXOpenDisplay(result, name) foreign "C" mXOpenDisplay(result, name)
#define mXCloseDisplay(dpy_ptr) foreign "C" mXCloseDisplay(dpy_ptr)
#define mXFlush(dpy_ptr) foreign "C" mXFlush(dpy_ptr)
#define mXSync(dpy_ptr, discard) foreign "C" mXSync(dpy_ptr, discard)

/* ===== Screen / Root ===== */
#define mXDefaultRootWindow(result, dpy_ptr) foreign "C" mXDefaultRootWindow(result, dpy_ptr)
#define mXDefaultScreen(result, dpy_ptr) foreign "C" mXDefaultScreen(result, dpy_ptr)
#define mXDisplayWidth(result, dpy_ptr, scr) foreign "C" mXDisplayWidth(result, dpy_ptr, scr)
#define mXDisplayHeight(result, dpy_ptr, scr) foreign "C" mXDisplayHeight(result, dpy_ptr, scr)
#define mXBlackPixel(result, dpy_ptr, scr) foreign "C" mXBlackPixel(result, dpy_ptr, scr)
#define mXWhitePixel(result, dpy_ptr, scr) foreign "C" mXWhitePixel(result, dpy_ptr, scr)

/* ===== Window creation ===== */
#define mXCreateSimpleWindow(result, dpy_ptr, parent_ptr, x, y, w, h, border_w, border_c, bg) foreign "C" mXCreateSimpleWindow(result, dpy_ptr, parent_ptr, x, y, w, h, border_w, border_c, bg)
#define mXCreateWindow(result, dpy_ptr, parent_ptr, x, y, w, h, border_w, depth, class_type, visual_ptr, valuemask, attributes_ptr) foreign "C" mXCreateWindow(result, dpy_ptr, parent_ptr, x, y, w, h, border_w, depth, class_type, visual_ptr, valuemask, attributes_ptr)

/* ===== Window mapping / destroying ===== */
#define mXMapWindow(dpy_ptr, win_ptr) foreign "C" mXMapWindow(dpy_ptr, win_ptr)
#define mXMapRaised(dpy_ptr, win_ptr) foreign "C" mXMapRaised(dpy_ptr, win_ptr)
#define mXUnmapWindow(dpy_ptr, win_ptr) foreign "C" mXUnmapWindow(dpy_ptr, win_ptr)
#define mXDestroyWindow(dpy_ptr, win_ptr) foreign "C" mXDestroyWindow(dpy_ptr, win_ptr)

/* ===== Window geometry ===== */
#define mXMoveWindow(dpy_ptr, win_ptr, x, y) foreign "C" mXMoveWindow(dpy_ptr, win_ptr, x, y)
#define mXResizeWindow(dpy_ptr, win_ptr, w, h) foreign "C" mXResizeWindow(dpy_ptr, win_ptr, w, h)
#define mXMoveResizeWindow(dpy_ptr, win_ptr, x, y, w, h) foreign "C" mXMoveResizeWindow(dpy_ptr, win_ptr, x, y, w, h)

/* ===== Window border / background ===== */
#define mXSetWindowBorderWidth(dpy_ptr, win_ptr, width) foreign "C" mXSetWindowBorderWidth(dpy_ptr, win_ptr, width)
#define mXSetWindowBorder(dpy_ptr, win_ptr, border_pixel) foreign "C" mXSetWindowBorder(dpy_ptr, win_ptr, border_pixel)
#define mXSetWindowBackground(dpy_ptr, win_ptr, color) foreign "C" mXSetWindowBackground(dpy_ptr, win_ptr, color)
#define mXClearWindow(dpy_ptr, win_ptr) foreign "C" mXClearWindow(dpy_ptr, win_ptr)
#define mXClearArea(dpy_ptr, win_ptr, x, y, w, h, exposures) foreign "C" mXClearArea(dpy_ptr, win_ptr, x, y, w, h, exposures)

/* ===== Window stacking ===== */
#define mXRaiseWindow(dpy_ptr, win_ptr) foreign "C" mXRaiseWindow(dpy_ptr, win_ptr)
#define mXLowerWindow(dpy_ptr, win_ptr) foreign "C" mXLowerWindow(dpy_ptr, win_ptr)

/* ===== Window title / input ===== */
#define mXSetWindowTitle(dpy_ptr, win_ptr, title) foreign "C" mXSetWindowTitle(dpy_ptr, win_ptr, title)
#define mXSelectInput(dpy_ptr, win_ptr, event_mask) foreign "C" mXSelectInput(dpy_ptr, win_ptr, event_mask)

/* ===== Events ===== */
#define mXAllocEvent(result) foreign "C" mXAllocEvent(result)
#define mXFreeEvent(ev_ptr) foreign "C" mXFreeEvent(ev_ptr)
#define mXNextEvent(dpy_ptr, ev_ptr) foreign "C" mXNextEvent(dpy_ptr, ev_ptr)
#define mXPeekEvent(dpy_ptr, ev_ptr) foreign "C" mXPeekEvent(dpy_ptr, ev_ptr)
#define mXPending(result, dpy_ptr) foreign "C" mXPending(result, dpy_ptr)
#define mXCheckTypedEvent(result, dpy_ptr, event_type, ev_ptr) foreign "C" mXCheckTypedEvent(result, dpy_ptr, event_type, ev_ptr)
#define mXGetEventType(result, ev_ptr) foreign "C" mXGetEventType(result, ev_ptr)
#define mXGetButtonCoordinates(x_ptr, y_ptr, ev_ptr) foreign "C" mXGetButtonCoordinates(x_ptr, y_ptr, ev_ptr)
#define mXGetPointerButton(result, ev_ptr) foreign "C" mXGetPointerButton(result, ev_ptr)
#define mXGetKeyCode(result, ev_ptr) foreign "C" mXGetKeyCode(result, ev_ptr)
#define mXLookupString(result_str, keysym_res, ev_ptr, max_len) foreign "C" mXLookupString(result_str, keysym_res, ev_ptr, max_len)
#define mXIsButtonClicked(result, ev_ptr, bx, by, bw, bh) foreign "C" mXIsButtonClicked(result, ev_ptr, bx, by, bw, bh)

/* ===== Graphics context ===== */
#define mXCreateGC(result, dpy_ptr, win_ptr) foreign "C" mXCreateGC(result, dpy_ptr, win_ptr)
#define mXFreeGC(dpy_ptr, gc_ptr) foreign "C" mXFreeGC(dpy_ptr, gc_ptr)
#define mXSetForeground(dpy_ptr, gc_ptr, color) foreign "C" mXSetForeground(dpy_ptr, gc_ptr, color)
#define mXSetBackground(dpy_ptr, gc_ptr, color) foreign "C" mXSetBackground(dpy_ptr, gc_ptr, color)
#define mXSetLineWidth(dpy_ptr, gc_ptr, width) foreign "C" mXSetLineWidth(dpy_ptr, gc_ptr, width)

/* ===== Drawing ===== */
#define mXDrawPoint(dpy_ptr, win_ptr, gc_ptr, x, y) foreign "C" mXDrawPoint(dpy_ptr, win_ptr, gc_ptr, x, y)
#define mXDrawLine(dpy_ptr, win_ptr, gc_ptr, x1, y1, x2, y2) foreign "C" mXDrawLine(dpy_ptr, win_ptr, gc_ptr, x1, y1, x2, y2)
#define mXDrawRectangle(dpy_ptr, win_ptr, gc_ptr, x, y, w, h) foreign "C" mXDrawRectangle(dpy_ptr, win_ptr, gc_ptr, x, y, w, h)
#define mXFillRectangle(dpy_ptr, win_ptr, gc_ptr, x, y, w, h) foreign "C" mXFillRectangle(dpy_ptr, win_ptr, gc_ptr, x, y, w, h)
#define mXDrawArc(dpy_ptr, win_ptr, gc_ptr, x, y, w, h, a1, a2) foreign "C" mXDrawArc(dpy_ptr, win_ptr, gc_ptr, x, y, w, h, a1, a2)
#define mXFillArc(dpy_ptr, win_ptr, gc_ptr, x, y, w, h, a1, a2) foreign "C" mXFillArc(dpy_ptr, win_ptr, gc_ptr, x, y, w, h, a1, a2)
#define mXDrawString(dpy_ptr, win_ptr, gc_ptr, x, y, str) foreign "C" mXDrawString(dpy_ptr, win_ptr, gc_ptr, x, y, str)
#define mXDrawButtonFrame(dpy_ptr, win_ptr, bx, by, bw, bh, bg_color, border_color) foreign "C" mXDrawButtonFrame(dpy_ptr, win_ptr, bx, by, bw, bh, bg_color, border_color)

/* ===== Pixmap / Image ===== */
#define mXCreatePixmap(result, dpy_ptr, d_ptr, width, height, depth) foreign "C" mXCreatePixmap(result, dpy_ptr, d_ptr, width, height, depth)
#define mXFreePixmap(dpy_ptr, pixmap_ptr) foreign "C" mXFreePixmap(dpy_ptr, pixmap_ptr)
#define mXCopyArea(dpy_ptr, src_ptr, dest_ptr, gc_ptr, src_x, src_y, w, h, dest_x, dest_y) foreign "C" mXCopyArea(dpy_ptr, src_ptr, dest_ptr, gc_ptr, src_x, src_y, w, h, dest_x, dest_y)
#define mXCreateImage(result, dpy_ptr, visual_ptr, depth, format, offset, data, width, height, bitmap_pad, bytes_per_line) foreign "C" mXCreateImage(result, dpy_ptr, visual_ptr, depth, format, offset, data, width, height, bitmap_pad, bytes_per_line)
#define mXPutImage(dpy_ptr, d_ptr, gc_ptr, img_ptr, src_x, src_y, dest_x, dest_y, w, h) foreign "C" mXPutImage(dpy_ptr, d_ptr, gc_ptr, img_ptr, src_x, src_y, dest_x, dest_y, w, h)
#define mXDestroyImage(img_ptr) foreign "C" mXDestroyImage(img_ptr)

/* ===== Fonts ===== */
#define mXLoadQueryFont(result, dpy_ptr, font_name) foreign "C" mXLoadQueryFont(result, dpy_ptr, font_name)
#define mXSetFont(dpy_ptr, gc_ptr, font_ptr) foreign "C" mXSetFont(dpy_ptr, gc_ptr, font_ptr)
#define mXTextWidth(result, font_ptr, str) foreign "C" mXTextWidth(result, font_ptr, str)
#define mXFreeFont(dpy_ptr, font_ptr) foreign "C" mXFreeFont(dpy_ptr, font_ptr)

/* ===== Atoms / Properties ===== */
#define mXInternAtom(result, dpy_ptr, name, only_if_exists) foreign "C" mXInternAtom(result, dpy_ptr, name, only_if_exists)
#define mXSetWMProtocols(dpy_ptr, win_ptr, atom_ptr, count) foreign "C" mXSetWMProtocols(dpy_ptr, win_ptr, atom_ptr, count)
#define mXChangeProperty(dpy_ptr, win_ptr, prop_atom, type_atom, format, mode, data, nelements) foreign "C" mXChangeProperty(dpy_ptr, win_ptr, prop_atom, type_atom, format, mode, data, nelements)

/* ===== Cursors ===== */
#define mXCreateFontCursor(result, dpy_ptr, shape) foreign "C" mXCreateFontCursor(result, dpy_ptr, shape)
#define mXDefineCursor(dpy_ptr, win_ptr, cursor_ptr) foreign "C" mXDefineCursor(dpy_ptr, win_ptr, cursor_ptr)
#define mXUndefineCursor(dpy_ptr, win_ptr) foreign "C" mXUndefineCursor(dpy_ptr, win_ptr)
#define mXFreeCursor(dpy_ptr, cursor_ptr) foreign "C" mXFreeCursor(dpy_ptr, cursor_ptr)

/* ===== Print helpers ===== */
#define mprint(str) foreign "C" mprint(str)
#define mprint_int(val) foreign "C" mprint_int(val)