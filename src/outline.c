#include "outline.h"

void transform_point(Point* before, Point* after, TransData* trans)
{
    /* degree to radian */
    double rad = trans->theta * DEG2RAD; /* from R_ext/GraphicsEngine.h */
    double cost = cos(rad);
    double sint = sin(rad);
    
    after->x = before->x * cost - trans->sign * before->y * sint + trans->x;
    after->y = trans->sign * before->x * sint + before->y * cost + trans->y;  
}

int outline_move_to(const FT_Vector* to, void* user)
{
    /* Rprintf("move\n"); */
    OutlineData* data = (OutlineData*) user;
    double ft_to_dev_ratio = data->ft_to_dev_ratio;
    
    Point to_dev, to_dev_trans;
    
    /* Calculate transformed coordinates */
    to_dev.x = to->x * ft_to_dev_ratio + data->offset_x;
    to_dev.y = data->trans.sign * to->y * ft_to_dev_ratio;
    transform_point(&to_dev, &to_dev_trans, &(data->trans));
    
    /* Add to the polygon data */
    Array_append(data->outline_x, to_dev_trans.x);
    Array_append(data->outline_y, to_dev_trans.y);
    
    /* "Move to" opens a new polygon */
    data->num_poly++;
    IntArray_append(data->points_in_poly, 1);
    
    /* Update current position */
    data->curr_pos = to_dev_trans;
    
    return 0;
}

int outline_line_to(const FT_Vector* to, void* user)
{
    /* Rprintf("line\n"); */
    OutlineData* data = (OutlineData*) user;
    double ft_to_dev_ratio = data->ft_to_dev_ratio;

    Point to_dev, to_dev_trans;
    
    /* Calculate transformed coordinates */
    to_dev.x = to->x * ft_to_dev_ratio + data->offset_x;
    to_dev.y = data->trans.sign * to->y * ft_to_dev_ratio;
    transform_point(&to_dev, &to_dev_trans, &(data->trans));
    
    /* Add to the polygon data */
    Array_append(data->outline_x, to_dev_trans.x);
    Array_append(data->outline_y, to_dev_trans.y);
    
    /* "Line to" adds a new point to the current polygon */
    if(data->points_in_poly->len > 0)
        (*(data->points_in_poly->data + data->points_in_poly->len - 1))++;
    
    /* Update current position */
    data->curr_pos = to_dev_trans;
    
    return 0;
}

int outline_conic_to(const FT_Vector* control, const FT_Vector* to, void* user)
{
    /* Rprintf("conic\n"); */
    OutlineData* data = (OutlineData*) user;
    double ft_to_dev_ratio = data->ft_to_dev_ratio;
    double lambda = 0.0, one_lambda = 1.0;
    double delta = 1.0 / data->num_segments;

    Point to_dev, to_dev_trans;
    Point control_dev, control_dev_trans;
    Point b;

    /* Calculate transformed coordinates */
    to_dev.x = to->x * ft_to_dev_ratio + data->offset_x;
    to_dev.y = data->trans.sign * to->y * ft_to_dev_ratio;
    transform_point(&to_dev, &to_dev_trans, &(data->trans));
    
    control_dev.x = control->x * ft_to_dev_ratio + data->offset_x;
    control_dev.y = data->trans.sign * control->y * ft_to_dev_ratio;
    transform_point(&control_dev, &control_dev_trans, &(data->trans));
    
    /* Calculate curve coordinates */
    for(lambda = 0.0; lambda < 1; lambda += delta, one_lambda -= delta)
    {
        b.x = one_lambda * one_lambda * data->curr_pos.x +
              2 * lambda * one_lambda * control_dev_trans.x +
              lambda * lambda * to_dev_trans.x;
        b.y = one_lambda * one_lambda * data->curr_pos.y +
              2 * lambda * one_lambda * control_dev_trans.y +
              lambda * lambda * to_dev_trans.y;
        Array_append(data->outline_x, b.x);
        Array_append(data->outline_y, b.y);
        if(data->points_in_poly->len > 0)
            (*(data->points_in_poly->data + data->points_in_poly->len - 1))++;
    }

    /* Update current position */
    data->curr_pos = to_dev_trans;
    
    return 0;
}

int outline_cubic_to(const FT_Vector* control1, const FT_Vector* control2,
                     const FT_Vector* to, void* user)
{
    /* Rprintf("cubic\n") */
    OutlineData* data = (OutlineData*) user;
    double ft_to_dev_ratio = data->ft_to_dev_ratio;
    double lambda = 0.0, one_lambda = 1.0;
    double delta = 1.0 / data->num_segments;

    Point to_dev, to_dev_trans;
    Point control1_dev, control1_dev_trans;
    Point control2_dev, control2_dev_trans;
    Point b;

    /* Calculate transformed coordinates */
    to_dev.x = to->x * ft_to_dev_ratio + data->offset_x;
    to_dev.y = data->trans.sign * to->y * ft_to_dev_ratio;
    transform_point(&to_dev, &to_dev_trans, &(data->trans));
    
    control1_dev.x = control1->x * ft_to_dev_ratio + data->offset_x;
    control1_dev.y = data->trans.sign * control1->y * ft_to_dev_ratio;
    transform_point(&control1_dev, &control1_dev_trans, &(data->trans));
    
    control2_dev.x = control2->x * ft_to_dev_ratio + data->offset_x;
    control2_dev.y = data->trans.sign * control2->y * ft_to_dev_ratio;
    transform_point(&control2_dev, &control2_dev_trans, &(data->trans));
    
    /* Calculate curve coordinates */
    for (lambda = 0.0; lambda < 1; lambda += delta, one_lambda -= delta)
    {
        b.x = one_lambda * one_lambda * one_lambda * data->curr_pos.x +
              3 * lambda * one_lambda * one_lambda * control1_dev_trans.x +
              3 * lambda * lambda * one_lambda * control2_dev_trans.x +
              lambda * lambda * lambda * to_dev_trans.x;
        b.y = one_lambda * one_lambda * one_lambda * data->curr_pos.y +
              3 * lambda * one_lambda * one_lambda * control1_dev_trans.y +
              3 * lambda * lambda * one_lambda * control2_dev_trans.y +
              lambda * lambda * lambda * to_dev_trans.y;
        Array_append(data->outline_x, b.x);
        Array_append(data->outline_y, b.y);
        if(data->points_in_poly->len > 0)
            (*(data->points_in_poly->data + data->points_in_poly->len - 1))++;
    }

    /* Update current position */
    data->curr_pos = to_dev_trans;
    
    return 0;
}



/* Finalizer of the external pointer */
static void outline_funs_finalizer(SEXP ext_ptr)
{
    FT_Outline_Funcs* funs = (FT_Outline_Funcs*) R_ExternalPtrAddr(ext_ptr);
    if(funs) free(funs);
}

SEXP showtext_new_outline_funs(void)
{
    FT_Outline_Funcs* funs = (FT_Outline_Funcs*) calloc(1, sizeof(FT_Outline_Funcs));
    SEXP ext_ptr;

    funs->move_to  = outline_move_to;
    funs->line_to  = outline_line_to;
    funs->conic_to = outline_conic_to;
    funs->cubic_to = outline_cubic_to;
    funs->shift = 0;
    funs->delta = 0;
    
    ext_ptr = PROTECT(R_MakeExternalPtr(funs, R_NilValue, R_NilValue));
    R_RegisterCFinalizerEx(ext_ptr, outline_funs_finalizer, TRUE);
    UNPROTECT(1);
    
    return ext_ptr;
}
