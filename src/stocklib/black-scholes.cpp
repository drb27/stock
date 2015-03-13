/**
 * @file 
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

#include <cmath>
#include "black-scholes.h"

using std::function;
using std::bind;
using std::placeholders::_1;

double bs_normal(double u, double s, double x)
{
    double t1 = 1.0d/(s*sqrt(2.0*M_PI));
    double t2 = -pow(x-u,2)/(2*pow(s,2));
    return t1*exp(t2);
}

function<double(double)> bs_bind_normal(double u, double s)
{
    return bind(bs_normal,u,s,_1);
}

double bs_cndf(double u, double s, double z)
{
    return 0.5d * ( 1 + erf((z-u)/(s*sqrt(2))) );
}

double bs_cndfn(double z)
{
    return bs_cndf(0.0,1.0,z);
}

static double bs_d1(double ap, double sp, double r, double s, double t)
{
    /// TODO: Consider guards and boundary conditions
    double e = log(ap/sp) + (r+(s*s/2))*t;
    double d = s*sqrt(t);
    return e/d;
}

static double bs_d2(double ap, double sp, double r, double s, double t)
{
    /// TODO: Consider guards and boundary conditions
    double e = log(ap/sp) + (r-(s*s/2))*t;
    double d = s*sqrt(t);
    return e/d;
}

double bs_call_price(double asset_price, double strike_price, double expiry, double r,
		     double volatility)
{
    /// TODO: Consider guards and boundary conditions
    double d1 = bs_d1(asset_price,strike_price,r,volatility,expiry);
    double d2 = bs_d2(asset_price,strike_price,r,volatility,expiry);
    
    return asset_price*bs_cndfn(d1) - strike_price*bs_cndfn(d2)*exp(-r*expiry);
}

double bs_put_price(double asset_price, double strike_price, double expiry, double r,
		     double volatility)
{
    /// TODO: Consider guards and boundary conditions
    double d1 = bs_d1(asset_price,strike_price,r,volatility,expiry);
    double d2 = bs_d2(asset_price,strike_price,r,volatility,expiry);

    return strike_price*exp(-r*expiry)*bs_cndfn(-d2) - asset_price*bs_cndfn(-d1);
}
