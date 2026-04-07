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

TEST(GeneralErrorsTest, SubjectWithEmptyRegularGradesIsNotPassed) {
    Subject subject("Test", 1, false);
    Assessments* task = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    subject.addAssessment(task);

    EXPECT_FALSE(subject.isPassed());
}

TEST(GeneralErrorsTest, SubjectWithRegularGradesBelowThresholdIsNotPassed) {
    Subject subject("Test", 1, false);
    Assessments* task = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    task->addGrade(3.0);
    subject.addAssessment(task);

    EXPECT_FALSE(subject.isPassed());
}

TEST(GeneralErrorsTest, SubjectWithRegularGradesAtThresholdIsPassed) {
    Subject subject("Test", 1, false);
    Assessments* task = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    task->addGrade(4.0);
    subject.addAssessment(task);

    EXPECT_TRUE(subject.isPassed());
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

TEST(GeneralErrorsTest, SaveGradesAppendsOnlyForRegular) {
    Assessments* regular = AssessmentFactory::createRegular(ScaleType::Accumulative);
    regular->saveGrades({10.0, 20.0}, true);
    regular->saveGrades({15.0}, true);

    const std::vector<double> grades = regular->getGrades();
    ASSERT_EQ(grades.size(), 3u);
    EXPECT_DOUBLE_EQ(grades[0], 10.0);
    EXPECT_DOUBLE_EQ(grades[1], 20.0);
    EXPECT_DOUBLE_EQ(grades[2], 15.0);
    delete regular;
}

TEST(GeneralErrorsTest, SaveGradesReplacesForRegularWhenAppendDisabled) {
    Assessments* regular = AssessmentFactory::createRegular(ScaleType::Accumulative);
    regular->saveGrades({10.0, 20.0}, true);
    regular->saveGrades({30.0}, false);

    const std::vector<double> grades = regular->getGrades();
    ASSERT_EQ(grades.size(), 1u);
    EXPECT_DOUBLE_EQ(grades[0], 30.0);
    delete regular;
}

TEST(GeneralErrorsTest, SaveGradesAlwaysReplacesForSingleGradeTypes) {
    Assessments* exam = AssessmentFactory::createExam(ScaleType::TwelvePoint);
    exam->saveGrades({7.0}, true);
    exam->saveGrades({11.0}, true);

    const std::vector<double> grades = exam->getGrades();
    ASSERT_EQ(grades.size(), 1u);
    EXPECT_DOUBLE_EQ(grades[0], 11.0);
    delete exam;
}
