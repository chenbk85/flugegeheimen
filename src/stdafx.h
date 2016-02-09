#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <thread>
#include <future>
#include <mutex>
#include <atomic>

#include <string>
#include <list>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <chrono>

#include <ctime>
#include <cstdlib>
#include <cstring>

#include "rapidxml/rapidxml.hpp"
#include <json/json.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

#include <boost/lockfree/spsc_queue.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/python.hpp>

#include <mongo/client/dbclient.h>
#include <mongo/db/json.h>
