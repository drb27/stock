/**
 * @file
 * Implementation of the point and rectangle classes
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
#include "rect.h"

double point::distance(const point& other) const
{
    return sqrt( pow(other.x-x,2) + pow(other.y-y,2) );
}

double point::horizontal_distance(const point& other) const
{
    return other.x - x;
}

double point::vertical_distance(const point& other) const
{
    return other.y - y;
}

/**
 * Represents a rectangular area with double precision coordinates.
 */

rect& rect::inset(double i)
{
    origin.offset(i,i);
    width -= 2.0*i;
    height -= 2.0*i;
    return *this;
}

rect& rect::offset( const point& p )
{
    origin.offset(p);
    return *this;
}

bool rect::is_inside(const point& p) const
{
    return ( ( (p.x > origin.x) && (p.x < (origin.x+width) ) ) && 
	     ( (p.y > origin.y) && (p.y < (origin.y+height)) ) 
	     );
}

bool rect::intersects(const rect& other) const
{
    bool outside = 
	(other.right() < left()) || 
	(other.left() > right()) || 
	(other.bottom() < top()) || 
	(other.top() > bottom());

    return !outside;
}

rect& rect::avoid(const rect& other, double margin)
{
    auto fnleft = [&](){
	/* Test to see if left edge needs to move */
	if ( (intersects(other)) && ( other.right() >= origin.x ) && (other.center().x <= center().x ) )
	{
	    setleft(other.right() + margin);
	}
    };

    auto fnright = [&]() {
	/* Test to see if right edge needs to move */
	if ( (intersects(other)) && (other.left() <= right() ) && (other.center().x > center().x) )
	{
	    setright(other.left()-margin);
	}
    };


    auto fntop = [&]() {
	/* Test to see if the top edge needs to move */
	if ( (intersects(other)) && (other.bottom() >= origin.y ) && (other.center().y <= center().y) )
	{
	    settop(other.bottom()+margin);
	}
    };

    auto fnbottom = [&]() {
	/* Test to see if the bottom edge needs to move */
	if ( (intersects(other)) && (other.top() <= bottom() ) && (other.center().y > center().y ) )
	{
	    setbottom(other.top()-margin);
	}
    };

    double h_distance = fabs( center().horizontal_distance(other.center()) );
    double v_distance = fabs( center().vertical_distance(other.center()) );

    if (h_distance<v_distance)
    {
	fntop(); fnbottom(); fnleft(); fnright();
    }
    else
    {
	fnleft(); fnright(); fntop(); fnbottom();
    }

    return *this;
}
