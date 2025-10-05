#include "ProcessManager.h"

Pipe::Pipe() {
    if (pipe(fd_.data()) == -1) {
        throw std::system_error(errno, std::system_category(), "pipe creation failed");
    }
}

Pipe::~Pipe() {
    if (fd_[0] != -1) {
        close(fd_[0]);
    }
    if (fd_[1] != -1) {
        close(fd_[1]);
    }
}

int Pipe::Read_fd() const { return fd_[0]; }

int Pipe::Write_fd() const { return fd_[1]; }

void Pipe::Close_read() {
    if (fd_[0] != -1) {
        close(fd_[0]);
        fd_[0] = -1;
    }
}

void Pipe::Close_write() {
    if (fd_[1] != -1) {
        close(fd_[1]);
        fd_[1] = -1;
    }
}

pid_t ProcessManager::Create_process() {
    pid_t pid = fork();
    if (pid == -1) {
        throw std::system_error(errno, std::system_category(), "fork failed");
    }
    return pid;
}

void ProcessManager::Wait_for_child(pid_t proc) {
    int status;
    waitpid(proc, &status, 0);
}

void ProcessManager::Dup2(pid_t fd, int f_num) { dup2(fd, f_num); }

void ProcessManager::ChangeProcess(const char *filename, char *const argv[], char *const envp[]) {
    execve(filename, argv, envp);
}

pid_t ProcessManager::Get_current_pid() { return getpid(); }

bool IsFileNameGood(const std::string &st) {
    if (st.empty()) {
        return false;
    }
    if (st.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != std::string::npos) {
        return false;
    }
    return true;
}

std::string RemoveVowels(const std::string &str) {
    std::string new_str = "";
    std::string pattern = "aeiouyAEIOUY";
    for (char chr : str) {
        if (pattern.find(chr) == std::string::npos) {
            new_str += chr;
        }
    }
    return new_str;
}

bool WriteFile(const std::string &filename, const std::string &content) {
    int fd = open(filename.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd == -1) {
        perror("open");
        return false;
    }
    int bytes_written = write(fd, content.c_str(), content.size());
    if (bytes_written == -1) {
        perror("write");
        close(fd);
        return false;
    }
    if (bytes_written != static_cast<int>(content.size())) {
        std::cerr << "Partial write" << std::endl;
        close(fd);
        return false;
    }
    if (close(fd) == -1) {
        perror("close");
        return false;
    }
    return true;
}