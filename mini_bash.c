#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

int main() {
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    int status;

    // Shell Loop: הלולאה המרכזית של מפרש הפקודות
    while (1) {
        printf("mini-bash$ ");
        fflush(stdout); // מוודא שהפרומפט יודפס מיד למסך

        // קריאת הקלט מהמשתמש
        if (!fgets(line, MAX_LINE, stdin)) break;

        // הסרת תו ירידת השורה מהקלט
        line[strcspn(line, "\n")] = '\0';

        // Parsing: פירוק השורה למילים (Tokens)
        int i = 0;
        args[i] = strtok(line, " \t");
        while (args[i] != NULL && i < MAX_ARGS - 1) {
            args[++i] = strtok(NULL, " \t");
        }

        if (args[0] == NULL) continue; // אם הוכנסה שורה ריקה

        // טיפול בפקודות פנימיות (Internal Commands)
        if (strcmp(args[0], "exit") == 0) {
            break; // יציאה מהלולאה וסיום התוכנית
        }

        if (strcmp(args[0], "cd") == 0) {
            // chdir System Call: משנה את תיקיית העבודה של התהליך הנוכחי
            // מחזירה 0 בהצלחה ו-1- בכישלון
            if (args[1] == NULL || chdir(args[1]) != 0) {
                perror("cd failed");
            }
            continue;
        }

        // הרצת פקודות חיצוניות באמצעות ניהול תהליכים
        
        // fork System Call: יוצר תהליך בן חדש שהוא עותק של האבא
        // מחזירה: 0 לתהליך הבן, PID של הבן לתהליך האבא, או 1- אם נכשלה
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed"); // הדפסת שגיאת מערכת מה-Kernel
        } 
        else if (pid == 0) {
            // --- תהליך הבן ---
            
            // execvp System Call: מחליפה את הקוד של הבן בפקודה החדשה
            // היא מחפשת את הפקודה ב-PATH ובתיקיית הבית
            // אם היא מצליחה, היא לא חוזרת. אם היא חוזרת, סימן שיש שגיאה
            execvp(args[0], args);
            
            // אם הגענו לכאן, הפקודה לא נמצאה או נכשלה
            fprintf(stderr, "%s: Unknown Command\n", args[0]);
            exit(1);
        } 
        else {
            // --- תהליך האבא ---
            
            // waitpid System Call: ממתינה לסיום תהליך הבן הספציפי
            // מחזירה את ה-PID של הבן שסיים או 1- בכישלון
            waitpid(pid, &status, 0);

            // בדיקה האם הבן סיים בצורה תקינה והדפסת סטטוס היציאה (Return Code)
            if (WIFEXITED(status)) {
                printf("Command exited with status %d\n", WEXITSTATUS(status));
            }
        }
    }

    return 0;
}