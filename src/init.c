#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

#include "renderer.h"

/* render_markdown.c */
extern SEXP C_render_markdown(SEXP, SEXP, SEXP, SEXP); 

/* markdown.c */
extern SEXP C_md_to_html(SEXP);

/* from testthat */
extern SEXP run_testthat_tests();

static const R_CallMethodDef CallEntries[] = {
  {"C_md_to_html", (DL_FUNC) &C_md_to_html, 1},
  {"C_render_markdown", (DL_FUNC) &C_render_markdown, 4},
  {"test_rdev_new_release", (DL_FUNC) &test_rdev_new_release, 1},
  {"gpar_empty", (DL_FUNC) &gpar_empty, 0},
  {"text_grob", (DL_FUNC) &text_grob, 6},
  {"rect_grob", (DL_FUNC) &rect_grob, 7},
  {"lines_grob", (DL_FUNC) &lines_grob, 3},
  {"unit_in", (DL_FUNC) &unit_in, 1},
  {"test_gpar_gcontext", (DL_FUNC) &test_gpar_gcontext, 0},
  {"run_testthat_tests", (DL_FUNC) &run_testthat_tests, 0},
  {NULL, NULL, 0}
};

void R_init_sinab(DllInfo *dll) {
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}
