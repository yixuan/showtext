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

SEXP GetVarFromPkgEnv(const char *varName, const char *pkgName);
SEXP GetPkgEnv(const char *pkgName);
FT_Outline_Funcs* GetFTOutlineFuncs();
int GetNseg();
pDevDesc GetSavedDevDesc();
pGEDevDesc GetSavedDeviceID();

#endif /* UTIL_H_INCLUDED */
