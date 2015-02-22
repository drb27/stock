# stock
Fetches stock information from Yahoo! Finance using YQL. It isn't very good, or very useful - I made it only to experiment with a few software techniques - namely *autotools*, *GTK+3*, *C++11*, and *libcurl*. 
##The Binaries
### The command line
The command line tool is simply called *stock*. Invoke it with a ticker symbol, like this:

	me@mymachine ~/stock$ stock AAPL
 
###The GUI
A simple GUI is provided, which requires a recent version of GTK+ (3.10 is recommended, it might work on slightly earlier versions). The binary is called stockgui. Invoke it like this:

	me@mymachine ~/stock$ stockgui &

## Building
### Automake
This source package supports autotools. If you downloaded the source from GitHub, you'll probably need *autoconf*, *autoheader*, and *automake*, to generate the *configure* script. If you downloaded a distribution ready to build, you don't need those. 

Once you have a configure script, simply:

	me@mymachine ~stock$ ./configure
	me@mymachine ~stock$ make 

You'll end up with the binaries in the current folder. You're ready to go.

### Dependencies
**This software requires g++ version 4.9. Don't try and waste your time building it if you don't have it. Even if you *do* succeed in getting a build, it probably won't work **

You'll need the following:

- jansson
- curl
- GTK+3
- cppunit
- doxygen *(optional)*

## License
This software is licenced to you under the standard MIT license. Please see the LICENSE file for full information.
 
