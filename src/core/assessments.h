#ifndef ASSESSMENTS_H
#define ASSESSMENTS_H

#include <string>
#include <vector>

enum class AssessmentType { EXAM, COURSEWORK, PRACTICE, REGULAR };
enum class ScaleType { TenPoint, TwelvePoint, FivePoint, Accumulative }; 

class ICalculationStrategy; 

class Assessments {
private:
    AssessmentType Type;
    ScaleType scale; 
    int basePriority;
    bool IsBlocker;
    std::vector<double> Grades;
    
    ICalculationStrategy* strategy; 

public:
    Assessments(AssessmentType Type, ScaleType scale, int basePriority, bool Isblocker, ICalculationStrategy* strategy, const std::vector<double>& Grades = {});
    
    Assessments(const Assessments&) = delete;
    Assessments& operator=(const Assessments&) = delete;

    ~Assessments();

    AssessmentType getType() const;
    ScaleType getScale() const; 
    int getBasePriority() const;
    bool getIsBlocker() const;
    std::vector<double> getGrades() const;
    bool hasGrades() const;

    void addGrade(double newGrade);
    double getCurrentScore() const;
    bool isPassed() const;
};

class AssessmentFactory {
public:
    static Assessments* createExam(ScaleType scale);
    static Assessments* createCoursework(ScaleType scale);
    static Assessments* createPractice(ScaleType scale);
    static Assessments* createRegular(ScaleType scale);
};

#endif