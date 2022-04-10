#pragma once

#include <cassert>

#define karmac_assert(x) assert(x)
#define karmac_unimplemented() karmac_assert(0 && "unimplemented")