#include <gtest/gtest.h>
#include "subject.h"
#include "assessments.h"
#include "averageCalculation.h"
#include "PriorityManager.h"


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

TEST(BusinessLogicTest, FactoryCreatesRegularAsNonBlockerWithLowPriority) {
    Assessments* regular = AssessmentFactory::createRegular(ScaleType::TenPoint);
    EXPECT_FALSE(regular->getIsBlocker());
    EXPECT_EQ(regular->getBasePriority(), 50);
    delete regular;
}


TEST(BusinessLogicTest, TaskAccumulatesGradesCorrectly) {
    Assessments* task = AssessmentFactory::createRegular(ScaleType::Accumulative);
    task->addGrade(10.5);
    task->addGrade(20.0);
    EXPECT_DOUBLE_EQ(task->getCurrentScore(), 30.5);
    delete task;
}

TEST(BusinessLogicTest, PriorityManagerNoPenaltyForGoodGrades) {
    Subject subject("Math", 1, false);
    Assessments* exam = AssessmentFactory::createExam(ScaleType::TwelvePoint);
    exam->addGrade(8.0);
    subject.addAssessment(exam);

    PriorityManager pm;
    pm.update(&subject);
    
    EXPECT_EQ(pm.getPriorityForSubject(&subject), 1000); 
}

TEST(BusinessLogicTest, PriorityManagerAppliesPenaltyForBadTwelvePointGrade) {
    Subject subject("Physics", 1, false);
    Assessments* task = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    task->addGrade(3.0);
    subject.addAssessment(task);

    PriorityManager pm;
    pm.update(&subject);

    EXPECT_EQ(pm.getPriorityForSubject(&subject), 2050); 
}

TEST(BusinessLogicTest, PriorityManagerAppliesPenaltyForBadAccumulativeGrade) {
    Subject subject("History", 1, false);
    Assessments* task = AssessmentFactory::createRegular(ScaleType::Accumulative);
    task->addGrade(59.0);
    subject.addAssessment(task);

    PriorityManager pm;
    pm.update(&subject);

    EXPECT_EQ(pm.getPriorityForSubject(&subject), 2050); 
}


TEST(BusinessLogicTest, SubjectIsNotCompletedIfBlockerFailed) {
    Subject subject("Programming", 1, false);
    Assessments* exam = AssessmentFactory::createExam(ScaleType::Accumulative);
    exam->addGrade(50.0);
    subject.addAssessment(exam);
    EXPECT_TRUE(subject.hasPendingBlockers());
}

TEST(BusinessLogicTest, SubjectIsCompletedIfBlockerPassed) {
    Subject subject("Databases", 2, false);
    Assessments* exam = AssessmentFactory::createExam(ScaleType::Accumulative);
    exam->addGrade(90.0); 
    subject.addAssessment(exam);

    EXPECT_FALSE(subject.hasPendingBlockers());
}