#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        toLowerCase(response);

        if (strcmp(response, "Yes") == 0 || strcmp(response, "Y") == 0) {
            char username[50];
            if (login(users, *userCount, username)) {
                showMainMenu(username);
            }
            strcpy(username, "User1"); // This should be part of login functionality
            showMainMenu(username);
        } else if (strcmp(response, "No") == 0 || strcmp(response, "N") == 0) {
            signUp(users, userCount);
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
            case 1:
                printf("Starting Quiz...\n");
                // Call quiz play function here
                break;
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

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.
