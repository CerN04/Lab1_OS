#include "Win_ProcessManager.h"

Pipe::Pipe() {
    if (_pipe(fd_.data(), 4096, _O_BINARY) == -1) {
        throw std::system_error(errno, std::system_category(), "Pipe creation failed");
    }
    if (fd_[0] == -1 || fd_[1] == -1) {
        throw std::system_error(errno, std::system_category(), "Pipe creation failed: invalid descriptors");
    }
}

Pipe::~Pipe() {
    if (fd_[0] != -1) {
        _close(fd_[0]);
    }
    if (fd_[1] != -1) {
        _close(fd_[1]);
    }
}

int Pipe::Read_fd() const { return fd_[0]; }

int Pipe::Write_fd() const { return fd_[1]; }

void Pipe::Close_read() {
    if (fd_[0] != -1) {
        _close(fd_[0]);
        fd_[0] = -1;
    }
}

void Pipe::Close_write() {
    if (fd_[1] != -1) {
        _close(fd_[1]);
        fd_[1] = -1;
    }
}

pid_t ProcessManager::ChangeProcess(const char *filename, char *const argv[], char *const envp[], HANDLE hStdInput) {
    std::string cmd_line = filename;
    int i = 1;
    while (argv[i] != nullptr) {
        cmd_line += " ";
        cmd_line += argv[i];
        i++;
    }

    STARTUPINFOA siStartInfo;
    PROCESS_INFORMATION piProcInfo;

    ZeroMemory(&siStartInfo, sizeof(siStartInfo));
    siStartInfo.cb = sizeof(siStartInfo);
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    siStartInfo.hStdInput = hStdInput ? hStdInput : GetStdHandle(STD_INPUT_HANDLE);
    siStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    BOOL bSuccess = CreateProcessA(NULL, const_cast<char *>(cmd_line.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo,
                                   &piProcInfo);

    if (bSuccess) {
        CloseHandle(piProcInfo.hThread);
        CloseHandle(piProcInfo.hProcess);
        return piProcInfo.dwProcessId;
    } else {
        throw std::system_error(GetLastError(), std::system_category(), "CreateProcess failed");
    }
}

HANDLE ProcessManager::Get_Handle(int crt_fd) {
    HANDLE handle = (HANDLE)_get_osfhandle(crt_fd);
    if (handle == INVALID_HANDLE_VALUE) {
        throw std::system_error(errno, std::system_category(), "Failed to convert CRT file descriptor to HANDLE");
    }
    return handle;
}

void ProcessManager::Wait_for_child(pid_t proc) {
    HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, proc);
    if (hProcess != NULL) {
        WaitForSingleObject(hProcess, INFINITE);
        CloseHandle(hProcess);
    }
}

pid_t ProcessManager::Get_current_pid() { return GetCurrentProcessId(); }

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
    HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        std::cerr << "CreateFile failed with error: " << error << std::endl;
        return false;
    }

    SetFilePointer(hFile, 0, NULL, FILE_END);

    DWORD bytes_written;
    BOOL result = ::WriteFile(hFile, content.c_str(), static_cast<DWORD>(content.size()), &bytes_written, NULL);

    if (!result) {
        DWORD error = GetLastError();
        std::cerr << "WriteFile failed with error: " << error << std::endl;
        CloseHandle(hFile);
        return false;
    }

    if (bytes_written != static_cast<DWORD>(content.size())) {
        std::cerr << "Partial write occurred" << std::endl;
        CloseHandle(hFile);
        return false;
    }

    CloseHandle(hFile);
    return true;
}