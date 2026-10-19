#include "AuditingEngine.hpp"
#include <numeric>
#include <stdexcept>

AuditResult AuditingEngine::audit_sequential_decay(
    const IExplainableModel& model,
    const double* patient_profile,
    const double* baseline_reference,
    std::size_t num_features,
    const std::vector<std::size_t>& ranked_feature_indices
) {
    if (patient_profile == nullptr || baseline_reference == nullptr) {
        throw std::invalid_argument("Input pointers cannot be null.");
    }

    AuditResult result;

    // 1. Initial prediction on the unaltered patient profile
    result.original_score = model.predict(patient_profile, num_features);

    // 2. Scratchpad buffer: allocated once, initialized with patient data
    std::vector<double> scratchpad(patient_profile, patient_profile + num_features);

    const std::size_t num_steps = ranked_feature_indices.size();
    result.step_scores.reserve(num_steps);
    result.step_drops.reserve(num_steps);

    double cumulative_drop = 0.0;

    // 3. The Perturbation Loop
    for (std::size_t step = 0; step < num_steps; ++step) {
        std::size_t gene_idx = ranked_feature_indices[step];

        if (gene_idx >= num_features) {
            throw std::out_of_range("Ranked feature index exceeds total feature dimensions.");
        }

        // Mask the feature: replace patient expression with neutral baseline
        scratchpad[gene_idx] = baseline_reference[gene_idx];

        // Evaluate model prediction on the mutated scratchpad
        double current_score = model.predict(scratchpad.data(), num_features);
        double drop = result.original_score - current_score;

        result.step_scores.push_back(current_score);
        result.step_drops.push_back(drop);
        cumulative_drop += drop;
    }

    // 4. Calculate AOPC (Area Over the Perturbation Curve)
    result.aopc = (num_steps > 0) ? (cumulative_drop / static_cast<double>(num_steps)) : 0.0;

    return result;
}
