/**
 * @file
 * This is the main application file for the stockgui application. 
 * stockgui is a simple GTK+3 application which allows the user to query the
 * bid/ask price of any ticker symbol. 
 */

#include <iostream>
#include <thread>
#include <sstream>

#include <gtk/gtk.h>
#include <curl/curl.h>

#include <worker_base.h>
#include <stock_fetcher.h>

/// The stock fetcher thread takes its input from here. 
std::string g_ticker = "AAPL";

/// The output from the stock fetcher thread is stored here.
std::string g_result = ""; 

/// Build datestamp
#define Q(X) #X
#define QUOTE(X) Q(X)
const std::string g_buildstamp = QUOTE(BUILDSTAMP);

using std::thread;

/** 
 * Structure to contain pointers to widgets of interest. 
 * Controls in this structure can be easily manipulated by the code,
 * without having to find the widget handle each time. 
 * @note Provides a level of abstraction from the widget id
 */
typedef struct _controls_t
{
    GtkWidget* gobutton;
    GtkEntry* lasttradeprice;
    GtkEntry* ticker;
    GtkDialog* about;
    GtkWidget* about_ok;
    GtkImageMenuItem* menu_about;
    GtkLabel* buildstamp;
} controls_t;

static controls_t controls;

/** @name Callbacks
 * @{
 */

/**
 * Callback invoked when the main window is destroyed.
 */
void on_window_destroy()
{
    gtk_main_quit();
}

/**
 * Callback invoked when the stock thread is finished 
 */
gboolean on_fetch_complete(gpointer pdata)
{
    gtk_widget_set_sensitive(controls.gobutton,true);
    return FALSE;
}

gboolean on_menu_about(gpointer pdata)
{
    gint result = gtk_dialog_run( controls.about );
    gtk_widget_hide(GTK_WIDGET(controls.about));
    return FALSE;
}

/**
 * Callback invoked each time a stock query result is available
 */
gboolean on_got_result(gpointer pdata)
{
    gtk_entry_set_text(controls.lasttradeprice,g_result.c_str());
}

/**
 * Callback invoked when the OK button of the about dialog is pressed
 */
void on_about_ok()
{
    gtk_dialog_response(controls.about,0);
}


/**
 * Callback invoked when the 'go' button is clicked.
 */
void on_button_clicked()
{
    // Disble the button, to permit only one look-up at a time
    gtk_widget_set_sensitive(controls.gobutton,false);

    // Set result text to holding message
    gtk_entry_set_text(controls.lasttradeprice,"Fetching...");

    // Get the ticker text, and store in g_ticker
    g_ticker = gtk_entry_get_text(controls.ticker);

    // Initiate a worker thread to fetch the stock details
    stock_fetcher s;
    s.set_completion_action( []()
			     { 
				 g_main_context_invoke(NULL,on_fetch_complete,nullptr); 
			     });
    
    s.set_result_callback( [](std::string s)
			   {
			       g_result = s;
			       g_main_context_invoke(NULL,on_got_result,nullptr); 
			   } );
    thread t(s,99);
    t.detach();
}

//@}

int main(int argc, char* argv[])
{
    GObject *window;
    GtkBuilder* builder;
 
    /* Initialize GTK */
    gtk_init(&argc,&argv);

    /* Initialize CURL library */
    curl_global_init(CURL_GLOBAL_ALL);

    /* Contruct the UI from the compiled-in resource data */
    GError *error=nullptr;
    builder = gtk_builder_new();
    gtk_builder_add_from_resource(builder,
				  "/org/david.bradshaw.usa.gmail.com/stockgui/window.ui",&error); 

    /* Set up the main window */
    window = gtk_builder_get_object(builder,"window");
    g_signal_connect(window,"destroy", G_CALLBACK(on_window_destroy), NULL);
 
    /* Hook up the clicked signal for the 'go' button */
    controls.gobutton = GTK_WIDGET( gtk_builder_get_object(builder,"gobutton"));
    g_signal_connect(controls.gobutton,"clicked", G_CALLBACK(on_button_clicked), NULL );

    /* Locate the lasttradeprice/ticker text entry widget */
    controls.lasttradeprice = GTK_ENTRY( gtk_builder_get_object(builder,"lasttradeprice") );
    controls.ticker = GTK_ENTRY( gtk_builder_get_object(builder,"ticker") );

    /* Locate the controls for the about box */
    controls.about = GTK_DIALOG( gtk_builder_get_object(builder,"dialog_about"));
    controls.about_ok = GTK_WIDGET( gtk_builder_get_object(builder,"dlg_about_ok_btn") );
    controls.menu_about = GTK_IMAGE_MENU_ITEM( gtk_builder_get_object(builder,"menu_about") );
    controls.buildstamp = GTK_LABEL( gtk_builder_get_object(builder,"buildstamp") );
    g_signal_connect(controls.menu_about,"activate", G_CALLBACK(on_menu_about), NULL );
    g_signal_connect(controls.about_ok,"clicked", G_CALLBACK(on_about_ok), NULL );

    /* Set the buildtstamp label */
    std::stringstream s;
    s << "Built on " << g_buildstamp;
    gtk_label_set_text(controls.buildstamp, s.str().c_str());
    
    /* Dispose of the builder */
    g_object_unref(builder);

    /* Run the main GTK+ execution loop */
    gtk_main();

    return 0;
}
