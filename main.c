#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STUDENTS 100 
#define MAX_SUBJECTS 5
#define FILE_NAME "attendance.dat"
#define TARGET_PERCENT 75

const char *SUBJECT_NAMES[MAX_SUBJECTS] = {
    "Math",
    "Physics",
    "Chemistry",
    "Computer Science",
    "English"
};

typedef struct {
    char name[30];
    int held;
    int attended;
    float percentage;
} Subject;

typedef struct {
    int roll;
    char name[40];
    Subject subjects[MAX_SUBJECTS];
    int subjectCount;
    float overall;
} Student;

static Student students[MAX_STUDENTS];
static int studentCount = 0;


float calculatePercentage(int attended, int held) {
    if (held <= 0) {
        return 0.0;
    }
    return ((float)attended / held) * 100.0;
}

void updateOverall(Student *student) {
    int totalHeld = 0;
    int totalAttended = 0;
    int i;

    for (i = 0; i < MAX_SUBJECTS; i++) {
        totalHeld += student->subjects[i].held;
        totalAttended += student->subjects[i].attended;
        student->subjects[i].percentage = calculatePercentage(student->subjects[i].attended, student->subjects[i].held);
    }

    if (totalHeld <= 0) {
        student->overall = 0.0f;
    } else {
        student->overall = ((float)totalAttended / totalHeld) * 100.0f;
    }
}

void initializeStudent(Student *student) {
    int i;

    student->roll = 0;
    student->name[0] = '\0';
    student->subjectCount = MAX_SUBJECTS;

    for (i = 0; i < MAX_SUBJECTS; i++) {
        snprintf(student->subjects[i].name, sizeof(student->subjects[i].name), "%s", SUBJECT_NAMES[i]);
        student->subjects[i].held = 0;
        student->subjects[i].attended = 0;
        student->subjects[i].percentage = 0.0f;
    }

    student->overall = 0.0f;
}

int findStudentIndex(int roll) {
    int i;

    for (i = 0; i < studentCount; i++) {
        if (students[i].roll == roll) {
            return i;
        }
    }

    return -1;
}

void saveToFile(void) {
    FILE *fp = fopen(FILE_NAME, "wb");
    if (fp == NULL) {
        printf("Error: Unable to save data.\n");
        return;
    }

    fwrite(&studentCount, sizeof(studentCount), 1, fp);
    fwrite(students, sizeof(Student), studentCount, fp);
    fclose(fp);
    printf("Data saved successfully.\n");
}

void loadFromFile(void) {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) {
        studentCount = 0;
        return;
    }

    if (fread(&studentCount, sizeof(studentCount), 1, fp) != 1) {
        studentCount = 0;
        fclose(fp);
        return;
    }

    if (studentCount < 0 || studentCount > MAX_STUDENTS) {
        studentCount = 0;
        fclose(fp);
        return;
    }

    if (fread(students, sizeof(Student), studentCount, fp) != (size_t)studentCount) {
        studentCount = 0;
    }

    fclose(fp);
    printf("Previous data loaded.\n");
}

void showStudent(Student *student) {
    int i;

    printf("\nRoll No: %d\n", student->roll);
    printf("Name: %s\n", student->name);
    printf("%-22s %-10s %-12s %-8s\n", "Subject", "Held", "Attended", "%");
    printf("----------------------------------------------\n");

    for (i = 0; i < MAX_SUBJECTS; i++) {
        printf("%-22s %-10d %-12d %-7.2f\n",
               student->subjects[i].name,
               student->subjects[i].held,
               student->subjects[i].attended,
               student->subjects[i].percentage);
    }

    printf("Overall Attendance: %.2f%%\n", student->overall);
}

void listAllStudents(void) {
    int i;

    if (studentCount == 0) {
        printf("No student records found.\n");
        return;
    }

    printf("\nStudent List\n");
    printf("---------------------------\n");
    for (i = 0; i < studentCount; i++) {
        printf("%d. Roll %d - %s | Overall: %.2f%%\n",
               i + 1,
               students[i].roll,
               students[i].name,
               students[i].overall);
    }
}

void addStudent(void) {
    Student newStudent;
    int i;

    if (studentCount >= MAX_STUDENTS) {
        printf("Maximum student records reached.\n");
        return;
    }

    initializeStudent(&newStudent);

    printf("Enter roll number: ");
    if (scanf("%d", &newStudent.roll) != 1) {
        printf("Invalid roll number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (findStudentIndex(newStudent.roll) != -1) {
        printf("A student with this roll number already exists.\n");
        return;
    }

    printf("Enter student name: ");
    if (fgets(newStudent.name, sizeof(newStudent.name), stdin) == NULL) {
        printf("Invalid name.\n");
        return;
    }
    newStudent.name[strcspn(newStudent.name, "\r\n")] = '\0';

    for (i = 0; i < MAX_SUBJECTS; i++) {
        snprintf(newStudent.subjects[i].name, sizeof(newStudent.subjects[i].name), "%s", SUBJECT_NAMES[i]);
    }

    updateOverall(&newStudent);
    students[studentCount++] = newStudent;
    printf("Student added successfully.\n");
}

void updateStudent(void) {
    int roll;
    int index;
    int i;

    printf("Enter roll number to update: ");
    if (scanf("%d", &roll) != 1) {
        printf("Invalid roll number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    index = findStudentIndex(roll);
    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    printf("Enter new student name: ");
    if (fgets(students[index].name, sizeof(students[index].name), stdin) == NULL) {
        printf("Invalid name.\n");
        return;
    }
    students[index].name[strcspn(students[index].name, "\r\n")] = '\0';

    for (i = 0; i < MAX_SUBJECTS; i++) {
        printf("Enter held classes for %s: ", students[index].subjects[i].name);
        if (scanf("%d", &students[index].subjects[i].held) != 1) {
            printf("Invalid value.\n");
            clearInputBuffer();
            return;
        }

        printf("Enter attended classes for %s: ", students[index].subjects[i].name);
        if (scanf("%d", &students[index].subjects[i].attended) != 1) {
            printf("Invalid value.\n");
            clearInputBuffer();
            return;
        }
        clearInputBuffer();

        if (students[index].subjects[i].attended > students[index].subjects[i].held) {
            printf("Attended classes cannot be greater than held classes.\n");
            printf("Please re-enter the values for %s.\n", students[index].subjects[i].name);
            i--; 
            continue;
        }
    }

    updateOverall(&students[index]);
    printf("Student updated successfully.\n");
}

void deleteStudent(void) {
    int roll;
    int index;
    int i;

    printf("Enter roll number to delete: ");
    if (scanf("%d", &roll) != 1) {
        printf("Invalid roll number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    index = findStudentIndex(roll);
    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    for (i = index; i < studentCount - 1; i++) {
        students[i] = students[i + 1];
    }
    studentCount--;
    printf("Student deleted successfully.\n");
}

void markAttendance(void) {
    int roll;
    int index;
    int subjectChoice;

    printf("Enter roll number: ");
    if (scanf("%d", &roll) != 1) {
        printf("Invalid roll number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    index = findStudentIndex(roll);
    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    printf("Choose subject:\n");
    for (int i = 0; i < MAX_SUBJECTS; i++) {
        printf("%d. %s\n", i + 1, students[index].subjects[i].name);
    }
    printf("Enter subject number: ");
    if (scanf("%d", &subjectChoice) != 1) {
        printf("Invalid subject choice.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (subjectChoice < 1 || subjectChoice > MAX_SUBJECTS) {
        printf("Invalid subject number.\n");
        return;
    }

    subjectChoice--;

    printf("Enter total classes held: ");
    if (scanf("%d", &students[index].subjects[subjectChoice].held) != 1) {
        printf("Invalid value.\n");
        clearInputBuffer();
        return;
    }

    printf("Enter classes attended: ");
    if (scanf("%d", &students[index].subjects[subjectChoice].attended) != 1) {
        printf("Invalid value.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (students[index].subjects[subjectChoice].attended > students[index].subjects[subjectChoice].held) {
        printf("Attended classes cannot be more than held classes.\n");
        return;
    }

    updateOverall(&students[index]);
    printf("Attendance marked successfully.\n");
}

void showDefaulters(void) {
    Student temp[MAX_STUDENTS];
    int count = 0;
    int i, j;

    for (i = 0; i < studentCount; i++) {
        if (students[i].overall < TARGET_PERCENT) {
            temp[count++] = students[i];
        }
    }

    if (count == 0) {
        printf("No student is below the attendance requirement.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        for (j = i + 1; j < count; j++) {
            if (temp[i].overall > temp[j].overall) {
                Student swap = temp[i];
                temp[i] = temp[j];
                temp[j] = swap;
            }
        }
    }

    printf("\nDefaulter List (sorted by lowest attendance)\n");
    printf("------------------------------------------\n");
    for (i = 0; i < count; i++) {
        printf("Roll %d - %s | Overall: %.2f%%\n",
               temp[i].roll,
               temp[i].name,
               temp[i].overall);
    }
}

void computeTotalAttendance(const Student *student, int *totalHeld, int *totalAttended) {
    int i;

    *totalHeld = 0;
    *totalAttended = 0;

    for (i = 0; i < MAX_SUBJECTS; i++) {
        *totalHeld += student->subjects[i].held;
        *totalAttended += student->subjects[i].attended;
    }
}

int calculateRecoveryNeeded(int attended, int held) {
    int required;

    if (held <= 0 || attended >= (TARGET_PERCENT * held / 100)) {
        return 0;
    }

    required = (TARGET_PERCENT * held - 100 * attended + (100 - TARGET_PERCENT) - 1) / (100 - TARGET_PERCENT);
    return required > 0 ? required : 0;
}

void recoveryCalculator(void) {
    int roll;
    int index;
    int subjectChoice;
    int needed;
    int totalHeld = 0;
    int totalAttended = 0;

    printf("Enter roll number: ");
    if (scanf("%d", &roll) != 1) {
        printf("Invalid roll number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    index = findStudentIndex(roll);
    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    printf("Choose subject (1-%d) or overall attendance (0):\n", MAX_SUBJECTS);
    for (int i = 0; i < MAX_SUBJECTS; i++) {
        printf("%d. %s\n", i + 1, students[index].subjects[i].name);
    }
    printf("0. Overall\nEnter choice: ");
    if (scanf("%d", &subjectChoice) != 1) {
        printf("Invalid choice.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (subjectChoice == 0) {
        computeTotalAttendance(&students[index], &totalHeld, &totalAttended);
        needed = calculateRecoveryNeeded(totalAttended, totalHeld);
        if (needed == 0) {
            printf("This student already meets the required attendance.\n");
        } else {
            printf("This student needs to attend %d more consecutive classes to reach 75%% overall.\n", needed);
        }
        return;
    }

    if (subjectChoice < 1 || subjectChoice > MAX_SUBJECTS) {
        printf("Invalid choice.\n");
        return;
    }

    subjectChoice--;
    needed = calculateRecoveryNeeded(students[index].subjects[subjectChoice].attended,
                                    students[index].subjects[subjectChoice].held);

    if (needed == 0) {
        printf("This student already meets the attendance target for %s.\n",
               students[index].subjects[subjectChoice].name);
    } else {
        printf("The student needs to attend %d more consecutive classes in %s to reach 75%%.\n",
               needed,
               students[index].subjects[subjectChoice].name);
    }
}

void viewStudentDetails(void) {
    int roll;
    int index;

    printf("Enter roll number: ");
    if (scanf("%d", &roll) != 1) {
        printf("Invalid roll number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    index = findStudentIndex(roll);
    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    showStudent(&students[index]);
}

int main(void) {
    int choice;

    loadFromFile();

    while (1) {
        printf("\n=== Attendance Shortage Tracker ===\n");
        printf("1. Add student\n");
        printf("2. View all students\n");
        printf("3. View student details\n");
        printf("4. Update student\n");
        printf("5. Delete student\n");
        printf("6. Mark attendance\n");
        printf("7. Show defaulters\n");
        printf("8. Recovery calculator\n");
        printf("9. Save and exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                addStudent();
                break;
            case 2:
                listAllStudents();
                break;
            case 3:
                viewStudentDetails();
                break;
            case 4:
                updateStudent();
                break;
            case 5:
                deleteStudent();
                break;
            case 6:
                markAttendance();
                break;
            case 7:
                showDefaulters();
                break;
            case 8:
                recoveryCalculator();
                break;
            case 9:
                saveToFile();
                printf("Program ended.\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }

    return 0;
}
