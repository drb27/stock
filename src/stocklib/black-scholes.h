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

#ifndef BLACK_SCHOLES_H
#define BLACK_SCHOLES_H

#include <functional>

/**
 * Calculates the probability density for the normal distribution with mean of u
 * and standard deviation of s.
 *
 * @param u the mean of the distribution
 * @param s the standard deviation of the distribution
 * @param x the x value of the distribution
 * @return the probability density
 */
double bs_normal(double u, double s, double x);

/**
 * Computes the cumulative normal probability distribution function for z. This
 * is equivalent to integrating the probability density function over the range
 * [-infinity,z].
 *
 * @param u The mean of the distribution
 * @param s The standard deviation of the distribution
 * @param z Specifies the upper bound of the integration
 * @return the result of the integration
 */
double bs_cndf(double u, double s, double z);

/**
 * Computes the cumulative normal probability distribution function for z. This
 * is equivalent to integrating the standard probability density function over
 * the range [-infinity,z].
 *
 * @param z Specifies the upper bound of the integration
 * @return the result of the integration
 */
double bs_cndfn(double z);

/**
 * Returns a function object which itself returns the probability density for a
 * given normal distribution represented by mean u and standard deviation s. 
 *
 * @param u The mean of the distribution
 * @param s The standard deviation of the distribution
 * @returns a probability density function for the requested normal distribution.
 */
std::function<double(double)> bs_bind_normal(double u, double s);

/**
 * Calculates the theoretical value of a call option using black-scholes. 
 *
 * @param asset_price The price of the underlying security
 * @param strike_price The strike price of the option
 * @param expiry Time to expiry, in years
 * @param r The risk-free interest rate
 * @param vol The volatility of the underlysing security
 *
 * @return The theoretical value of the option, in the same unit as the prices passed in
 * for asset_price and strike_price
 */
double bs_call_price(double asset_price, double strike_price, double expiry, double r,
		     double vol);

/**
 * Calculates the theoretical value of a put option using black-scholes. 
 *
 * @param asset_price The price of the underlying security
 * @param strike_price The strike price of the option
 * @param expiry Time to expiry, in years
 * @param r The risk-free interest rate
 * @param vol The volatility of the underlysing security
 *
 * @return The theoretical value of the option, in the same unit as the prices passed in
 * for asset_price and strike_price
 */
double bs_put_price(double asset_price, double strike_price, double expiry, double r,
		     double vol);

#endif
