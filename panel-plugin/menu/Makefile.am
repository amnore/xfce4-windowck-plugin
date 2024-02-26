AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-DG_LOG_DOMAIN=\"xfce4-wckmenu-plugin\" \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\" \
	$(PLATFORM_CPPFLAGS)

#
# Windowck Menu plugin
#
plugin_LTLIBRARIES = \
	libwckmenu.la

plugindir = \
	$(libdir)/xfce4/panel/plugins

libwckmenu_built_sources = \
	wckmenu-dialogs_ui.h

libwckmenu_la_SOURCES = \
	$(libwckmenu_built_sources) \
	wckmenu.c \
	wckmenu.h \
	wckmenu-dialogs.c \
	wckmenu-dialogs.h \
	wckmenu-icon.c \
	wckmenu-icon.h

libwckmenu_la_CFLAGS = \
	$(GTK_CFLAGS) \
	$(LIBXFCE4PANEL_CFLAGS) \
	$(LIBXFCE4UI_CFLAGS) \
	$(LIBXFCE4UTIL_CFLAGS) \
	$(LIBXFCONF_CFLAGS) \
	$(LIBXFCE4WINDOWING_CFLAGS) \
	$(LIBXFCE4WINDOWINGUI_CFLAGS) \
	$(PLATFORM_CFLAGS)

libwckmenu_la_LDFLAGS = \
	-avoid-version \
	-module \
	-no-undefined \
	-export-symbols-regex '^xfce_panel_module_(preinit|init|construct)' \
	$(PLATFORM_LDFLAGS)

libwckmenu_la_LIBADD = \
	$(top_builddir)/common/libwck-common.la \
	$(GTK_LIBS) \
	$(LIBXFCE4PANEL_LIBS) \
	$(LIBXFCE4UI_LIBS) \
	$(LIBXFCE4UTIL_LIBS) \
	$(LIBXFCONF_LIBS) \
	$(LIBXFCE4WINDOWING_LIBS) \
	$(LIBXFCE4WINDOWINGUI_LIBS)

libwckmenu_la_DEPENDENCIES = \
	$(top_builddir)/common/libwck-common.la

#
# Desktop file
#
desktopdir = \
	$(datadir)/xfce4/panel/plugins

desktop_DATA = \
	wckmenu.desktop

@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = \
	wckmenu-dialogs.glade \
	wckmenu.desktop.in

CLEANFILES = \
	$(desktop_DATA)

BUILT_SOURCES = \
	$(libwckmenu_built_sources)

CLEANFILES += \
	$(libwckmenu_built_sources)

wckmenu-dialogs_ui.h: wckmenu-dialogs.glade
	$(AM_V_GEN) xdt-csource --static --strip-comments --strip-content --name=wckmenu_dialogs_ui $< >$@

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
