/**
 * @file
 * Implementation of the custom stock charting widget
 */

/*
The MIT License (MIT)

Copyright (c) 2015 David Bradshaw

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <gtk/gtk.h>
#include <pango/pangocairo.h>
#include "rect.h"
#include "stockchart.h"

G_DEFINE_TYPE (GtkStockChart, stock_chart, GTK_TYPE_DRAWING_AREA);

static GtkDrawingAreaClass* parent_class=nullptr;

/**
 * Gets the maximum value in an unsorted array
 */
static double sd_max(double* data,int sz)
{
    if (sz<1) return 0.0;

    double* best=data;
    for (double* i=data+1; i< data+sz; i++)
    {
	if ( *i > *best )
	    best = i;
    }

    return *best;
}

/**
 * Gets the minimum value in an unsorted array
 */
static double sd_min(double* data,int sz)
{
    if (sz<1) return 0.0;

    double* best=data;
    for (double* i=data+1; i< data+sz; i++)
    {
	if ( *i < *best )
	    best = i;
    }

    return *best;
}

static void draw_background(cairo_t* cr, rect r)
{
    cairo_pattern_t* pat;
    cairo_save(cr);
    pat=cairo_pattern_create_linear(r.width/2-5,5, r.width/2-5, r.height-5);
    cairo_pattern_add_color_stop_rgba(pat, 0.0, 0.4, 0.4, 0.4, 1.0);
    cairo_pattern_add_color_stop_rgba(pat, 1.0, 0.0, 0.0, 0.0, 1.0);
    cairo_set_source(cr, pat);
    cairo_rectangle(cr, r.left(),r.top(), r.width, r.height);
    cairo_fill(cr);
    cairo_restore(cr);
}

static void draw_grid(cairo_t* cr,const rect& area, guint divs_x, guint divs_y)
{
    cairo_set_line_width(cr,1.0);
    cairo_set_source_rgb(cr, 1, 1, 1);

    double x_size = area.width / ((double)divs_x);
    double y_size = area.height / ((double)divs_y);

    for ( double x = area.left(); x < (area.right() + 0.5*x_size ); x+= x_size )
    {
	cairo_move_to(cr,x,area.bottom());
	cairo_line_to(cr,x,area.top());
    }

    for ( double y = area.top(); y < ( area.bottom() + 0.5*y_size ); y+= y_size )
    {
	cairo_move_to(cr,area.left(),y);
	cairo_line_to(cr,area.right(),y);
    }

    cairo_stroke(cr);
}

static PangoLayout* render_title(cairo_t* cr, const gchar* title)
{

    PangoLayout* layout = pango_cairo_create_layout(cr);

    pango_layout_set_text(layout,title,-1);
    PangoFontDescription* desc = pango_font_description_from_string("Sans Bold 11");
    pango_layout_set_font_description(layout,desc);
    pango_font_description_free(desc);

    return layout;
}

static void draw_title(cairo_t* cr, const rect& area, PangoLayout* layout)
{

    cairo_save(cr);

    cairo_set_source_rgb(cr,1,0,0);
    cairo_move_to(cr,area.left(),area.top());
    pango_cairo_show_layout(cr,layout);

    cairo_restore(cr);
}

static gboolean stock_chart_draw(GtkWidget* w, cairo_t* cr)
{
    GtkStockChart* sc = GTK_STOCKCHART(w);

    guint width = gtk_widget_get_allocated_width(w);
    guint height = gtk_widget_get_allocated_height(w);

    rect area(0.0,0.0,width,height);

    double x, y;
    x = width / 2;
    y = height / 2;

    draw_background(cr,area);

    PangoLayout* layout = render_title(cr,sc->title);
    PangoRectangle ink,logical;
    pango_layout_get_pixel_extents(layout,&ink,&logical);

    rect titlearea( (width-ink.width)/2,
		   4.0,
		   ink.width,
		   4.0+ink.height);

    rect gridarea = area;
    gridarea.inset(10.0).avoid(titlearea, 5.0);
    draw_grid(cr,gridarea,10,10);

    draw_title(cr,titlearea,layout);

    g_object_unref(layout);

    return FALSE;
}

static void stock_chart_finalize(GObject* obj)
{
    GtkStockChart* sc = GTK_STOCKCHART(obj);
    if (sc->title)
    {
	g_free(sc->title);
	sc->title = nullptr;
    }

    if ( G_OBJECT_CLASS(parent_class)->finalize )
	G_OBJECT_CLASS(parent_class)->finalize(obj);
}

static void stock_chart_class_init(GtkStockChartClass* c)
{
    parent_class = (GtkDrawingAreaClass*)(g_type_class_peek_parent(c));
    GtkWidgetClass* wc = GTK_WIDGET_CLASS(c);
    GObjectClass* oc = G_OBJECT_CLASS(c);
    wc->draw = stock_chart_draw;
    oc->finalize = stock_chart_finalize;
}

static void stock_chart_init(GtkStockChart* obj)
{
    obj->title = g_strdup("[New Stock Chart]");
}

GtkWidget* stock_chart_new()
{
    return GTK_WIDGET( g_object_new( GTK_TYPE_STOCKCHART, NULL) ); 
}

void stock_chart_set_title(GtkStockChart* sc, const gchar* title)
{
    if (sc->title)
	g_free(sc->title);

    sc->title = g_strdup(title);
}

const gchar* stock_chart_get_title(GtkStockChart* sc)
{
    return sc->title;
}
