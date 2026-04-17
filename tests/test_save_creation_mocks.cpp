#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "AcademicSystem.h"
#include "subject.h"
#include "assessments.h"

namespace {

class PassthroughGradeConverter final : public IGradeConverter {
public:
    double convert(double grade, ScaleType, ScaleType) const override {
        return grade;
    }
};

class MockSystemStateRepository final : public ISystemStateRepository {
public:
    mutable int saveCallCount = 0;
    mutable int loadCallCount = 0;
    mutable int getLatestSaveCallCount = 0;

    mutable int lastCurrentSemester = -1;
    mutable size_t lastActiveSubjectCount = 0;
    mutable size_t lastArchivedSubjectCount = 0;
    mutable std::string lastFilename;

    bool loadResult = false;
    int loadedSemester = 1;
    std::string latestSaveFileName;
    std::vector<std::string> saveNames;

    void saveCurrentProgress(
        const Gradebook& gradebook,
        const std::vector<Subject*>& archivedSubjects,
        int currentSemester,
        const std::string& filename
    ) const override {
        saveCallCount++;
        lastCurrentSemester = currentSemester;
        lastFilename = filename;
        lastActiveSubjectCount = gradebook.getSubjects().size();
        lastArchivedSubjectCount = archivedSubjects.size();
    }

    bool loadCurrentProgress(
        Gradebook&,
        std::vector<Subject*>&,
        int& currentSemester,
        const std::string&
    ) const override {
        loadCallCount++;
        if (loadResult) {
            currentSemester = loadedSemester;
        }
        return loadResult;
    }

    std::vector<std::string> getListOfSaves() const override {
        return saveNames;
    }

    std::string getLatestSaveFileName() const override {
        getLatestSaveCallCount++;
        return latestSaveFileName;
    }
};

} // namespace

TEST(SaveCreationMockTest, SaveSystemStatePassesFilenameAndCurrentSemesterToRepository) {
    PassthroughGradeConverter converter;
    MockSystemStateRepository repository;
    AcademicSystem system(&converter, &repository);

    system.saveSystemState("save_sem_1.json");

    EXPECT_EQ(repository.saveCallCount, 1);
    EXPECT_EQ(repository.lastFilename, "save_sem_1.json");
    EXPECT_EQ(repository.lastCurrentSemester, 1);
}

TEST(SaveCreationMockTest, SaveAfterSemesterTransitionIncludesArchivedSubjects) {
    PassthroughGradeConverter converter;
    MockSystemStateRepository repository;
    AcademicSystem system(&converter, &repository);

    Subject* algebra = new Subject("Algebra", 1, false);
    Assessments* regular = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    regular->addGrade(9.0);
    algebra->addAssessment(regular);
    system.addSubjectToCurrentSemester(algebra);

    ASSERT_TRUE(system.endSemester());

    system.saveSystemState("after_semester_1.json");

    EXPECT_EQ(repository.saveCallCount, 1);
    EXPECT_EQ(repository.lastCurrentSemester, 2);
    EXPECT_EQ(repository.lastFilename, "after_semester_1.json");
    EXPECT_EQ(repository.lastActiveSubjectCount, 0u);
    EXPECT_EQ(repository.lastArchivedSubjectCount, 1u);
}

TEST(SaveCreationMockTest, RepeatedSaveCreationCallsRepositoryEachTime) {
    PassthroughGradeConverter converter;
    MockSystemStateRepository repository;
    AcademicSystem system(&converter, &repository);

    system.saveSystemState("first_save.json");
    system.saveSystemState("second_save.json");

    EXPECT_EQ(repository.saveCallCount, 2);
    EXPECT_EQ(repository.lastFilename, "second_save.json");
}
