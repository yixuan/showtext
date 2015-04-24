#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#ifndef R_USE_PROTOTYPES
  #define R_USE_PROTOTYPES 1
#endif
#include <R_ext/GraphicsEngine.h>
#include <R_ext/GraphicsDevice.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H


/* Get the .pkg.env variable (an environment) defined in the package */
/* The sysfonts and showtext packages are using this way to store variables */
SEXP GetPkgEnv(const char *pkgName);

/* Get a variable from .pkg.env */
SEXP GetVarFromPkgEnv(const char *varName, const char *pkgName);

/* Get .pkg.env$.outline_funs */
FT_Outline_Funcs* GetFTOutlineFuncs();

/* Get pkg.env$.nseg */
int GetNseg();

/* Get .pkg.env$.dev_units_per_point */
double GetDevUnitsPerPoint();

/* Get .pkg.env$.dd_saved */
pDevDesc GetSavedDevDesc();

/* Get .pkg.env$.device_id */
pGEDevDesc GetSavedDeviceID();

/* Get .pkg.env$.use_raster */
Rboolean UseRaster();


#endif /* UTIL_H_INCLUDED */
