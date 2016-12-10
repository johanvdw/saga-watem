#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libwatem.la
libwatem_la_SOURCES =\
Calculate_LS_Watem.cpp\
Calculate_Uparea.cpp\
MLB_Interface.cpp\
Tillage_Erosion.cpp\
Water_Erosion.cpp\
MLB_Interface.h\
Calculate_LS_Watem.h\
Calculate_Uparea.h\
Tillage_Erosion.h\
Water_Erosion.h

libwatem_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
