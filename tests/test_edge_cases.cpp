#include <gtest/gtest.h>
#include "subject.h"
#include "assessments.h"
#include "averageCalculation.h"
#include "PriorityManager.h"


TEST(EdgeCasesTest, SumStrategyEmptyGradesReturnsZero) {
    SumStrategy sum;
    std::vector<double> empty;
    EXPECT_DOUBLE_EQ(sum.calculate(empty), 0.0);
}

TEST(EdgeCasesTest, AverageStrategyEmptyGradesReturnsZero) {
    AverageStrategy avg;
    std::vector<double> empty;
    EXPECT_DOUBLE_EQ(avg.calculate(empty), 0.0); 
}

TEST(EdgeCasesTest, EmptySubjectHasNoPendingBlockers) {
    Subject subject("Ghost Course", 1, false);
    EXPECT_FALSE(subject.hasPendingBlockers());
}

TEST(EdgeCasesTest, PriorityManagerHandlesEmptySubject) {
    Subject subject("Ghost Course", 1, false);
    PriorityManager pm;
    pm.update(&subject);
    EXPECT_EQ(pm.getPriorityForSubject(&subject), 0); 
}


TEST(EdgeCasesTest, AccumulativePassingBoundaryExactNoPenalty) {
    Subject subject("Test", 1, false);
    Assessments* task = AssessmentFactory::createRegular(ScaleType::Accumulative);
    task->addGrade(60.0);
    subject.addAssessment(task);
    
    PriorityManager pm;
    pm.update(&subject);
    EXPECT_EQ(pm.getPriorityForSubject(&subject), 50); 
}

TEST(EdgeCasesTest, AccumulativePassingBoundaryJustBelowPenalty) {
    Subject subject("Test", 1, false);
    Assessments* task = AssessmentFactory::createRegular(ScaleType::Accumulative);
    task->addGrade(59.9); 
    subject.addAssessment(task);
    
    PriorityManager pm;
    pm.update(&subject);
    EXPECT_EQ(pm.getPriorityForSubject(&subject), 2050); 
}

TEST(EdgeCasesTest, TwelvePointPassingBoundaryExactNoPenalty) {
    Subject subject("Test", 1, false);
    Assessments* task = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    task->addGrade(4.0);
    subject.addAssessment(task);
    
    PriorityManager pm;
    pm.update(&subject);

    EXPECT_EQ(pm.getPriorityForSubject(&subject), 50); 
}

TEST(EdgeCasesTest, TwelvePointPassingBoundaryJustBelowPenalty) {
    Subject subject("Test", 1, false);
    Assessments* task = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    task->addGrade(3.9);
    subject.addAssessment(task);
    
    PriorityManager pm;
    pm.update(&subject);
    EXPECT_EQ(pm.getPriorityForSubject(&subject), 2050); 
}


TEST(EdgeCasesTest, MultipleBlockersOneFailedMeansPendingIsTrue) {
    Subject subject("Hardcore Course", 2, false);
    
    Assessments* exam1 = AssessmentFactory::createExam(ScaleType::Accumulative);
    exam1->addGrade(90.0); 
    
    Assessments* exam2 = AssessmentFactory::createExam(ScaleType::Accumulative);
    exam2->addGrade(50.0);
    
    subject.addAssessment(exam1);
    subject.addAssessment(exam2);
    
    EXPECT_TRUE(subject.hasPendingBlockers());
}

TEST(EdgeCasesTest, MultipleBlockersAllPassedMeansPendingIsFalse) {
    Subject subject("Hardcore Course", 2, false);
    
    Assessments* exam1 = AssessmentFactory::createExam(ScaleType::Accumulative);
    exam1->addGrade(80.0);
    
    Assessments* exam2 = AssessmentFactory::createExam(ScaleType::Accumulative);
    exam2->addGrade(60.0); 
    
    subject.addAssessment(exam1);
    subject.addAssessment(exam2);
    
    EXPECT_FALSE(subject.hasPendingBlockers());
}