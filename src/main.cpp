#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <vector>

std::vector<std::filesystem::path> find_executables() {
    std::vector<std::filesystem::path> executables;

    for (const auto &entry :
         std::filesystem::recursive_directory_iterator("build")) {

        if (!entry.is_regular_file()) {
            continue;
        }

        auto path = entry.path().string();

        if (path.find("meson-private") != std::string::npos ||
            path.find("CMakeFiles") != std::string::npos) {
            continue;
        }

        auto perms = entry.status().permissions();

        bool executable = (perms & std::filesystem::perms::owner_exec) !=
                              std::filesystem::perms::none ||
                          (perms & std::filesystem::perms::group_exec) !=
                              std::filesystem::perms::none ||
                          (perms & std::filesystem::perms::others_exec) !=
                              std::filesystem::perms::none;

        if (executable) {
            executables.push_back(entry.path());
        }
    }
    return executables;
}

int main() {
    auto current = std::filesystem::current_path();

    std::cout << "Current directory: " << current << '\n';

    if (std::filesystem::exists("meson.build")) {
        std::cout << "Meson project detected.\n";

        if (!std::filesystem::exists("build")) {
            std::cout << "Setting up Meson build directory...\n";

            if (std::system("meson setup build") != 0) {
                std::cerr << "Meson setup failed.\n";
                return 1;
            }
        }

        if (std::system("meson compile -C build") != 0) {
            std::cerr << "Meson compile failed.\n";
            return 1;
        }

    } else if (std::filesystem::exists("CMakeLists.txt")) {
        std::cout << "CMake project detected.\n";

        if (!std::filesystem::exists("build")) {
            std::cout << "Setting up CMake build directory...\n";

            if (std::system("cmake -S . -B build") != 0) {
                std::cerr << "CMake setup failed.\n";
                return 1;
            }
        }

        if (std::system("cmake --build build") != 0) {
            std::cerr << "CMake build failed.\n";
            return 1;
        }

    } else {
        std::cerr << "No supported build system found.\n";
        return 1;
    }

    auto executables = find_executables();

    for (const auto &exe : executables) {
        std::cout << "Found executable: " << exe << '\n';
    }

    if (executables.empty()) {
        std::cerr << "No executable found.\n";
        return 1;
    }

    if (executables.size() > 1) {
        std::cerr << "Multiple executables found.\n";
        return 1;
    }

    std::string command = executables[0].string();

    std::cout << "Running: " << command << '\n';

    return std::system(command.c_str());
}
