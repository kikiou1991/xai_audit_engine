#include "DataLoader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace DataLoader {

std::vector<double> load_vector_csv(const std::string& filepath, bool skip_header) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("DataLoader: Unable to open file at " + filepath);
    }

    std::string line;

    // Discard header line if present (e.g., gene symbols / feature names)
    if (skip_header && !std::getline(file, line)) {
        throw std::runtime_error("DataLoader: File is empty or missing data row after header at " + filepath);
    }

    // Read the expression row
    if (!std::getline(file, line)) {
        throw std::runtime_error("DataLoader: Empty file or no data row found at " + filepath);
    }

    std::vector<double> values;
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, ',')) {
        // Strip carriage returns if file originated on Windows (\r\n)
        if (!token.empty() && token.back() == '\r') {
            token.pop_back();
        }

        // Ignore empty tokens (e.g. trailing commas)
        if (token.empty()) {
            continue;
        }

        try {
            values.push_back(std::stod(token));
        } catch (const std::exception& e) {
            throw std::runtime_error("DataLoader: Failed to parse float token '" + token + "' in " + filepath);
        }
    }

    if (values.empty()) {
        throw std::runtime_error("DataLoader: No numeric values could be extracted from " + filepath);
    }

    return values;
}

std::vector<std::size_t> load_indices_csv(const std::string& filepath, bool skip_header) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("DataLoader: Unable to open file at " + filepath);
    }

    std::string line;

    if (skip_header && !std::getline(file, line)) {
        throw std::runtime_error("DataLoader: File is empty or missing data row after header at " + filepath);
    }

    if (!std::getline(file, line)) {
        throw std::runtime_error("DataLoader: Empty file or no data row found at " + filepath);
    }

    std::vector<std::size_t> indices;
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, ',')) {
        if (!token.empty() && token.back() == '\r') {
            token.pop_back();
        }

        if (token.empty()) {
            continue;
        }

        try {
            unsigned long idx = std::stoul(token);
            indices.push_back(static_cast<std::size_t>(idx));
        } catch (const std::exception& e) {
            throw std::runtime_error("DataLoader: Failed to parse index token '" + token + "' in " + filepath);
        }
    }

    if (indices.empty()) {
        throw std::runtime_error("DataLoader: No indices could be extracted from " + filepath);
    }

    return indices;
}

} // namespace DataLoader
