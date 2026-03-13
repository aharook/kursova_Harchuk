#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "Gradebook.h"
#include "subject.h"
#include "assessments.h"

class DataManager {
private:
    std::string saveDirectory;
    void ensureDirectoryExists() const;

public:
    DataManager(const std::string& directory = "Saves");

    void saveCurrentProgress(const Gradebook& gradebook, int currentSemester, const std::string& filename) const;
    
    bool loadCurrentProgress(Gradebook& gradebook, int& currentSemester, const std::string& filename) const;

    std::vector<std::string> getListOfSaves() const;
};

#endif
