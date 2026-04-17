#ifndef ACADEMIC_PORTS_H
#define ACADEMIC_PORTS_H

#include <string>
#include <vector>
#include "assessments.h"

class Gradebook;
class Subject;

class IGradeConverter {
public:
    virtual ~IGradeConverter() = default;
    virtual double convert(double grade, ScaleType fromScale, ScaleType toScale) const = 0;
};

class ISystemStateRepository {
public:
    virtual ~ISystemStateRepository() = default;

    virtual void saveCurrentProgress(
        const Gradebook& gradebook,
        const std::vector<Subject*>& archivedSubjects,
        int currentSemester,
        const std::string& filename
    ) const = 0;

    virtual bool loadCurrentProgress(
        Gradebook& gradebook,
        std::vector<Subject*>& archivedSubjects,
        int& currentSemester,
        const std::string& filename
    ) const = 0;

    virtual std::vector<std::string> getListOfSaves() const = 0;
    virtual std::string getLatestSaveFileName() const = 0;
};

#endif