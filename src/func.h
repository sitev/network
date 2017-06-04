#pragma once

#include "core.h"
using namespace core;

namespace network {
	Str getIpByHost(Str host, Str port = "80");
}