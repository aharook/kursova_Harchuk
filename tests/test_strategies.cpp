#include <gtest/gtest.h>
#include "averageCalculation.h"
#include <vector>

TEST(StrategyTest, AccumulativeStrategySumsCorrectly) {
    SumStrategy sumStrategy; // Або SumStrategy, залежно як ти його назвав
    std::vector<double> grades = {10.5, 20.0, 30.5};
    EXPECT_DOUBLE_EQ(sumStrategy.calculate(grades), 61.0);
}

TEST(StrategyTest, AverageStrategyCalculatesMeanCorrectly) {
    AverageStrategy avgStrategy;
    std::vector<double> grades = {10.0, 12.0, 8.0};
    // (10 + 12 + 8) / 3 = 10.0
    EXPECT_DOUBLE_EQ(avgStrategy.calculate(grades), 10.0);
}

TEST(StrategyTest, EmptyGradesReturnZero) {
    SumStrategy sumStrategy;
    AverageStrategy avgStrategy;
    std::vector<double> emptyGrades = {};
    
    EXPECT_DOUBLE_EQ(sumStrategy.calculate(emptyGrades), 0.0);
    EXPECT_DOUBLE_EQ(avgStrategy.calculate(emptyGrades), 0.0);
}

TEST(StrategyTest, NegativeGradesHandling) {
    SumStrategy strategy;
    std::vector<double> grades = {50.0, -20.0, 10.0};
    
    // Залежно від твоєї реалізації, вона або відніме, або проігнорує.
    // Якщо просто сумує: 50 - 20 + 10 = 40
    EXPECT_DOUBLE_EQ(strategy.calculate(grades), 40.0);
}