#ifndef AUDITING_ENGINE_HPP
#define AUDITING_ENGINE_HPP

#include "IExplainableModel.hpp"
#include <vector>
#include <cstddef>

namespace AuditConfig {
    // Compile-time defaults: embedded directly in the binary instructions
    constexpr std::size_t DEFAULT_MONTE_CARLO_RUNS = 100;
    constexpr unsigned int DEFAULT_SEED = 42;
}


struct AuditResult {
    double original_score;
    std::vector<double> step_scores;     // Prediction after each masked gene
    std::vector<double> step_drops;      // original_score - step_scores[k]
    double aopc;                         // Mean drop across all steps
};

class AuditingEngine {
public:
    // Sequential perturbation using a specific ranked order (e.g. SHAP)
    static AuditResult audit_sequential_decay(
        const IExplainableModel& model,
        const double* patient_profile,
        const double* baseline_reference,
        std::size_t num_features,
        const std::vector<std::size_t>& ranked_feature_indices
    );

    // Monte Carlo random baseline: runs 'num_runs' random permutations
    // and returns the averaged AuditResult (the null hypothesis)
    static AuditResult audit_random_baseline(
        const IExplainableModel& model,
        const double* patient_profile,
        const double* baseline_reference,
        std::size_t num_features,
        std::size_t num_steps,
        std::size_t num_runs = AuditConfig::DEFAULT_MONTE_CARLO_RUNS,
        unsigned int seed = AuditConfig::DEFAULT_SEED
    );
};

#endif // AUDITING_ENGINE_HPP
