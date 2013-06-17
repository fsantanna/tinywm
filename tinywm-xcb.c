/* TinyWM is written by Nick Welch <mack@incise.org>, 2005.
 * TinyWM-XCB is rewritten by Ping-Hsun Chen <penkia@gmail.com>, 2010
 *
 * This software is in the public domain
 * and is provided AS IS, with NO WARRANTY. */

#include <X11/keysym.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

/* wrapper to get xcb keycodes from keysymbol */
static xcb_keycode_t* xcb_get_keycodes(xcb_connection_t* dis, xcb_keysym_t keysym) {
    xcb_key_symbols_t *keysyms;
    xcb_keycode_t     *keycode;

    if (!(keysyms = xcb_key_symbols_alloc(dis))) return NULL;
    keycode = xcb_key_symbols_get_keycode(keysyms, keysym);
    xcb_key_symbols_free(keysyms);

    return keycode;
}

int main (int argc, char **argv)
{

    uint32_t values[3];

    xcb_connection_t *dpy;
    xcb_screen_t *screen;
    xcb_drawable_t win;
    xcb_drawable_t root;

    xcb_generic_event_t *ev;
    xcb_get_geometry_reply_t *geom;

    dpy = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(dpy)) return 1;

    screen = xcb_setup_roots_iterator(xcb_get_setup(dpy)).data;
    root = screen->root;

    xcb_keycode_t* keycode = xcb_get_keycodes(dpy, XK_F1);
    int k;
    for (k=0; keycode[k] != XCB_NO_SYMBOL; k++)
        xcb_grab_key(dpy, 1, root, XCB_MOD_MASK_1, keycode[k],
                     XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);

    xcb_grab_button(dpy, 0, root, XCB_EVENT_MASK_BUTTON_PRESS | 
                XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC,
                XCB_GRAB_MODE_ASYNC, root, XCB_NONE, 1, XCB_MOD_MASK_1);

    xcb_grab_button(dpy, 0, root, XCB_EVENT_MASK_BUTTON_PRESS | 
                XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC, 
                XCB_GRAB_MODE_ASYNC, root, XCB_NONE, 3, XCB_MOD_MASK_1);
    xcb_flush(dpy);

    for (;;)
    {
        ev = xcb_wait_for_event(dpy);
        switch (ev->response_type & ~0x80) {

        case XCB_KEY_PRESS: {
xcb_key_press_event_t *kev       = (xcb_key_press_event_t *)ev;
printf("xcb: keypress: code: %d mod: %d\n", kev->detail, kev->state);
break;
}
        case XCB_KEY_RELEASE:
printf("release\n");
break;
        
        case XCB_BUTTON_PRESS:
        {
            xcb_button_press_event_t *e;
            e = ( xcb_button_press_event_t *) ev;
            win = e->child; 
            values[0] = XCB_STACK_MODE_ABOVE;
            xcb_configure_window(dpy, win, XCB_CONFIG_WINDOW_STACK_MODE, values);
            geom = xcb_get_geometry_reply(dpy, xcb_get_geometry(dpy, win), NULL);
printf("but = %p\n", geom);
            if (1 == e->detail) {
                values[2] = 1; 
                xcb_warp_pointer(dpy, XCB_NONE, win, 0, 0, 0, 0, 1, 1);
            } else {
                values[2] = 3; 
                xcb_warp_pointer(dpy, XCB_NONE, win, 0, 0, 0, 0, geom->width, geom->height);
            }
            xcb_grab_pointer(dpy, 0, root, XCB_EVENT_MASK_BUTTON_RELEASE
                    | XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_POINTER_MOTION_HINT, 
                    XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, root, XCB_NONE, XCB_CURRENT_TIME);
            xcb_flush(dpy);
        }
        break;

        case XCB_MOTION_NOTIFY:
        {
            xcb_query_pointer_reply_t *pointer;
            pointer = xcb_query_pointer_reply(dpy, xcb_query_pointer(dpy, root), 0);
            if (values[2] == 1) {/* move */
                geom = xcb_get_geometry_reply(dpy, xcb_get_geometry(dpy, win), NULL);
                values[0] = (pointer->root_x + geom->width > screen->width_in_pixels)?
                    (screen->width_in_pixels - geom->width):pointer->root_x;
                values[1] = (pointer->root_y + geom->height > screen->height_in_pixels)?
                    (screen->height_in_pixels - geom->height):pointer->root_y;
                xcb_configure_window(dpy, win, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
                xcb_flush(dpy);
            } else if (values[2] == 3) { /* resize */
                geom = xcb_get_geometry_reply(dpy, xcb_get_geometry(dpy, win), NULL);
                values[0] = pointer->root_x - geom->x;
                values[1] = pointer->root_y - geom->y;
                xcb_configure_window(dpy, win, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
                xcb_flush(dpy);
            }
        }
        break;

        case XCB_BUTTON_RELEASE:
            xcb_ungrab_pointer(dpy, XCB_CURRENT_TIME);
            xcb_flush(dpy); 
        break;
        }
    }

return 0;
}
