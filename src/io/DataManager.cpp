#include "DataManager.h"
#include <iostream>
#include <map>
#include <functional>

namespace fs = std::filesystem;

DataManager::DataManager(const std::string& directory) : saveDirectory(directory) {
    ensureDirectoryExists();
}

void DataManager::ensureDirectoryExists() const {
    if (!fs::exists(saveDirectory)) {
        fs::create_directory(saveDirectory);
    }
}


void DataManager::saveCurrentProgress(const Gradebook& gradebook, int currentSemester, const std::string& filename) const {
    ensureDirectoryExists();
    std::string fullPath = saveDirectory + "/" + filename;
    std::ofstream outFile(fullPath);

    if (!outFile.is_open()) return;

    outFile << currentSemester << "\n";

    const std::vector<Subject*>& subjects = gradebook.getSubjects();
    outFile << subjects.size() << "\n"; 

    for (const Subject* sub : subjects) {
        std::string safeName = sub->Getname();
        for (char& c : safeName) if (c == ' ') c = '_';

        outFile << safeName << " " 
                << sub->getSemester() << " " 
                << sub->getIsMultiSemester() << " " 
                << sub->getLinkId() << "\n";

        const std::vector<Assessments*>& tasks = sub->GetAssessments();
        outFile << tasks.size() << "\n"; 

        for (const Assessments* task : tasks) {
            outFile << static_cast<int>(task->getType()) << " "
                    << static_cast<int>(task->getScale()) << " "
                    << task->getBasePriority() << " "
                    << task->getIsBlocker() << " ";
            
            std::vector<double> grades = task->getGrades();
            outFile << grades.size() << " "; 
            for (double g : grades) {
                outFile << g << " ";
            }
            outFile << "\n";
        }
    }
    outFile.close();
}


bool DataManager::loadCurrentProgress(Gradebook& gradebook, int& currentSemester, const std::string& filename) const {
    std::string fullPath = saveDirectory + "/" + filename;
    std::ifstream inFile(fullPath);

    if (!inFile.is_open()) return false;

    // Replace current in-memory state with loaded save content.
    gradebook.clear();

    inFile >> currentSemester;

    size_t subjectCount;
    inFile >> subjectCount;


    static const std::map<AssessmentType, std::function<Assessments*(ScaleType)>> taskCreators = {
        {AssessmentType::EXAM, AssessmentFactory::createExam},
        {AssessmentType::COURSEWORK, AssessmentFactory::createCoursework},
        {AssessmentType::PRACTICE, AssessmentFactory::createPractice},
        {AssessmentType::REGULAR, AssessmentFactory::createRegular}
    };

    for (size_t i = 0; i < subjectCount; ++i) {
        std::string name, linkId;
        int sem;
        bool isMulti;

        inFile >> name >> sem >> isMulti >> linkId;
        for (char& c : name) if (c == '_') c = ' ';

        Subject* newSubject = new Subject(name, sem, isMulti, linkId);

        size_t taskCount;
        inFile >> taskCount;

        for (size_t j = 0; j < taskCount; ++j) {
            int typeInt, scaleInt, priority;
            bool isBlocker;
            size_t gradesCount;

            inFile >> typeInt >> scaleInt >> priority >> isBlocker >> gradesCount;

            AssessmentType type = static_cast<AssessmentType>(typeInt);
            ScaleType scale = static_cast<ScaleType>(scaleInt);

            Assessments* newTask = nullptr;
            

            auto it = taskCreators.find(type);
            if (it != taskCreators.end()) {
                newTask = it->second(scale); 
            }

            if (newTask) {
                for (size_t k = 0; k < gradesCount; ++k) {
                    double grade;
                    inFile >> grade;
                    newTask->addGrade(grade);
                }
                newSubject->addAssessment(newTask);
            }
        }

        gradebook.addSubject(newSubject);
    }

    inFile.close();
    return true;
}

std::vector<std::string> DataManager::getListOfSaves() const {
    std::vector<std::string> files;
    ensureDirectoryExists();
    for (const auto& entry : fs::directory_iterator(saveDirectory)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().filename().string());
        }
    }
    return files;
}
