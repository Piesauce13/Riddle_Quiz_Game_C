#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "QuizManager.h"
#include "UserManager.h"
#include "Leaderboard.h"

char username[50];
int score = 0;
double timeTaken = 0.0;

void showWelcomeScreen(User *users, int *userCount);
void showMainMenu(User *users, int *userCount, const char *username);

int main() {
    User users[MAX_USERS];
    int userCount = 0;

    srand((unsigned int)time(NULL));
    userCount = loadUsers(users);
    showWelcomeScreen(users, &userCount);
    return 0;
}

void showWelcomeScreen(User *users, int *userCount) {
    char response[10];
    printf("|///////////////////////////////////////////////////////////|\n");
    printf("|                                                           |\n");
    printf("|            Welcome to the Riddle Quiz Game!               |\n");
    printf("|                                                           |\n");
    printf("|///////////////////////////////////////////////////////////|\n");
    printf("|                                                           |\n");
    printf("| Test your brainpower with a variety of riddles, ranging   |\n");
    printf("| from easy to challenging. Can you conquer them all?       |\n");
    printf("| Answer quickly, stay sharp, and see if you can top the    |\n");
    printf("| leaderboard!                                              |\n");
    printf("|___________________________________________________________|\n");
    pause();

    while (1) {
        clearConsole();
        printf("\nDo you have an account yet? (Yes/No): ");
        fgets(response, sizeof(response), stdin);
        response[strcspn(response, "\n")] = '\0';  // Remove newline
        toLowerCase(response);

        if (strcmp(response, "yes") == 0 || strcmp(response, "y") == 0) {
            if (login(users, *userCount, username)) {
                showMainMenu(users, userCount, username);
            } else {
                printf("\nLogin failed. Please try again.\n");
            }
        } else if (strcmp(response, "no") == 0 || strcmp(response, "n") == 0) {
            signUp(users, userCount);
        } else {
            printf("\nInvalid input. Please enter 'Yes' or 'No'.\n");
            pause();
        }
    }
}

void showMainMenu(User *users, int *userCount, const char *username) {
    int choice;

    while (1) {
        clearConsole();
        showUserHighscore();
        printf("\n");
        printf("/////////////////////////////////////////////////////////////////////////\n");
        printf("//                                                                     //\n");
        printf("//                             MAIN MENU                               //\n");
        printf("//                                                                     //\n");
        printf("/////////////////////////////////////////////////////////////////////////\n\n");
        printf("1.  Play Quiz\n");
        printf("2.  View Leaderboard\n");
        printf("3.  Edit Profile\n");
        printf("4.  Delete Account\n");
        printf("5.  Logout\n");
        printf("6.  Quit Game\n");
        printf("\nChoose an option: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: {
                Question easy[MAX_QUESTIONS], medium[MAX_QUESTIONS], hard[MAX_QUESTIONS];
                int eCount = 0, mCount = 0, hCount = 0;

                loadQuestions("EasyQuestions.txt", easy, &eCount);
                loadQuestions("MediumQuestions.txt", medium, &mCount);
                loadQuestions("HardQuestions.txt", hard, &hCount);

                playQuiz(username, easy, eCount, medium, mCount, hard, hCount);
                break;
            }
            case 2: {
                clearConsole();
                printf("Viewing Leaderboard...\n\n");
                showLeaderboard();
                pause();
                break;
            }
            case 3: {
                clearConsole();
                editProfile(users, userCount, (char *)username);
                saveUsers(users, *userCount);
                break;
            }
            case 4:
                clearConsole();
                deleteUserAccount(users, userCount, username);
                pause();
                clearConsole();
                showWelcomeScreen(users, userCount);
            case 5:
                printf("\nLogging out... Goodbye, %s!\n", username);
                pause();
                clearConsole();
                return;
            case 6:
                clearConsole();
                printf("\nExiting program... Thank you for playing!\n");
                pause();

                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}


