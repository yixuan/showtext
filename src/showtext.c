#include "devfuns.h"
#include "util.h"

/* Finalizer of the external pointer. */
static void dev_desc_finalizer(SEXP ext_ptr)
{
    pDevDesc dd_saved = (pDevDesc) R_ExternalPtrAddr(ext_ptr);
    if(dd_saved) free(dd_saved);
}

/* Generate an ID for the given device.
   id is assumed to be pre-allocated with 32 bytes. */
static void get_device_id(pGEDevDesc gdd, char* id)
{
    strcpy(id, "dev_");
    snprintf(id + 4, 20, "%p", gdd);
}

/* Add a device to the showtext list by creating an object in the 
   showtext:::.pkg.env$.devs environment. The object name is the ID
   of the device, and the content is a list:
   dev_data: list(use_raster, dev_units_per_point, dd_saved) */
static void add_device(const char* dev_id, SEXP dev_data)
{
    /* showtext:::.pkg.env$.devs */
    SEXP devs_env = PROTECT(get_var_from_pkg_env(".devs", "showtext"));
    Rf_defineVar(Rf_install(dev_id), dev_data, devs_env);
    UNPROTECT(1);  /* devs_env */
}

/* Remove a device from the showtext list. */
static void remove_device(const char* dev_id)
{
    /* showtext:::.pkg.env$.devs */
    SEXP devs_env = PROTECT(get_var_from_pkg_env(".devs", "showtext"));
    R_removeVarFromFrame(Rf_install(dev_id), devs_env);
    UNPROTECT(1);  /* devs_env */
}

/* Check whether a device has already been added. */
static Rboolean device_exists(const char* dev_id)
{
    /* showtext:::.pkg.env$.devs */
    SEXP devs_env = PROTECT(get_var_from_pkg_env(".devs", "showtext"));
    SEXP dev_data = PROTECT(Rf_findVar(Rf_install(dev_id), devs_env));
    Rboolean res = (dev_data != R_UnboundValue);
    UNPROTECT(2);
    return res;
}



/* dev_data: list(use_raster, dev_units_per_point, dd_saved) */
SEXP showtext_begin(SEXP dev_data)
{
    /* curr_dev is an integer assigned to the current graphics device.
       If curr_dev == 0 (in R it is 1), then there is no active device, since the null
       device is always assigned the number 0 (1 in R). */
    int curr_dev = curDevice();
    /* gdd serves as the identifier of a graphics device. We need an ID
       for each device since showtext_begin() and showtext_end() must
       work on the same graphics device.
    
       When calling showtext_begin(), we save an object to the package database
       (showtext:::.pkg.env$.devs), with a variable name generated from gdd.
       In the call of showtext_end(), we then remove this object from the database. */
    pGEDevDesc gdd;
    /* gdd is a pointer, and we convert it into a string that is used as an ID. */
    char dev_id[32];
    /* The device structure containing all the plotting functions.
       Before turning on showtext we save the original structure to dd_save. */
    pDevDesc dd, dd_save;

    SEXP ext_ptr;
    Rboolean use_raster = Rf_asLogical(VECTOR_ELT(dev_data, DEV_DATA_USE_RASTER));

    /* Early return if no device is active. */
    if(curr_dev == 0)
        Rf_error("no active graphics device");

    /* Get the current device and generate an ID. */
    gdd = GEgetDevice(curr_dev);
    get_device_id(gdd, dev_id);
    
    /* Check whether the device already has showtext turned on. */
    if(device_exists(dev_id))
        return R_NilValue;
    
    /* Save the current dd */
    dd = gdd->dev;
    dd_save = (pDevDesc) calloc(1, sizeof(DevDesc));
    *dd_save = *dd;
    ext_ptr = PROTECT(R_MakeExternalPtr(dd_save, R_NilValue, R_NilValue));
    R_RegisterCFinalizerEx(ext_ptr, dev_desc_finalizer, TRUE);
    SET_VECTOR_ELT(dev_data, DEV_DATA_DD_SAVED, ext_ptr);
    UNPROTECT(1);  /* ext_ptr */
    
    /* Save device data to showtext:::.pkg.env$.devs.
       Use dev_id as the variable name. */
    add_device(dev_id, dev_data);

    /* Replace the text functions */
    dd->canHAdj = 2;
    dd->metricInfo = showtext_metric_info;
    dd->hasTextUTF8 = TRUE;
    if(use_raster)
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
    char dev_id[32];
    pDevDesc dd, dd_saved;
    SEXP ext_ptr, dev_data;
    
    if(curr_dev == 0)
        Rf_error("no active graphics device");
        
    gdd = GEgetDevice(curr_dev);
    get_device_id(gdd, dev_id);
    if(!device_exists(dev_id))
        Rf_error("current device did not turn on showtext");

    dev_data = PROTECT(get_device_data(gdd));
    ext_ptr = VECTOR_ELT(dev_data, DEV_DATA_DD_SAVED);
    dd_saved = (pDevDesc) R_ExternalPtrAddr(ext_ptr);
    
    /* Restore dd */
    dd = gdd->dev;
    
    dd->canHAdj        = dd_saved->canHAdj;
    dd->metricInfo     = dd_saved->metricInfo;
    dd->hasTextUTF8    = dd_saved->hasTextUTF8;
    dd->text           = dd_saved->text;
    dd->textUTF8       = dd_saved->textUTF8;
    dd->strWidth       = dd_saved->strWidth;
    dd->strWidthUTF8   = dd_saved->strWidthUTF8;
    dd->wantSymbolUTF8 = dd_saved->wantSymbolUTF8;
    
    /* Remove device data from showtext:::.pkg.env$.devs */
    UNPROTECT(1); /* dev_data */
    remove_device(dev_id);

    return R_NilValue;
}
