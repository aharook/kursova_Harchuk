#ifndef DASHBOARD_PARTS_INTERNAL_H
#define DASHBOARD_PARTS_INTERNAL_H

#include <string>
#include "dashboard_parts.h"
#include "assessments.h"

namespace UI {
namespace DashboardParts {
namespace Detail {

std::string NormalizeSaveName(const char* rawName);
std::string BuildSemesterSaveName(int currentSemester);
void RefreshSaves(AppState& appState);
void SetSystemMessage(AppState& appState, const std::string& message);
bool OpenYearlyReportsFolder(std::string& errorMessage);
const Assessments* FindAssessmentByType(const Subject* subject, AssessmentType type);
void DrawAssessmentScoreCell(AppState& appState, const Subject* subject, AssessmentType type, ScaleType targetScale);

} 
} 
} 

#endif
