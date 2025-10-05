#pragma once

#ifdef _WIN32
#include <fcntl.h>
#include <handleapi.h>
#include <io.h>
#include <processthreadsapi.h>
#include <synchapi.h>
#include <sys/stat.h>
#include <windows.h>

#include <array>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <iostream>
#include <string>
#include <system_error>


#define close _close
#define read _read
#define write _write
#define pipe _pipe
#define dup2 _dup2


using pid_t = DWORD;

class Pipe {
private:
    std::array<int, 2> fd_;

public:
    Pipe();
    ~Pipe();
    int Read_fd() const;
    int Write_fd() const;
    void Close_read();
    void Close_write();
};

class ProcessManager {
public:
    static pid_t ChangeProcess(const char *filename, char *const argv[], char *const envp[] = nullptr,
                               HANDLE hStdInput = NULL);
    static void Wait_for_child(pid_t proc);
    static pid_t Get_current_pid();
    static HANDLE Get_Handle(int crt_fd);
};

bool IsFileNameGood(const std::string &st);
std::string RemoveVowels(const std::string &str);
bool WriteFile(const std::string &filename, const std::string &content);

#endif