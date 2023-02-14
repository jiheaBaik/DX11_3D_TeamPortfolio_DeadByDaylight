#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <thread>
#include <WinSock2.h>
#include <mutex>
#include <queue>
#include <string>
#include <memory>
#include <list>
#include <fstream>
using namespace std;

#include "json/json.h"
#include "json/json-forwards.h"