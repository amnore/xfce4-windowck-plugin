/*  $Id$
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  Copyright (C) 2013 Cedric Leporcq  <cedl38@gmail.com>
 *
 *  This code is derived from original 'Window Applets' from Andrej Belcijan.
 */

#ifndef __WCK_UTILS_H__
#define __WCK_UTILS_H__

#ifndef WNCK_I_KNOW_THIS_IS_UNSTABLE
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#endif
#include <libxfce4windowing/libxfce4windowing.h>

G_BEGIN_DECLS

/* Wnck structure */
typedef struct {
    XfwScreen *activescreen;          // Active screen
    XfwWorkspaceManager *workspacemanager; // Workspace manager
    XfwWindow *controlwindow;          // Controled window according to only_maximized option
    XfwWindow *activewindow;          // Active window
    XfwWindow *umaxwindow;            // Upper maximized window

    gulong msh;                         // upper maximized window state handler id
    gulong ash;                         // active state handler id
    gulong mwh;                         // upper maximized workspace handler id

    gulong sah;                         // active window changed handler id
    gulong sch;                         // window closed handler id
    gulong soh;                         // window opened handler id
    GList *sgh;                         // workspace group handler ids
    gulong sgch;                        // workspace group created handler id
    gulong sgdh;                        // workspace group deleted handler id

    gboolean only_maximized;           // [T/F] Only track maximized windows

    gpointer data;
} WckUtils;

void init_wnck (WckUtils *win, gboolean only_maximized, gpointer data);
void disconnect_wnck (WckUtils *win);
void on_wck_state_changed (XfwWindow *controlwindow, gpointer data);
void on_control_window_changed(XfwWindow *controlwindow, XfwWindow *previous, gpointer data);
void reload_wnck (WckUtils *win, gboolean only_maximized, gpointer data);
void toggle_maximize (XfwWindow *window);
gboolean wck_signal_handler_disconnect (GObject *object, gulong handler);

static inline gboolean
window_is_desktop (XfwWindow *window)
{
    return xfw_window_get_window_type (window) == XFW_WINDOW_TYPE_DESKTOP;
}

G_END_DECLS

#endif /* __WCK_UTILS_H__ */
