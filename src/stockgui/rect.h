/**
 * @file
 * Public header file for the point and rect classes.
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

#ifndef RECT_H
#define RECT_H

/**
 * Represets a point in 2D space with double precision cartesian coordinates.
 */
class point
{
public:
    point(double xx,double yy) : x(xx), y(yy) {}
    double x{0},y{0};

    point& offset(double xx, double yy) { x+=xx; y+=yy; return *this; }
    point& offset(const point& other) { x+=other.x; y+=other.y; return *this; }

    double distance(const point& other) const;
    double horizontal_distance( const point& other) const;
    double vertical_distance( const point& other) const;
};

/**
 * Represents a rectangle with aligned horizontal/vertical axes to a cartesian
 * x/y coordinate system, in 2D space. 
 */
class rect
{
public:
    rect(double xx, double yy, double ww, double hh) : origin{xx,yy},width(ww),height(hh) {}
    rect( const rect& other) : origin{other.left(),other.top()},width(other.width),height(other.height) {}

    point origin{0.0,0.0};
    double width{0},height{0};

    double top() const { return origin.y; }
    double bottom() const { return origin.y+height; }
    double left() const { return origin.x; }
    double right() const { return origin.x+width; }
    
    double area() const { return width*height; }
    
    point center() const { return point(origin.x + width/2.0, origin.y + height/2); }
    point topleft() const { return origin; }
    point topright() const { return point(origin.x + width, origin.y); }
    point bottomleft() const { return point(origin.x, origin.y + height); }
    point bottomright() const { return point(origin.x + width, origin.y + height); }

    rect& shiftleft(double i) { origin.x += i; width -=i; return *this; }
    rect& shiftright(double i) { width +=i; return *this; }
    rect& shifttop(double i) { origin.y += i; height -=i; return *this; }
    rect& shiftbottom(double i) { height +=i; return *this; }

    rect& setleft(double i) { double old_right = right(); origin.x = i; width = old_right - i; return *this; }
    rect& setright(double i) { width = i - origin.x; return *this; }
    rect& settop(double i) { double old_bottom = bottom(); origin.y = i; height = old_bottom - i; return *this; }
    rect& setbottom(double i) { height = i - origin.y; return *this; }

    rect& inset(double i);
    rect& offset( const point& p );

    rect& avoid(const rect& other, double margin);

    bool is_inside( const point& p) const;
    bool intersects( const rect& other) const;
};

#endif
