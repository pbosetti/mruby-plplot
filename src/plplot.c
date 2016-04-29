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
  PLFLT xmin, xmax, ymin, ymax;
  mrb_int nargs, scaling, axes;
  nargs = mrb_get_args(mrb, "ffff", &xmin, &xmax, &ymin, &ymax);
  
  scaling = mrb_fixnum(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@scaling")));
  axes = mrb_fixnum(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@box")));
  
  plcol0( 15 );
  plenv( xmin, xmax, ymin, ymax, scaling, axes );
  return self;
}

static mrb_value mrb_plplot_sdidev(mrb_state *mrb, mrb_value self) {
  PLFLT mar, aspect, jx, jy;
  mrb_get_args(mrb, "ffff", &mar, &aspect, &jx, &jy);
  if (jx < -0.5 || jx > 0.5) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "X relative justification must be in [-0.5, 0.5]");
  }
  if (jy < -0.5 || jy > 0.5) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "Y relative justification must be in [-0.5, 0.5]");
  }
  plsdidev(mar, aspect, jx, jy);
  return self;
}

static mrb_value mrb_plplot_gdidev(mrb_state *mrb, mrb_value self) {
  mrb_value ary;
  PLFLT mar, aspect, jx, jy;
  plgdidev(&mar, &aspect, &jx, &jy);
  ary = mrb_ary_new_capa(mrb, 4);
  mrb_ary_push(mrb, ary, mrb_float_value(mrb, mar));
  mrb_ary_push(mrb, ary, mrb_float_value(mrb, aspect));
  mrb_ary_push(mrb, ary, mrb_float_value(mrb, jx));
  mrb_ary_push(mrb, ary, mrb_float_value(mrb, jy));
  return ary;
}

static mrb_value mrb_plplot_spage(mrb_state *mrb, mrb_value self) {
  PLFLT xp, yp;
  PLINT xleng, yleng, xoff, yoff;
  mrb_int nargs;
  nargs = mrb_get_args(mrb, "ff|iiii", &xp, &yp, &xleng, &yleng, &xoff, &yoff);
  if (nargs == 6) {
    plspage(xp, yp, xleng, yleng, xoff, yoff);    
  }
  else if (nargs == 2) {
    plspage(0, 0, (PLINT)xp, (PLINT)yp, 0, 0);
  }
  else {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "Only 2 or 6 arguments");
  }
  return self;
}

static mrb_value mrb_plplot_gpage(mrb_state *mrb, mrb_value self) {
  PLFLT xp, yp;
  PLINT xleng, yleng, xoff, yoff;
  mrb_value ary;
  plgpage(&xp, &yp, &xleng, &yleng, &xoff, &yoff);
  ary = mrb_ary_new_capa(mrb, 6);
  mrb_ary_push(mrb, ary, mrb_float_value(mrb, xp));
  mrb_ary_push(mrb, ary, mrb_float_value(mrb, yp));
  mrb_ary_push(mrb, ary, mrb_fixnum_value(xleng));
  mrb_ary_push(mrb, ary, mrb_fixnum_value(yleng));
  mrb_ary_push(mrb, ary, mrb_fixnum_value(xoff));
  mrb_ary_push(mrb, ary, mrb_fixnum_value(yoff));
  return ary;
}

static mrb_value mrb_plplot_schr(mrb_state *mrb, mrb_value self) {
  PLFLT scale;
  mrb_int nargs;
  nargs = mrb_get_args(mrb, "f", &scale);
  plschr(0, scale);
  return self;
}

static mrb_value mrb_plplot_gchr(mrb_state *mrb, mrb_value self) {
  PLFLT def, scale;
  plgchr(&def, &scale);
  return mrb_float_value(mrb, scale);
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

static mrb_value mrb_plplot_legend(mrb_state *mrb, mrb_value self) {
  mrb_value position, *names,  *options;
  mrb_int              names_l, options_l;
  mrb_value *line_c,  *line_w,  *line_s;
  mrb_int    line_c_l, line_w_l, line_s_l;
  mrb_value *point_c,  *point_g,  *point_s;
  mrb_int    point_c_l, point_g_l, point_s_l;
  
  mrb_get_args(mrb, "oaaaaaaaa", 
    &position, &names, &names_l, &options, &options_l,
    &line_c, &line_c_l, &line_w, &line_w_l, &line_s, &line_s_l,
    &point_c, &point_c_l, &point_g, &point_g_l, &point_s, &point_s_l
  );
  return mrb_nil_value();
}



/*
  SERIES CLASS
*/
static mrb_value mrb_series_line(mrb_state *mrb, mrb_value self) {
  mrb_int len, i, style, col;
  mrb_float width;
  mrb_value x, y;
  PLFLT *px, *py;
  mrb_get_args(mrb, "ifi", &col, &width, &style);
  pllsty(style);
  plwidth(width);
  plcol0(col);
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
  mrb_int len, i, nargs, glyph, col;
  mrb_value x, y;
  PLFLT *px, *py;
  nargs = mrb_get_args(mrb, "|ii", &col, &glyph);
  plcol0(col);
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
  mrb_define_class_method(mrb, plot, "set_shape", mrb_plplot_sdidev, MRB_ARGS_REQ(4));
  mrb_define_class_method(mrb, plot, "shape", mrb_plplot_gdidev, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, plot, "set_page", mrb_plplot_spage, MRB_ARGS_ARG(2,4));
  mrb_define_class_method(mrb, plot, "page", mrb_plplot_gpage, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, plot, "chr_scale=", mrb_plplot_schr, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, plot, "chr_scale", mrb_plplot_gchr, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, plot, "end", mrb_plplot_end, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, plot, "version", mrb_plplot_ver, MRB_ARGS_NONE());
  // arguments: position, names, options, 3xlines, 3xpoints = 9
  mrb_define_class_method(mrb, plot, "_legend", mrb_plplot_legend, MRB_ARGS_REQ(9));
  
  series = mrb_define_class_under(mrb, plot, "Series", mrb->object_class);
  mrb_define_method(mrb, series, "_line", mrb_series_line, MRB_ARGS_REQ(3));  
  mrb_define_method(mrb, series, "_points", mrb_series_points, MRB_ARGS_REQ(2));
  
}

void mrb_mruby_plplot_gem_final(mrb_state *mrb) {}
