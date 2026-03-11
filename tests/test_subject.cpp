#include <gtest/gtest.h>
#include "subject.h"
#include "assessments.h"

TEST(SubjectTest, SubjectInitialization) {
    Subject math("Mathematics", 1, true);
    
    EXPECT_EQ(math.Getname(), "Mathematics");
    EXPECT_TRUE(math.getIsMultiSemester());
    EXPECT_FALSE(math.getLinkId().empty()); // Перевіряємо, що унікальний ID згенерувався
}

TEST(SubjectTest, AddingAssessmentsToSubject) {
    Subject physics("Physics", 1, false);
    Assessments* exam = AssessmentFactory::createExam(ScaleType::TenPoint);
    Assessments* practice = AssessmentFactory::createPractice(ScaleType::TenPoint);
    
    physics.addAssessment(exam);
    physics.addAssessment(practice);
    
    auto tasks = physics.GetAssessments();
    EXPECT_EQ(tasks.size(), 2);
    EXPECT_EQ(tasks[0]->getType(), AssessmentType::EXAM);
}
TEST(SubjectTest, MultiSemesterToggle) {
    Subject s1("History", 1, true);
    Subject s2("PE", 1, false);
    
    EXPECT_TRUE(s1.getIsMultiSemester());
    EXPECT_FALSE(s2.getIsMultiSemester());
}