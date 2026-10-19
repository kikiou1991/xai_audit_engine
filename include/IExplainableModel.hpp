#ifndef I_EXPLAINABLE_MODEL_HPP
#define I_EXPLAINABLE_MODEL_HPP

#include <cstddef>

class IExplainableModel {
public:
    virtual ~IExplainableModel() = default;

    virtual double predict(const double* input, std::size_t num_features) const = 0;
};

#endif // I_EXPLAINABLE_MODEL_HPP
