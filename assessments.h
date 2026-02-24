#ifndef ASSESSMENTS_H
#define ASSESSMENTS_H

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "Grades.h"

// Parse deadline string in format "DD.MM.YYYY"
std::chrono::system_clock::time_point parseDeadline(const std::string& datetime_str);

// Enum for assessment types
enum class AssessmentType {
    EXAM,
    COURSEWORK,
    PRACTICE,
    REGULAR
};

// Main assessments class
class Assessments {
    AssessmentType Type;
    int maxPoints;
    std::chrono::system_clock::time_point deadline;
    bool IsBlocker;
    std::vector<Grade*> Grades;

public:
    // Constructor
    Assessments(AssessmentType Type, int maxPoints, std::chrono::system_clock::time_point deadline, bool Isblocker, const std::vector<Grade*> Grades = {});

    // Getters
    AssessmentType getType() const;
    int getMaxPoints() const;
    bool getIsBlocker() const;
    std::vector<Grade*> getGrades() const;
    std::chrono::system_clock::time_point getDeadline() const;

    // Methods
    void addGrade(Grade* newGrade);
    bool isOverdue() const;
    double getCurrentScore() const;
    bool isPassed() const;
};

// Factory class for creating assessments
class AssessmentFactory {
public:
    static Assessments* createExam(int maxPoints, const std::string& deadline_str);
    static Assessments* createCoursework(int maxPoints, const std::string& deadline_str);
    static Assessments* createPractice(int maxPoints, const std::string& deadline_str = "");
    static Assessments* createRegular(int maxPoints, const std::string& deadline_str = "");
};

#endif // ASSESSMENTS_H
