#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "ResultLogger.hpp"


// -------------------------------------------------------------
// Part A: The Data Structure
// -------------------------------------------------------------
// A struct is simply a bundle of variables.
// We provide default values so the program can run even if
// the user passes no command-line flags at all.
struct Config {
    std::string patient_path      = "data/patient.csv";
    std::string baseline_path     = "data/baseline.csv";
    std::string ranked_genes_path = "data/ranked_genes.csv";
    std::string output_path       = "output/benchmark_results.csv";
    int top_k                     = 50;
    int mc_samples                = 100;
    bool help_requested           = false;
};

// -------------------------------------------------------------
// Part B: The Help Message
// -------------------------------------------------------------
// Marked 'inline' so that if multiple .cpp files include this header,
// the compiler won't complain about "multiple definitions".
inline void print_usage(const std::string& program_name) {
    std::cout << "Usage: " << program_name << " [options]\n\n"
              << "Options:\n"
              << "  --patient <path>      Path to patient gene expression CSV (default: data/patient.csv)\n"
              << "  --baseline <path>     Path to baseline reference CSV (default: data/baseline.csv)\n"
              << "  --ranked <path>       Path to attribution-ranked genes CSV (default: data/ranked_genes.csv)\n"
              << "  --output <path>       Path to save AOPC benchmark output (default: output/benchmark_results.csv)\n"
              << "  --k <int>             Max top-k genes to perturb (default: 50)\n"
              << "  --mc <int>            Number of Monte Carlo iterations (default: 100)\n"
              << "  -h, --help            Show this help message\n";
}

// -------------------------------------------------------------
// Part C: The Parsing Loop
// -------------------------------------------------------------
inline Config parse_arguments(int argc, char* argv[]) {
    Config config;

    // Why start at i = 1?
    // argv[0] is always the name of the executable itself (e.g. "./xai_engine").
    // The actual user flags start at index 1.
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // 1. Check if the user asked for help
        if (arg == "-h" || arg == "--help") {
            config.help_requested = true;
            return config;
        }

        // 2. String parameters (Files)
        // Notice the check: (i + 1 < argc)
        // This ensures the user didn't write "--patient" at the very end
        // without providing the actual filename after it.
        else if (arg == "--patient" && i + 1 < argc) {
            // '++i' advances the loop counter to grab the next word
            config.patient_path = argv[++i];
        }
        else if (arg == "--baseline" && i + 1 < argc) {
            config.baseline_path = argv[++i];
        }
        else if (arg == "--ranked" && i + 1 < argc) {
            config.ranked_genes_path = argv[++i];
        }
        else if (arg == "--output" && i + 1 < argc) {
            config.output_path = argv[++i];
        }

        // 3. Integer parameters (Numbers)
        // std::stoi converts a string like "50" into an integer 50.
        else if (arg == "--k" && i + 1 < argc) {
            config.top_k = std::stoi(argv[++i]);
        }
        else if (arg == "--mc" && i + 1 < argc) {
            config.mc_samples = std::stoi(argv[++i]);
        }

        // 4. Fallback for typos
        else {
            std::cerr << "Warning: Unknown or incomplete argument '" << arg << "'\n";
        }
    }

    return config;
}
