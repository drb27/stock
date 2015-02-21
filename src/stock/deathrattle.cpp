#include "deathrattle.h"

deathrattle::~deathrattle()
{
    if (!aborted) _f();
}

