AC_INIT([libDwmWebUtils],m4_esyscmd_s([./getvers.sh -v]),[dwmcrobb@me.com],[libDwmWebUtils],[http://www.mcplex.net])

AC_PROG_CC
AC_PROG_CXX
AC_CANONICAL_HOST

dnl  ###  Read some generally useful macros.
builtin(include, ./dwm.m4)  dnl

dnl  Set variables for building shared library
DWM_SET_CXX_SHARED_FLAGS

dnl  Set variables for pthreads
DWM_SET_PTHREADFLAGS

OSLIBS="-lssl -lcrypto"
KVMLIB="-lkvm"
LIBTOOL="libtool"

case $host_os in
  freebsd[[89]]*)
    CXXFLAGS="-O2"
    LDFLAGS="-static"
    ;;
  freebsd1[[0123]]*)
    CXXFLAGS="-O2"
    LDFLAGS="-static"
    ;;
  linux*)
    CXXFLAGS="-O2"
    KVMLIB=""
    LDFLAGS="$LDFLAGS"
    AC_CHECK_LIB(procps, readproctab, [OSLIBS+=" -lprocps"], [exit 1])
    DEBARCH=`dpkg --print-architecture`
    AC_SUBST(DEBARCH)
    ;;
  darwin*)
    CC="clang++"
    CXX="clang++"
    CXXFLAGS="${CXXFLAGS} -O2 -stdlib=libc++"
    KVMLIB=""
    LDFLAGS="${LDFLAGS} -O2 -stdlib=libc++ -std=c++14"
    LIBTOOL="glibtool"
    OSLIBS+="-framework Foundation -framework Security"
    ;;
esac

echo "host_os ${host_os}"

dnl  Check for C++17
DWM_CHECK_CPLUSPLUS_17

DWM_CHECK_NEED_LIBATOMIC
DWM_CHECK_NEED_LIBRT

AC_SUBST(KVMLIB)
AC_SUBST(LIBTOOL)
AC_SUBST(OSLIBS)

DWMDIR=`pkg-config --variable=prefix libDwm`
DWMINCS=`pkg-config --cflags-only-I libDwm`
DWMLIBS=`pkg-config --libs libDwm`

AC_SUBST(DWMDIR)
AC_SUBST(DWMINCS)
AC_SUBST(DWMLIBS)

DWM_CHECK_BOOSTASIO
DWM_CHECK_NEED_LIBIBVERBS

AC_SUBST(LDFLAGS)
AC_SUBST(PCAPINC)
AC_SUBST(PCAPLIB)

DWM_CHECK_STRTOF()

DWM_PREREQ_PATH(htmlman,share/htmlman,[HTML manpage path])

DWM_SET_PKGVARS(libDwm,[libDwm-0.0.0])
DWM_GET_TAG([libDwm])

dnl ------------------------------------------------------------------------
dnl //  Check for CoreFoundation/CFByteOrder.h, an OS X thing.  We need
dnl //  this so we can define be64toh() and htobe64() to use
dnl //  CFSwapInt64BigToHost() and CFSwapInt64HostToBig() respectively.
dnl ------------------------------------------------------------------------
AC_CHECK_HEADER(CoreFoundation/CFByteOrder.h, [AC_DEFINE(HAVE_CFBYTE_ORDER_H)])

dnl ------------------------------------------------------------------------
dnl //  Check for byteswap.h, a Linux thing.  We need this so we can define
dnl //  be64toh() and htobe64() to use bswap_64() on Linux running on 
dnl //  little-endian hardware.
dnl ------------------------------------------------------------------------
AC_CHECK_HEADER(byteswap.h, [AC_DEFINE(HAVE_BYTESWAP_H)])
AC_CHECK_HEADER(endian.h, [AC_DEFINE(HAVE_ENDIAN_H)])

dnl ------------------------------------------------------------------------
dnl //  Check for <kvm.h>
dnl ------------------------------------------------------------------------
AC_CHECK_HEADER(kvm.h, [AC_DEFINE(HAVE_KVM_H)])

dnl ------------------------------------------------------------------------
dnl //  Check for <net/pfvar.h> and <net/if_pflog.h>
dnl ------------------------------------------------------------------------
AC_CHECK_HEADER(net/pfvar.h, [AC_DEFINE(HAVE_NET_PFVAR_H)], [],
                [#include <net/if.h>])
AC_CHECK_HEADER(net/if_pflog.h, [AC_DEFINE(HAVE_NET_IF_PFLOG_H)], [],
                [#include <net/if.h>
		 #include <net/pfvar.h>])

AC_CHECK_PROG([MANDOC], [mandoc], [mandoc], [])

case $host_os in
  freebsd1*)  
    DWM_CHECK_SQLITE3()
    AC_SUBST(SQLITEDIR)
    ;;
esac

AC_SUBST(CXXFLAGS)

DWM_CHECK_LIBSTDCPPFS

BUILD_DOCS=""
AC_ARG_ENABLE([docs],[AS_HELP_STRING([--enable-docs],[build documentation])],
	      [BUILD_DOCS="yes"], [])
AC_SUBST(BUILD_DOCS)

AC_CONFIG_FILES([Makefile.vars packaging/debcontrol
                 packaging/fbsd_manifest packaging/libDwmWebUtils.pc])
AC_OUTPUT
