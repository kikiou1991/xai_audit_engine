#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>

struct PerturbationPoint {
    int step;                   // k = 0, 1, 2, ...
    std::size_t gene_index;     // index of gene perturbed at this step (0 for step 0)
    double shap_score;          // model output after replacing top-k SHAP genes
    double random_mean;         // average score across MC random runs at step k
    double random_std;          // standard deviation across MC random runs at step k
};

class ResultLogger {
public:
    static bool save_aopc_curve(const std::string& filepath,
                                const std::vector<PerturbationPoint>& points) {
        std::ofstream file(filepath);

        if (!file.is_open()) {
            std::cerr << "[ResultLogger] Error: Could not open output path: " << filepath << "\n";
            return false;
        }

        // CSV Header
        file << "step,gene_index,shap_score,random_mean,random_std\n";

        // Fix precision to 6 decimal places for clean floating point data
        file << std::fixed << std::setprecision(6);

        for (const auto& pt : points) {
            file << pt.step << ","
                 << pt.gene_index << ","
                 << pt.shap_score << ","
                 << pt.random_mean << ","
                 << pt.random_std << "\n";
        }

        file.close();
        std::cout << "[ResultLogger] Successfully exported " << points.size()
                  << " trajectory steps to: " << filepath << "\n";
        return true;
    }
};
