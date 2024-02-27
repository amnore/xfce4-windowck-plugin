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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "wck-utils.h"

typedef struct {
    XfwWorkspaceGroup *workspace_group;
    gulong svh; // viewport changed handler id
    gulong swh; // workspace hanaged handler id
} WorkspaceGroupHandlers;


/* Prototypes */
static XfwWindow *get_upper_maximized(WckUtils *);
static void track_controlled_window (WckUtils *);
static void active_workspace_changed(XfwScreen *, XfwWorkspace *, WckUtils *);
static void active_window_changed(XfwScreen *, XfwWindow *, WckUtils *);
static void track_changed_max_state(XfwWindow *, XfwWindowState, XfwWindowState, WckUtils *);
static void on_umaxed_window_state_changed(XfwWindow *, XfwWindowState, XfwWindowState, WckUtils *);
static void on_viewports_changed(XfwScreen *, WckUtils *);
static void on_window_closed(XfwScreen *, XfwWindow *, WckUtils *);
static void on_window_opened(XfwScreen *, XfwWindow *, WckUtils *);
static void object_ref_nonnull(gpointer object);
static void object_unref_nonnull(gpointer object);


gboolean wck_signal_handler_disconnect (GObject *object, gulong handler)
{
    if (object && handler > 0)
    {
        if (g_signal_handler_is_connected(object, handler))
        {
            g_signal_handler_disconnect(object, handler);
            return TRUE;
        }
    }
    return FALSE;
}


/* Increase reference count if object is not NULL */
static void object_ref_nonnull(gpointer object)
{
    if (object)
        g_object_ref(object);
}


/* Decrease reference count if object is not NULL */
static void object_unref_nonnull(gpointer object)
{
    if (object)
        g_object_unref(object);
}


/* Trigger when activewindow's workspaces changes */
static void umax_window_workspace_changed (XfwWindow *window, WckUtils *win)
{
        track_controlled_window (win);
}


/* Trigger when a specific window's state changes */
static void track_changed_max_state (XfwWindow *window,
                                     XfwWindowState changed_mask,
                                     XfwWindowState new_state,
                                     WckUtils *win)
{
    /* track the window max state only if it isn't the control window */
    if (window != win->controlwindow)
    {
        if (window
            && !xfw_window_is_minimized(window)
            && xfw_window_is_maximized(window))
        {
            track_controlled_window (win);
        }
    }
}


/* Triggers when umaxedwindow's state changes */
static void on_umaxed_window_state_changed (XfwWindow *window,
                                            XfwWindowState changed_mask,
                                            XfwWindowState new_state,
                                            WckUtils *win)
{
    /* WARNING : only if window is unmaximized to prevent growing loop !!!*/
    if (!xfw_window_is_maximized(window)
        || xfw_window_is_minimized(window)
        || changed_mask & (XFW_WINDOW_STATE_ABOVE))
    {
        track_controlled_window (win);
    }
    else {
        on_wck_state_changed(win->controlwindow, win->data);
    }
}


/* Returns the highest maximized window */
static XfwWindow *get_upper_maximized (WckUtils *win)
{
    XfwWindow      *umaxedwindow = NULL;
    XfwWorkspace *window_workspace;

    GList *windows = xfw_screen_get_windows_stacked(win->activescreen);

    while (windows && windows->data)
    {
        if ((window_workspace = xfw_window_get_workspace(windows->data))
            && (xfw_workspace_get_state(window_workspace) & XFW_WORKSPACE_STATE_ACTIVE)
            && xfw_window_is_maximized(windows->data)
            && !xfw_window_is_minimized(windows->data))
        {
            umaxedwindow = windows->data;
        }
        windows = windows->next;
    }
    /* NULL if no maximized window found */
    return umaxedwindow;
}


/* track the new controlled window according to preferences */
static void track_controlled_window (WckUtils *win)
{
    XfwWindow       *previous_umax = NULL;
    XfwWindow       *previous_control = NULL;
    XfwWorkspace    *window_workspace = NULL;

    previous_umax = win->umaxwindow;
    previous_control = win->controlwindow;
    object_ref_nonnull(previous_control);
    object_ref_nonnull(previous_umax);

    if (win->only_maximized)
    {
        object_unref_nonnull(win->umaxwindow);
        object_unref_nonnull(win->controlwindow);
        win->umaxwindow = get_upper_maximized(win);
        win->controlwindow = win->umaxwindow;
        object_ref_nonnull(win->umaxwindow);
        object_ref_nonnull(win->controlwindow);
    }
    else if (win->activewindow
            && (window_workspace = xfw_window_get_workspace(win->activewindow))
            && (xfw_workspace_get_state(window_workspace) & XFW_WORKSPACE_STATE_ACTIVE)
            && !xfw_window_is_minimized(win->activewindow))
    {
        object_unref_nonnull(win->controlwindow);
        win->controlwindow = win->activewindow;
        object_ref_nonnull(win->controlwindow);
    }
    else if (!win->activewindow) {
        object_unref_nonnull(win->controlwindow);
        win->controlwindow = NULL;
    }

    if (!win->umaxwindow || (win->umaxwindow != previous_umax))
    {
        wck_signal_handler_disconnect (G_OBJECT(previous_umax), win->msh);
        wck_signal_handler_disconnect (G_OBJECT(previous_umax), win->mwh);
    }

    if (win->only_maximized)
    {
        if (win->umaxwindow && (win->umaxwindow != previous_umax))
        {
            /* track the new upper maximized window state */
            win->msh = g_signal_connect(G_OBJECT(win->umaxwindow),
                                           "state-changed",
                                           G_CALLBACK (on_umaxed_window_state_changed),
                                           win);
            win->mwh = g_signal_connect(G_OBJECT (win->umaxwindow),
                                        "workspace-changed",
                                        G_CALLBACK (umax_window_workspace_changed),
                                        win);
        }
        else if (win->controlwindow == previous_control)
        {
            /* track previous upper maximized window state on desktop */
            object_unref_nonnull(win->umaxwindow);
            win->umaxwindow = previous_umax;
            object_ref_nonnull(win->umaxwindow);
            if (win->umaxwindow) {
                win->msh = g_signal_connect(G_OBJECT(win->umaxwindow),
                                               "state-changed",
                                               G_CALLBACK (track_changed_max_state),
                                               win);
            }
        }
    }

    if (win->controlwindow != previous_control)
        on_control_window_changed(win->controlwindow, previous_control, win->data);
    else
        on_wck_state_changed(win->controlwindow, win->data);

    object_unref_nonnull(previous_control);
    object_unref_nonnull(previous_umax);
}


/* Triggers when a new window has been opened */
static void on_window_opened (XfwScreen *screen,
                              XfwWindow *window,
                              WckUtils *win)
{
    // track new maximized window
    if (xfw_window_is_maximized(window))
        track_controlled_window (win);
}


/* Triggers when a window has been closed */
static void on_window_closed (XfwScreen *screen,
                              XfwWindow *window,
                              WckUtils *win)
{
    // track closed maximized window
    if (xfw_window_is_maximized(window))
        track_controlled_window (win);
}


/* Triggers when a new active window is selected */
static void active_window_changed (XfwScreen *screen,
                                   XfwWindow *previous,
                                   WckUtils *win)
{

    object_unref_nonnull(win->activewindow);
    win->activewindow = xfw_screen_get_active_window(screen);
    object_ref_nonnull(win->activewindow);

    if (win->activewindow != previous)
    {
        wck_signal_handler_disconnect (G_OBJECT(previous), win->ash);

        track_controlled_window (win);
    }

    if (win->activewindow
        && (win->activewindow != previous)
        && !window_is_desktop (win->activewindow))
    {
        /* Start tracking the new active window */
        win->ash = g_signal_connect(G_OBJECT (win->activewindow), "state-changed", G_CALLBACK (track_changed_max_state), win);
    }
}


/* Triggers when user changes viewports on Compiz */
// We ONLY need this for Compiz (Marco doesn't use viewports)
static void on_viewports_changed (XfwScreen *screen, WckUtils *win)
{
    reload_wnck (win, win->only_maximized, win->data);
}


/* Triggers when user changes workspace on Marco (?) */
static void active_workspace_changed (XfwScreen *screen,
                                      XfwWorkspace *previous,
                                      WckUtils *win)
{
    reload_wnck (win, win->only_maximized, win->data);
}


/* Triggers when workspace groups are created or destroyed */
static void on_workspace_group_changed(XfwWorkspaceManager *manager,
                                       WckUtils *win)
{
    reload_wnck(win, win->only_maximized, win->data);
}


void toggle_maximize (XfwWindow *window)
{
    if (window && xfw_window_is_maximized(window))
        xfw_window_set_maximized(window, TRUE, NULL);
    else
        xfw_window_set_maximized(window, FALSE, NULL);
}


void reload_wnck (WckUtils *win, gboolean only_maximized, gpointer data)
{
    disconnect_wnck (win);

    init_wnck (win, only_maximized, data);
}


void init_wnck (WckUtils *win, gboolean only_maximized, gpointer data)
{
    /* save data */
    win->data = data;

    /* get window proprieties */
    win->activescreen = xfw_screen_get_default();
    win->workspacemanager = xfw_screen_get_workspace_manager(win->activescreen);
    win->activewindow = xfw_screen_get_active_window(win->activescreen);
    object_ref_nonnull(win->activewindow);
    win->umaxwindow = NULL;
    win->controlwindow = NULL;
    win->only_maximized = only_maximized;

    /* Global window tracking */
    win->sah = g_signal_connect(win->activescreen, "active-window-changed", G_CALLBACK (active_window_changed), win);

    if (win->only_maximized)
    {
        win->sch = g_signal_connect(win->activescreen, "window-closed", G_CALLBACK (on_window_closed), win);
        win->soh = g_signal_connect(win->activescreen, "window-opened", G_CALLBACK (on_window_opened), win);
    }

    win->sgch = g_signal_connect(win->workspacemanager, "workspace-group-created", G_CALLBACK(on_workspace_group_changed), win);
    win->sgdh = g_signal_connect(win->workspacemanager, "workspace-group-destroyed", G_CALLBACK(on_workspace_group_changed), win);

    win->sgh = NULL;
    for (GList *workspace_groups =
             xfw_workspace_manager_list_workspace_groups(win->workspacemanager);
         workspace_groups;
         workspace_groups = g_list_next(workspace_groups))
    {
        WorkspaceGroupHandlers *group_handlers = g_new(WorkspaceGroupHandlers, 1);
        group_handlers->workspace_group = XFW_WORKSPACE_GROUP(workspace_groups->data);
        group_handlers->svh = g_signal_connect(workspace_groups->data, "viewports-changed", G_CALLBACK(on_viewports_changed), win);
        group_handlers->swh = g_signal_connect(workspace_groups->data, "active-workspace-changed", G_CALLBACK(active_workspace_changed), win);
        g_object_ref(group_handlers->workspace_group);
        win->sgh = g_list_prepend(win->sgh, group_handlers);
    }

    /* Get controlled window */
    track_controlled_window (win);
    
    if (!win->controlwindow)
        on_control_window_changed (NULL, NULL, win->data);
}


void disconnect_wnck (WckUtils *win)
{
    WorkspaceGroupHandlers *handlers;

    /* disconnect all signal handlers */
    if (win->activewindow)
    {
        wck_signal_handler_disconnect (G_OBJECT(win->activewindow), win->ash);
    }

    if (win->umaxwindow)
    {
        wck_signal_handler_disconnect (G_OBJECT(win->umaxwindow), win->msh);
        wck_signal_handler_disconnect (G_OBJECT(win->umaxwindow), win->mwh);
    }

    wck_signal_handler_disconnect (G_OBJECT(win->activescreen), win->sah);
    wck_signal_handler_disconnect (G_OBJECT(win->activescreen), win->sch);
    wck_signal_handler_disconnect (G_OBJECT(win->activescreen), win->soh);

    wck_signal_handler_disconnect(G_OBJECT(win->workspacemanager), win->sgch);
    wck_signal_handler_disconnect(G_OBJECT(win->workspacemanager), win->sgdh);

    for (GList *sgh = win->sgh; sgh; sgh = g_list_next(sgh))
    {
        handlers = (WorkspaceGroupHandlers *)sgh->data;
        wck_signal_handler_disconnect(G_OBJECT(handlers->workspace_group),
                                        handlers->svh);
        wck_signal_handler_disconnect(G_OBJECT(handlers->workspace_group),
                                        handlers->swh);
        g_object_unref(handlers->workspace_group);
        g_free(handlers);
    }
    g_list_free(win->sgh);
    win->sgh = NULL;
}
