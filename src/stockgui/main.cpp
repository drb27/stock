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
#include <stocklib/stocklib.h>

using std::string;
using std::thread;

/// The stock fetcher thread takes its input from here. 
std::string g_ticker = "AAPL";

/// The output from the stock fetcher thread is stored here.
std::string g_result = ""; 

/// Build datestamp
#define Q(X) #X
#define QUOTE(X) Q(X)
const std::string g_buildstamp = QUOTE(BUILDSTAMP);


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
    GtkMenuItem* menu_about;
    GtkMenuItem* menu_quit;
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
    /* Ensure all asynchronous tasks have finished */
    stocklib_wait_all();

    /* Tidy up any open handles */
    stocklib_cleanup();

    /* Quit the main GTK processing loop */
    gtk_main_quit();
}

/**
 * Callback invoked when the About menu option is selected
 */
gboolean on_menu_about(gpointer pdata)
{
    /* Run the dialog box until the user clicks the 'ok' button */
    gint result = gtk_dialog_run( controls.about );

    /* Must manually hide the about box - but don't destroy it! */
    gtk_widget_hide(GTK_WIDGET(controls.about));
    return FALSE;
}

/**
 * Callback invoked when the Quit menu option is selected
 */
gboolean on_menu_quit(gpointer pdata)
{
    on_window_destroy();
}

/**
 * Callback invoked each time a stock query result is available
 */
gboolean on_got_result(gpointer pdata)
{
    stocklib_asynch_dispose((SLHANDLE)pdata);
    gtk_entry_set_text(controls.lasttradeprice,g_result.c_str());
    gtk_widget_set_sensitive(controls.gobutton,true);
    return FALSE;
}

static char rbuffer[SL_MAX_BUFFER]="Oh boy!";

/**
 * Callback invoked when the OK button of the about dialog is pressed
 */
void on_about_ok()
{
    gtk_dialog_response(controls.about,0);
}

void on_asynch_result( SLHANDLE h, void* pData )
{
    // Determine the outcome of the request, store result
    if ( SL_OK != stocklib_asynch_result(h) )
	g_result = "[ERROR]";
    else
	g_result = rbuffer;

    // Update the UI on the correct thread
    g_main_context_invoke(NULL,on_got_result,h);
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

    // Invoke an asynchronous fetch of the data
    SLHANDLE h = stocklib_fetch_asynch( g_ticker.c_str(), rbuffer );
    stocklib_asynch_register_callback(h,&on_asynch_result,rbuffer); 
}

//@}

/**
 * Application entry point.
 *
 * Constructuts the UI from compiled-in resources, then hands control over
 * to the GTK+ event processing loop.
 */
int main(int argc, char* argv[])
{
    GObject *window;
    GtkBuilder* builder;
 
    /* Initialize GTK */
    gtk_init(&argc,&argv);

    /* Initialize CURL library */
    curl_global_init(CURL_GLOBAL_ALL);

    /* Initialize the stocklib library */
    stocklib_init();

    /* Contruct the UI from the compiled-in resource data */
    GError *error=nullptr;
    builder = gtk_builder_new();

    gtk_builder_add_from_resource(builder,
				  "/org/david.bradshaw.usa.gmail.com/stockgui/window.ui",
				  &error);

    /* Set up the main window */
    window = gtk_builder_get_object(builder,"window");
    g_signal_connect(window,"destroy", G_CALLBACK(on_window_destroy), NULL);
 
    /* Hook up the clicked signal for the 'go' button */
    controls.gobutton = GTK_WIDGET( gtk_builder_get_object(builder,"gobutton"));
    g_signal_connect(controls.gobutton,"clicked", G_CALLBACK(on_button_clicked), NULL );

    /* Locate the lasttradeprice/ticker text entry widget */
    controls.lasttradeprice = GTK_ENTRY( gtk_builder_get_object(builder,"lasttradeprice") );
    controls.ticker = GTK_ENTRY( gtk_builder_get_object(builder,"ticker") );

    /* Locate the controls for the menu */
    controls.menu_about = GTK_MENU_ITEM( gtk_builder_get_object(builder,"menu_about") );
    controls.menu_quit = GTK_MENU_ITEM( gtk_builder_get_object(builder,"menu_quit") );
    g_signal_connect(controls.menu_about,"activate", G_CALLBACK(on_menu_about), NULL );
    g_signal_connect(controls.menu_quit,"activate", G_CALLBACK(on_menu_quit), NULL );

    /* Locate the controls for the about box */
    controls.about = GTK_DIALOG( gtk_builder_get_object(builder,"dialog_about"));
    controls.about_ok = GTK_WIDGET( gtk_builder_get_object(builder,"dlg_about_ok_btn") );
    controls.buildstamp = GTK_LABEL( gtk_builder_get_object(builder,"buildstamp") );
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
