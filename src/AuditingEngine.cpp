#include "AuditingEngine.hpp"
#include <numeric>
#include <stdexcept>
#include <random>
#include <algorithm>


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

AuditResult AuditingEngine::audit_random_baseline(
    const IExplainableModel& model,
    const double *patient_profile,
    const double* baseline_reference,
    std::size_t num_features,
    std::size_t num_steps,
    std::size_t num_runs,
    unsigned int seed)

{
    if (patient_profile == nullptr || baseline_reference == nullptr) {
        throw std::invalid_argument("Input pointers cannot be null.");
    }
    if (num_steps > num_features) {
        throw std::out_of_range("Number of steps exceeds total feature dimensions.");
    }
    if (num_runs == 0) {
        throw std::invalid_argument("num_runs must be at least 1");
    }

    // 1. Create the "hat": a vector containing[0,1,2...,num_feautres -1]
    std::vector<std::size_t> indices(num_features);
    std:iota(indices.begin(), indices.end(), 0);

    // 2. The Container where we collect our running results
    AuditResult aggregated_result;
    aggregated_result.original_score = model.predict(patient_profile, num_features);
    aggregated_result.step_scores.assign(num_steps, 0.0);
    aggregated_result.step_drops.assign(num_steps, 0.0);
    aggregated_result.aopc = 0.0;

    // 3. Initilise the sudo-num generator
    std::mt19937 rng(seed);


    // 4. Monte Carlo runs
    for (std::size_t run = 0; run < num_runs; ++run) {

        // Your partial Fisher-Yates swap:
        // We only swap 'num_steps' items directly inside 'indices'
        for (std::size_t i = 0; i < num_steps; ++i) {
            std::uniform_int_distribution<std::size_t> dist(i, num_features - 1);
            std::size_t j = dist(rng);
            std::swap(indices[i], indices[j]);
        }

        // Now indices[0 ... num_steps - 1] hold our random draw!

        // 1. Slice out our random draw of size num_steps
        std::vector<std::size_t> random_order(indices.begin(), indices.begin() + num_steps);

        // 2. Run the audit on this random order
        AuditResult single_run = audit_sequential_decay(
            model,
            patient_profile,
            baseline_reference,
            num_features,
            random_order
        );
        // 3. Accumulate scores across steps
        for (std::size_t s = 0; s < num_steps; ++s) {
            aggregated_result.step_scores[s] += single_run.step_scores[s];
            aggregated_result.step_drops[s]  += single_run.step_drops[s];
        }
        aggregated_result.aopc += single_run.aopc;


    }
    // 5. Divide accumulated sums by num_runs to compute the arithmetic mean
    const double inv_runs = 1.0 / static_cast<double>(num_runs);

    for (std::size_t s = 0; s < num_steps; ++s) {
        aggregated_result.step_scores[s] *= inv_runs;
        aggregated_result.step_drops[s]  *= inv_runs;
    }
    aggregated_result.aopc *= inv_runs;



    return aggregated_result;
}
