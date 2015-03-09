/**
 * @file
 * Public header file for the urltask class. 
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

#ifndef URLTASK_H
#define URLTASK_H

#include <string>
#include <functional>
#include <type_traits>

#include "buffer.h"
#include "task.h"

class urlproblem : public contained_problem<std::string,std::string>
{
public:
    urlproblem(const std::string& url);

    urlproblem( urlproblem&& o)=delete;
    urlproblem( const urlproblem& o)=delete;
    urlproblem& operator=( const urlproblem& )=delete;
    urlproblem& operator=( const urlproblem&& )=delete;

protected:

    virtual std::string do_work(std::string) final;
    virtual void fetch(buffer&, const std::string&);
    virtual std::string preprocess_url(const std::string&);
    virtual std::string decode_response(const std::string&);

private:

    static size_t rx_data(void*,size_t,size_t,void*);
    static void perform_query(buffer&,const std::string&);

};

class urltask : public task<std::string,std::string>
{
    
public:
    
    urltask(const std::string& url);
    urltask(urlproblem*);

    urltask( urltask&& ) = delete;
    urltask( const urltask& ) = delete;

    typedef void (callback)(urltask*,void*);
    void set_completion_callback( callback* c, void* data );
    
protected:

private:

    void notify_callback();

    std::function<callback> _callback_fn;
    void* _callback_data;    
    
};

#endif
