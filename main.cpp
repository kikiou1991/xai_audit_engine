#include <iostream>
#include <vector>
#include "LogisticRegressionModel.hpp"
#include "AuditingEngine.hpp"
#include "DataLoader.hpp"

int main() {
    std::cout << "=== XAI Transcriptomics Auditing Engine ===\n\n";

    // 1. Define the 5-gene linear model
    // Gene 2 (weight 3.0) and Gene 4 (weight 2.5) are dominant cancer drivers
    std::vector<double> weights = {0.2, 0.1, 3.0, 0.1, 2.5};
    double bias = -1.0;
    LogisticRegressionModel model(weights, bias);

    try {
        // 2. Ingest patient expression, baseline, and rankings dynamically
        std::cout << "[DataLoader] Loading CSV files from disk...\n";

        std::vector<double> patient = DataLoader::load_vector_csv("data/patient.csv", /*skip_header=*/true);
        std::vector<double> baseline = DataLoader::load_vector_csv("data/baseline.csv", /*skip_header=*/true);
        std::vector<std::size_t> ranked_genes = DataLoader::load_indices_csv("data/ranked_genes.csv", /*skip_header=*/true);

        // Validation guard
        if (patient.size() != baseline.size()) {
            throw std::runtime_error("Dimension mismatch: patient has " +
                std::to_string(patient.size()) + " genes, baseline has " +
                std::to_string(baseline.size()) + " genes.");
        }

        std::cout << "[DataLoader] Successfully loaded " << patient.size()
                  << " genes and " << ranked_genes.size() << " ranked attribution steps.\n\n";

        // 3. Run the sequential perturbation audit
        AuditResult result = AuditingEngine::audit_sequential_decay(
            model,
            patient.data(),
            baseline.data(),
            patient.size(),
            ranked_genes
        );

        // 4. Run Monte Carlo random baseline
        AuditResult random_result = AuditingEngine::audit_random_baseline(
            model,
            patient.data(),
            baseline.data(),
            patient.size(),
            ranked_genes.size(),
            AuditConfig::DEFAULT_MONTE_CARLO_RUNS,
            AuditConfig::DEFAULT_SEED
        );

        // 5. Output comparison
        std::cout << "--- Auditing Results ---\n";
        std::cout << "Original Prediction Score : " << result.original_score << "\n";
        std::cout << "AOPC (SHAP Explainer)     : " << result.aopc << "\n";
        std::cout << "AOPC (Random Baseline)    : " << random_result.aopc << "\n";

        double delta_aopc = result.aopc - random_result.aopc;
        std::cout << "Delta AOPC (Fidelity Gain): " << delta_aopc << "\n";

        if (delta_aopc > 0.0) {
            std::cout << "Result: [PASS] SHAP is statistically superior to random chance.\n";
        } else {
            std::cout << "Result: [FAIL] Explainer is no better than random noise.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
