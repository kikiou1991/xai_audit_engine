#ifndef LOGISTIC_REGRESSION_MODEL_HPP
#define LOGISTIC_REGRESSION_MODEL_HPP

#include "IExplainableModel.hpp"
#include <vector>
#include <cstddef>

class LogisticRegressionModel : public IExplainableModel {
private:
    std::vector<double> weights_;
    double bias_;

public:
    LogisticRegressionModel(std::vector<double> weights, double bias);

    double predict(const double* input, std::size_t num_features) const override;

    [[nodiscard]] std::size_t num_features() const noexcept { return weights_.size(); }
};

#endif // LOGISTIC_REGRESSION_MODEL_HPP
