#include <iostream>
#include <vector>
#include "LogisticRegressionModel.hpp"
#include "AuditingEngine.hpp"
#include "DataLoader.hpp"
#include "Config.hpp"

int main(int argc, char* argv[]) {
    // --- 1. Parse arguments ---
    Config config = parse_arguments(argc, argv);

    // If user passed -h or --help, show instructions and exit cleanly
    if (config.help_requested) {
        print_usage(argv[0]);
        return 0;
    }

    std::cout << "=== XAI Transcriptomics Auditing Engine ===\n";
    std::cout << "Patient CSV     : " << config.patient_path << "\n";
    std::cout << "Baseline CSV    : " << config.baseline_path << "\n";
    std::cout << "Ranked Genes CSV: " << config.ranked_genes_path << "\n";
    std::cout << "Output CSV      : " << config.output_path << "\n";
    std::cout << "Top K Steps     : " << config.top_k << "\n";
    std::cout << "MC Iterations   : " << config.mc_samples << "\n";
    std::cout << "===========================================\n\n";

    // 1. Define the 5-gene linear model
    std::vector<double> weights = {0.2, 0.1, 3.0, 0.1, 2.5};
    double bias = -1.0;
    LogisticRegressionModel model(weights, bias);

    try {
        // 2. Ingest patient expression, baseline, and rankings dynamically using config paths
        std::cout << "[DataLoader] Loading CSV files from disk...\n";

        std::vector<double> patient = DataLoader::load_vector_csv(config.patient_path, /*skip_header=*/true);
        std::vector<double> baseline = DataLoader::load_vector_csv(config.baseline_path, /*skip_header=*/true);
        std::vector<std::size_t> ranked_genes = DataLoader::load_indices_csv(config.ranked_genes_path, /*skip_header=*/true);

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

        // 4. Run Monte Carlo random baseline using config.mc_samples
        AuditResult random_result = AuditingEngine::audit_random_baseline(
            model,
            patient.data(),
            baseline.data(),
            patient.size(),
            ranked_genes.size(),
            config.mc_samples,             // <-- dynamically uses config.mc_samples now
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

        // 6. Build the perturbation points trajectory for export
        std::vector<PerturbationPoint> points;

        // Step 0: The baseline starting state (unperturbed)
        points.push_back({
            0,                          // step
            0,                          // gene_index (0 for unperturbed)
            result.original_score,      // shap_score
            random_result.original_score, // random_mean
            0.0                         // random_std (0 at baseline)
        });

        // Steps 1 to K
        std::size_t num_steps = std::min(result.step_scores.size(), random_result.step_scores.size());
        for (std::size_t i = 0; i < num_steps; ++i) {
            points.push_back({
                static_cast<int>(i + 1),                                    // step
                (i < ranked_genes.size()) ? ranked_genes[i] : 0,           // gene_index
                result.step_scores[i],                                      // shap_score
                random_result.step_scores[i],                               // random_mean
                0.0                                                         // random_std (plug in std dev if computed, else 0.0)
            });
        }

        // 7. Save trajectory curve to CSV
        ResultLogger::save_aopc_curve(config.output_path, points);


    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
