#ifndef APP_STATE_H
#define APP_STATE_H

#include <vector>
#include "subject.h"
#include "PriorityManager.h"
#include "assessments.h"

struct AppState {
    std::vector<Subject*> subjects; 
    PriorityManager pm;
    int selectedSubjectIndex = -1;

    char newSubjName[128] = "";
    int newSubjScale = 0; 
    
    bool hasRegular = true;
    bool hasCoursework = false;
    bool hasPractice = false;
    bool hasExam = false;

    Assessments* selectedAssessmentForGrade = nullptr;
    float newGradeValue = 0.0f;
    bool openGradeModal = false;
};

#endif