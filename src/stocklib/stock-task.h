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

#ifndef STOCK_TASK_H
#define STOCK_TASK_H

#include <string>
#include <functional>

#include <stocklib/stock-task-modes.h>
#include "task.h"

typedef struct
{
    std::string ticker;
    sl_test_behavior_t behavior;
} stock_task_params_t;

class stock_task : public task<stock_task_params_t,std::string>
{
public:
    stock_task(std::string, sl_test_behavior_t);

    typedef void (callback)(stock_task*,void*);

    void set_completion_callback( callback* c, void* data );

protected:

    void notify_callback();

private:
    std::function<callback> _callback_fn;
    void* _callback_data;
    
};

#endif
