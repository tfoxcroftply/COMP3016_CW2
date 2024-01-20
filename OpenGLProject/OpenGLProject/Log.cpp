#include "Camera.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <format>
#include <sstream>
#include <fstream>
#include <GLFW/glfw3.h>

enum class LogType { General, Error, Fatal }; // 3 enums which control log state

bool HasLogged = false; // for setting the time stamp at the top
string InitialTimeForLog = "";

void log(string Input, LogType InputLogType = LogType::General) { // logging function that takes text and the enum from above
    auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now()); // retrieves time
    tm* localTime = localtime(&currentTime);

    if (InitialTimeForLog == "") { // this sets the filename
        stringstream tempStream;
        tempStream << put_time(localTime, "%d%m%Y%H%M%S");
        InitialTimeForLog = tempStream.str();
    }

    stringstream ss; // string stream for adding strings together
    ss << put_time(localTime, "%T");
    auto ms = duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()) % 1000; // get milliseconds
    ss << '.' << setfill('0') << setw(3) << ms.count(); // adds neccessary 0s to time

    string Placeholder = "";
    if (InputLogType == LogType::Fatal) { // error type markers
        Placeholder = "FATAL: ";
    }
    else if (InputLogType == LogType::Error) {
        Placeholder = "ERROR: ";
    }

    stringstream finalText; // final output string
    finalText << "[" << ss.str() << "]: " << Placeholder << Input;
    cout << finalText.str() << endl;
  
    stringstream filePath; // make file name
    filePath << "logs/log-" << InitialTimeForLog << ".txt";
    ofstream log_file(filePath.str(), ios_base::out | ios_base::app); // open file
    if (HasLogged == false) {
        HasLogged = true;
        stringstream temp;
        temp << endl << put_time(localTime, "%c") << endl;
        log_file << temp.str();
    }
    log_file << finalText.str() << endl; // update file
    log_file.close(); // close file
    if (!log_file) {
        cout << "Failed to write to log file." << endl;
    }
    if (InputLogType == LogType::Fatal) { // error type markers
        //glfwTerminate(); temporary no kill
    }
}