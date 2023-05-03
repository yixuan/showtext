#ifndef CAIRODEV_H_INCLUDED
#define CAIRODEV_H_INCLUDED

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#ifndef R_USE_PROTOTYPES
#define R_USE_PROTOTYPES 1
#endif
#include <R_ext/GraphicsEngine.h>
#include <R_ext/GraphicsDevice.h>

/***************************************************************
 * Internal structures of the devices in the Cairo package
 ***************************************************************/

/* Backend ID, defined in Cairo/src/backend.h */
#define BET_IMAGE   1
#define BET_PDF     2
#define BET_PS      3
#define BET_SVG     4
#define BET_XLIB    5
#define BET_W32     6
#define BET_QUARTZ  7
#define BET_USER   64

/* Backend object, defined in Cairo/src/backend.h */
typedef struct st_Rcairo_backend {
    int backend_type;  /* BET_xxx constants */
    void *backendSpecific;
    void *cc;  /* should be cairo_t* but we don't need it */
    void *cs;  /* should be cairo_surface_t* but we don't need it */
    void *dd;  /* should be NewDevDesc* but we don't need it */
    double width, height;
    int in_replay;
    int truncate_rect;
    int serial;
    SEXP onSave;
    int flags;
    double dpix, dpiy;
    void (*save_page)(struct st_Rcairo_backend *be, int pageno);
    void (*destroy_backend)(struct st_Rcairo_backend *be);
    int  (*locator)(struct st_Rcairo_backend *be, double *x, double *y);
    void (*activation)(struct st_Rcairo_backend *be, int activate);
    void (*mode)(struct st_Rcairo_backend *be, int mode);
    void (*resize)(struct st_Rcairo_backend *be, double width, double height);
    void (*sync)(struct st_Rcairo_backend *be);
} Rcairo_backend;

/*
 * Device specific information, defined in Cairo/src/cairogd.h 
 */
typedef struct {
#ifdef USE_MAGIC
    int magic;
#endif
    double cex;
    int lty;
    double lwd;
    int col;
    int fill;
    int canvas;
    int fontface;
    int fontsize;
    int basefontface;
    int basefontsize;
    Rcairo_backend *cb; /* contains the backend information */
    int bg;
    double gamma;
    double dpix, dpiy, asp;
    double fontscale;
    int gd_fill, gd_draw;
    double gd_ftsize, gd_ftm_ascent, gd_ftm_descent, gd_ftm_width;
    int gd_ftm_char;
    int npages;
    int holdlevel;
} CairoGDDesc;



/* 
 * Returns TRUE if the current Cairo device is bitmap-based
 * (e.g. png, screen window, etc.)
 */
SEXP showtext_cairo_device_bitmap(void);

#endif /* CAIRODEV_H_INCLUDED */
