#pragma once

#ifdef __linux__
#include <array>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <sched.h>
#include <string>
#include <sys/stat.h>
#include <sys/wait.h>
#include <system_error>
#include <unistd.h>

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
    static pid_t Create_process();
    static void Wait_for_child(pid_t proc);
    static void Dup2(pid_t fd, int f_num);
    static void ChangeProcess(const char *filename, char *const argv[], char *const envp[] = nullptr);
    static pid_t Get_current_pid();
};

bool IsFileNameGood(const std::string &st);
std::string RemoveVowels(const std::string &str);
bool WriteFile(const std::string &filename, const std::string &content);
#endif