#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

std::string searchRes() {
    auto currentPath = fs::current_path();

    auto orgiPath = currentPath;
    bool resPathFound = false, appPathFound = false;

    while (true) {
        if (!resPathFound && !appPathFound && currentPath.has_parent_path()) {
            fs::current_path(currentPath.parent_path());
            currentPath = fs::current_path();
        } else if (appPathFound) {
            try {
#ifdef _WIN_32
                auto a = currentPath.string().append("\\res");
#else
                auto a = currentPath.string().append("res");
#endif
                fs::current_path(a);
                fs::current_path(orgiPath);
                return a;
            } catch (const std::exception &e) {
                std::cout << e.what()
                          << "\n \"res\" folder not found inside "
                             "\"Application\" folder \n";
                return std::string();
            }
        } else
            break;

        // std::cout << "currently in path: " << currentPath.string() <<
        // std::endl;

        for (auto &dirs : fs::directory_iterator(currentPath)) {
            if (dirs.is_directory() &&
                dirs.path().filename().string() == "src") {
                appPathFound = true;
                break;
            }
        }
    }

    std::cout << "folder \"Application\" not found in 4 back iterations"
              << std::endl;
    return std::string();
}
