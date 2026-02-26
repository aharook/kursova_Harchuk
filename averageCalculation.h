#ifndef AVERCALCULATOR_H
#define AVERCALCULATOR_H

#include <vector>
#include "assessments.h"

class ICalculationStrategy {
public:
    virtual ~ICalculationStrategy() = default;
    virtual double calculate(const std::vector<double>& grades) = 0;
};

// 1. Для регулярних (12/5 балів) - Середнє арифметичне
class AverageStrategy : public ICalculationStrategy {
public:
    double calculate(const std::vector<double>& grades) override;
};

// 2. Для регулярних (100 балів) - Сума всіх оцінок
class SumStrategy : public ICalculationStrategy {
public:
    double calculate(const std::vector<double>& grades) override;
};

// 3. НОВЕ: Для Екзаменів/Курсових - просто повертає останню (єдину) оцінку
class SingleGradeStrategy : public ICalculationStrategy {
public:
    double calculate(const std::vector<double>& grades) override;
};

class StrategyFactory {
public:
    static ICalculationStrategy* createStrategy(ScaleType type);
};

#endif