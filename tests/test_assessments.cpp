#include <gtest/gtest.h>
#include "assessments.h"

// Тестуємо створення регулярного завдання з накопичувальною системою
TEST(AssessmentsTest, FactoryCreatesAccumulativeCorrectly) {
    // Arrange (Підготовка)
    Assessments* task = AssessmentFactory::createRegular(ScaleType::Accumulative);
    
    // Assert (Перевірка базових параметрів)
    EXPECT_EQ(task->getType(), AssessmentType::REGULAR);
    EXPECT_EQ(task->getScale(), ScaleType::Accumulative);
    EXPECT_FALSE(task->getIsBlocker());
    EXPECT_EQ(task->getCurrentScore(), 0.0);
    
    // Act (Дія - додаємо бали)
    task->addGrade(20.0);
    task->addGrade(45.0);
    
    // Assert (Перевірка логіки)
    // 20 + 45 = 65. Прохідний для Accumulative = 60.
    EXPECT_EQ(task->getCurrentScore(), 65.0);
    EXPECT_TRUE(task->isPassed()); // Має бути true, бо 65 >= 60
    
    delete task; // Не забуваємо чистити пам'ять
}

// Тестуємо систему для екзамену
TEST(AssessmentsTest, ExamIsBlockerAndHasHighPriority) {
    Assessments* exam = AssessmentFactory::createExam(ScaleType::TenPoint);
    
    EXPECT_TRUE(exam->getIsBlocker());
    EXPECT_EQ(exam->getBasePriority(), 1000);
    
    delete exam;
}

TEST(AssessmentsTest, AccumulativeBoundaryValue) {
    Assessments* task = AssessmentFactory::createRegular(ScaleType::Accumulative);
    
    // Граничне значення для Accumulative — 60.0
    task->addGrade(59.9);
    EXPECT_FALSE(task->isPassed()); // Ще не пройшов
    
    task->addGrade(0.1);
    EXPECT_TRUE(task->isPassed());  // Рівно 60 — тепер пройшов
    
    delete task;
}