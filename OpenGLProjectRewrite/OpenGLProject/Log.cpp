#include "Camera.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <format>
#include <sstream>
#include <fstream>

enum class LogType { General, Error, Fatal };

bool HasLogged = false;
string InitialTimeForLog = "";

void log(string Input, LogType InputLogType = LogType::General) {
    auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
    tm* localTime = localtime(&currentTime);

    if (InitialTimeForLog == "") {
        stringstream tempStream;
        tempStream << put_time(localTime, "%d%m%Y%H%M%S");
        InitialTimeForLog = tempStream.str();
    }

    stringstream ss;
    ss << put_time(localTime, "%T");
    auto ms = duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()) % 1000;
    ss << '.' << setfill('0') << setw(3) << ms.count();

    string Placeholder = "";
    if (InputLogType == LogType::Fatal) {
        Placeholder = "FATAL: ";
    }
    else if (InputLogType == LogType::Error) {
        Placeholder = "ERROR: ";
    }

    stringstream finalText;
    finalText << "[" << ss.str() << "]: " << Placeholder << Input;

    cout << finalText.str() << endl;

    stringstream filePath;
    filePath << "logs/log-" << InitialTimeForLog << ".txt";
    ofstream log_file(filePath.str(), ios_base::out | ios_base::app);
    if (HasLogged == false) {
        HasLogged = true;
        stringstream temp;
        temp << endl << put_time(localTime, "%c") << endl;
        log_file << temp.str();
    }
    log_file << finalText.str() << endl;
    log_file.close();
    if (!log_file) {
        cout << "Failed to write to log file." << endl;
    }
}