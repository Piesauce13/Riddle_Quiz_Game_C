#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "QuizManager.h"

void showWelcomeScreen();
void showMainMenu(const char *username);
int loadUsers();
void saveUsers();

int main() {
    loadUsers();
    showWelcomeScreen();
    return 0;
}

void showWelcomeScreen() {
    char response[10];
    printf("---------------------- Welcome to Riddle Quiz Game! ----------------------\n");
    printf("\"Test yourself with challenging riddles across different difficulty levels.\"\n");

    while (1) {
        printf("\nDo you have an account yet? (Yes/No): ");
        fgets(response, sizeof(response), stdin);
        response[strcspn(response, "\n")] = '\0';  // Remove newline

        if (strcmp(response, "Yes") == 0 || strcmp(response, "Y") == 0) {
            char username[50];
            // Login function can return the username
            // If successful, call showMainMenu
            // For now, hardcoding a username
            strcpy(username, "User1"); // This should be part of login functionality
            showMainMenu(username);
        } else if (strcmp(response, "No") == 0 || strcmp(response, "N") == 0) {
            // Sign up function
            // loadUsers, saveUsers etc.
        } else {
            printf("Invalid input. Please enter 'Yes' or 'No'.\n");
        }
    }
}

void showMainMenu(const char *username) {
    int choice;

    while (1) {
        printf("\n\n                                Main Menu                                \n");
        printf("1. Play Quiz\n");
        printf("2. View Leaderboard\n");
        printf("3. Edit Profile\n");
        printf("4. Logout\n");
        printf("Choose an option: ");
        scanf("%d", &choice);
        getchar(); // to consume the newline character

        switch (choice) {
            case 1: {
                printf("Starting Quiz...\n");
                Question easy[MAX_QUESTIONS], medium[MAX_QUESTIONS], hard[MAX_QUESTIONS];
                int eCount = 0, mCount = 0, hCount = 0;

                loadQuestions("EasyQuestions.txt", easy, &eCount);
                loadQuestions("MediumQuestions.txt", medium, &mCount);
                loadQuestions("HardQuestions.txt", hard, &hCount);

                char username[50];
                printf("Enter your username: ");
                scanf("%s", username);

                playQuiz(username, easy, eCount, medium, mCount, hard, hCount);
                break;
            }
            case 2:
                printf("Viewing Leaderboard...\n");
                // Call leaderboard display function
                break;
            case 3:
                printf("Editing Profile...\n");
                // Call profile edit function
                break;
            case 4:
                printf("Logging out... Goodbye, %s!\n", username);
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

int loadUsers() {
    FILE *file = fopen("users.txt", "r");
    if (!file) {
        printf("No previous user data found. Starting fresh.\n");
        return 0;
    }

    // Load users logic
    fclose(file);
    return 1;
}

void saveUsers() {
    FILE *file = fopen("users.txt", "w");
    if (!file) {
        printf("Error saving users.\n");
        return;
    }

    // Save users logic
    fclose(file);
}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.