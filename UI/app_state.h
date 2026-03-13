#ifndef APP_STATE_H
#define APP_STATE_H

#include <vector>
#include "subject.h"
#include "PriorityManager.h"
#include "assessments.h"

struct AppState {
    std::vector<Subject*> subjects; 
    PriorityManager pm;
    
    // ЗМІНА 1: Тепер ми тримаємо вказівник, тому сортування нічого не зламає!
    Subject* selectedSubject = nullptr; 

    char newSubjName[128] = "";
    int newSubjScale = 0; 
    
    bool hasRegular = true;
    bool hasCoursework = false;
    bool hasPractice = false;
    bool hasExam = true;

    // ... в самому кінці структури AppState ...
    Assessments* selectedAssessmentForGrade = nullptr;
    
    char newGradesBuffer[256] = ""; 
    bool openGradeModal = false;
    bool showGradeError = false;   
    std::string gradeErrorMessage = ""; 
    bool openEditSubjectModal = false;
    char editSubjName[256] = "";

};

#endif