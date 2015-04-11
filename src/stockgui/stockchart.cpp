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

#include <string.h>
#include <sstream>
#include <gtk/gtk.h>
#include <pango/pangocairo.h>
#include "rect.h"
#include "stockchart.h"

using std::stringstream;

G_DEFINE_TYPE (GtkStockChart, stock_chart, GTK_TYPE_DRAWING_AREA);

#define GTK_STOCKCHART_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE( (obj), GTK_TYPE_STOCKCHART , GtkStockChartPrivate ))

static GtkDrawingAreaClass* parent_class=nullptr;

struct _GtkStockChartPrivate
{
    double* data;
    size_t data_size;
    double upper;
    double lower;
    guint xdivs;
};

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

static PangoLayout* render_text(cairo_t* cr, const gchar* str, PangoFontDescription* font)
{
    PangoLayout* layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout,str,-1);
    pango_layout_set_font_description(layout,font);

    return layout;
}

static PangoLayout* render_text(cairo_t* cr, const gchar* str, const gchar* font)
{
    PangoFontDescription* desc = pango_font_description_from_string(font);
    PangoLayout* layout = render_text(cr,str,desc);
    pango_font_description_free(desc);
    
    return layout;

}

static void draw_background(cairo_t* cr, rect r)
{
    cairo_pattern_t* pat;
    cairo_save(cr);
    pat=cairo_pattern_create_linear(r.width/2-5,5, r.width/2-5, r.height-5);
    cairo_pattern_add_color_stop_rgba(pat, 0.0, 0.3, 0.3, 0.3, 1.0);
    cairo_pattern_add_color_stop_rgba(pat, 1.0, 0.0, 0.0, 0.0, 1.0);
    cairo_set_source(cr, pat);
    cairo_rectangle(cr, r.left(),r.top(), r.width, r.height);
    cairo_fill(cr);
    cairo_restore(cr);
}

static void draw_text(cairo_t* cr, const GdkRGBA& color, const rect& area, PangoLayout* layout)
{

    cairo_save(cr);

    cairo_set_source_rgb(cr,color.red,color.green,color.blue);
    cairo_move_to(cr,area.left(),area.top());
    pango_cairo_show_layout(cr,layout);

    cairo_restore(cr);
}

static void draw_data_series(GtkStockChart* sc, cairo_t* cr, const lrect& area, double* data, size_t sz)
{
    point lp(0,0),cp(0,0);
    double lx = sc->priv->lower;
    double ldx = (sc->priv->upper - sc->priv->lower) / ( sc->priv->data_size - 1);

    lp.x = lx;
    lp.y = sc->priv->data[0];
    cp = area.logical_to_context(lp);
    cairo_move_to(cr,cp.x,cp.y);

    int index=1;
    cairo_set_line_width(cr,2.0);
    for ( lx = sc->priv->lower + ldx; lx < (sc->priv->upper + 0.5*ldx); lx += ldx, index++ )
    {
	lp.x = lx;
	lp.y = sc->priv->data[index];
	cp = area.logical_to_context(lp);
	cairo_line_to(cr,cp.x,cp.y);
    }

    cairo_stroke(cr);
    
}

static rect draw_grid(GtkStockChart* sc, cairo_t* cr, const GdkRGBA& color, rect area, 
		      guint divs_x, guint divs_y)
{
    /* Create a font for the axis labels */
    GdkRGBA labelColor;
    labelColor.red=1.0;
    labelColor.blue=1.0;
    labelColor.green=1.0;
    PangoFontDescription* labelfont = pango_font_description_from_string("Sans 8");
    
    /* Get label height and width */
    PangoLayout* test_layout = render_text(cr,"XXX",labelfont);
    PangoRectangle ink,logical;
    pango_layout_get_pixel_extents(test_layout,&ink,&logical);

    g_object_unref(test_layout);

    static const double X_LABEL_HEIGHT=logical.height+4;
    static const double Y_LABEL_WIDTH=logical.width+4;

    /* Shrink the chart area to acommodate axis labels */
    area.setbottom(area.bottom()-X_LABEL_HEIGHT);
    area.setleft( area.left()+Y_LABEL_WIDTH);

    /* Create X and Y axis label areas */
    rect xaxislabel(area.left(),area.bottom(),area.width,X_LABEL_HEIGHT);
    rect yaxislabel(area.left()-Y_LABEL_WIDTH,area.top(),Y_LABEL_WIDTH,area.height);

    /* Now on to the actual grid ... */
    cairo_set_line_width(cr,1.0);
    cairo_set_source_rgb(cr, color.red,color.green,color.blue);

    double x_size = area.width / ((double)divs_x);
    double y_size = area.height / ((double)divs_y);

    double xval = sc->priv->lower;
    double delta_xval = (sc->priv->upper - sc->priv->lower)/divs_x;

    double yval = sd_max( sc->priv->data, sc->priv->data_size);
    double delta_yval = ( yval - sd_min( sc->priv->data, sc->priv->data_size) )/divs_y; 

    for ( double x = area.left(); x < (area.right() + 0.5*x_size ); x+= x_size )
    {
	/* Draw grid line */
	cairo_move_to(cr,x,area.bottom());
	cairo_line_to(cr,x,area.top());

	/* Render x axis label */
	stringstream s;
	s.precision(2);
	s << xval;
	PangoLayout* label = render_text(cr,s.str().c_str(),labelfont);
	pango_layout_get_pixel_extents(label,&ink,&logical);
	rect labelArea(x-ink.width/2.0,xaxislabel.top()+2.0,1,1);
	draw_text(cr,labelColor,labelArea,label);
	g_object_unref(label);
	xval += delta_xval;
    }

    for ( double y = area.top(); y < ( area.bottom() + 0.5*y_size ); y+= y_size )
    {
	/* Draw grid line */
	cairo_move_to(cr,area.left(),y);
	cairo_line_to(cr,area.right(),y);

	/* Render y axis label */
	stringstream s;
	s.precision(2);
	s << yval;
	PangoLayout* label = render_text(cr,s.str().c_str(),labelfont);
	pango_layout_get_pixel_extents(label,&ink,&logical);
	rect labelArea(yaxislabel.left(),y-ink.height/2.0,1.0,1.0);
	draw_text(cr,labelColor,labelArea,label);
	g_object_unref(label);
	yval -= delta_yval;
    }

    cairo_stroke(cr);

    /* Free the font description */
    pango_font_description_free(labelfont);

    return area;
}

static gboolean stock_chart_draw(GtkWidget* w, cairo_t* cr)
{
    GtkStockChart* sc = GTK_STOCKCHART(w);

    guint width = gtk_widget_get_allocated_width(w);
    guint height = gtk_widget_get_allocated_height(w);

    /* Draw the background */
    rect area(0.0,0.0,width,height);
    draw_background(cr,area);

    /* Layout title and grid */
    PangoLayout* layout = render_text(cr,sc->title,"Sans Bold 12");
    PangoRectangle ink,logical;
    pango_layout_get_pixel_extents(layout,&ink,&logical);

    rect titlearea( (width-ink.width)/2,
		   4.0,
		   ink.width,
		   4.0+ink.height);

    rect gridarea = area;
    gridarea.inset(10.0).avoid(titlearea, 5.0);

    /* Draw the grid */
    gridarea = draw_grid(sc,cr,sc->grid_color,gridarea,10,10);

    double xl = sc->priv->lower;
    double xh = sc->priv->upper;
    double yl = sd_min( sc->priv->data, sc->priv->data_size );
    double yh = sd_max( sc->priv->data, sc->priv->data_size );

    /* Create a logical conversion rectangle */
    lrect lgridarea(gridarea, xl,xh,yl,yh );

    /* Draw the data line using the logical grid area */
    draw_data_series(sc,cr,lgridarea,sc->priv->data,sc->priv->data_size);

    /* Draw the title */
    draw_text(cr,sc->accent_color,titlearea,layout);
    g_object_unref(layout);

    /* Draw the grid line */



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

    if (sc->priv->data)
    {
	delete[] sc->priv->data;
	sc->priv->data_size = 0;
	sc->priv->data = nullptr;
    }

    if ( G_OBJECT_CLASS(parent_class)->finalize )
	G_OBJECT_CLASS(parent_class)->finalize(obj);
}

static void stock_chart_class_init(GtkStockChartClass* c)
{
    g_type_class_add_private(c,sizeof(GtkStockChartPrivate));
    parent_class = (GtkDrawingAreaClass*)(g_type_class_peek_parent(c));

    GtkWidgetClass* wc = GTK_WIDGET_CLASS(c);
    GObjectClass* oc = G_OBJECT_CLASS(c);
    wc->draw = stock_chart_draw;
    oc->finalize = stock_chart_finalize;
}

static void stock_chart_init(GtkStockChart* obj)
{
    obj->title = g_strdup("[New Stock Chart]");
    obj->grid_color = GdkRGBA{0.6,0.6,0.6,1.0};
    obj->accent_color = GdkRGBA{1.0,0.2,0.3,1.0};
    obj->priv = GTK_STOCKCHART_GET_PRIVATE(obj);
    obj->priv->data = nullptr;
    obj->priv->data_size = 0;
}


/**
 * Creates a new stock chart widget.
 *
 * @return A pointer to a widget object
 */
GtkWidget* stock_chart_new()
{
    return GTK_WIDGET( g_object_new( GTK_TYPE_STOCKCHART, NULL) ); 
}

/**
 * Sets the title of the chart. The title is displayed prominently at the top of
 * the chart. A copy of the text is taken, rather than storing a pointer to the
 * string passed in.
 *
 * @param sc A pointer to a stock chart widget
 * @param title A pointer to a string to use as the title.
 */
void stock_chart_set_title(GtkStockChart* sc, const gchar* title)
{
    if (sc->title)
	g_free(sc->title);

    sc->title = g_strdup(title);
}

/**
 * Returns a pointer to the current title of the chart.
 *
 * @note Note that if the title is changed, the pointer previously returned by
 * this function becomes invalid.
 *
 * @param sc A pointer to a stock chart widget
 */
const gchar* stock_chart_get_title(GtkStockChart* sc)
{
    return sc->title;
}

/**
 * Sets the accent color for the chart. This color is used for the title text,
 * as well as the data series line itself.
 *
 * @param sc A pointer to a stock chart widget
 */
void stock_chart_set_accent_color( GtkStockChart* sc, const GdkRGBA& c )
{
    sc->accent_color = c;
}

/**
 * Sets the grid color for the chart. This is the color used to draw the axes
 * and grid lines.
 *
 * @param sc A pointer to a stock chart widget
 */
void stock_chart_set_grid_color( GtkStockChart* sc, const GdkRGBA& c )
{
    sc->grid_color = c;
}

/**
 * Sets the data to display on the chart. 
 */
void stock_chart_set_data( GtkStockChart* sc, const double* data, size_t sz,
			   double lower, double upper, guint xdivs)
{
    if (sc->priv->data)
	delete[] sc->priv->data;

    sc->priv->data = new double[sz];
    sc->priv->data_size = sz;
    memcpy(sc->priv->data,data,sz*sizeof(double));

    sc->priv->upper = upper;
    sc->priv->lower = lower;
    sc->priv->xdivs = xdivs;
}
