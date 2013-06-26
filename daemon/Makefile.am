if BUILD_MTPD

NULL =

INCLUDES = \
        -I$(top_builddir)/src \
        $(GLIB_CFLAGS)       \
        $(NULL)

libraries = \
        $(top_builddir)/src/libmtp.la \
        $(GLIB_LIBS)                 \
        $(NULL)

dbus_built_sources = mtp-dbus.c mtp-dbus.h
BUILT_SOURCES = $(dbus_built_sources)

$(dbus_built_sources) : Makefile.am dbus-interface.xml
	$(AM_V_GEN) gdbus-codegen                               \
                --interface-prefix org.libmtp.                  \
                --c-namespace MTP                               \
                --generate-c-code mtp-dbus                      \
                $(srcdir)/dbus-interface.xml                    \
                $(NULL)

sbin_PROGRAMS = mtpd

mtpd_SOURCES = mtpd.c
mtpd_LDADD = $(libraries)

# D-BUS service file
%.service: %.service.in ../config.log
	$(AM_V_GEN) $(SED) -e "s|\@libexecdir\@|$(libexecdir)|" $< > $@

servicedir = $(DBUS_SERVICE_DIR)
service_in_files = mtpd.service.in
service_DATA = mtpd.service

EXTRA_DIST =                   \
        mtpd.service.in        \
        dbus-interface.xml     \
        $(NULL)

clean-local:
	rm -f mtpd.service

CLEANFILES =                            \
        $(dbus_built_sources)           \
        $(NULL)

endif
