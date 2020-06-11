#include "grid_renderer.h"

/* Create a grid renderer object. Must be deleted with gr_release().*/
GR_Object* gr_create() {
  GR_Object* gro = (GR_Object*) Calloc(1, GR_Object);
  gro->capacity = 5;
  
  gro->grobs = PROTECT(allocVector(VECSXP, gro->capacity));
  R_PreserveObject(gro->grobs);
  UNPROTECT(1);
  gro->size = 0;
  
  return gro;
}

/* Consume the grid renderer object and return a vector of grobs.*/
SEXP gr_release(GR_Object* gro) {
  /* In general, the capacity is larger than the current size
   * of the list, so we copy the relevant part into a new list.
   */
  SEXP grobs_old, grobs_new, cl;
  grobs_old = gro->grobs;
  PROTECT(grobs_new = allocVector(VECSXP, gro->size));
  
  for (R_xlen_t i = 0; i < gro->size; i++) {
    SET_VECTOR_ELT(grobs_new, i, VECTOR_ELT(grobs_old, i));
  }
  
  R_ReleaseObject(grobs_old);
  Free(gro);
  
  /* set class to "gList" */
  PROTECT(cl = mkString("gList"));
  classgets(grobs_new, cl);
  UNPROTECT(2);

  return grobs_new;
}

/* Grow the capacity of the list of grobs. Internal. */
void gr_grow_capacity(GR_Object* gro) {
  /* Whenever we're running out of space, we grow the
   * capacity by doubling the grobs vector size
   */
  SEXP grobs_old, grobs_new;
  R_xlen_t cap_new = 2*gro->capacity;
  grobs_old = gro->grobs;
  PROTECT(grobs_new = allocVector(VECSXP, cap_new));
  
  /* We only need to copy to size, not to capacity */
  for (R_xlen_t i = 0; i < gro->size; i++) {
    SET_VECTOR_ELT(grobs_new, i, VECTOR_ELT(grobs_old, i));
  }
  
  R_ReleaseObject(grobs_old);
  R_PreserveObject(grobs_new);
  gro->grobs = grobs_new;
  gro->capacity = cap_new;
  UNPROTECT(1);
}

/* Add a SEXP to the list of grobs. Internal. */
void gr_add_SEXP(GR_Object* gro, SEXP s) {
  if (gro->size == gro->capacity) {
    gr_grow_capacity(gro);
    /*
    warning("Doubling list capacity. New capacity: %i", gro->capacity);
    */
  }
  
  SET_VECTOR_ELT(gro->grobs, gro->size, s);
  gro->size += 1;
}

void gr_draw_text(GR_Object* gro, const char* label, double x, double y, const GR_GContext *gc) {
  SEXP slabel, sx, sy, sxu, syu, hjust, vjust, gp, grob;
  
  PROTECT(slabel = mkString(label));
  PROTECT(sx = ScalarReal(x));
  PROTECT(sxu = unit_in(sx));
  PROTECT(sy = ScalarReal(y));
  PROTECT(syu = unit_in(sy));
  PROTECT(hjust = ScalarReal(0));
  PROTECT(vjust = ScalarReal(0));
  PROTECT(gp = gpar_gcontext(gc));
  
  PROTECT(grob = text_grob(slabel, sxu, syu, hjust, vjust, gp));
  
  gr_add_SEXP(gro, grob);
  
  UNPROTECT(9);
}

SEXP gr_string_metrics() {
  const char* label = "Hello!";
  
  pGEDevDesc dev = GEcurrentDevice();
  R_GE_gcontext gc = {
    .fontfamily = "Helvetica",
    .fontface = 0,
    .ps = 12,
    .cex = 1
  };
  double width = 0, ascent = 0, descent = 0;
    
  GEStrMetric(label, CE_UTF8, &gc,
              &ascent, &descent, &width, dev);
  
  GEUnit u = GE_INCHES;
  double w, a, d; 
  w = GEfromDeviceWidth(width, u, dev);
  a = GEfromDeviceWidth(ascent, u, dev);
  d = GEfromDeviceWidth(descent, u, dev);
  //Rprintf("width: %g, ascent: %g, descent: %g\n", w, a, d);
    
  SEXP out = ScalarReal(w);
  return out;
}

/* write graphics context defaults into gc object */
void gr_gcontext_defaults(GR_Object* gro, GR_GContext* gc) {
  strcpy(gc->color, "black");
  strcpy(gc->fill, "black");
  strcpy(gc->fontfamily, "");
  gc->fontface = 1;
  gc->fontsize = 12;
  gc->lineheight = 1.2;
}

/* Test routines */

SEXP test_gr_create_release(SEXP n_) {
  int n = asInteger(n_);
  
  GR_Object* gro = gr_create();
  
  for (int i = 0; i<n; i++) {
    SEXP s;
    PROTECT(s = mkString("test"));
    gr_add_SEXP(gro, s);
    UNPROTECT(1);
  }
    
  return gr_release(gro);
}

SEXP test_gr_draw_text() {
  GR_Object* gro = gr_create();
  
  GR_GContext gc;
  gr_gcontext_defaults(gro, &gc); /* initialize gc object */
  gr_draw_text(gro, "Hello", .5, 2, &gc);
  
  strcpy(gc.color, "blue");
  gr_draw_text(gro, "World", .95, 2, &gc);
  
  return gr_release(gro);
}
