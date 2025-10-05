#ifdef _WIN32
#include "Win_ProcessManager.h"
#elif __linux__
#include "ProcessManager.h"
#endif


int main() {
    try {
        std::cout << "Parent started with id: " << ProcessManager::Get_current_pid() << '\n' << std::endl;
        std::string filename1, filename2;
        std::cout << "Hello from ProgramLabOSVar19v1! \nFirst, enter the name of the first file! \nRemember, name can only "
                     "consist of latin letters, numbers, and a symbol _!\n"
                  << std::endl;
        std::getline(std::cin, filename1);
        if (!IsFileNameGood(filename1)) {
            throw std::invalid_argument("String must consist only 'a'-'Z' && '0'-'9' && '_'!");
        }
        std::cout << "Then enter the name of the second file! The rules are same!\n" << std::endl;
        std::getline(std::cin, filename2);
        if (!IsFileNameGood(filename2)) {
            throw std::invalid_argument("String must consist only 'a'-'Z' && '0'-'9' && '_'!");
        }

        Pipe pip_child1, pip_child2;
#ifdef _WIN32
        char *child1_argv[] = {const_cast<char *>("./child1.exe"), const_cast<char *>(filename1.c_str()), nullptr};
        char *child2_argv[] = {const_cast<char *>("./child2.exe"), const_cast<char *>(filename2.c_str()), nullptr};
#elif __linux__
        char *child1_argv[] = {const_cast<char *>("./child1"), const_cast<char *>(filename1.c_str()), nullptr};
        char *child2_argv[] = {const_cast<char *>("./child2"), const_cast<char *>(filename2.c_str()), nullptr};
#endif

        pid_t pid1, pid2;
#ifdef _WIN32
        HANDLE hPipe1Read = ProcessManager::Get_Handle(pip_child1.Read_fd());
        HANDLE hPipe2Read = ProcessManager::Get_Handle(pip_child2.Read_fd());
        pid1 = ProcessManager::ChangeProcess("./child1.exe", child1_argv, nullptr, hPipe1Read);
        pid2 = ProcessManager::ChangeProcess("./child2.exe", child2_argv, nullptr, hPipe2Read);

#else
        pid1 = ProcessManager::Create_process();
        if (pid1 == 0) {
            pip_child1.Close_write();
            pip_child2.Close_read();
            pip_child2.Close_write();

            ProcessManager::Dup2(pip_child1.Read_fd(), 0);
            pip_child1.Close_read();
            ProcessManager::ChangeProcess("./child1", child1_argv, nullptr);
        }

        pid2 = ProcessManager::Create_process();
        if (pid2 == 0) {
            pip_child2.Close_write();
            pip_child1.Close_read();
            pip_child1.Close_write();

            ProcessManager::Dup2(pip_child2.Read_fd(), 0);
            pip_child2.Close_read();
            ProcessManager::ChangeProcess("./child2", child2_argv, nullptr);
        }
#endif

        pip_child1.Close_read();
        pip_child2.Close_read();

        std::cout << "Now you can enter lines that are 80 procent likely to end up in the first file and 20 procent likely "
                     "to end up in the second file!\n"
                  << std::endl;
        std::cout << "Enter will ending when you enter STOP" << std::endl;

        srand(time(NULL) + ProcessManager::Get_current_pid());

        std::string current_string;
        std::getline(std::cin, current_string);
        while (current_string != "STOP") {
            int res = rand() % 100;
            if (res < 80) {
                size_t length_str = current_string.size();
                int bytes_written = write(pip_child1.Write_fd(), &length_str, sizeof(length_str));
                if (bytes_written == -1) {
                    throw std::system_error(errno, std::system_category(), "Error writing length to pipe1");
                }
                bytes_written = write(pip_child1.Write_fd(), current_string.c_str(), length_str);
                if (bytes_written == -1) {
                    throw std::system_error(errno, std::system_category(), "Error writing string to pipe1");
                }
            } else {
                size_t length_str = current_string.size();
                int bytes_written = write(pip_child2.Write_fd(), &length_str, sizeof(length_str));
                if (bytes_written == -1) {
                    throw std::system_error(errno, std::system_category(), "Error writing length to pipe2");
                }
                bytes_written = write(pip_child2.Write_fd(), current_string.c_str(), length_str);
                if (bytes_written == -1) {
                    throw std::system_error(errno, std::system_category(), "Error writing string to pipe2");
                }
            }
            std::getline(std::cin, current_string);
        }

        pip_child1.Close_write();
        pip_child2.Close_write();

        ProcessManager::Wait_for_child(pid1);
        ProcessManager::Wait_for_child(pid2);

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}