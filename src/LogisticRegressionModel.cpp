#include "LogisticRegressionModel.hpp"
#include <cmath>
#include <stdexcept>
#include <utility>

LogisticRegressionModel::LogisticRegressionModel(std::vector<double> weights, double bias)
    : weights_(std::move(weights)), bias_(bias) {}

double LogisticRegressionModel::predict(const double* input, std::size_t num_features) const {
    if (num_features != weights_.size()) {
        throw std::invalid_argument("Input dimension does not match model weights dimension.");
    }

    double z = bias_;
    for (std::size_t i = 0; i < num_features; ++i) {
        z += weights_[i] * input[i];
    }

    // Numerically stable sigmoid: prevents exp() overflow when z is negative
    if (z >= 0.0) {
        return 1.0 / (1.0 + std::exp(-z));
    } else {
        double exp_z = std::exp(z);
        return exp_z / (1.0 + exp_z);
    }
}
