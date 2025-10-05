#include <system_error>
#ifdef _WIN32
#include "Win_ProcessManager.h"
#elif __linux__
#include "ProcessManager.h"
#endif


int main(int argc, char *argv[]) {
    try {
        std::cout << "Child1 started with id: " << ProcessManager::Get_current_pid() << '\n' << std::endl;
        if (argc != 2) {
            throw std::invalid_argument("Usage: " + std::string(argv[0]) + " <filename>");
        }
        std::string output_filename = argv[1];

        while (true) {
            size_t length_str;
            int bytes_read = read(0, &length_str, sizeof(length_str));

            if (bytes_read == 0) {
                break;
            }
            if (bytes_read == -1) {
                throw std::invalid_argument("Error reading length from stdin");
            }

            std::string received_str;
            received_str.resize(length_str);
            bytes_read = read(0, &received_str[0], length_str);

            if (bytes_read == -1) {
                throw std::invalid_argument("Error reading length from stdin");
            }
            if (bytes_read != static_cast<int>(length_str)) {
                throw std::invalid_argument("Partial read occurred");
            }

            std::string result_str = RemoveVowels(received_str);
            if (!WriteFile(output_filename, result_str + "\n")) {
                throw std::system_error(errno, std::system_category(), "Error writing to file: " + output_filename);
            }
        }

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}