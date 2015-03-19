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
#include <cmath>
#include "stockchart.h"

G_DEFINE_TYPE (GtkStockChart, stock_chart, GTK_TYPE_DRAWING_AREA);

class point
{
public:
    point(double xx,double yy) : x(xx), y(yy) {}
    const double x{0},y{0};
};

class rect
{
public:
    rect(double xx, double yy, double ww, double hh) : origin{xx,yy},width(ww),height(hh) {}
    const point origin{0.0,0.0};
    const double width{0},height{0};

    double top() const { return origin.y+height; }
    double bottom() const { return origin.y; }
    double left() const { return origin.x; }
    double right() const { return origin.x+width; }
    
    double area() const { return width*height; }
    
    point center() const { return point(origin.x + width/2.0, origin.y + height/2); }
    point bottomleft() const { return origin; }
    point bottomright() const { return point(origin.x + width, origin.y); }
    point topleft() const { return point(origin.x, origin.y + height); }
    point topright() const { return point(origin.x + width, origin.y + height); }

    rect inset(double i) const { return rect(origin.x+i,origin.y+i,width-2*i,height-2*i); }
    rect offset( const point& p ) const { return rect(origin.x+p.x,origin.y+p.y,width,height); }
};

static void draw_background(cairo_t* cr, rect r)
{
    cairo_pattern_t* pat;
    cairo_save(cr);
    pat=cairo_pattern_create_linear(r.width/2-5,5, r.width/2-5, r.height-5);
    cairo_pattern_add_color_stop_rgba(pat, 0.0, 0.4, 0.4, 0.4, 1.0);
    cairo_pattern_add_color_stop_rgba(pat, 1.0, 0.0, 0.0, 0.0, 1.0);
    cairo_set_source(cr, pat);
    cairo_rectangle(cr, r.left(),r.bottom(), r.width, r.height);
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

    for ( double y = area.bottom(); y < ( area.top() + 0.5*y_size ); y+= y_size )
    {
	cairo_move_to(cr,area.left(),y);
	cairo_line_to(cr,area.right(),y);
    }

    cairo_stroke(cr);
}


static gboolean stock_chart_draw(GtkWidget* w, cairo_t* cr)
{
    guint width = gtk_widget_get_allocated_width(w);
    guint height = gtk_widget_get_allocated_height(w);

    rect area(0.0,0.0,width,height);

    double x, y;
    x = width / 2;
    y = height / 2;

    draw_background(cr,area);
    draw_grid(cr,area.inset(10),10,10);

    return FALSE;
}

static void stock_chart_class_init(GtkStockChartClass* c)
{
    GtkWidgetClass* wc = GTK_WIDGET_CLASS(c);
    wc->draw = stock_chart_draw;
}

static void stock_chart_init(GtkStockChart* obj)
{
}

GtkWidget* stock_chart_new()
{
    return GTK_WIDGET( g_object_new( GTK_TYPE_STOCKCHART, NULL) ); 
}
