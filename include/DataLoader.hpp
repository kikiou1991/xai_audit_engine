#pragma once

#include <vector>
#include <string>
#include <cstddef>

namespace DataLoader {

    // Reads comma-separated floating-point values into a std::vector<double>.
    // If skip_header is true, the first line is discarded.
    std::vector<double> load_vector_csv(
        const std::string& filepath,
        bool skip_header = false
    );

    // Reads comma-separated non-negative integer indices into a std::vector<std::size_t>.
    // If skip_header is true, the first line is discarded.
    std::vector<std::size_t> load_indices_csv(
        const std::string& filepath,
        bool skip_header = false
    );

}
