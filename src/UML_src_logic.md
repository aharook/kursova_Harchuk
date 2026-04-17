classDiagram


    class AcademicSystem {
        +canEndCurrentSemester() bool
        +endSemester() bool
        +calculateCurrentSemesterAverage(targetScale, includedSubjects) double
        +tryGenerateLatestYearlyReport(result) bool
        +endYear(year) YearlyReport
        +generateAndSaveYearlyReport(year, savedFileName) YearlyReport
    }

    class Gradebook {
        -vector~Subject*~ subjects
        +hasPendingBlockers() bool
        +extractAllSubjects() vector~Subject*~
    }

    class SemesterManager {
        -int currentSemester
        +canEndSemester(activeGradebook) bool
        +transitionToNextSemester(activeGradebook, archive)
    }
namespace Observer {
    class Subject {
        
        +addAssessment()
        +addGradeToTask(taskIndex, grade)
        +attach(observer)
        +notifyObservers()
        +setUsersPriority(priority)
        +hasPendingBlockers() bool
        +isPassed() bool
    }

    class IObserver {
        <<interface>>
        +update(Subject* s)
    }

    class PriorityManager {
        -map~Subject*,int~ subjectPriorities
        +update(Subject* s)
        +getPriorityForSubject(Subject* s) int
    }

    class StatisticsManager {
        -double currentGPA
        +update(Subject* s)
        +getGPA() double
        +isSemesterFinished() bool
    }
}

namespace Factory {
    class Assessments {
        -int basePriority
        -bool IsBlocker
        -vector~double~ Grades
        -ICalculationStrategy* strategy
        +hasGrades() bool
        +addGrade(newGrade)
        +saveGrades(newGrades, appendIfRegular)
        +isPassed() bool
    }

    class AssessmentFactory {
        +createExam(scale) Assessments*
        +createCoursework(scale) Assessments*
        +createPractice(scale) Assessments*
        +createRegular(scale) Assessments*
    }
}

namespace  Strategy {
    class ICalculationStrategy {
        <<interface>>
        +calculate(grades) double
    }

    class AverageStrategy {
        +calculate(grades) double
    }

    class SumStrategy {
        +calculate(grades) double
    }

    class SingleGradeStrategy {
        +calculate(grades) double
    }

    class StrategyFactory {
        +createStrategy(type) ICalculationStrategy*
    }
}
    class GradePolicy {
        +BLOCKER_PENALTY int
        +BAD_GRADE_PENALTY int
        +getPassingThreshold(scale) double
        +getMaxAllowedGrade(scale) double
        +isPassingScore(scale,score) bool
    }

    class AssessmentPerformanceService {
        +hasPendingBlockers(assessments) bool
        +hasAnyRegularGrades(assessments) bool
        +getPriorityScore(assessments) int
        +resolveRegularScale(assessments) ScaleType
        +calculateRegularScore(assessments) double
    }

    class SubjectGroupPerformanceService {
        +resolveRegularScale(subjects) ScaleType
        +calculateRegularScore(subjects) double
    }

    class ConversionRule {
        -map~ScaleType,double~ scaleValues
        +addValue(scale,value)
        +getValueForScale(scale) double
    }

    class GradeConverter {
        -vector~ConversionRule~ rules
        +convert(grade,fromScale,toScale) double
    }

    class DataManager {
        -string saveDirectory
        +saveCurrentProgress()
        +loadCurrentProgress() bool
        +getListOfSaves() vector~string~
        +getLatestSaveFileName() string
    }


    class YearlyReport {
        -int academicYear
        -vector~SubjectResult~ results
        -double annualGPA
        -bool canProceedToNextYear
        +getAcademicYear() int
        +getResults() vector~SubjectResult~
        +getAnnualGPA() double
        +getCanProceed() bool
    }

    class AnnualReportBuilder {
        +generateReport(year, allYearSubjects, converter) YearlyReport
    }

    class YearlyReportSaver {
        +save(report, year, directory) string
    }


    AcademicSystem *-- Gradebook
    AcademicSystem *-- SemesterManager
    AcademicSystem *-- GradeConverter
    AcademicSystem *-- DataManager
    AcademicSystem *-- AnnualReportBuilder
    AcademicSystem o-- Subject
    Gradebook o-- Subject
    Subject o-- Assessments
    Subject o-- IObserver
    PriorityManager ..|> IObserver
    StatisticsManager ..|> IObserver

    Assessments --> ICalculationStrategy
    AssessmentFactory ..> Assessments
    AssessmentFactory ..> AverageStrategy
    AssessmentFactory ..> SumStrategy
    AssessmentFactory ..> SingleGradeStrategy

    AverageStrategy ..|> ICalculationStrategy
    SumStrategy ..|> ICalculationStrategy
    SingleGradeStrategy ..|> ICalculationStrategy
    StrategyFactory ..> ICalculationStrategy

    Subject ..> AssessmentPerformanceService
    Subject ..> GradePolicy
    Assessments ..> GradePolicy
    AssessmentPerformanceService ..> StrategyFactory
    AssessmentPerformanceService ..> GradePolicy
    SubjectGroupPerformanceService ..> StrategyFactory

    GradeConverter o-- ConversionRule
    DataManager ..> Gradebook
    DataManager ..> Subject
    DataManager ..> Assessments
    DataManager ..> AssessmentFactory

    AnnualReportBuilder ..> SubjectGroupPerformanceService
    AnnualReportBuilder ..> GradeConverter
    AnnualReportBuilder ..> YearlyReport
    YearlyReportSaver ..> YearlyReport


    SemesterManager ..> Gradebook
    SemesterManager ..> Subject
    SemesterManager ..> AssessmentFactory



