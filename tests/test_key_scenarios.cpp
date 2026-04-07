#include <gtest/gtest.h>
#include "subject.h"
#include "assessments.h"
#include "PriorityManager.h"
#include "SemesterManager.h"
#include "SubjectGroupPerformanceService.h"

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

TEST(KeyScenariosTest, MultiSemesterSubjectCarriesOnlyToNextSemester) {
    Gradebook gradebook;
    std::vector<Subject*> archive;

    Subject* multi = new Subject("Algorithms", 1, true);
    Assessments* regular = AssessmentFactory::createRegular(ScaleType::Accumulative);
    regular->addGrade(60.0);
    multi->addAssessment(regular);
    gradebook.addSubject(multi);

    SemesterManager manager;
    manager.transitionToNextSemester(gradebook, archive);

    ASSERT_EQ(manager.getCurrentSemester(), 2);
    ASSERT_EQ(gradebook.getSubjects().size(), 1u);
    Subject* carried = gradebook.getSubjects().front();
    EXPECT_EQ(carried->Getname(), "Algorithms");
    EXPECT_FALSE(carried->getIsMultiSemester());
    ASSERT_EQ(carried->GetAssessments().size(), 1u);
    EXPECT_TRUE(carried->GetAssessments().front()->getGrades().empty());

    carried->GetAssessments().front()->addGrade(70.0);
    manager.transitionToNextSemester(gradebook, archive);

    EXPECT_EQ(manager.getCurrentSemester(), 3);
    EXPECT_TRUE(gradebook.getSubjects().empty());

    for (Subject* sub : archive) {
        delete sub;
    }
}

TEST(KeyScenariosTest, YearlyAggregationMergesRegularGradeArraysForMultiSemesterSubject) {
    Subject firstPart("Databases", 1, true);
    Assessments* regularFirst = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    regularFirst->saveGrades({10.0, 8.0}, true);
    firstPart.addAssessment(regularFirst);

    Subject secondPart("Databases", 2, false, firstPart.getLinkId());
    Assessments* regularSecond = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    regularSecond->saveGrades({12.0}, true);
    secondPart.addAssessment(regularSecond);

    const std::vector<Subject*> parts = { &firstPart, &secondPart };
    const double score = SubjectGroupPerformanceService::calculateRegularScore(parts);

    EXPECT_DOUBLE_EQ(score, 10.0);
}
