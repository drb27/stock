#ifndef STOCK_TASK_H
#define STOCK_TASK_H

#include <string>
#include "task.h"


class stock_task : public task<const std::string&,std::string>
{
public:
    stock_task();
};

#endif
