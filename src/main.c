/* vim: set expandtab ts=2 sw=2: */
/* main.c

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

int
main(int argc,
     char **argv)
{
  Imlib_Image image;
  Imlib_Image thumbnail;
  Imlib_Load_Error err;
  char *filename_im = NULL, *filename_thumb = NULL;

  time_t t;
  struct tm *tm;

  init_parse_options(argc, argv);

  init_x_and_imlib(NULL, 0);

  if (!opt.output_file) {
    opt.output_file = gib_estrdup("%Y-%m-%d-%H%M%S_$wx$h_scrot.png");
    opt.thumb_file = gib_estrdup("%Y-%m-%d-%H%M%S_$wx$h_scrot-thumb.png");
  }


  if (opt.focused)
    image = scrot_grab_focused();
  else if (opt.window)
    image = scrot_grab_window();
  else if (opt.select)
    image = scrot_sel_and_grab_image();
  else {
    scrot_do_delay();
    if (opt.multidisp) {
      image = scrot_grab_shot_multi();
    } else {
      image = scrot_grab_shot();
    }
  }

  if (!image)
    gib_eprintf("no image grabbed");

  time(&t); /* Get the time directly after the screenshot */
  tm = localtime(&t);

  imlib_context_set_image(image);
  imlib_image_attach_data_value("quality", NULL, opt.quality, NULL);

  filename_im = im_printf(opt.output_file, tm, NULL, NULL, image);
  gib_imlib_save_image_with_error_return(image, filename_im, &err);
  if (err)
    gib_eprintf("Saving to file %s failed\n", filename_im);
  if (opt.thumb)
  {
    int cwidth, cheight;
    int twidth, theight;

    cwidth = gib_imlib_image_get_width(image);
    cheight = gib_imlib_image_get_height(image);

    /* Geometry based thumb size */
    if (opt.thumb_width || opt.thumb_height)
    {
      if (!opt.thumb_width)
      {
        twidth = cwidth * opt.thumb_height / cheight;
        theight = opt.thumb_height;
      }
      else if (!opt.thumb_height)
      {
        twidth = opt.thumb_width;
        theight = cheight * opt.thumb_width / cwidth;
      }
      else
      {
        twidth = opt.thumb_width;
        theight = opt.thumb_height;
      }
    }
    else
    {
      twidth = cwidth * opt.thumb / 100;
      theight = cheight * opt.thumb / 100;
    }

    thumbnail =
      gib_imlib_create_cropped_scaled_image(image, 0, 0, cwidth, cheight,
                                            twidth, theight, 1);
    if (thumbnail == NULL)
      gib_eprintf("Unable to create scaled Image\n");
    else
    {
      filename_thumb = im_printf(opt.thumb_file, tm, NULL, NULL, thumbnail);
      gib_imlib_save_image_with_error_return(thumbnail, filename_thumb, &err);
      if (err)
        gib_eprintf("Saving thumbnail %s failed\n", filename_thumb);
    }
  }
  if (opt.exec)
    scrot_exec_app(image, tm, filename_im, filename_thumb);
  gib_imlib_free_image_and_decache(image);

  return 0;
}

void
scrot_do_delay(void)
{
  if (opt.delay) {
    if (opt.countdown) {
      int i;

      printf("Taking shot in %d.. ", opt.delay);
      fflush(stdout);
      sleep(1);
      for (i = opt.delay - 1; i > 0; i--) {
        printf("%d.. ", i);
        fflush(stdout);
        sleep(1);
      }
      printf("0.\n");
      fflush(stdout);
    } else
      sleep(opt.delay);
  }
}

Imlib_Image
scrot_grab_shot(void)
{
  Imlib_Image im;

  XBell(disp, 0);
  im =
    gib_imlib_create_image_from_drawable(root, 0, 0, 0, scr->width,
                                         scr->height, 1);
  return im;
}

void
scrot_exec_app(Imlib_Image image, struct tm *tm,
               char *filename_im, char *filename_thumb)
{
  char *execstr;
  int status;

  execstr = im_printf(opt.exec, tm, filename_im, filename_thumb, image);
  status = system(execstr);
  exit(WEXITSTATUS(status));
}

Imlib_Image
scrot_grab_identified_window(Window target)
{
  Imlib_Image im = NULL;
  int rx = 0, ry = 0, rw = 0, rh = 0;
  Window client_window = None;

  if (!scrot_get_geometry(target, &client_window, &rx, &ry, &rw, &rh))
    return NULL;
  scrot_nice_clip(&rx, &ry, &rw, &rh);
  if(opt.alpha)
    im = scrot_grab_transparent_shot(disp, client_window, rx, ry, rw, rh);
  else
    im = gib_imlib_create_image_from_drawable(root, 0, rx, ry, rw, rh, 1);
  return im;
}

Imlib_Image
scrot_grab_focused(void)
{
  Window target = None;
  int ignored;

  scrot_do_delay();
  XGetInputFocus(disp, &target, &ignored);
  return scrot_grab_identified_window(target);
}

Imlib_Image
scrot_grab_window(void)
{
  Window target = opt.window;

  scrot_do_delay();
  return scrot_grab_identified_window(target);
}

Imlib_Image
scrot_sel_and_grab_image(void)
{
  Imlib_Image im = NULL;
  static int xfd = 0;
  static int fdsize = 0;
  XEvent ev;
  fd_set fdset;
  int count = 0, done = 0;
  int rx = 0, ry = 0, rw = 0, rh = 0, btn_pressed = 0;
  int rect_x = 0, rect_y = 0, rect_w = 0, rect_h = 0;
  Cursor cursor, cursor_nw, cursor_ne, cursor_se, cursor_sw;
  Window target = None;
  GC gc;
  XGCValues gcval;

  xfd = ConnectionNumber(disp);
  fdsize = xfd + 1;

  cursor    = XCreateFontCursor(disp, XC_crosshair);
  cursor_nw = XCreateFontCursor(disp, XC_ul_angle);
  cursor_ne = XCreateFontCursor(disp, XC_ur_angle);
  cursor_se = XCreateFontCursor(disp, XC_lr_angle);
  cursor_sw = XCreateFontCursor(disp, XC_ll_angle);

  gcval.foreground = XWhitePixel(disp, 0);
  gcval.function = GXxor;
  gcval.background = XBlackPixel(disp, 0);
  gcval.plane_mask = gcval.background ^ gcval.foreground;
  gcval.subwindow_mode = IncludeInferiors;

  gc =
    XCreateGC(disp, root,
              GCFunction | GCForeground | GCBackground | GCSubwindowMode,
              &gcval);

  if ((XGrabPointer
       (disp, root, False,
        ButtonMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync,
        GrabModeAsync, root, cursor, CurrentTime) != GrabSuccess))
    gib_eprintf("couldn't grab pointer:");

  if ((XGrabKeyboard
       (disp, root, False, GrabModeAsync, GrabModeAsync,
        CurrentTime) != GrabSuccess))
    gib_eprintf("couldn't grab keyboard:");

  while (1) {
    /* handle events here */
    while (!done && XPending(disp)) {
      XNextEvent(disp, &ev);
      switch (ev.type) {
        case MotionNotify:
          if (btn_pressed) {
            if (rect_w) {
              /* re-draw the last rect to clear it */
              XDrawRectangle(disp, root, gc, rect_x, rect_y, rect_w, rect_h);
            }

            rect_x = rx;
            rect_y = ry;
            rect_w = ev.xmotion.x - rect_x;
            rect_h = ev.xmotion.y - rect_y;

            /* Change the cursor to show we're selecting a region */
            if (rect_w < 0 && rect_h < 0)
              XChangeActivePointerGrab(disp,
                                       ButtonMotionMask | ButtonReleaseMask,
                                       cursor_nw, CurrentTime);
            else if (rect_w < 0 && rect_h > 0)
              XChangeActivePointerGrab(disp,
                                       ButtonMotionMask | ButtonReleaseMask,
                                       cursor_sw, CurrentTime);
            else if (rect_w > 0 && rect_h < 0)
              XChangeActivePointerGrab(disp,
                                       ButtonMotionMask | ButtonReleaseMask,
                                       cursor_ne, CurrentTime);
            else if (rect_w > 0 && rect_h > 0)
              XChangeActivePointerGrab(disp,
                                       ButtonMotionMask | ButtonReleaseMask,
                                       cursor_se, CurrentTime);

            if (rect_w < 0) {
              rect_x += rect_w;
              rect_w = 0 - rect_w;
            }
            if (rect_h < 0) {
              rect_y += rect_h;
              rect_h = 0 - rect_h;
            }
            /* draw rectangle */
            XDrawRectangle(disp, root, gc, rect_x, rect_y, rect_w, rect_h);
            XFlush(disp);
          }
          break;
        case ButtonPress:
          btn_pressed = 1;
          rx = ev.xbutton.x;
          ry = ev.xbutton.y;
          target =
            scrot_get_window(disp, ev.xbutton.subwindow, ev.xbutton.x,
                             ev.xbutton.y);
          if (target == None)
            target = root;
          break;
        case ButtonRelease:
          done = 1;
          break;
        case KeyPress:
          fprintf(stderr, "Key was pressed, aborting shot\n");
          done = 2;
          break;
        case KeyRelease:
          /* ignore */
          break;
        default:
          break;
      }
    }
    if (done)
      break;

    /* now block some */
    FD_ZERO(&fdset);
    FD_SET(xfd, &fdset);
    errno = 0;
    count = select(fdsize, &fdset, NULL, NULL, NULL);
    if ((count < 0)
        && ((errno == ENOMEM) || (errno == EINVAL) || (errno == EBADF)))
      gib_eprintf("Connection to X display lost");
  }
  if (rect_w) {
    XDrawRectangle(disp, root, gc, rect_x, rect_y, rect_w, rect_h);
    XFlush(disp);
  }
  XUngrabPointer(disp, CurrentTime);
  XUngrabKeyboard(disp, CurrentTime);
  XFreeCursor(disp, cursor);
  XFreeGC(disp, gc);
  XSync(disp, True);


  if (done < 2) {
    scrot_do_delay();

    Window client_window = None;

    if (rect_w > 5) {
      /* if a rect has been drawn, it's an area selection */
      rw = ev.xbutton.x - rx;
      rh = ev.xbutton.y - ry;

      if (rw < 0) {
        rx += rw;
        rw = 0 - rw;
      }
      if (rh < 0) {
        ry += rh;
        rh = 0 - rh;
      }
    } else {
      /* else it's a window click */
      if (!scrot_get_geometry(target, &client_window, &rx, &ry, &rw, &rh))
        return NULL;
    }
    scrot_nice_clip(&rx, &ry, &rw, &rh);

    XBell(disp, 0);
    if(opt.alpha)
      im = scrot_grab_transparent_shot(disp, client_window, rx, ry, rw, rh);
    else
      im = gib_imlib_create_image_from_drawable(root, 0, rx, ry, rw, rh, 1);
  }
  return im;
}


/* clip rectangle nicely */
void
scrot_nice_clip(int *rx, 
		int *ry, 
		int *rw, 
		int *rh)
{
  if (*rx < 0) {
    *rw += *rx;
    *rx = 0;
  }
  if (*ry < 0) {
    *rh += *ry;
    *ry = 0;
  }
  if ((*rx + *rw) > scr->width)
    *rw = scr->width - *rx;
  if ((*ry + *rh) > scr->height)
    *rh = scr->height - *ry;
}


/* get geometry of window and use that */
int
scrot_get_geometry(Window target,
                   Window *client_window,
                   int *rx, 
                   int *ry, 
                   int *rw, 
                   int *rh)
{
  Window child;
  XWindowAttributes attr;
  int stat;

  /* Get window manager frame and detect application window     */
  /* from pointed window.                                       */
  if (target != root) {
    int x;
    unsigned int d;
    int status;
    
    status = XGetGeometry(disp, target, &root, &x, &x, &d, &d, &d, &d);
    if (status != 0) {
      Window rt, *children, parent;
      
      /* Find toplevel window.                                  */
      /* It will have coordinates of window, that we look for   */
      /* But it may be completely different window (it depends  */
      /* on window manager).                                    */
      for (;;) {
        status = XQueryTree(disp, target, &rt, &parent, &children, &d);
        if (status && (children != None))
          XFree((char *) children);
        if (!status || (parent == None) || (parent == rt))
          break;
        target = parent;
      }

      /* Get client window. */
      if (opt.border)
      {
        *client_window = scrot_get_client_window(disp, target);
        target = scrot_get_net_frame_window(disp, target);
      }
      else
      {
        target = scrot_get_client_window(disp, target);
        *client_window = target;
      }

      XRaiseWindow(disp, target);
      XSetInputFocus(disp, target, RevertToParent, CurrentTime);
    }
  }
  stat = XGetWindowAttributes(disp, target, &attr);
  if ((stat == False) || (attr.map_state != IsViewable))
    return 0;
  *rw = attr.width;
  *rh = attr.height;
  XTranslateCoordinates(disp, target, root, 0, 0, rx, ry, &child);

  // additional border for shadows:
  // FIXME apply only if grabbing transparent screenshot
  // and not maximised nor fullscreen
  if(opt.alpha)
  {
    *rx -= 20;
    *ry -= 20;
    *rw += 40;
    *rh += 40;
  }

  return 1;
}


Imlib_Image
scrot_grab_transparent_shot(Display *dpy, 
                            Window client_window,
                            int x,
                            int y,
                            int width,
                            int height)
{
  Imlib_Image black_shot, white_shot;

  Window w = scrot_create_window(dpy, x, y, width, height);
  
  XSetInputFocus(dpy, client_window, RevertToParent, CurrentTime);

  // compiz raises window as expected
  // metacity have problems, so we need
  // to set always-on-top flag temporarily for metacity
  // FIXME 
  //
  // XRaiseWindow(dpy, client_window);
  window_set_above(dpy, client_window, 1);

  XFlush(dpy);
  // wait 1s until WM will finish animations
  sleep(1); // FIXME try to disable animations for this window
  white_shot = gib_imlib_create_image_from_drawable(root,
      0, x, y, width, height, 1);

  GC gc = XCreateGC(dpy, w, 0, 0);
  XFillRectangle(dpy, w, gc, 0, 0, width, height);
  XFlush(dpy);
  sleep(1); // FIXME
  black_shot = gib_imlib_create_image_from_drawable(root,
      0, x, y, width, height, 1);

  window_set_above(dpy, client_window, 0);
  XFlush(dpy);

  return create_transparent_image(white_shot, black_shot);
}


Window
scrot_create_window(Display *dpy, 
                    int x,
                    int y,
                    int width,
                    int height)
{
  Window w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), x, y, 
         width, height, 0, 
         XBlackPixel(disp, 0),
         XWhitePixel(disp, 0));
  XSelectInput(dpy, w, PropertyChangeMask | StructureNotifyMask
      | SubstructureRedirectMask | ConfigureRequest  /* | ExposureMask */ );

  struct {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
  } hints = { 2, 1, 0, 0, 0 };

  // _MOTIF_WM_HINTS is respected by most window managers
  // but freedesktop intends to deprecate it
  // replace with _NET_WM_WINDOW_TYPE_DESKTOP ?
  XChangeProperty (dpy, w,
    XInternAtom (dpy, "_MOTIF_WM_HINTS", False),
    XInternAtom (dpy, "_MOTIF_WM_HINTS", False),
    32, PropModeReplace,
    (const unsigned char *) &hints,
    sizeof (hints) / sizeof (long));

  window_set_skip_taskbar(dpy, w);
 
  XMapWindow(dpy, w);
  XMoveWindow(dpy, w, x, y);
  for(;;) {
    XEvent e;
    XNextEvent(dpy, &e);
    if (e.type == MapNotify)
      break;
  }
  return w;
}


Window
scrot_get_window(Display * display,
                 Window window,
                 int x,
                 int y)
{
  Window source, target;

  int status, x_offset, y_offset;

  source = root;
  target = window;
  if (window == None)
    window = root;
  while (1) {
    status =
      XTranslateCoordinates(display, source, window, x, y, &x_offset,
                            &y_offset, &target);
    if (status != True)
      break;
    if (target == None)
      break;
    source = window;
    window = target;
    x = x_offset;
    y = y_offset;
  }
  if (target == None)
    target = window;
  return (target);
}


char *
im_printf(char *str, struct tm *tm,
          char *filename_im,
          char *filename_thumb,
          Imlib_Image im)
{
  char *c;
  char buf[20];
  char ret[4096];
  char strf[4096];
  char *tmp;
  struct stat st;

  ret[0] = '\0';
  strftime(strf, 4095, str, tm);

  for (c = strf; *c != '\0'; c++) {
    if (*c == '$') {
      c++;
      switch (*c) {
        case 'f':
          if (filename_im)
            strcat(ret, filename_im);
          break;
        case 'm': /* t was allready taken, so m as in mini */
          if (filename_thumb)
            strcat(ret, filename_thumb);
          break;
        case 'n':
          if (filename_im) {
            tmp = strrchr(filename_im, '/');
            if (tmp)
              strcat(ret, tmp + 1);
            else
              strcat(ret, filename_im);
          }
          break;
        case 'w':
          snprintf(buf, sizeof(buf), "%d", gib_imlib_image_get_width(im));
          strcat(ret, buf);
          break;
        case 'h':
          snprintf(buf, sizeof(buf), "%d", gib_imlib_image_get_height(im));
          strcat(ret, buf);
          break;
        case 's':
          if (filename_im) {
            if (!stat(filename_im, &st)) {
              int size;

              size = st.st_size;
              snprintf(buf, sizeof(buf), "%d", size);
              strcat(ret, buf);
            } else
              strcat(ret, "[err]");
          }
          break;
        case 'p':
          snprintf(buf, sizeof(buf), "%d",
                   gib_imlib_image_get_width(im) *
                   gib_imlib_image_get_height(im));
          strcat(ret, buf);
          break;
        case 't':
          strcat(ret, gib_imlib_image_format(im));
          break;
        case '$':
          strcat(ret, "$");
          break;
        default:
          strncat(ret, c, 1);
          break;
      }
    } else if (*c == '\\') {
      c++;
      switch (*c) {
        case 'n':
          if (filename_im)
            strcat(ret, "\n");
          break;
        default:
          strncat(ret, c, 1);
          break;
      }
    } else
      strncat(ret, c, 1);
  }
  return gib_estrdup(ret);
}

Window
scrot_get_client_window(Display * display,
                        Window target)
{
  Atom state;
  Atom type = None;
  int format, status;
  unsigned char *data;
  unsigned long after, items;
  Window client;

  state = XInternAtom(display, "WM_STATE", True);
  if (state == None)
    return target;
  status =
    XGetWindowProperty(display, target, state, 0L, 0L, False,
                       (Atom) AnyPropertyType, &type, &format, &items, &after,
                       &data);
  if ((status == Success) && (type != None))
    return target;
  client = scrot_find_window_by_property(display, target, state);
  if (!client)
    return target;
  return client;
}

Window
scrot_get_net_frame_window(Display * display,
                           Window target)
{
  /* window's _NET_FRAME_WINDOW property       */
  /* points to window decoration frame xid     */
  /* it's useful for WMs, that do not reparent */
  /* client window to decoration windows       */
  /* (e.g. compiz <= 0.9)                      */
  Atom net_frame;
  Atom type = None;
  int format, status;
  unsigned char *data;
  unsigned long after, items;
  net_frame = XInternAtom(display, "_NET_FRAME_WINDOW", True);
  if (None == net_frame)
    return target;
  status = XGetWindowProperty(display, target, net_frame, 0L, 1L, False,
                         (Atom) AnyPropertyType, &type, &format,
                         &items, &after, &data);
  if (Success!=status || None==type || 32!=format || 1!=items)
    return target;
  target = *(Window*)data;
  if (data)
    XFree(data);
  return target;
}

Window
scrot_find_window_by_property(Display * display,
                              const Window window,
                              const Atom property)
{
  Atom type = None;
  int format, status;
  unsigned char *data;
  unsigned int i, number_children;
  unsigned long after, number_items;
  Window child = None, *children, parent, root;

  status =
    XQueryTree(display, window, &root, &parent, &children, &number_children);
  if (!status)
    return None;
  for (i = 0; (i < number_children) && (child == None); i++) {
    status =
      XGetWindowProperty(display, children[i], property, 0L, 0L, False,
                         (Atom) AnyPropertyType, &type, &format,
                         &number_items, &after, &data);
    if (data)
      XFree(data);
    if ((status == Success) && (type != (Atom) NULL))
      child = children[i];
  }
  for (i = 0; (i < number_children) && (child == None); i++)
    child = scrot_find_window_by_property(display, children[i], property);
  if (children != None)
    XFree(children);
  return (child);
}

Imlib_Image
scrot_grab_shot_multi(void)
{
  int screens;
  int i;
  char *dispstr, *subdisp;
  char newdisp[255];
  gib_list *images = NULL;
  Imlib_Image ret = NULL;

  screens = ScreenCount(disp);
  if (screens < 2)
    return scrot_grab_shot();

  dispstr = DisplayString(disp);

  subdisp = gib_estrdup(DisplayString(disp));

  for (i = 0; i < screens; i++) {
    dispstr = strchr(subdisp, ':');
    if (dispstr) {
      dispstr = strchr(dispstr, '.');
      if (NULL != dispstr)
        *dispstr = '\0';
    }
    snprintf(newdisp, sizeof(newdisp), "%s.%d", subdisp, i);
    init_x_and_imlib(newdisp, i);
    ret =
      gib_imlib_create_image_from_drawable(root, 0, 0, 0, scr->width,
                                           scr->height, 1);
    images = gib_list_add_end(images, ret);
  }
  free(subdisp);

  ret = stalk_image_concat(images);

  return ret;
}

Imlib_Image
stalk_image_concat(gib_list * images)
{
  int tot_w = 0, max_h = 0, w, h;
  int x = 0;
  gib_list *l, *item;
  Imlib_Image ret, im;

  if (gib_list_length(images) == 0)
    return NULL;

  l = images;
  while (l) {
    im = (Imlib_Image) l->data;
    h = gib_imlib_image_get_height(im);
    w = gib_imlib_image_get_width(im);
    if (h > max_h)
      max_h = h;
    tot_w += w;
    l = l->next;
  }
  ret = imlib_create_image(tot_w, max_h);
  gib_imlib_image_fill_rectangle(ret, 0, 0, tot_w, max_h, 255, 0, 0, 0);
  l = images;
  while (l) {
    im = (Imlib_Image) l->data;
    item = l;
    l = l->next;
    h = gib_imlib_image_get_height(im);
    w = gib_imlib_image_get_width(im);
    gib_imlib_blend_image_onto_image(ret, im, 0, 0, 0, w, h, x, 0, w, h, 1, 0,
                                     0);
    x += w;
    gib_imlib_free_image_and_decache(im);
    free(item);
  }
  return ret;
}


Imlib_Image
create_transparent_image(Imlib_Image w_image, Imlib_Image b_image)
{
  int w, h;
  DATA32 *dst_data, *src_data;
  imlib_context_set_image(w_image);
  dst_data = imlib_image_get_data();
  imlib_context_set_image(b_image);
  src_data = imlib_image_get_data();
  h = gib_imlib_image_get_height(w_image);
  w = gib_imlib_image_get_width(w_image);

  unsigned long i;
  for(i=0; i<w*h; i++)
    if(dst_data[i] != src_data[i])
    {
      DATA32 alpha;
      alpha = (src_data[i] & 0xff) - (dst_data[i] & 0xff);
      alpha = (alpha << 24) & 0xff000000;
      dst_data[i] = (src_data[i] & 0xffffff) | alpha;
    }
  Imlib_Image ret_img;
  ret_img = imlib_create_image_using_data(w, h, dst_data);
  imlib_context_set_image(ret_img);
  imlib_image_set_has_alpha(1);
  return ret_img;
}


void
window_set_skip_taskbar(Display *dpy, Window window)
{
  XEvent event;
  event.xclient.type = ClientMessage;
  event.xclient.display = dpy;
  event.xclient.window = window;
  event.xclient.message_type = XInternAtom(dpy, "_NET_WM_STATE", True);
  event.xclient.format = 32;
  event.xclient.data.l[0] = 1;
  event.xclient.data.l[1] = XInternAtom(dpy, "_NET_WM_STATE_SKIP_TASKBAR", True);
  event.xclient.data.l[2] = 0;
  event.xclient.data.l[3] = 0;
  event.xclient.data.l[4] = 0;

  XSendEvent(dpy, DefaultRootWindow(disp), True, PropertyChangeMask, &event);
}


void
window_set_above(Display *dpy, Window window, int enable)
{
  XEvent event;
  event.xclient.type = ClientMessage;
  event.xclient.display = dpy;
  event.xclient.window = window;
  event.xclient.message_type = XInternAtom(dpy, "_NET_WM_STATE", True);
  event.xclient.format = 32;
  event.xclient.data.l[0] = enable;
  event.xclient.data.l[1] = XInternAtom(dpy, "_NET_WM_STATE_ABOVE", True);
  event.xclient.data.l[2] = 0;
  event.xclient.data.l[3] = 0;
  event.xclient.data.l[4] = 0;

  XSendEvent(dpy, DefaultRootWindow(disp), True, PropertyChangeMask, &event);
}

