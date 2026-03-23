#ifndef DASHBOARD_PARTS_H
#define DASHBOARD_PARTS_H

#include <vector>
#include "app_state.h"

namespace UI {
namespace DashboardParts {
    std::vector<Subject*> BuildSortedSubjects(AppState& appState);
    bool DrawTopPanel(AppState& appState);
    void DrawSubjectsList(AppState& appState, const std::vector<Subject*>& sortedSubjects);
    bool DrawSelectedSubjectDetails(AppState& appState);
    void DrawSemesterOverview(AppState& appState, const std::vector<Subject*>& sortedSubjects, bool hasDebts);
}
}

#endif