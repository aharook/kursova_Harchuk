#ifndef APP_STATE_H
#define APP_STATE_H

#include <vector>
#include <string>
#include "AcademicSystem.h" 
#include "PriorityManager.h"

struct AppState {
    AcademicSystem system; 
    PriorityManager pm;

    Subject* selectedSubject = nullptr;
    Assessments* selectedAssessmentForGrade = nullptr;
    
    int selectedDisplayScale = 0;
    bool isDarkTheme = false;
 

    bool openEditSubjectModal = false;
    bool openGradeModal = false;

    char newSubjName[128] = "";       
    char editSubjName[128] = "";      
    char newGradesBuffer[256] = "";   

    int newSubjScale = 0; 
    bool hasCustomPriority = false;
    int customPriorityIndex = 0;
    bool hasRegular = false;
    bool hasCoursework = false;
    bool hasPractice = false;
    bool hasExam = false;
    bool newSubjIsMultiSemester = false;

    bool overwriteGradesOnSave = false;

    bool showGradeError = false;
    char gradeErrorMessage[256] = "";

    char saveFileName[128] = "save.dat";
    std::vector<std::string> availableSaves;
    int selectedSaveIndex = -1;
    bool saveListInitialized = false;

    bool showSystemMessage = false;
    char systemMessage[256] = "";

    bool dataReloadedThisFrame = false;
};

#endif 
