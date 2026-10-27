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
    // 2. Audit Random Baseline (100 Monte Carlo runs)
    AuditResult random_result = AuditingEngine::audit_random_baseline(
        model,
        patient.data(),
        baseline.data(),
        patient.size(),
        ranked_genes.size(), // audit the same number of steps as SHAP
        100 // 100 random runs
    );

    std::cout << "\n--- Auditing Results ---\n";
    std::cout << "AOPC (SHAP Explainer): " << result.aopc << "\n";
    std::cout << "AOPC (Random Baseline):  " << random_result.aopc << "\n";

    double delta_aopc = result.aopc - random_result.aopc;
    std::cout << "Delta AOPC (Fidelity Gain): " << delta_aopc << "\n";

    if (delta_aopc > 0.0) {
        std::cout << "Result: [PASS] SHAP is statistically superior to random chance.\n";
    } else {
        std::cout << "Result: [FAIL] Explainer is no better than random noise.\n";
    }
    return 0;
}
