#include "devfuns.h"
#include "raster.h"
#include "outline.h"
#include "util.h"

/* Get device-specific dev_units_per_point */
static double get_dev_units_per_point(pDevDesc dd)
{
    pGEDevDesc gdd = desc2GEDesc(dd);
    SEXP dev_data = PROTECT(get_device_data(gdd));
    SEXP dev_units_per_point = VECTOR_ELT(dev_data, DEV_DATA_DEV_UNITS_PER_POINT);
    double res = REAL(dev_units_per_point)[0];
    UNPROTECT(1);  /* dev_data */
    return res;
}

void showtext_metric_info(int c, const pGEcontext gc, double* ascent, double* descent, double* width, pDevDesc dd)
{
    FT_Face face;
    FT_Error err;

    /* Font size in points (1/72 inches) */
    double font_size;
    /* Metrics given by FreeType are represented in EM units when
       we set FT_LOAD_NO_SCALE.
       Therefore, we first transform EM units into points. */
    double pts_per_EM_unit;
    /* Then we further convert points to device units.
       Device unit can be in points (usually vector graphics),
       or in pixels (usually bitmap graphics). */
    double dev_units_per_EM_unit;

    if(c == 0) c = 77;  /* Letter 'M' */
    if(c < 0)  c = -c;

    /* c is the Unicode of the character */
    face = get_ft_face(gc, (c < 1024) ? "sans" : "wqy-microhei");
    err = FT_Load_Char(face, c, FT_LOAD_NO_SCALE);
    if(err)
    {
        forward_ft_error(err);
        *ascent = *descent = *width = 0.0;
        return;
    }
    
    font_size = gc->ps * gc->cex;
    pts_per_EM_unit = font_size / face->units_per_EM;
    dev_units_per_EM_unit = pts_per_EM_unit * get_dev_units_per_point(dd);

    *ascent  = face->glyph->metrics.horiBearingY * dev_units_per_EM_unit;
    *descent = face->glyph->metrics.height * dev_units_per_EM_unit - *ascent;
    *width   = face->glyph->metrics.horiAdvance * dev_units_per_EM_unit;
}

double showtext_str_width_utf8(const char* str, const pGEcontext gc, pDevDesc dd)
{
    /* Convert UTF-8 string to Unicode array */
    int max_len = strlen(str);
    unsigned int* unicode =
        (unsigned int*) calloc(max_len + 1, sizeof(unsigned int));
    int len = utf8_to_ucs4(unicode, str, max_len);

    FT_Face face = get_ft_face(gc, all_smaller_than_1024(unicode, len) ? "sans" : "wqy-microhei");
    FT_Error err;

    double font_size = gc->ps * gc->cex;
    double pts_per_EM_unit = font_size / face->units_per_EM;
    double dev_units_per_EM_unit = pts_per_EM_unit * get_dev_units_per_point(dd);

    double width = 0.0;
    int i;
    /* Add up the 'advance' of each character */
    for(i = 0; i < len; i++)
    {
        err = FT_Load_Char(face, unicode[i], FT_LOAD_NO_SCALE);
        if(err)
        {
            forward_ft_error(err);
            continue;
        }
        width += face->glyph->metrics.horiAdvance * dev_units_per_EM_unit;
    }

    free(unicode);

    return width;
}

void showtext_text_utf8_raster(double x, double y, const char* str, double rot, double hadj, const pGEcontext gc, pDevDesc dd)
{
    /* Convert UTF-8 string to Unicode array */
    int max_len = strlen(str);
    if(max_len < 1)
        return;
    
    unsigned int *unicode =
        (unsigned int *) calloc(max_len + 1, sizeof(unsigned int));
    int len = utf8_to_ucs4(unicode, str, max_len);

    /* raster() rotates around the bottom-left corner,
       and text() rotates around the center indicated by hadj. */
    int trans_sign = dd->bottom > dd->top ? -1: 1;
    double trans_x, trans_y;

    /* Calculate pixel size */
    int px = (int) (gc->ps * gc->cex * get_dev_units_per_point(dd) + 0.5);

    /* Get raster data */
    RasterData *rd = get_string_raster_image(unicode, len, px, px,
        rot * DEG2RAD, hadj, gc, &trans_x, &trans_y);

    /* Only plot if we have data, so spaces are excluded. */
    if((rd->ncol > 0) && (rd->nrow > 0))
        dd->raster(rd->data, rd->ncol, rd->nrow,
                   x - trans_x, y - trans_sign * trans_y,
                   rd->ncol, -rd->nrow, 0.0, FALSE, gc, dd);
    
    RasterData_destroy(rd);
    free(unicode);
}

void showtext_text_utf8_polygon(double x, double y, const char* str, double rot, double hadj, const pGEcontext gc, pDevDesc dd)
{
    /* Convert UTF-8 string to Unicode array */
    int max_len = strlen(str);
    if(max_len < 1)
        return;
    
    unsigned int *unicode =
        (unsigned int *) calloc(max_len + 1, sizeof(unsigned int));
    int len = utf8_to_ucs4(unicode, str, max_len);

    FT_Outline_Funcs* funs = get_ft_outline_funcs();
    FT_Face face = get_ft_face(gc, all_smaller_than_1024(unicode, len) ? "sans" : "wqy-microhei");
    double fontSize = gc->ps * gc->cex;

    R_GE_gcontext gc_modify = *gc;

    OutlineData data;
    FT_Outline outline;
    FT_Error err;
    int i;

    double str_width = showtext_str_width_utf8(str, gc, dd);
    double l = hadj * str_width;

    data.ft_to_dev_ratio = fontSize / face->units_per_EM;
    data.offset_x = 0.0;
    data.num_segments = get_num_segments();
    data.trans.sign = dd->bottom > dd->top ? -1: 1;
    data.trans.theta = rot;
    data.trans.x = x - l * cos(rot * DEG2RAD);
    data.trans.y = y - data.trans.sign * l * sin(rot * DEG2RAD);
    data.curr_pos.x = 0;
    data.curr_pos.y = 0;
    data.outline_x = Array_new(100);
    data.outline_y = Array_new(100);
    data.num_poly = 0;
    data.points_in_poly = IntArray_new(10);

    gc_modify.fill = gc->col;
    gc_modify.col = R_RGBA(0xFF, 0xFF, 0xFF, 0x00);
    //gc_modify.col = NA_INTEGER;

    for(i = 0; i < len; i++)
    {
        err = FT_Load_Char(face, unicode[i], FT_LOAD_NO_SCALE);
        if(err)
        {
            forward_ft_error(err);
            continue;
        }
        outline = face->glyph->outline;
        err = FT_Outline_Decompose(&outline, funs, &data);
        if(err)
        {
            forward_ft_error(err);
            Array_destroy(data.outline_x);
            Array_destroy(data.outline_y);
            IntArray_destroy(data.points_in_poly);
            data.outline_x = Array_new(100);
            data.outline_y = Array_new(100);
            data.points_in_poly = IntArray_new(10);
            continue;
        }
        if(data.outline_x->len > 0)
        {
            if(dd->path)
            {
                dd->path(data.outline_x->data,
                         data.outline_y->data,
                         data.num_poly,
                         data.points_in_poly->data,
                         TRUE,
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
                for(p = 0; p < data.num_poly; p++)
                {
                    x0 = x_curr = *x_ptr;
                    y0 = y_curr = *y_ptr;
                    for(l = 0; l < data.points_in_poly->data[p] - 1; l++)
                    {
                        x_ptr++;
                        y_ptr++;
                        dd->line(x_curr, y_curr, *x_ptr, *y_ptr, gc, dd);
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
        Array_destroy(data.outline_x);
        Array_destroy(data.outline_y);
        IntArray_destroy(data.points_in_poly);
        data.outline_x = Array_new(100);
        data.outline_y = Array_new(100);
        data.points_in_poly = IntArray_new(10);
        data.num_poly = 0;
        /*
           After we draw a character, we move the pen right to a distance
           of the advance.
           See the picture in
           http://www.freetype.org/freetype2/docs/tutorial/step2.html
        */
        data.offset_x += face->glyph->metrics.horiAdvance * data.ft_to_dev_ratio;
    }
    Array_destroy(data.outline_x);
    Array_destroy(data.outline_y);
    IntArray_destroy(data.points_in_poly);
    free(unicode);
}
