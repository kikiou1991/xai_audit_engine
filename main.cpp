#include <iostream>
#include <vector>
#include <iomanip>
#include "LogisticRegressionModel.hpp"
#include "AuditingEngine.hpp"

int main() {
    std::cout << "=== XAI Transcriptomics Auditing Engine ===\n\n";

    // 1. Define a 5-gene linear model
    // Gene 2 (weight 3.0) and Gene 4 (weight 2.5) are the dominant cancer drivers.
    //              Gene:    0    1    2    3    4
    std::vector<double> weights = {0.2, 0.1, 3.0, 0.1, 2.5};
    double bias = -1.0;

    LogisticRegressionModel model(weights, bias);

    // 2. Patient profile: high expression (1.0) across all 5 genes
    std::vector<double> patient = {1.0, 1.0, 1.0, 1.0, 1.0};

    // 3. Healthy Baseline reference: low/zero expression (0.0) across all 5 genes
    std::vector<double> baseline = {0.0, 0.0, 0.0, 0.0, 0.0};

    // 4. Feature importance ranking (e.g., from SHAP: Gene 2 is #1, Gene 4 is #2)
    std::vector<std::size_t> ranked_genes = {2, 4, 0, 1, 3};

    // 5. Run the sequential perturbation audit
    AuditResult result = AuditingEngine::audit_sequential_decay(
        model,
        patient.data(),
        baseline.data(),
        patient.size(),
        ranked_genes
    );

    // 6. Print the formatted auditing report
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Original Patient Cancer Probability: " << result.original_score << "\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << "Step | Masked Gene | Decayed Score | Drop (Comprehensiveness)\n";
    std::cout << "--------------------------------------------------------\n";

    for (std::size_t i = 0; i < ranked_genes.size(); ++i) {
        std::cout << std::setw(4) << (i + 1) << " | "
                  << "Gene " << std::setw(6) << ranked_genes[i] << " | "
                  << std::setw(13) << result.step_scores[i] << " | "
                  << std::setw(10) << result.step_drops[i] << "\n";
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "Final AOPC (Area Over Perturbation Curve): " << result.aopc << "\n";

    return 0;
}
