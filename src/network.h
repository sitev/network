#pragma once

#include "socket.h"
#include "func.h"

#include "socket_handler.h"

#ifdef OS_WINDOWS
#include "wsa_socket_handler.h"
#endif

#ifdef OS_LINUX
#include "epoll_handler.h"
#endif