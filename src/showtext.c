#include "devfuns.h"
#include "util.h"

SEXP showtext_null_pointer()
{
    SEXP ext_ptr;

    ext_ptr = PROTECT(R_MakeExternalPtr(NULL, R_NilValue, R_NilValue));
    UNPROTECT(1);
    return ext_ptr;
}

SEXP showtext_new_dev_desc()
{
    pDevDesc dd_save = (pDevDesc) calloc(1, sizeof(DevDesc));
    SEXP ext_ptr;

    ext_ptr = PROTECT(R_MakeExternalPtr(dd_save, R_NilValue, R_NilValue));
    UNPROTECT(1);
    return ext_ptr;
}

SEXP showtext_free_dev_desc(SEXP ext_ptr)
{
    pDevDesc dd_saved = (pDevDesc) R_ExternalPtrAddr(ext_ptr);
    if(dd_saved) free(dd_saved);
    
    return R_NilValue;
}

SEXP showtext_begin()
{
    /* curr_dev is an integer assigned to the current graphics device.
       If curr_dev == 0 (in R it is 1), then there is no active device, since the null
       device is always assigned the number 0 (1 in R). */
    int curr_dev = curDevice();
    SEXP ext_ptr, showtext_env;
    /* gdd serves as the identifier of a graphics device. We need an id
       for each device since showtext_begin() and showtext_end() must
       work on the same graphics device.
    
       When calling showtext_begin(), we save the gdd of the current device
       to the package database (showtext:::.pkg.env), and in the call of
       showtext_end(), we compare it with the active device at that time. */
    pGEDevDesc gdd;
    /* The device structure that we want to modify */
    pDevDesc dd;
    
    if(curr_dev == 0)
        Rf_error("no active graphics device");

    /* Save the current gdd to showtext:::.pkg.env */
    gdd = GEgetDevice(curr_dev);
    ext_ptr = PROTECT(R_MakeExternalPtr(gdd, R_NilValue, R_NilValue));
    showtext_env = PROTECT(get_pkg_env("showtext"));
    Rf_setVar(Rf_install(".device_id"), ext_ptr, showtext_env);
    UNPROTECT(2);
    
    /* Save the current dd */
    dd = gdd->dev;
    *(get_saved_dev_desc()) = *dd;
    
    /* Replace the text functions */
    dd->canHAdj = 2;
    dd->metricInfo = showtext_metric_info;
    dd->hasTextUTF8 = TRUE;
    if(use_raster())
    {
        dd->text = showtext_text_utf8_raster;
        dd->textUTF8 = showtext_text_utf8_raster;
    } else {
        dd->text = showtext_text_utf8_polygon;
        dd->textUTF8 = showtext_text_utf8_polygon;
    }
    dd->strWidth = showtext_str_width_utf8;
    dd->strWidthUTF8 = showtext_str_width_utf8;
    dd->wantSymbolUTF8 = TRUE;

    return R_NilValue;
}

SEXP showtext_end()
{
    int curr_dev = curDevice();
    pGEDevDesc gdd;
    pDevDesc dd;
    pDevDesc dd_saved;
    
    if(curr_dev == 0)
        Rf_error("no active graphics device");
        
    gdd = GEgetDevice(curr_dev);
    if(gdd != get_saved_device_id())
    {
        Rf_error("current device does not match the one that uses showtext_begin()");
    }
    
    /* Restore dd */
    dd = gdd->dev;
    dd_saved = get_saved_dev_desc();
    
    dd->canHAdj        = dd_saved->canHAdj;
    dd->metricInfo     = dd_saved->metricInfo;
    dd->hasTextUTF8    = dd_saved->hasTextUTF8;
    dd->text           = dd_saved->text;
    dd->textUTF8       = dd_saved->textUTF8;
    dd->strWidth       = dd_saved->strWidth;
    dd->strWidthUTF8   = dd_saved->strWidthUTF8;
    dd->wantSymbolUTF8 = dd_saved->wantSymbolUTF8;

    return R_NilValue;
}

