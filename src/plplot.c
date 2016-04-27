#include <math.h>
#include <string.h>
#include <ctype.h>

#include <plConfig.h>
#include <plplot.h>

#include "mruby.h"
#include "mruby/variable.h"
#include "mruby/string.h"
#include "mruby/data.h"
#include "mruby/class.h"
#include "mruby/value.h"
#include "mruby/array.h"
#include "mruby/hash.h"
#include "mruby/numeric.h"
#include "mruby/compile.h"


static mrb_value mrb_plplot_init(mrb_state *mrb, mrb_value self) {
  plscol0( 0, 255, 255, 255 );  /* White, color 0, background */
  plscol0( 15, 0, 0, 0 );  /* Black, color 15 */
  plinit();
  return mrb_nil_value();
}

static mrb_value mrb_plplot_star(mrb_state *mrb, mrb_value self) {
  mrb_int nx, ny;
  mrb_value grid;
  grid = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@grid"));
  nx = mrb_fixnum(mrb_ary_entry(grid, 0));
  ny = mrb_fixnum(mrb_ary_entry(grid, 1));
  plscol0( 0, 255, 255, 255 );  /* White, color 0, background */
  plscol0( 15, 0, 0, 0 );  /* Black, color 15 */
  plstar(nx, ny);
  return mrb_nil_value();
}

static mrb_value mrb_plplot_sdev(mrb_state *mrb, mrb_value self) {
  mrb_int nargs;
  mrb_value dev;
  nargs = mrb_get_args(mrb, "|S", &dev);
  if (nargs == 1) {
    plsdev(mrb_string_value_cstr(mrb, &dev));
  }
  else {
    plsdev("pdfcairo");
  }
  return self;
}


static mrb_value mrb_plplot_sfnam(mrb_state *mrb, mrb_value self) {
  mrb_int nargs;
  mrb_value filename;
  nargs = mrb_get_args(mrb, "|S", &filename);
  if (nargs == 1) {
    plsfnam(mrb_string_value_cstr(mrb, &filename));
  }
  else {
    plsfnam("plot.pdf");
  }
  return self;
}

static mrb_value mrb_plplot_adv(mrb_state *mrb, mrb_value self) {
  mrb_int nargs, page;
  nargs = mrb_get_args(mrb, "|i", &page);
  if (nargs == 1) {
    pladv(page);
  }
  else {
    pladv(0);
  }
  return self;
}

static mrb_value mrb_plplot_env(mrb_state *mrb, mrb_value self) {
  mrb_float xmin, xmax, ymin, ymax;
  mrb_int nargs, scaling, axes;
  nargs = mrb_get_args(mrb, "ffff", &xmin, &xmax, &ymin, &ymax);
  
  scaling = mrb_fixnum(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@scaling")));
  axes = mrb_fixnum(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@box")));
  
  plcol0( 15 );
  plenv( xmin, xmax, ymin, ymax, scaling, axes );
  return self;
}

static mrb_value mrb_plplot_lab(mrb_state *mrb, mrb_value self) {
  mrb_int nargs, xlab_l, ylab_l, title_l;
  char *xlab, *ylab, *title;
  nargs = mrb_get_args(mrb, "ss|s", &xlab, &xlab_l, &ylab, &ylab_l, &title, &title_l);
  if (nargs == 2) {
    pllab(xlab, ylab, "");
  }
  else {
    pllab(xlab, ylab, title);
  }
  return self;
}

static mrb_value mrb_plplot_end(mrb_state *mrb, mrb_value self) {
  plend();
  return mrb_nil_value();
}

static mrb_value mrb_plplot_ver(mrb_state *mrb, mrb_value self) {
  char  ver[80];
  plgver(ver);
  return mrb_str_new_cstr(mrb, ver);
}


/*
  SERIES CLASS
*/
static mrb_value mrb_series_line(mrb_state *mrb, mrb_value self) {
  mrb_int len, i, nargs, style;
  mrb_float width;
  mrb_value col, x, y;
  PLFLT *px, *py;
  nargs = mrb_get_args(mrb, "|ofi", &col, &width, &style);
  if (nargs == 3) {
    pllsty(style);
  }
  if (nargs >= 2) {
    plwidth(width);
  }
  if (nargs >= 1) {
    if (mrb_fixnum_p(col)) {
      plcol0(mrb_fixnum(col));
    }
    else if (mrb_symbol_p(col)) {
      mrb_value mod = mrb_obj_value(mrb_module_get(mrb, "PLPlot"));
      mrb_value colors = mrb_const_get(mrb, mod, mrb_intern_lit(mrb, "COLORS"));
      mrb_value c = mrb_hash_get(mrb, colors, col);
      plcol0(mrb_fixnum(c));
    }
    else {
      plcol0(15);
    }
  }
  else {
    plcol0(15);
  }
  len = mrb_fixnum(mrb_funcall(mrb, self, "length", 0));
  x = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@x"));
  y = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@y"));
  px = (PLFLT*)calloc(len, sizeof(PLFLT));
  py = (PLFLT*)calloc(len, sizeof(PLFLT));
  
  for (i = 0; i < len; i++) {
    px[i] = mrb_to_flo(mrb, mrb_ary_entry(x, i));
    py[i] = mrb_to_flo(mrb, mrb_ary_entry(y, i));
  }
  plline(len, px, py);
  plwidth(0);
  pllsty(1);
  free(px);
  free(py);
  return mrb_fixnum_value(len);
}


static mrb_value mrb_series_points(mrb_state *mrb, mrb_value self) {
  mrb_int len, i, nargs, glyph;
  mrb_value col, x, y;
  PLFLT *px, *py;
  nargs = mrb_get_args(mrb, "|oi", &col, &glyph);
  if (nargs < 2) {
    glyph = 0;
  }
  if (nargs >= 1) {
    if (mrb_fixnum_p(col)) {
      plcol0(mrb_fixnum(col));
    }
    else if (mrb_symbol_p(col)) {
      mrb_value mod = mrb_obj_value(mrb_module_get(mrb, "PLPlot"));
      mrb_value colors = mrb_const_get(mrb, mod, mrb_intern_lit(mrb, "COLORS"));
      mrb_value c = mrb_hash_get(mrb, colors, col);
      plcol0(mrb_fixnum(c));
    }
    else {
      plcol0(15);
    }
  }
  else {
    plcol0(15);
  }
  len = mrb_fixnum(mrb_funcall(mrb, self, "length", 0));
  x = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@x"));
  y = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@y"));
  px = (PLFLT*)calloc(len, sizeof(PLFLT));
  py = (PLFLT*)calloc(len, sizeof(PLFLT));
  
  for (i = 0; i < len; i++) {
    px[i] = mrb_to_flo(mrb, mrb_ary_entry(x, i));
    py[i] = mrb_to_flo(mrb, mrb_ary_entry(y, i));
  }
  plpoin(len, px, py, glyph);
  free(px);
  free(py);
  return mrb_fixnum_value(len);
}




void mrb_mruby_plplot_gem_init(mrb_state *mrb) {
  struct RClass *plot, *series;
  plot = mrb_define_module(mrb, "PLPlot");

  mrb_define_class_method(mrb, plot, "init", mrb_plplot_init, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, plot, "star", mrb_plplot_star, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, plot, "sfnam", mrb_plplot_sfnam, MRB_ARGS_OPT(1));
  mrb_define_class_method(mrb, plot, "sdev", mrb_plplot_sdev, MRB_ARGS_OPT(1));
  mrb_define_class_method(mrb, plot, "adv", mrb_plplot_adv, MRB_ARGS_OPT(1));
  mrb_define_class_method(mrb, plot, "env", mrb_plplot_env, MRB_ARGS_ARG(4,2));
  mrb_define_class_method(mrb, plot, "lab", mrb_plplot_lab, MRB_ARGS_ARG(2,1));
  mrb_define_class_method(mrb, plot, "end", mrb_plplot_end, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, plot, "version", mrb_plplot_ver, MRB_ARGS_NONE());
  
  series = mrb_define_class_under(mrb, plot, "Series", mrb->object_class);
  mrb_define_method(mrb, series, "line", mrb_series_line, MRB_ARGS_OPT(3));  
  mrb_define_method(mrb, series, "points", mrb_series_points, MRB_ARGS_OPT(2));
  
}

void mrb_mruby_plplot_gem_final(mrb_state *mrb) {}
