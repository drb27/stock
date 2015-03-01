#include "stock-task.h"

using std::string;

namespace
{
    std::string fetch(const string& ticker)
    {
	return "UNIMPLEMENTED";
    }

    auto static_problem = problem<const string&,string>(&fetch,"AAPL");
}

stock_task::stock_task() : task<const string&,string>(static_problem)
{
}
