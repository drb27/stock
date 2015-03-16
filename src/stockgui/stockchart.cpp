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

static gboolean stock_chart_draw(GtkWidget* w, cairo_t* cr)
{
    guint width = gtk_widget_get_allocated_width(w);
    guint height = gtk_widget_get_allocated_height(w);

   /* GtkWidgets know their size on screen, specified in widget->allocation */
    double x, y;
    x = width / 2;
    y = height / 2;

    double radius;
    radius = MIN (width / 2,
                  height / 2) - 5;

    /* M_PI - a macro to express the value of pi (precision?) - provided by math.h */
    cairo_arc (cr, x, y, radius, 0, 2 * M_PI);

    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill_preserve (cr);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_stroke (cr);

    int i;
    for (i = 0; i < 12; i++)
    {
        int inset;
        inset = 0.1 * radius;
        cairo_move_to (cr,
                       x + (radius - inset) * cos (i * M_PI / 6),
                       y + (radius - inset) * sin (i * M_PI / 6));
        cairo_line_to (cr,
                       x + radius * cos (i * M_PI / 6),
                       y + radius * sin (i * M_PI / 6));
        cairo_stroke (cr);
    } 

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
