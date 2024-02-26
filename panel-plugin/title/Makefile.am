AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-DG_LOG_DOMAIN=\"xfce4-wcktitle-plugin\" \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\" \
	$(PLATFORM_CPPFLAGS)

#
# Windowck Title plugin
#
plugin_LTLIBRARIES = \
	libwcktitle.la

plugindir = \
	$(libdir)/xfce4/panel/plugins

libwcktitle_built_sources = \
	wcktitle-dialogs_ui.h

libwcktitle_la_SOURCES = \
	$(libwcktitle_built_sources) \
	wcktitle.c \
	wcktitle.h \
	wcktitle-dialogs.c \
	wcktitle-dialogs.h \
	wcktitle-title.h \
	wcktitle-title.c

libwcktitle_la_CFLAGS = \
	$(GTK_CFLAGS) \
	$(LIBXFCE4PANEL_CFLAGS) \
	$(LIBXFCE4UI_CFLAGS) \
	$(LIBXFCE4UTIL_CFLAGS) \
	$(LIBXFCONF_CFLAGS) \
	$(LIBXFCE4WINDOWING_CFLAGS) \
	$(LIBXFCE4WINDOWINGUI_CFLAGS) \
	$(PLATFORM_CFLAGS)

libwcktitle_la_LDFLAGS = \
	-avoid-version \
	-module \
	-no-undefined \
	-export-symbols-regex '^xfce_panel_module_(preinit|init|construct)' \
	$(PLATFORM_LDFLAGS)

libwcktitle_la_LIBADD = \
	$(top_builddir)/common/libwck-common.la \
	$(GTK_LIBS) \
	$(LIBXFCE4PANEL_LIBS) \
	$(LIBXFCE4UI_LIBS) \
	$(LIBXFCE4UTIL_LIBS) \
	$(LIBXFCONF_LIBS) \
	$(LIBXFCE4WINDOWING_LIBS) \
	$(LIBXFCE4WINDOWINGUI_LIBS)

libwcktitle_la_DEPENDENCIES = \
	$(top_builddir)/common/libwck-common.la

#
# Desktop file
#
desktopdir = \
	$(datadir)/xfce4/panel/plugins

desktop_DATA = \
	wcktitle.desktop

@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = \
	wcktitle-dialogs.glade \
	wcktitle.desktop.in

CLEANFILES = \
	$(desktop_DATA)

BUILT_SOURCES = \
	$(libwcktitle_built_sources)

CLEANFILES += \
	$(libwcktitle_built_sources)

wcktitle-dialogs_ui.h: wcktitle-dialogs.glade
	$(AM_V_GEN) xdt-csource --static --strip-comments --strip-content --name=wcktitle_dialogs_ui $< >$@

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
