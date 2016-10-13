#include "devfuns.h"
#include "raster.h"
#include "outline.h"
#include "util.h"

void showtextMetricInfo(int c, const pGEcontext gc, double* ascent, double* descent, double* width, pDevDesc dd)
{
    FT_Face face = GetFTFace(gc);
    FT_Error err;

    /* Font size in points (1/72 inches) */
    double font_size = gc->ps * gc->cex;
    /* Metrics given by FreeType are represented in EM units when
       we set FT_LOAD_NO_SCALE.
       Therefore, we first transform EM units into points. */
    double pts_per_EM_unit = font_size / face->units_per_EM;
    /* Then we further convert points to device units.
       Device unit can be in points (usually vector graphics),
       or in pixels (usually bitmap graphics). */
    double dev_units_per_EM_unit = pts_per_EM_unit * GetDevUnitsPerPoint();

    if(c == 0) c = 77;  /* Letter 'M' */
    if(c < 0)  c = -c;

    /* c is the unicode of the character */
    err = FT_Load_Char(face, c, FT_LOAD_NO_SCALE);
    if(err)
    {
        FTError(err);
        *ascent = *descent = *width = 0.0;
        return;
    }

    *ascent = face->glyph->metrics.horiBearingY * dev_units_per_EM_unit;
    *descent = face->glyph->metrics.height * dev_units_per_EM_unit - *ascent;
    *width = face->glyph->metrics.horiAdvance * dev_units_per_EM_unit;
}

double showtextStrWidthUTF8(const char *str, const pGEcontext gc, pDevDesc dd)
{
    /* Convert UTF-8 string to Unicode array */
    int max_len = strlen(str);
    unsigned int *unicode =
        (unsigned int *) calloc(max_len + 1, sizeof(unsigned int));
    int len = utf8toucs4(unicode, str, max_len);

    FT_Face face = GetFTFace(gc);
    FT_Error err;

    double font_size = gc->ps * gc->cex;
    double pts_per_EM_unit = font_size / face->units_per_EM;
    double dev_units_per_EM_unit = pts_per_EM_unit * GetDevUnitsPerPoint();

    double width = 0.0;
    int i;
    /* Add up the 'advance' of each character */
    for(i = 0; i < len; i++)
    {
        err = FT_Load_Char(face, unicode[i], FT_LOAD_NO_SCALE);
        if(err)
        {
            FTError(err);
            continue;
        }
        width += face->glyph->metrics.horiAdvance * dev_units_per_EM_unit;
    }

    free(unicode);

    return width;
}

void showtextTextUTF8Raster(double x, double y, const char *str, double rot, double hadj, const pGEcontext gc, pDevDesc dd)
{
    /* Convert UTF-8 string to Unicode array */
    int max_len = strlen(str);
    if(max_len < 1)
        return;
    
    unsigned int *unicode =
        (unsigned int *) calloc(max_len + 1, sizeof(unsigned int));
    int len = utf8toucs4(unicode, str, max_len);

    /* raster() rotates around the bottom-left corner,
       and text() rotates around the center indicated by hadj. */
    int trans_sign = dd->bottom > dd->top ? -1: 1;
    double trans_X, trans_Y;

    /* Calculate pixel size */
    int px = (int) (gc->ps * gc->cex * GetDevUnitsPerPoint() + 0.5);

    /* Get raster data */
    RasterData *rd = GetStringRasterImage(unicode, len, px, px,
        rot * DEG2RAD, hadj, gc, &trans_X, &trans_Y);

    dd->raster(rd->data, rd->ncol, rd->nrow,
               x - trans_X, y - trans_sign * trans_Y,
               rd->ncol, -rd->nrow, 0.0, FALSE, gc, dd);
    FreeRasterData(rd);
    free(unicode);
}

void showtextTextUTF8Polygon(double x, double y, const char *str, double rot, double hadj, const pGEcontext gc, pDevDesc dd)
{
    /* Convert UTF-8 string to Unicode array */
    int max_len = strlen(str);
    if(max_len < 1)
        return;
    
    unsigned int *unicode =
        (unsigned int *) calloc(max_len + 1, sizeof(unsigned int));
    int len = utf8toucs4(unicode, str, max_len);

    FT_Outline_Funcs *funs = GetFTOutlineFuncs();
    FT_Face face = GetFTFace(gc);
    double fontSize = gc->ps * gc->cex;

    R_GE_gcontext gc_modify = *gc;

    OutlineData data;
    FT_Outline outline;
    FT_Error err;
    int i;

    double strWidth = showtextStrWidthUTF8(str, gc, dd);
    double l = hadj * strWidth;

    data.ratio_EM = fontSize / face->units_per_EM;
    data.deltax = 0.0;
    data.nseg = GetNseg();
    data.trans.sign = dd->bottom > dd->top ? -1: 1;
    data.trans.theta = rot;
    data.trans.x = x - l * cos(rot * DEG2RAD);
    data.trans.y = y - data.trans.sign * l * sin(rot * DEG2RAD);
    data.curr_dev_trans.x = 0;
    data.curr_dev_trans.y = 0;
    data.outline_x = ArrayNew(100);
    data.outline_y = ArrayNew(100);
    data.npoly = 0;

    gc_modify.fill = gc->col;
    gc_modify.col = R_RGBA(0xFF, 0xFF, 0xFF, 0x00);
    //gc_modify.col = NA_INTEGER;

    for(i = 0; i < len; i++)
    {
        err = FT_Load_Char(face, unicode[i], FT_LOAD_NO_SCALE);
        if(err)
        {
            FTError(err);
            continue;
        }
        outline = face->glyph->outline;
        err = FT_Outline_Decompose(&outline, funs, &data);
        if(err)
        {
            FTError(err);
            ArrayDestroy(data.outline_x);
            ArrayDestroy(data.outline_y);
            data.outline_x = ArrayNew(100);
            data.outline_y = ArrayNew(100);
            continue;
        }
        if(data.outline_x->len > 0)
        {
            if(dd->path)
            {
                dd->path(data.outline_x->data,
                         data.outline_y->data,
                         data.npoly,
                         data.nper,
                         FALSE,
                         &gc_modify, dd);
            } else if(dd->polygon) {
                dd->polygon(data.outline_x->len,
                        data.outline_x->data,
                        data.outline_y->data,
                        &gc_modify, dd);
            } else if(dd->line) {
                int p, l;
                double *x_ptr = data.outline_x->data;
                double *y_ptr = data.outline_y->data;
                double x_curr, y_curr, x0, y0;
                for(p = 0; p < data.npoly; p++)
                {
                    x0 = x_curr = *x_ptr;
                    y0 = y_curr = *y_ptr;
                    for(l = 0; l < data.nper[p] - 1; l++)
                    {
                        x_ptr++;
                        y_ptr++;
                        dd->line(x_curr, y_curr, *x_ptr, *y_ptr,
                                 gc, dd);
                        x_curr = *x_ptr;
                        y_curr = *y_ptr;
                    }
                    /* Close the polygon */
                    dd->line(x_curr, y_curr, x0, y0, gc, dd);
                    x_ptr++;
                    y_ptr++;
                }
            } else {
                Rf_error("device should be capable of path(), polygon() or line()");
            }
        }
        ArrayDestroy(data.outline_x);
        ArrayDestroy(data.outline_y);
        data.outline_x = ArrayNew(100);
        data.outline_y = ArrayNew(100);
        data.npoly = 0;
        /*
           After we draw a character, we move the pen right to a distance
           of the advance.
           See the picture in
           http://www.freetype.org/freetype2/docs/tutorial/step2.html
        */
        data.deltax += face->glyph->metrics.horiAdvance * data.ratio_EM;
    }
    ArrayDestroy(data.outline_x);
    ArrayDestroy(data.outline_y);
    free(unicode);
}
