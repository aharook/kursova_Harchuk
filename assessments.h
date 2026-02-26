#ifndef ASSESSMENTS_H
#define ASSESSMENTS_H

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

std::chrono::system_clock::time_point parseDeadline(const std::string& datetime_str);

enum class AssessmentType { EXAM, COURSEWORK, PRACTICE, REGULAR };
enum class ScaleType { HundredPoint, TwelvePoint, FivePoint, Accumulative }; 

// Кажемо компілятору, що такий клас існує (він у averCalculator.h)
class ICalculationStrategy; 

class Assessments {
private:
    AssessmentType Type;
    ScaleType scale; 
    int maxPoints;
    std::chrono::system_clock::time_point deadline;
    bool IsBlocker;
    std::vector<double> Grades;
    
    // Прямий вказівник на математичну логіку
    ICalculationStrategy* strategy; 

public:
    // Конструктор приймає стратегію
    Assessments(AssessmentType Type, ScaleType scale, int maxPoints, std::chrono::system_clock::time_point deadline, bool Isblocker, ICalculationStrategy* strategy, const std::vector<double>& Grades = {});
    
    ~Assessments();

    AssessmentType getType() const;
    ScaleType getScale() const; 
    int getMaxPoints() const;
    bool getIsBlocker() const;
    std::vector<double> getGrades() const;
    std::chrono::system_clock::time_point getDeadline() const;

    void addGrade(double newGrade);
    bool isOverdue() const;
    double getCurrentScore() const;
    bool isPassed() const;
};

class AssessmentFactory {
public:
    static Assessments* createExam(int maxPoints, const std::string& deadline_str);
    static Assessments* createCoursework(int maxPoints, const std::string& deadline_str);
    static Assessments* createPractice(int maxPoints, const std::string& deadline_str = "");
    static Assessments* createRegular(int maxPoints, ScaleType scale, const std::string& deadline_str = "");
};

#endif