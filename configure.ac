AC_INIT(SHOWTEXT, 0.9, yixuan.qiu@cos.name)

# Find R_HOME and set CC/CFLAGS/CPPFLAGS
: ${R_HOME=`R RHOME`}
if test -z "${R_HOME}"; then
    echo "could not determine R_HOME"
    exit 1
fi
CC=`"${R_HOME}/bin/R" CMD config CC`
CFLAGS=`"${R_HOME}/bin/R" CMD config CFLAGS`
CPPFLAGS=`"${R_HOME}/bin/R" CMD config CPPFLAGS`

# Checks for programs.
AC_PROG_CC

# Checks for pkg-config
AC_PATH_PROG([PKGCONF], [pkg-config], [], [$PATH:/usr/bin:/usr/local/bin:/opt/bin:/opt/homebrew/bin])

# If pkg-config is available, use it to find needed libraries
if test -n "${PKGCONF}"; then
    # Check zlib
    AC_MSG_CHECKING([whether pkg-config could find zlib])
    if "${PKGCONF}" --exists zlib; then
        AC_MSG_RESULT([yes])
        ZLIB_CPPFLAGS=`"${PKGCONF}" --cflags zlib`
        ZLIB_LIBS=`"${PKGCONF}" --libs zlib`
    else
        AC_MSG_RESULT([no])
    fi
    # Check libpng
    AC_MSG_CHECKING([whether pkg-config could find libpng])
    if "${PKGCONF}" --exists libpng; then
        AC_MSG_RESULT([yes])
        LIBPNG_CPPFLAGS=`"${PKGCONF}" --cflags libpng`
        LIBPNG_LIBS=`"${PKGCONF}" --libs libpng`
    else
        AC_MSG_RESULT([no])
    fi
    # Check freetype2
    AC_MSG_CHECKING([whether pkg-config could find freetype2])
    if "${PKGCONF}" --exists freetype2; then
        AC_MSG_RESULT([yes])
        FT_CPPFLAGS=`"${PKGCONF}" --cflags freetype2`
        FT_LIBS=`"${PKGCONF}" --libs freetype2`
    else
        AC_MSG_RESULT([no])
    fi
fi

# If pkg-config is not found, or if some library is not detected
# by pkg-config, try to check them directly

# Check zlib
if test -z "${ZLIB_LIBS}"; then
    AC_CHECK_LIB([z], [deflate], [ZLIB_LIBS=-lz], [ZLIB_LIBS=])
    if test -z "${ZLIB_LIBS}"; then
        echo ""
        echo "****************************************************"
        echo "Error: zlib not found"
        echo "If you have not installed zlib, you can download"
        echo "the source code from http://www.zlib.net/"
        echo ""
        echo "In Debian/Ubuntu-like systems, you can use"
        echo '  "sudo apt-get install zlib1g-dev"'
        echo "to install zlib"
        echo ""
        echo "For rpm-based systems, try"
        echo '  "sudo yum install zlib-devel"'
        echo "****************************************************"
        echo ""
        exit 1
    fi
fi

# Check libpng
if test -z "${LIBPNG_LIBS}"; then
    AC_CHECK_LIB([png], [png_init_io], [LIBPNG_LIBS=-lpng], [LIBPNG_LIBS=])
    if test -z "${LIBPNG_LIBS}"; then
        echo ""
        echo "****************************************************"
        echo "Error: libpng not found."
        echo "If you have not installed libpng, you can download"
        echo "the source code from http://www.libpng.org/"
        echo ""
        echo "In Debian/Ubuntu-like systems, you can use"
        echo '  "sudo apt-get install libpng12-dev"'
        echo "to install libpng"
        echo ""
        echo "For rpm-based systems, try"
        echo '  "sudo yum install libpng-devel"'
        echo "****************************************************"
        echo ""
        exit 1
    fi
fi

## Check freetype
if test -z "${FT_LIBS}"; then
    AC_CHECK_PROG([HAVE_FREETYPE_CONFIG], [freetype-config], [yes], [no])
    if test [ "x$HAVE_FREETYPE_CONFIG" = "xyes" ]; then
        FT_CPPFLAGS="`freetype-config --cflags`"
        FT_LIBS="`freetype-config --libs`"
    else
        echo ""
        echo "****************************************************"
        echo "Error: freetype-config not found."
        echo "Please install FreeType with freetype-config script."
        echo "If you have not installed FreeType, you can"
        echo "download the source code from http://freetype.org/"
        echo ""
        echo "In Debian/Ubuntu-like systems, you can use"
        echo '  "sudo apt-get install libfreetype6-dev"'
        echo "to install FreeType"
        echo ""
        echo "For rpm-based systems, try"
        echo '  "sudo yum install freetype-devel"'
        echo "****************************************************"
        echo ""
    exit 1
    fi
fi

# Additional check for linking
CPPFLAGS="$CPPFLAGS ${FT_CPPFLAGS}"
LIBS0="$LIBS"
LIBS="$LIBS0 ${FT_LIBS}"
AC_MSG_CHECKING([whether freetype2 flags work])
AC_LINK_IFELSE([AC_LANG_CALL([], [FT_Init_FreeType])], AC_MSG_RESULT([yes]), [
    AC_MSG_RESULT([no])
    AC_MSG_CHECKING([whether --static helps])
    FT_LIBS=`"${PKGCONF}" --libs --static freetype2`
    LIBS="$LIBS0 ${FT_LIBS}"
    AC_LINK_IFELSE([AC_LANG_CALL([], [FT_Init_FreeType])], AC_MSG_RESULT([yes]), [
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([Cannot get FreeType to work, check config.log for details.])
    ])
])

SHOWTEXT_CPPFLAGS="${FT_CPPFLAGS} ${LIBPNG_CPPFLAGS} ${ZLIB_CPPFLAGS}"
SHOWTEXT_LIBS="${FT_LIBS} ${LIBPNG_LIBS} ${ZLIB_LIBS}"

AC_SUBST(SHOWTEXT_CPPFLAGS)
AC_SUBST(SHOWTEXT_LIBS)
AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT

