#pragma once

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <format>
#include <sstream>
#include <fstream>

enum class LogType { General, Error, Fatal };
extern bool HasLogged;
void log(std::string Input, LogType InputLogType = LogType::General);