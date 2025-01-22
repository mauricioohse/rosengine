#pragma once

#define xcomponent(name, type, id, ...) name,
enum component_type
{
#include "components.def"
    COMPONENT_LAST_VALUE // always is the last value on the enum
};
#undef xcomponent