#include "cairodev.h"

/* 
 * Returns TRUE if the current Cairo device is bitmap-based
 * (e.g. png, screen window, etc.)
 * 
 * We assume that the current device is either from the Cairo package
 * or from the cairoDevice package, but we only test the devices in the
 * Cairo package
 */
SEXP showtext_cairo_device_bitmap(void)
{
    /* curr_dev is an integer assigned to the current graphics device.
     If curr_dev == 0 (in R it is 1), then there is no active device, since the null
     device is always assigned the number 0 (1 in R). */
    int curr_dev = curDevice();

    /* Device objects */
    pGEDevDesc gdd;
    pDevDesc dd;

    /* No active graphics device */
    if(curr_dev == 0)
        return Rf_ScalarLogical(FALSE);

    gdd = GEgetDevice(curr_dev);
    dd = gdd->dev;
    if(!dd)
        return Rf_ScalarLogical(FALSE);

    /* 
     * For now, one way to test whether the device is from Cairo or from
     * cairoDevice is to see the dd->canHAdj field. Cairo defines it to be 2,
     * whereas cairoDevice defines it to be 0. This may change in the future.
     */
    if(dd->canHAdj > 0)
    {
        CairoGDDesc *xd = (CairoGDDesc *) dd->deviceSpecific;
        int backend = xd->cb->backend_type;
        if(backend == BET_IMAGE || backend == BET_XLIB ||
           backend == BET_W32   || backend == BET_QUARTZ)
            return Rf_ScalarLogical(TRUE);
    }

    return Rf_ScalarLogical(FALSE);
}
