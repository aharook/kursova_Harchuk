#include <gtest/gtest.h>
#include "subject.h"
#include "assessments.h"
#include "PriorityManager.h"
#include "averageCalculation.h"

TEST(GeneralErrorsTest, TaskInitializesWithZeroScore) {
    Assessments* task = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    EXPECT_DOUBLE_EQ(task->getCurrentScore(), 0.0);
    delete task;
}

TEST(GeneralErrorsTest, TaskInitializesAsNotPassed) {
    Assessments* task = AssessmentFactory::createRegular(ScaleType::Accumulative);
    EXPECT_FALSE(task->isPassed());
    delete task;
}

TEST(GeneralErrorsTest, SubjectInitializesWithNoPendingBlockers) {
    Subject subject("Test", 1, false);
    EXPECT_FALSE(subject.hasPendingBlockers());
}

TEST(GeneralErrorsTest, PriorityManagerUpdateWithNullptrDoesNotCrash) {
    PriorityManager pm;
    EXPECT_NO_THROW(pm.update(nullptr)); 
}

TEST(GeneralErrorsTest, PriorityManagerGetPriorityForNullptrReturnsZero) {
    PriorityManager pm;
    EXPECT_EQ(pm.getPriorityForSubject(nullptr), 0);
}


TEST(GeneralErrorsTest, PriorityManagerMultipleUpdatesDoNotCompoundInfinity) {
    Subject subject("Math", 1, false);
    Assessments* task = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    task->addGrade(3.0);
    subject.addAssessment(task);

    PriorityManager pm;
    pm.update(&subject);
    int firstUpdate = pm.getPriorityForSubject(&subject); 
    
    pm.update(&subject); 
    int secondUpdate = pm.getPriorityForSubject(&subject);

    EXPECT_EQ(firstUpdate, secondUpdate);
}

TEST(GeneralErrorsTest, PriorityManagerHandlesSubjectWithNoAssessments) {
    Subject subject("Empty", 1, false);
    PriorityManager pm;
    pm.update(&subject);
    EXPECT_EQ(pm.getPriorityForSubject(&subject), 0);
}

TEST(GeneralErrorsTest, AddNegativeGradeDoesNotCrash) {
    Assessments* task = AssessmentFactory::createRegular(ScaleType::Accumulative);
    EXPECT_NO_THROW(task->addGrade(-15.0)); 
    delete task;
}

TEST(GeneralErrorsTest, FactoryReturnsValidPointers) {
    Assessments* task1 = AssessmentFactory::createRegular(ScaleType::TenPoint);
    Assessments* task2 = AssessmentFactory::createExam(ScaleType::TwelvePoint);

    EXPECT_NE(task1, nullptr);
    EXPECT_NE(task2, nullptr);
    
    delete task1;
    delete task2;
}

TEST(GeneralErrorsTest, AverageStrategyWithSingleElementReturnsElement) {
    AverageStrategy avg;
    std::vector<double> grades = {7.5};
    EXPECT_DOUBLE_EQ(avg.calculate(grades), 7.5);
}
