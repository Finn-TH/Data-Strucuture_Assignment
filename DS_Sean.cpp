#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
using namespace std;
//---------------------------------------------------------------------------------------------------------------

// Define the WeeklyCases struct
struct WeeklyCases {
    int year;
    int totalCases;
};

//Define Constant Logins 
const std::string username = "123";
const std::string password = "123";


//---------------------------------------------------------------------------------------------------------------


std::string** getContent(const std::string& fileName, int& numRows, int& numCols) {
    std::ifstream file(fileName);
    if (!file) {
        std::cout << "File can't be opened..." << std::endl;
        return nullptr;
    }

    std::string line, word;
    int row = 0;

    // Count the number of rows and columns in the CSV file
    while (std::getline(file, line)) {
        numRows++;
        if (row == 0) {
            std::istringstream ss(line);
            while (std::getline(ss, word, ','))
                numCols++;
        }
        row++;
    }

    file.clear();
    file.seekg(0);

    // Allocate memory for the 2D array to store the CSV content
    std::string** content = new std::string*[numRows];
    for (int i = 0; i < numRows; i++) {
        content[i] = new std::string[numCols];
    }

    row = 0;
    // Read data from the file and store it in the 2D array
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        int col = 0;
        while (std::getline(ss, word, ',')) {
            content[row][col] = word;
            col++;
        }
        row++;
    }

    return content;
}


//---------------------------------------------------------------------------------------------------------------

void deallocateContent(std::string** content, int numRows) {
    for (int i = 0; i < numRows; i++) {
        delete[] content[i];
    }
    delete[] content;
}


//---------------------------------------------------------------------------------------------------------------

void calculateTotalAnnualCases(std::string** content, int numRows, int numCols) {
    const int maxYear = 9999;
    int* totalCasesByYear = new int[maxYear + 1](); // Initialize all elements to 0

    // Calculate total cases by year
    for (int i = 1; i < numRows; ++i) {
        int year = std::stoi(content[i][0]);
        int total = 0;
        for (int j = 2; j < numCols; ++j) { // Starting from column 3 to calculate cases
            total += std::stoi(content[i][j]);
        }
        totalCasesByYear[year] += total;
    }

    // Display available years
    std::cout << "Available Years:" << std::endl;
    for (int i = 0; i <= maxYear; ++i) {
        if (totalCasesByYear[i] > 0) {
            std::cout << i << std::endl;
        }
    }

    // Ask the user to input the year(s) to view total cases
    std::cout << "Enter the year(s) to view total cases (separated by space, e.g., 2018 2020): ";
    std::string input;
    std::getline(std::cin, input); // Clear the input buffer
    std::getline(std::cin, input);

    std::istringstream iss(input);
    int year;
    bool found = false;
    std::cout << "\nYear | Total Cases" << std::endl;
    std::cout << "===================" << std::endl;
    while (iss >> year) {
        if (year >= 0 && year <= maxYear && totalCasesByYear[year] > 0) {
            std::cout << year << "   | " << totalCasesByYear[year] << std::endl;
            found = true;
        }
    }

    if (!found) {
        std::cout << "No data available for the selected year(s)." << std::endl;
    }

    delete[] totalCasesByYear;
}


//---------------------------------------------------------------------------------------------------------------




void calculateTotalCasesByState(std::string** content, int numRows, int numCols) {
    const int numStates = numCols - 2; // Subtracting 2 for Year and Age Group columns

    // Create arrays to store the total cases for each state
    int* totalCasesByState = new int[numStates]();

    // Accumulate cases for each state
    for (int i = 2; i < numCols; ++i) { // Start from index 2 (skipping Year and Age Group)
        for (int j = 1; j < numRows; ++j) {
            if (content[j][i].find_first_not_of("0123456789") == std::string::npos) {
                try {
                    totalCasesByState[i - 2] += std::stoi(content[j][i]); // Adjusting the index
                } catch (std::invalid_argument const& e) {
                    std::cerr << "Invalid argument: " << e.what() << '\n';
                    // Handle invalid data as needed
                }
            }
        }
    }

    // Display available states
    std::cout << "Available States:" << std::endl;
    for (int i = 2; i < numCols; ++i) {
        std::cout << content[0][i] << std::endl;
    }

    // Ask the user to input the state(s) to view total cases
    std::string input;
    std::cout << "Enter the state(s) to view total cases (separated by space, e.g., JOHOR KEDAH): ";
    std::getline(std::cin, input); // Clear the input buffer
    std::getline(std::cin, input);

    std::istringstream iss(input);
    std::string state;
    bool found = false;
    std::cout << "\nState      | Total Cases" << std::endl;
    std::cout << "=========================" << std::endl;
    while (iss >> state) {
        // Convert user input to uppercase for consistency
        std::transform(state.begin(), state.end(), state.begin(), ::toupper);

        for (int i = 2; i < numCols; ++i) {
            std::string stateInData = content[0][i];
            // Convert state in data to uppercase and remove spaces for comparison
            std::string stateInDataModified = content[0][i];
            stateInDataModified.erase(std::remove(stateInDataModified.begin(), stateInDataModified.end(), ' '), stateInDataModified.end());
            std::transform(stateInDataModified.begin(), stateInDataModified.end(), stateInDataModified.begin(), ::toupper);

            if (state == stateInDataModified) {
                std::cout << content[0][i] << " | " << totalCasesByState[i - 2] << std::endl;
                found = true;
                break;
            }
        }
    }

    if (!found) {
        std::cout << "No data available for the selected state(s)." << std::endl;
    }

    delete[] totalCasesByState;
}


//---------------------------------------------------------------------------------------------------------------


void handleChoice(std::string** content, int numRows, int numCols) {
    std::string choice;
    std::cout << "Select (Y) for Yearly cases or (S) for State cases: ";
    std::cin >> choice;

    if (choice == "Y" || choice == "y") {
        calculateTotalAnnualCases(content, numRows, numCols);
    } else if (choice == "S" || choice == "s") {
        calculateTotalCasesByState(content, numRows, numCols);
    } else {
        std::cout << "Invalid choice. Please select (Y) for Yearly cases or (S) for State cases." << std::endl;
    }
}



//---------------------------------------------------------------------------------------------------------------

struct PersonalInfo {
    std::string name = "John Doe";
    std::string age = "30";
    std::string gender = "Male";
    std::string email = "johndoe@example.com";
    std::string city = "New York";
    std::string phoneNumber = "123-456-7890";
};


//---------------------------------------------------------------------------------------------------------------

void viewInfo(const PersonalInfo& info) {
    system("cls||clear");

    std::cout << "---- Personal Information ----" << std::endl;
    std::cout << "Name: " << info.name << std::endl;
    std::cout << "Age: " << info.age << std::endl;
    std::cout << "Gender: " << info.gender << std::endl;
    std::cout << "Email: " << info.email << std::endl;
    std::cout << "City: " << info.city << std::endl;
    std::cout << "Phone Number: " << info.phoneNumber << std::endl;
    std::cout << "-----------------------------" << std::endl;
}

//---------------------------------------------------------------------------------------------------------------


void updateInfo(PersonalInfo& info) {
    system("cls||clear");

    viewInfo(info);

    std::cout << "---- Update Personal Information ----" << std::endl;
    std::cout << "Enter Email: ";
    std::cin >> info.email;

    std::cout << "Enter City: ";
    std::cin >> info.city;

    std::cout << "Enter Phone Number: ";
    std::cin >> info.phoneNumber;

    std::cout << "Information updated successfully!" << std::endl;
    std::cout << "-----------------------------" << std::endl;

    viewInfo(info);
}

//---------------------------------------------------------------------------------------------------------------


void UserMenu(std::string** content, int numRows, int numCols) {
    PersonalInfo userPersonalInfo;
    userPersonalInfo.phoneNumber = "1234567890";

    std::string inputUsername, inputPassword;

    system("cls||clear");
    std::cout << "---- Login ----" << std::endl;
    std::cout << "Enter Username: ";
    std::cin >> inputUsername;

    std::cout << "Enter Password: ";
    std::cin >> inputPassword;

    if (inputUsername == username && inputPassword == password) {
        system("cls||clear");
        std::cout << "Login successful!" << std::endl;

        int choice;

        while (true) {
            std::cout << "---- User Information Menu ----" << std::endl;
            std::cout << "1. View Personal Information" << std::endl;
            std::cout << "2. Update Personal Information" << std::endl;
            std::cout << "3. Total Dengue Cases" << std::endl;
            std::cout << "4. Exit" << std::endl;
            std::cout << "Enter your choice: ";
            std::cin >> choice;

            switch (choice) {
                case 1:
                    viewInfo(userPersonalInfo);
                    break;
                case 2:
                    updateInfo(userPersonalInfo);
                    break;
                case 3:
                    handleChoice(content, numRows, numCols);
                    break;
                case 4:
                    system("cls||clear");
                    std::cout << "Exiting..." << std::endl;
                    return;
                default:
                    system("cls||clear");
                    std::cout << "Invalid choice. Please try again." << std::endl;
            }
        }
    } else {
        system("cls||clear");
        std::cout << "Invalid username or password. Login failed." << std::endl;
    }
}


//---------------------------------------------------------------------------------------------------------------







int main() {
    int numRows = 0;
    int numCols = 0;
    string** content = getContent("AnnDengueCases.csv", numRows, numCols);

    if (content) {
        UserMenu(content, numRows, numCols);
        deallocateContent(content, numRows);
    } else {
        std::cout << "Failed to retrieve data from the file." << std::endl;
    }

    return 0;
}



