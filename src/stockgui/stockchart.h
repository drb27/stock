/**
 * @file
 * Public header file for the custom stock charting widget
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

#ifndef _STOCKCHART_H
#define _STOCKCHART_H

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GTK_TYPE_STOCKCHART          (stock_chart_get_type ())
#define GTK_STOCKCHART(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_STOCKCHART, GtkStockChart))
#define GTK_STOCKCHART_CLASS(cls)    (G_TYPE_CHECK_CLASS_CAST ((obj), GTK_STOCKCHART, GtkStockChartClass))
#define GTK_IS_STOCKCHART(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_STOCKCHART))
#define GTK_IS_CLOCK_FACE_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE ((obj), GTK_TYPE_STOCKCHART))
#define GTK_STOCKCHART_GET_CLASS     (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_STOCKCHART, GtkStockChartClass))

typedef struct _GtkStockChart GtkStockChart;
typedef struct _GtkStockChartClass GtkStockChartClass;

/**
 *
 */
struct _GtkStockChart
{
    GtkDrawingArea* parent;
};

/**
 *
 */
struct _GtkStockChartClass
{
    GtkDrawingAreaClass parent_class;
};

#ifdef __cplusplus
}
#endif

#endif	/* _STOCKCHART_H */
