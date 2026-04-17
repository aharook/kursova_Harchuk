#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "Gradebook.h"
#include "subject.h"
#include "assessments.h"
#include "AcademicPorts.h"

class DataManager : public ISystemStateRepository {
private:
    std::string saveDirectory;
    void ensureDirectoryExists() const;

public:
    DataManager(const std::string& directory = "Saves");

    void saveCurrentProgress(
        const Gradebook& gradebook,
        const std::vector<Subject*>& archivedSubjects,
        int currentSemester,
        const std::string& filename
    ) const override;
    
    bool loadCurrentProgress(
        Gradebook& gradebook,
        std::vector<Subject*>& archivedSubjects,
        int& currentSemester,
        const std::string& filename
    ) const override;

    std::vector<std::string> getListOfSaves() const override;
    std::string getLatestSaveFileName() const override;
};

#endif
