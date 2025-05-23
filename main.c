#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "UserManager.h"
#include "QuizManager.h"

void showWelcomeScreen(User *users, int *userCount);
void showMainMenu(User *users, int *userCount, const char *username);
void toLowerCase(char *str);

int main() {
    User users[MAX_USERS];
    int userCount = 0;
    
    srand(time(NULL));  // Initialize random seed for salt generation
    userCount = loadUsers(users);  // Load existing users at startup
    showWelcomeScreen(users, &userCount);
    return 0;
}

void showWelcomeScreen(User *users, int *userCount) {
    char response[10];
    printf("---------------------- Welcome to Riddle Quiz Game! ppp ----------------------\n");
    printf("\"Test yourself with challenging riddles across different difficulty levels.\"\n");

    while (1) {
        printf("\nDo you have an account yet? (Yes/No): ");
        fgets(response, sizeof(response), stdin);
        response[strcspn(response, "\n")] = '\0';  // Remove newline
        toLowerCase(response);

        if (strcmp(response, "yes") == 0 || strcmp(response, "y") == 0) {
            char username[MAX_LEN];
            if (login(users, *userCount, username)) {
                showMainMenu(users, userCount, username);
            }
        } else if (strcmp(response, "no") == 0 || strcmp(response, "n") == 0) {
            signUp(users, userCount);
        } else {
            printf("Invalid input. Please enter 'Yes' or 'No'.\n");
        }
    }
}

void showMainMenu(User *users, int *userCount, const char *username) {
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
            case 1:
                printf("Starting Quiz...\n");
            Question easy[MAX_QUESTIONS], medium[MAX_QUESTIONS], hard[MAX_QUESTIONS];
            int eCount = 0, mCount = 0, hCount = 0;

            loadQuestions("/Users/chanpanha/Documents/Riddle_Quiz_Game_C_1/EasyQuestions.txt", easy, &eCount);
            loadQuestions("/Users/chanpanha/Documents/Riddle_Quiz_Game_C_1/MediumQuestions.txt", medium, &mCount);
            loadQuestions("/Users/chanpanha/Documents/Riddle_Quiz_Game_C_1/HardQuestions.txt", hard, &hCount);

            playQuiz(username, easy, eCount, medium, mCount, hard, hCount);
            break;
            case 2:
                printf("Viewing Leaderboard...\n");

                break;
            case 3:
                printf("Editing Profile...\n");
                editProfile(users, userCount, (char *)username);
                break;
            case 4:
                printf("Logging out... Goodbye, %s!\n", username);
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

