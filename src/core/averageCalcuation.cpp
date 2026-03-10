#include "averageCalculation.h"

double AverageStrategy::calculate(const std::vector<double>& grades) {
    if (grades.empty()) return 0.0;
    double sum = 0;
    for (double g : grades) sum += g;
    return sum / grades.size();
}

double SumStrategy::calculate(const std::vector<double>& grades) {
    double sum = 0;
    for (double g : grades) sum += g;
    return sum;
}

double SingleGradeStrategy::calculate(const std::vector<double>& grades) {
    return grades.empty() ? 0.0 : grades.back();
}

ICalculationStrategy* StrategyFactory::createStrategy(ScaleType type) {
    if (type == ScaleType::Accumulative) {
        return new SumStrategy();
    }
    return new AverageStrategy();
}