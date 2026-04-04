#include <gtest/gtest.h>
#include "subject.h"
#include "assessments.h"
#include "PriorityManager.h"
#include "SemesterManager.h"

TEST(KeyScenariosTest, PerfectStudentPassesEverything) {
    Subject math("Mathematics", 5, false);
    
    Assessments* hw = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    Assessments* exam = AssessmentFactory::createExam(ScaleType::Accumulative);
    
    hw->addGrade(12.0);
    exam->addGrade(95.0);
    
    math.addAssessment(hw);
    math.addAssessment(exam);
    
    PriorityManager pm;
    pm.update(&math);
    EXPECT_FALSE(math.hasPendingBlockers());
    EXPECT_EQ(pm.getPriorityForSubject(&math), 1000);
}

TEST(KeyScenariosTest, StrugglingStudentFailsEverything) {
    Subject physics("Physics", 4, false);
    
    Assessments* lab = AssessmentFactory::createRegular(ScaleType::FivePoint);
    Assessments* exam = AssessmentFactory::createExam(ScaleType::Accumulative);
    
    lab->addGrade(2.0); 
    exam->addGrade(30.0); 
    
    physics.addAssessment(lab);
    physics.addAssessment(exam);
    
    PriorityManager pm;
    pm.update(&physics);

    EXPECT_TRUE(physics.hasPendingBlockers());

    EXPECT_EQ(pm.getPriorityForSubject(&physics), 3000);
}


TEST(KeyScenariosTest, MixedSemesterHandlingMultipleSubjects) {
    PriorityManager pm;
    
    Subject programming("Programming", 5, false);
    Assessments* progExam = AssessmentFactory::createExam(ScaleType::TwelvePoint);
    progExam->addGrade(10.0);
    programming.addAssessment(progExam);
    Subject philosophy("Philosophy", 3, false);
    Assessments* essay = AssessmentFactory::createRegular(ScaleType::Accumulative);
    essay->addGrade(50.0); 
    philosophy.addAssessment(essay);

    pm.update(&programming);
    pm.update(&philosophy);
    
    EXPECT_FALSE(programming.hasPendingBlockers());
    EXPECT_EQ(pm.getPriorityForSubject(&programming), 1000);

    EXPECT_EQ(pm.getPriorityForSubject(&philosophy), 2050); 
}


TEST(KeyScenariosTest, StudentRetakesAndFixesPriority) {
    Subject history("History", 2, false);
    Assessments* test = AssessmentFactory::createRegular(ScaleType::Accumulative);
    history.addAssessment(test);
    
    PriorityManager pm;

    test->addGrade(50.0);
    pm.update(&history);
    
    EXPECT_EQ(pm.getPriorityForSubject(&history), 2050);
    test->addGrade(20.0);
    pm.update(&history);
    EXPECT_EQ(pm.getPriorityForSubject(&history), 50);
}

TEST(KeyScenariosTest, SemesterCannotEndIfRegularScoreIsBelowThreshold) {
    Gradebook gradebook;
    Subject* math = new Subject("Math", 1, false);
    Assessments* homework = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    homework->addGrade(3.0);
    math->addAssessment(homework);
    gradebook.addSubject(math);

    SemesterManager manager;
    EXPECT_FALSE(manager.canEndSemester(gradebook));
}
