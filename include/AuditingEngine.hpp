#ifndef AUDITING_ENGINE_HPP
#define AUDITING_ENGINE_HPP

#include "IExplainableModel.hpp"
#include <vector>
#include <cstddef>

struct AuditResult {
    double original_score;
    std::vector<double> step_scores;     // Prediction after each masked gene
    std::vector<double> step_drops;      // original_score - step_scores[k]
    double aopc;                         // Mean drop across all steps (Area Over Perturbation Curve)
};

class AuditingEngine {
public:
    static AuditResult audit_sequential_decay(
        const IExplainableModel& model,
        const double* patient_profile,
        const double* baseline_reference,
        std::size_t num_features,
        const std::vector<std::size_t>& ranked_feature_indices
    );
};

#endif // AUDITING_ENGINE_HPP
