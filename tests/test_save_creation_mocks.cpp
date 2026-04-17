#include <gtest/gtest.h>

#include <stdexcept>
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

class MockYearlyReportSaver final : public IYearlyReportSaver {
public:
    mutable int saveCallCount = 0;
    mutable int lastYear = 0;
    mutable int lastReportAcademicYear = 0;

    bool shouldThrow = false;
    std::string nextFileName = "mock_report.xls";

    std::string saveYearlyReport(const YearlyReport& report, int year) const override {
        saveCallCount++;
        lastYear = year;
        lastReportAcademicYear = report.getAcademicYear();

        if (shouldThrow) {
            throw std::runtime_error("mock yearly report save failure");
        }

        return nextFileName;
    }
};

void addPassedRegularSubject(AcademicSystem& system, const std::string& name, int semester, double grade) {
    Subject* subject = new Subject(name, semester, false);
    Assessments* regular = AssessmentFactory::createRegular(ScaleType::TwelvePoint);
    regular->addGrade(grade);
    subject->addAssessment(regular);
    system.addSubjectToCurrentSemester(subject);
}

}

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

TEST(YearlyReportSaverMockTest, TryGenerateLatestYearlyReportUsesInjectedSaverAndReturnsSuccess) {
    PassthroughGradeConverter converter;
    MockSystemStateRepository repository;
    MockYearlyReportSaver yearlySaver;
    yearlySaver.nextFileName = "year_1_mock.xls";

    AcademicSystem system(&converter, &repository, &yearlySaver);

    addPassedRegularSubject(system, "Linear Algebra", 1, 10.0);
    ASSERT_TRUE(system.endSemester());

    addPassedRegularSubject(system, "Discrete Math", 2, 11.0);
    ASSERT_TRUE(system.endSemester());

    AcademicSystem::YearlyReportGenerationResult result;
    const bool generated = system.tryGenerateLatestYearlyReport(result);

    ASSERT_TRUE(generated);
    EXPECT_EQ(result.status, AcademicSystem::YearlyReportGenerationResult::Status::Success);
    EXPECT_EQ(result.year, 1);
    EXPECT_EQ(result.fileName, "year_1_mock.xls");
    EXPECT_EQ(yearlySaver.saveCallCount, 1);
    EXPECT_EQ(yearlySaver.lastYear, 1);
    EXPECT_EQ(yearlySaver.lastReportAcademicYear, 1);
}

TEST(YearlyReportSaverMockTest, TryGenerateLatestYearlyReportReturnsSaveFailedWhenSaverThrows) {
    PassthroughGradeConverter converter;
    MockSystemStateRepository repository;
    MockYearlyReportSaver yearlySaver;
    yearlySaver.shouldThrow = true;

    AcademicSystem system(&converter, &repository, &yearlySaver);

    addPassedRegularSubject(system, "Physics I", 1, 9.0);
    ASSERT_TRUE(system.endSemester());

    addPassedRegularSubject(system, "Physics II", 2, 10.0);
    ASSERT_TRUE(system.endSemester());

    AcademicSystem::YearlyReportGenerationResult result;
    const bool generated = system.tryGenerateLatestYearlyReport(result);

    EXPECT_FALSE(generated);
    EXPECT_EQ(result.status, AcademicSystem::YearlyReportGenerationResult::Status::SaveFailed);
    EXPECT_EQ(yearlySaver.saveCallCount, 1);
    EXPECT_EQ(yearlySaver.lastYear, 1);
}
