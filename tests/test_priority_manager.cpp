#include <gtest/gtest.h>
#include "PriorityManager.h"
#include "subject.h"
#include "assessments.h"

TEST(PriorityManagerTest, NormalPriorityWithoutBadGrades) {
    Subject math("Math", 1, false);
    // Курсова: базова пріоритетність 500
    Assessments* coursework = AssessmentFactory::createCoursework(ScaleType::Accumulative);
    coursework->addGrade(80.0); // 80 - це більше за 60, оцінка хороша
    math.addAssessment(coursework);

    PriorityManager pm;
    pm.update(&math);

    // Має бути просто максимальний базовий пріоритет (500)
    EXPECT_EQ(pm.getPriorityForSubject(&math), 500);
}

TEST(PriorityManagerTest, PenaltyAddedForBadGrades) {
    Subject physics("Physics", 1, false);
    // Екзамен: базова пріоритетність 1000
    Assessments* exam = AssessmentFactory::createExam(ScaleType::TwelvePoint);
    exam->addGrade(3.0); // 3.0 у 12-бальній системі - це ПОГАНА оцінка (менше 4.0)
    physics.addAssessment(exam);

    PriorityManager pm;
    pm.update(&physics);

    // Має бути: базовий (1000) + штраф (2000) = 3000
    EXPECT_EQ(pm.getPriorityForSubject(&physics), 3000);
}
TEST(PriorityManagerTest, CorrectMaxBasePriority) {
    Subject math("Math", 1, false);
    
    // Додаємо домашку (пріоритет 100) і екзамен (пріоритет 1000)
    math.addAssessment(AssessmentFactory::createRegular(ScaleType::FivePoint)); 
    math.addAssessment(AssessmentFactory::createExam(ScaleType::FivePoint));
    
    PriorityManager pm;
    pm.update(&math);
    
    // Має обрати 1000, бо це максимум серед завдань
    EXPECT_GE(pm.getPriorityForSubject(&math), 1000);
}