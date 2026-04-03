#include "DataManager.h"
#include <iostream>
#include <map>
#include <functional>
#include <sstream>

namespace fs = std::filesystem;

namespace {

const std::map<AssessmentType, std::function<Assessments*(ScaleType)>> kTaskCreators = {
    {AssessmentType::EXAM, AssessmentFactory::createExam},
    {AssessmentType::COURSEWORK, AssessmentFactory::createCoursework},
    {AssessmentType::PRACTICE, AssessmentFactory::createPractice},
    {AssessmentType::REGULAR, AssessmentFactory::createRegular}
};

void writeSubject(std::ofstream& outFile, const Subject* sub) {
    std::string safeName = sub->Getname();
    for (char& c : safeName) {
        if (c == ' ') c = '_';
    }

    outFile << safeName << " "
            << sub->getSemester() << " "
            << sub->getIsMultiSemester() << " "
            << sub->getLinkId() << " "
            << sub->getUsersPriority() << "\n";

    const std::vector<Assessments*>& tasks = sub->GetAssessments();
    outFile << tasks.size() << "\n";

    for (const Assessments* task : tasks) {
        outFile << static_cast<int>(task->getType()) << " "
                << static_cast<int>(task->getScale()) << " "
                << task->getBasePriority() << " "
                << task->getIsBlocker() << " ";

        const std::vector<double> grades = task->getGrades();
        outFile << grades.size() << " ";
        for (double g : grades) {
            outFile << g << " ";
        }
        outFile << "\n";
    }
}

Subject* readSubject(std::ifstream& inFile) {
    std::string name;
    std::string linkId;
    int sem = 1;
    bool isMulti = false;
    int usersPriority = 0;

    if (!(inFile >> name >> sem >> isMulti >> linkId)) {
        return nullptr;
    }
    std::string tail;
    std::getline(inFile, tail);
    if (!tail.empty()) {
        std::istringstream tailStream(tail);
        tailStream >> usersPriority;
    }
    for (char& c : name) {
        if (c == '_') c = ' ';
    }

    Subject* newSubject = new Subject(name, sem, isMulti, linkId);
    if (usersPriority > 0) {
        newSubject->setUsersPriority(usersPriority);
    }

    size_t taskCount = 0;
    inFile >> taskCount;

    for (size_t j = 0; j < taskCount; ++j) {
        int typeInt = 0;
        int scaleInt = 0;
        int priority = 0;
        bool isBlocker = false;
        size_t gradesCount = 0;

        inFile >> typeInt >> scaleInt >> priority >> isBlocker >> gradesCount;

        const AssessmentType type = static_cast<AssessmentType>(typeInt);
        const ScaleType scale = static_cast<ScaleType>(scaleInt);

        Assessments* newTask = nullptr;
        const auto it = kTaskCreators.find(type);
        if (it != kTaskCreators.end()) {
            newTask = it->second(scale);
        }

        if (newTask) {
            for (size_t k = 0; k < gradesCount; ++k) {
                double grade = 0.0;
                inFile >> grade;
                newTask->addGrade(grade);
            }
            newSubject->addAssessment(newTask);
        } else {
            for (size_t k = 0; k < gradesCount; ++k) {
                double ignoredGrade = 0.0;
                inFile >> ignoredGrade;
            }
        }
    }

    return newSubject;
}

} // namespace

DataManager::DataManager(const std::string& directory) : saveDirectory(directory) {
    ensureDirectoryExists();
}

void DataManager::ensureDirectoryExists() const {
    if (!fs::exists(saveDirectory)) {
        fs::create_directory(saveDirectory);
    }
}


void DataManager::saveCurrentProgress(
    const Gradebook& gradebook,
    const std::vector<Subject*>& archivedSubjects,
    int currentSemester,
    const std::string& filename
) const {
    ensureDirectoryExists();
    std::string fullPath = saveDirectory + "/" + filename;
    std::ofstream outFile(fullPath);

    if (!outFile.is_open()) return;

    outFile << "SEMESTER " << currentSemester << "\n";

    const std::vector<Subject*>& subjects = gradebook.getSubjects();
    outFile << subjects.size() << "\n"; 

    for (const Subject* sub : subjects) {
        writeSubject(outFile, sub);
    }

    outFile << archivedSubjects.size() << "\n";
    for (const Subject* sub : archivedSubjects) {
        writeSubject(outFile, sub);
    }

    outFile.close();
}


bool DataManager::loadCurrentProgress(
    Gradebook& gradebook,
    std::vector<Subject*>& archivedSubjects,
    int& currentSemester,
    const std::string& filename
) const {
    std::string fullPath = saveDirectory + "/" + filename;
    std::ifstream inFile(fullPath);

    if (!inFile.is_open()) return false;

    std::string headerToken;
    int loadedSemester = 1;
    size_t subjectCount = 0;

    if (!(inFile >> headerToken)) {
        return false;
    }

    if (headerToken == "SEMESTER") {
        if (!(inFile >> loadedSemester)) {
            return false;
        }
        if (!(inFile >> subjectCount)) {
            return false;
        }
    } else {
        try {
            loadedSemester = std::stoi(headerToken);
        } catch (...) {
            return false;
        }
        if (!(inFile >> subjectCount)) {
            return false;
        }
    }

    // Replace current in-memory state with loaded save content.
    gradebook.clear();
    for (Subject* sub : archivedSubjects) {
        delete sub;
    }
    archivedSubjects.clear();

    currentSemester = loadedSemester;


    for (size_t i = 0; i < subjectCount; ++i) {
        Subject* newSubject = readSubject(inFile);
        if (newSubject == nullptr) {
            gradebook.clear();
            for (Subject* sub : archivedSubjects) {
                delete sub;
            }
            archivedSubjects.clear();
            return false;
        }
        gradebook.addSubject(newSubject);
    }

    size_t archivedCount = 0;
    if (inFile >> archivedCount) {
        for (size_t i = 0; i < archivedCount; ++i) {
            Subject* archivedSubject = readSubject(inFile);
            if (archivedSubject == nullptr) {
                gradebook.clear();
                for (Subject* sub : archivedSubjects) {
                    delete sub;
                }
                archivedSubjects.clear();
                return false;
            }
            archivedSubjects.push_back(archivedSubject);
        }
    } else {
        // Older save format: archived block is absent.
        inFile.clear();
    }

    inFile.close();
    return true;
}

std::vector<std::string> DataManager::getListOfSaves() const {
    std::vector<std::string> files;
    ensureDirectoryExists();
    for (const auto& entry : fs::directory_iterator(saveDirectory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".dat") {
            files.push_back(entry.path().filename().string());
        }
    }
    return files;
}

std::string DataManager::getLatestSaveFileName() const {
    ensureDirectoryExists();

    fs::file_time_type latestTime{};
    std::string latestFile;

    for (const auto& entry : fs::directory_iterator(saveDirectory)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const fs::path path = entry.path();
        if (path.extension() != ".dat") {
            continue;
        }

        const auto currentTime = fs::last_write_time(path);
        if (latestFile.empty() || currentTime > latestTime) {
            latestTime = currentTime;
            latestFile = path.filename().string();
        }
    }

    return latestFile;
}
