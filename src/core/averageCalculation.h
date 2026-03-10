#ifndef AVERCALCULATOR_H
#define AVERCALCULATOR_H

#include <vector>
#include "assessments.h"

class ICalculationStrategy {
public:
    virtual ~ICalculationStrategy() = default;
    virtual double calculate(const std::vector<double>& grades) = 0;
};

class AverageStrategy : public ICalculationStrategy {
public:
    double calculate(const std::vector<double>& grades) override;
};

class SumStrategy : public ICalculationStrategy {
public:
    double calculate(const std::vector<double>& grades) override;
};

class SingleGradeStrategy : public ICalculationStrategy {
public:
    double calculate(const std::vector<double>& grades) override;
};

class StrategyFactory {
public:
    static ICalculationStrategy* createStrategy(ScaleType type);
};

#endif