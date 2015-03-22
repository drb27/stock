/**
 * @file
 * This is the main application file for the stockgui application. 
 * stockgui is a simple GTK+3 application which allows the user to query the
 * bid/ask price of any ticker symbol. 
 */

#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <exception>
#include <stdexcept>
#include <regex>

#include <gtk/gtk.h>
#include <curl/curl.h>
#include <stocklib/stocklib.h>
#include "stockchart.h"

#define Q(X) #X
#define QUOTE(X) Q(X)

using std::string;
using std::regex;

/// The stock fetcher thread takes its input from here. 
std::string g_ticker = "AAPL";

/// The output from the stock fetcher thread is stored here.
std::string g_result = ""; 

/// Embeds the build date and time into the output
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
    GtkEntry* companyname;
    GtkDialog* about;
    GtkWidget* about_ok;
    GtkMenuItem* menu_about;
    GtkMenuItem* menu_quit;
    GtkMenuItem* menu_stockchart;
    GtkLabel* buildstamp;
} controls_t;

typedef struct _opt_controls_t
{
    GtkEntry* asset_price;
    GtkEntry* strike_price;
    GtkEntry* volatility;
    GtkEntry* interest;
    GtkEntry* expiry;
    GtkEntry* option_value;
    GtkEntry* option_delta;
    GtkWidget* calc_button;
    GtkToggleButton* call_radio;
} opt_controls_t;

static controls_t controls;
static opt_controls_t octrls;

/**
 * @name Input Validation
 * Functions and types pertaining to input validation
 * @{
 */

/**
 * The type for an input validation function
 */
typedef bool entry_validator_fn(const string&);

/**
 * Validates that the given string can be converted into a double precision
 * floating point number by the runtime library. 
 *
 * @param input The string to validate
 * @return true if validation succeeded, false otherwise
 */
bool validate_double(const string& input)
{
    regex ve("^-?[0-9]+\\.?[0-9]+$");
    return std::regex_match(input,ve);
}

/**
 * Validates that the given string can be converted into a double precision
 * floating point number by the runtime library, and that the resulting value is
 * greater than zero.  
 *
 * @param input The string to validate
 * @return true if validation succeeded, false otherwise
 */
bool validate_positive_double(const string& input)
{
    regex ve("^[0-9]*\\.?[0-9]+$");
    return std::regex_match(input,ve);
}

///@}

/**
 * @name Application Logic
 * @{
 */

/**
 * Fetches the parameter options from the controls into the given structure.
 * Throws logic_error if the translation couldn't be done. 
 *
 * @param params The structure into which to place the data
 */
void fetch_option_params( option_params_t& params)
{
    try
    {
	if (gtk_toggle_button_get_active(octrls.call_radio))
	    params.otype = SLOTCall;
	else
	    params.otype = SLOTPut;

	params.asset_price = std::stod( gtk_entry_get_text( octrls.asset_price ), nullptr );
	params.strike_price = std::stod( gtk_entry_get_text( octrls.strike_price ), nullptr );
	params.volatility = std::stod( gtk_entry_get_text( octrls.volatility ), nullptr );
	params.interest = std::stod( gtk_entry_get_text( octrls.interest ), nullptr );
	params.expiry = std::stod( gtk_entry_get_text( octrls.expiry ), nullptr )/365.0d;
    }
    catch(...)
    {
	throw std::logic_error("Conversion failed");
    }
}

///@}

/** 
 * @name GTK Event Handlers
 * Functions that respond to GTK signals
 * @{
 */



/**
 * Called when the calculate button is pressed in the options screen.
 */
gboolean on_calculate_option_price(gpointer pdata)
{
    /* Retrieve the user input from the UI */
    option_params_t params;
    fetch_option_params(params);

    /* Get the price, write the output */
    std::stringstream s;
    s << stocklib_option_price(params);
    gtk_entry_set_text(octrls.option_value, s.str().c_str());

    s.str("");
    s.clear();

    /* Get the greeks, write the output */
    s << stocklib_option_greek(params,SLGDelta);
    gtk_entry_set_text(octrls.option_delta, s.str().c_str());
}


/**
 * Called each time a GtkEntry widget loses focus in the options calculator
 * window. Determines if a complete recipe is available, and therefore whether
 * or not the calculate button can be enabled. 
 */
gboolean on_options_recipe_changed(gpointer pdata)
{
    static std::set<GtkEntry*> bad_ctrls;

    GdkRGBA red;
    red.red = 1.0;
    red.green  = 0.0;
    red.blue=0.0;
    red.alpha = 1.0;

    GtkEntry* ctrl = GTK_ENTRY(pdata);
    string txt = gtk_entry_get_text(ctrl);
    if (validate_positive_double(txt))
    {
	if (bad_ctrls.find(ctrl)!=bad_ctrls.end())
	{
	    bad_ctrls.erase(ctrl);
	    gtk_widget_override_color(GTK_WIDGET(ctrl),GTK_STATE_FLAG_NORMAL,nullptr);
	}
    }
    else
    {
	bad_ctrls.insert(ctrl);
	gtk_widget_override_color(GTK_WIDGET(ctrl),GTK_STATE_FLAG_NORMAL,&red);
    }

    gtk_widget_set_sensitive( octrls.calc_button, bad_ctrls.size()==0);
    return FALSE;
}

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
 * Called when Developer_StockChart Demo option is selected
 */
gboolean on_menu_stockchart(gpointer pdata)
{
    auto window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),"StockChart Demo");
    GtkWidget* sc = stock_chart_new();
    gtk_container_add(GTK_CONTAINER(window),sc);
    gtk_widget_show_all(window);
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

    gtk_entry_set_text(controls.companyname,
		       stocklib_ticker_to_name( g_ticker.c_str() )
		       );

    gtk_widget_set_sensitive(controls.gobutton,true);
    return FALSE;
}

static char rbuffer[SL_MAX_BUFFER]="";

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

///@}

/**
 * Application entry point.
 *
 * Constructuts the UI from compiled-in resources, then hands control over
 * to the GTK+ event processing loop.
 */
int main(int argc, char* argv[])
{
    GObject *window,*optioncalc_window;
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
 
    /* Set up the option calculator window */
    optioncalc_window = gtk_builder_get_object(builder,"optioncalc");

    /* Hook up the clicked signal for the 'go' button */
    controls.gobutton = GTK_WIDGET( gtk_builder_get_object(builder,"gobutton"));
    g_signal_connect(controls.gobutton,"clicked", G_CALLBACK(on_button_clicked), NULL );

    /* Locate the lasttradeprice/ticker/company name text entry widget */
    controls.lasttradeprice = GTK_ENTRY( gtk_builder_get_object(builder,"lasttradeprice") );
    controls.ticker = GTK_ENTRY( gtk_builder_get_object(builder,"ticker") );
    controls.companyname = GTK_ENTRY( gtk_builder_get_object(builder,"companyname") );

    /* Locate the controls for the menu */
    controls.menu_about = GTK_MENU_ITEM( gtk_builder_get_object(builder,"menu_about") );
    controls.menu_quit = GTK_MENU_ITEM( gtk_builder_get_object(builder,"menu_quit") );
    controls.menu_stockchart = GTK_MENU_ITEM( gtk_builder_get_object(builder,"menu_developer_stockchart"));
    g_signal_connect(controls.menu_about,"activate", G_CALLBACK(on_menu_about), NULL );
    g_signal_connect(controls.menu_quit,"activate", G_CALLBACK(on_menu_quit), NULL );
    g_signal_connect(controls.menu_stockchart,"activate", G_CALLBACK(on_menu_stockchart), NULL );

    /* Locate the controls for the about box */
    controls.about = GTK_DIALOG( gtk_builder_get_object(builder,"dialog_about"));
    controls.about_ok = GTK_WIDGET( gtk_builder_get_object(builder,"dlg_about_ok_btn") );
    controls.buildstamp = GTK_LABEL( gtk_builder_get_object(builder,"buildstamp") );
    g_signal_connect(controls.about_ok,"clicked", G_CALLBACK(on_about_ok), NULL );

    /* Locate the controls for the option calculator */
    octrls.interest = GTK_ENTRY( gtk_builder_get_object(builder,"interest") );
    octrls.expiry = GTK_ENTRY( gtk_builder_get_object(builder,"expiry") );
    octrls.volatility = GTK_ENTRY( gtk_builder_get_object(builder,"volatility") );
    octrls.asset_price = GTK_ENTRY( gtk_builder_get_object(builder,"asset_price") );
    octrls.strike_price = GTK_ENTRY( gtk_builder_get_object(builder,"strike_price") );
    octrls.calc_button = GTK_WIDGET( gtk_builder_get_object(builder,"option_calc_button") );
    octrls.option_value = GTK_ENTRY( gtk_builder_get_object(builder,"option_value") );
    octrls.option_delta = GTK_ENTRY( gtk_builder_get_object(builder,"option_delta") );
    octrls.call_radio = GTK_TOGGLE_BUTTON( gtk_builder_get_object(builder, "oc_call_radio") );

    std::set<GtkEntry*> ctrls{octrls.interest,octrls.expiry,octrls.volatility,
	    octrls.asset_price,octrls.strike_price};

    for ( auto c : ctrls )
    {
	g_signal_connect(c,"focus-out-event",
			 G_CALLBACK(on_options_recipe_changed), c);
    }

    gtk_entry_set_text(octrls.option_value,"???");
    g_signal_connect(octrls.calc_button,"clicked", G_CALLBACK(on_calculate_option_price),nullptr);

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

