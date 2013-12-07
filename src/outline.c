#include "outline.h"

void transformPoint(Point *before, Point *after,
                    TransData *trans)
{
    /* degree to radian */
    double rad = trans->theta * DEG2RAD; /* from R_ext/GraphicsEngine.h */
    double cost = cos(rad);
    double sint = sin(rad);
    
    after->x = before->x * cost - trans->sign * before->y * sint + trans->x;
    after->y = trans->sign * before->x * sint + before->y * cost + trans->y;  
}

int outlineMoveTo(const FT_Vector* to, void* user)
{
    /* Rprintf("move\n"); */
    OutlineData *data = (OutlineData *) user;
    double ratio_EM = data->ratio_EM;
    
    Point to_dev, to_dev_trans;
    
    /* Calculate transformed coordinates */
    to_dev.x = to->x * ratio_EM + data->deltax;
    to_dev.y = data->trans.sign * to->y * ratio_EM;
    transformPoint(&to_dev, &to_dev_trans, &(data->trans));
    
    /* Add to the polygon data */
    ArrayAppend(data->outline_x, to_dev_trans.x);
    ArrayAppend(data->outline_y, to_dev_trans.y);
    data->npoly++;
    data->nper[data->npoly - 1] = 1;
    
    /* Update current position */
    data->curr_dev_trans = to_dev_trans;
    
    return 0;
}

int outlineLineTo(const FT_Vector* to, void* user)
{
    /* Rprintf("line\n"); */
    OutlineData *data = (OutlineData *) user;
    double ratio_EM = data->ratio_EM;

    Point to_dev, to_dev_trans;
    
    /* Calculate transformed coordinates */
    to_dev.x = to->x * ratio_EM + data->deltax;
    to_dev.y = data->trans.sign * to->y * ratio_EM;
    transformPoint(&to_dev, &to_dev_trans, &(data->trans));
    
    /* Add to the polygon data */
    ArrayAppend(data->outline_x, to_dev_trans.x);
    ArrayAppend(data->outline_y, to_dev_trans.y);
    data->nper[data->npoly - 1]++;
    
    /* Update current position */
    data->curr_dev_trans = to_dev_trans;
    
    return 0;
}

int outlineConicTo(const FT_Vector* control, const FT_Vector* to, void* user)
{
    /* Rprintf("conic\n"); */
    OutlineData *data = (OutlineData *) user;
    double ratio_EM = data->ratio_EM;
    double lambda = 0.0, one_lambda = 1.0;
    double delta = 1.0 / data->nseg;

    Point to_dev, to_dev_trans;
    Point control_dev, control_dev_trans;
    Point b;

    /* Calculate transformed coordinates */
    to_dev.x = to->x * ratio_EM + data->deltax;
    to_dev.y = data->trans.sign * to->y * ratio_EM;
    transformPoint(&to_dev, &to_dev_trans, &(data->trans));
    
    control_dev.x = control->x * ratio_EM + data->deltax;
    control_dev.y = data->trans.sign * control->y * ratio_EM;
    transformPoint(&control_dev, &control_dev_trans, &(data->trans));
    
    /* Calculate curve coordinates */
    for (lambda = 0.0; lambda < 1; lambda += delta, one_lambda -= delta)
    {
        b.x = one_lambda * one_lambda * data->curr_dev_trans.x +
              2 * lambda * one_lambda * control_dev_trans.x +
              lambda * lambda * to_dev_trans.x;
        b.y = one_lambda * one_lambda * data->curr_dev_trans.y +
              2 * lambda * one_lambda * control_dev_trans.y +
              lambda * lambda * to_dev_trans.y;
        ArrayAppend(data->outline_x, b.x);
        ArrayAppend(data->outline_y, b.y);
        data->nper[data->npoly - 1]++;
    }

    /* Update current position */
    data->curr_dev_trans = to_dev_trans;
    
    return 0;
}

int outlineCubicTo(const FT_Vector* control1, const FT_Vector* control2,
                   const FT_Vector* to, void* user)
{
    /* Rprintf("cubic\n") */
    OutlineData *data = (OutlineData *) user;
    double ratio_EM = data->ratio_EM;
    double lambda = 0.0, one_lambda = 1.0;
    double delta = 1.0 / data->nseg;

    Point to_dev, to_dev_trans;
    Point control1_dev, control1_dev_trans;
    Point control2_dev, control2_dev_trans;
    Point b;

    /* Calculate transformed coordinates */
    to_dev.x = to->x * ratio_EM + data->deltax;
    to_dev.y = data->trans.sign * to->y * ratio_EM;
    transformPoint(&to_dev, &to_dev_trans, &(data->trans));
    
    control1_dev.x = control1->x * ratio_EM + data->deltax;
    control1_dev.y = data->trans.sign * control1->y * ratio_EM;
    transformPoint(&control1_dev, &control1_dev_trans, &(data->trans));
    
    control2_dev.x = control2->x * ratio_EM + data->deltax;
    control2_dev.y = data->trans.sign * control2->y * ratio_EM;
    transformPoint(&control2_dev, &control2_dev_trans, &(data->trans));
    
    /* Calculate curve coordinates */
    for (lambda = 0.0; lambda < 1; lambda += delta, one_lambda -= delta)
    {
        b.x = one_lambda * one_lambda * one_lambda * data->curr_dev_trans.x +
              3 * lambda * one_lambda * one_lambda * control1_dev_trans.x +
              3 * lambda * lambda * one_lambda * control2_dev_trans.x +
              lambda * lambda * lambda * to_dev_trans.x;
        b.y = one_lambda * one_lambda * one_lambda * data->curr_dev_trans.y +
              3 * lambda * one_lambda * one_lambda * control1_dev_trans.y +
              3 * lambda * lambda * one_lambda * control2_dev_trans.y +
              lambda * lambda * lambda * to_dev_trans.y;
        ArrayAppend(data->outline_x, b.x);
        ArrayAppend(data->outline_y, b.y);
        data->nper[data->npoly - 1]++;
    }

    /* Update current position */
    data->curr_dev_trans = to_dev_trans;
    
    return 0;
}


// Errors that may occur in loading font characters.
// Here we just give warnings.
void errorcode(FT_Error err)
{
    switch(err)
    {
        case 0x10:
            Rf_warning("freetype: invalid glyph index");
            break;
        case 0x11:
            Rf_warning("freetype: invalid character code");
            break;
        case 0x12:
            Rf_warning("freetype: unsupported glyph image format");
            break;
        case 0x13:
            Rf_warning("freetype: cannot render this glyph format");
            break;
        case 0x14:
            Rf_warning("freetype: invalid outline");
            break;
        case 0x15:
            Rf_warning("freetype: invalid composite glyph");
            break;
        case 0x16:
            Rf_warning("freetype: too many hints");
            break;
        case 0x17:
            Rf_warning("freetype: invalid pixel size");
            break;
        default:
            Rf_warning("freetype: error code %d", err);
            break;
    }
}

SEXP showtextLoadOutlineFuns()
{
    FT_Outline_Funcs *funs = (FT_Outline_Funcs *) calloc(1, sizeof(FT_Outline_Funcs));
    SEXP extPtr;

    funs->move_to = outlineMoveTo;
    funs->line_to = outlineLineTo;
    funs->conic_to = outlineConicTo;
    funs->cubic_to = outlineCubicTo;
    funs->shift = 0;
    funs->delta = 0;
    
    extPtr = R_MakeExternalPtr(funs, R_NilValue, R_NilValue);
    return extPtr;
}

SEXP showtextCleanOutlineFuns(SEXP extPtr)
{
    FT_Outline_Funcs *funs = (FT_Outline_Funcs *) R_ExternalPtrAddr(extPtr);
    if(funs) free(funs);
    
    return R_NilValue;
}

