#include <gtest/gtest.h>
#include "subject.h"
#include "assessments.h"
#include "averageCalculation.h"
#include "PriorityManager.h" // У тебе він написаний з великої літери

// ==========================================
// 1. ТЕСТИ СТРАТЕГІЙ (Математика)
// ==========================================
TEST(BusinessLogicTest, AccumulativeStrategyCalculatesSum) {
    SumStrategy sumStrategy;
    std::vector<double> grades = {15.0, 25.0, 10.0};
    EXPECT_DOUBLE_EQ(sumStrategy.calculate(grades), 50.0);
}

TEST(BusinessLogicTest, AverageStrategyCalculatesMean) {
    AverageStrategy avgStrategy;
    std::vector<double> grades = {10.0, 11.0, 12.0};
    EXPECT_DOUBLE_EQ(avgStrategy.calculate(grades), 11.0);
}

// ==========================================
// 2. ТЕСТИ ФАБРИКИ (Продовження)
// ==========================================
TEST(BusinessLogicTest, FactoryCreatesRegularAsNonBlockerWithLowPriority) {
    Assessments* regular = AssessmentFactory::createRegular(ScaleType::TenPoint);
    EXPECT_FALSE(regular->getIsBlocker());
    EXPECT_EQ(regular->getBasePriority(), 50); // Твоя цифра: 50
    delete regular;
}

// ==========================================
// 3. ТЕСТИ ОЦІНОК (Накопичення)
// ==========================================
TEST(BusinessLogicTest, TaskAccumulatesGradesCorrectly) {
    Assessments* task = AssessmentFactory::createRegular(ScaleType::Accumulative);
    task->addGrade(10.5);
    task->addGrade(20.0);
    EXPECT_DOUBLE_EQ(task->getCurrentScore(), 30.5);
    delete task;
}

// ==========================================
// 4. ТЕСТИ PRIORITY MANAGER (Головна логіка пріоритетів)
// ==========================================
TEST(BusinessLogicTest, PriorityManagerNoPenaltyForGoodGrades) {
    Subject subject("Math", 1, false);
    Assessments* exam = AssessmentFactory::createExam(ScaleType::TwelvePoint);
    exam->addGrade(8.0); // Хороша оцінка
    subject.addAssessment(exam);

    PriorityManager pm;
    pm.update(&subject);
    
    // Екзамен без штрафу (базовий 1000)
    EXPECT_EQ(pm.getPriorityForSubject(&subject), 1000); 
}

TEST(BusinessLogicTest, PriorityManagerAppliesPenaltyForBadTwelvePointGrade) {
    Subject subject("Physics", 1, false);
    Assessments* task = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    task->addGrade(3.0); // Менше 4.0 - це ПОГАНА оцінка
    subject.addAssessment(task);

    PriorityManager pm;
    pm.update(&subject);
    
    // Твої цифри: базова (50) + штраф (2000) = 2050
    EXPECT_EQ(pm.getPriorityForSubject(&subject), 2050); 
}

TEST(BusinessLogicTest, PriorityManagerAppliesPenaltyForBadAccumulativeGrade) {
    Subject subject("History", 1, false);
    Assessments* task = AssessmentFactory::createRegular(ScaleType::Accumulative);
    task->addGrade(59.0); // Менше 60.0 - це ПОГАНА оцінка
    subject.addAssessment(task);

    PriorityManager pm;
    pm.update(&subject);
    
    // Твої цифри: базова (50) + штраф (2000) = 2050
    EXPECT_EQ(pm.getPriorityForSubject(&subject), 2050); 
}

// ==========================================
// 5. ТЕСТИ SUBJECT (Загальний стан предмета)
// ==========================================
TEST(BusinessLogicTest, SubjectIsNotCompletedIfBlockerFailed) {
    Subject subject("Programming", 1, false);
    Assessments* exam = AssessmentFactory::createExam(ScaleType::Accumulative);
    exam->addGrade(50.0); // Не складено
    subject.addAssessment(exam);

    // Якщо екзамен не складено, блокувальники Є (TRUE)
    EXPECT_TRUE(subject.hasPendingBlockers());
}

TEST(BusinessLogicTest, SubjectIsCompletedIfBlockerPassed) {
    Subject subject("Databases", 2, false);
    Assessments* exam = AssessmentFactory::createExam(ScaleType::Accumulative);
    exam->addGrade(90.0); // Складено успішно
    subject.addAssessment(exam);

    // Якщо екзамен складено, блокувальників НЕМАЄ (FALSE)
    EXPECT_FALSE(subject.hasPendingBlockers());
}