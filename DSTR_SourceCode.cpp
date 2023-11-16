//----------{IMPORTED LIBRARIES}----------

#include <iostream> // input/output
#include <fstream>
#include <sstream> //string stream
#include <string>  // enable string usage
#include <algorithm>
#include <iomanip>
#include <cstdlib>
using namespace std;

//----------{GLOBAL VARIABLES}----------

string loginFailedMsg = "Invalid credentials!"; // global variable for invalid login message
string invalidInput = "Invalid input!";         // global variable for invalid input message
string inp;                                     // to clear the screen by input
string loggedInUser;                            // to store name for user and doctor menu

//----------{STRUCTURES}----------

struct User // linked lists to store users
{
    string username;
    string password;
    string state;
    int age;
    string phoneNum;
    bool isActive;
    User *next;
};

struct Doctor // linked lists to store doctors
{
    string name;
    string password;
    string state;
    string phoneNum;
    Doctor *next;
};

struct WeeklyCases // linked lists to store weekly cases for admin function
{
    int year;
    int week;
    int totalCases;
};

struct StateData // struct to store state names
{
    int numStates;
    string *stateNames;
};

struct Date // struct to store date
{
    int day;
    int month;
    int year;
};

struct DenguePatient // linked lists to store
{
    string username;
    string state;
    int age;
    Date date; // Use the Date struct for the date
    string userNum;
    string docName;
    string docNum;
    DenguePatient *next;
};

//----------{INITIALIZING LINKED LISTS}----------

User *userHead = nullptr; // initialize singly linked list for user accounts

Doctor *doctorHead = nullptr; // initialize singly linked list for doctor accounts

DenguePatient *dengueCases = nullptr; // initialize singly linked list for doctor accounts

//----------{GENERAL FUNCTIONS}----------

void clearTerminal() // to clear the terminal before displaying the next output
{
#ifdef _WIN32
    // For Windows systems
    system("cls");
#else
    // For Unix-based systems (Linux, macOS)
    system("clear");
#endif
}

void inpCheck() // to freeze the terminal before clearing the terminal by input
{
    cout << "Press any key to proceed: ";
    cin >> inp;
}

namespace CSVReader // to read the CSV file
{
    string **getContent(string fileName, int &numRows, int &numCols)
    {
        ifstream file(fileName);
        if (!file)
        {
            cout << "File can't be opened..." << endl;
            return nullptr;
        }

        string line, word;
        int row = 0;

        while (getline(file, line))
        {
            numRows++;
            if (row == 0)
            {
                istringstream ss(line);
                while (getline(ss, word, ','))
                    numCols++;
            }
            row++;
        }

        string **content = new string *[numRows];
        for (int i = 0; i < numRows; i++)
        {
            content[i] = new string[numCols];
        }

        file.clear();
        file.seekg(0);

        row = 0;
        while (getline(file, line))
        {
            istringstream ss(line);
            int col = 0;
            while (getline(ss, word, ','))
            {
                content[row][col] = word;
                col++;
            }
            row++;
        }

        return content;
    }
}

StateData getStates() // calls StateData struct to store the state names as array from the CSV file
{
    StateData stateData;
    int numRows = 0;
    int numCols = 0;

    string **content = CSVReader::getContent("Number of dengue fever cases weekly by state.csv", numRows, numCols);

    if (content)
    {
        stateData.numStates = numCols - 2; // Number of state columns
        stateData.stateNames = new string[stateData.numStates];

        for (int i = 2; i < numCols; i++)
        {
            stateData.stateNames[i - 2] = content[0][i];
        }
    }

    return stateData;
}

void displayStates() // to display all the existing states based on the CSV file
{
    int numRows = 0;
    int numCols = 0;
    string **content = CSVReader::getContent("Annual number of dengue cases by state.csv", numRows, numCols);

    // Display available states
    cout << "Available States:" << endl;
    for (int i = 2; i < numCols; ++i)
    {
        cout << content[0][i] << endl;
    }
}

bool checkState(const string &state) // to check if the input state is valid
{
    StateData statesData = getStates();

    for (int i = 0; i < statesData.numStates; ++i)
    {
        string statesModified = statesData.stateNames[i];
        statesModified.erase(remove_if(statesModified.begin(), statesModified.end(), ::isspace), statesModified.end());
        if (statesModified == state)
        {
            delete[] statesData.stateNames; // Don't forget to free memory allocated dynamically
            return true;                    // Found the state in the array
        }
    }

    delete[] statesData.stateNames; // Free memory before returning if state not found
    return false;                   // State not found or file not read properly
}

string stateForDisplay(const string &state) // to properly display the states
{
    StateData statesData = getStates();

    for (int i = 0; i < statesData.numStates; ++i)
    {
        // to display state in displayUser()
        string currentState = statesData.stateNames[i];
        string statesModified = statesData.stateNames[i];
        statesModified.erase(remove_if(statesModified.begin(), statesModified.end(), ::isspace), statesModified.end());
        if (statesModified == state)
        {
            delete[] statesData.stateNames; // Don't forget to free memory allocated dynamically
            return currentState;            // Found the state in the array
        }
    }
}

bool isUsernameTaken(const string &username) // to check if username of new user is already taken
{
    User *currentUser = userHead;

    while (currentUser != nullptr)
    {
        if (currentUser->username == username)
        {
            return true; // Username is found, it's already taken
        }
        currentUser = currentUser->next;
    }

    return false; // Username is not taken
}

string capitalize(const string &word) // to capitalize doctor's name
{
    string capitalizedWord = word; // Make a copy of the word to preserve the original

    if (!capitalizedWord.empty())
    {
        capitalizedWord[0] = toupper(capitalizedWord[0]);
    }

    return capitalizedWord;
}

string formatDate(int day, int month, int year) // to format date variables
{
    // Format the date to "DD/MM/YYYY"
    return to_string(day) + "/" + to_string(month) + "/" + to_string(year);
}

bool isValidDate(const Date &date) // to check if inputted date is a valid date
{
    if (date.year < 0)
    {
        return false;
    }

    if (date.month < 1 || date.month > 12)
    {
        return false;
    }

    int maxDays = 31; // By default, consider maximum days as 31
    if (date.month == 4 || date.month == 6 || date.month == 9 || date.month == 11)
    {
        maxDays = 30;
    }
    else if (date.month == 2)
    {
        if ((date.year % 400 == 0) || ((date.year % 100 != 0) && (date.year % 4 == 0)))
        {
            maxDays = 29; // Leap year
        }
        else
        {
            maxDays = 28; // Non-leap year
        }
    }

    if (date.day < 1 || date.day > maxDays)
    {
        return false;
    }

    return true;
}

bool isDateInRange(const Date &date, const Date &startDate, const Date &endDate) // to check date range for user function
{
    // Convert dates to a comparable integer format (e.g., YYYYMMDD)
    int start = startDate.year * 10000 + startDate.month * 100 + startDate.day;
    int end = endDate.year * 10000 + endDate.month * 100 + endDate.day;
    int current = date.year * 10000 + date.month * 100 + date.day;

    return (current >= start && current <= end);
}

void sortWeeklyCases(WeeklyCases arr[], int low, int high) // to sort weekly cases using quick sort for the admin function
{
    if (low < high)
    {
        int pivot = arr[high].totalCases;
        int i = low - 1;

        for (int j = low; j < high; j++)
        {
            if (arr[j].totalCases < pivot)
            {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);

        int pi = i + 1;

        sortWeeklyCases(arr, low, pi - 1);
        sortWeeklyCases(arr, pi + 1, high);
    }
}

void updateUser(const string &username) // to update user details for user and admin function
{
    string password, state, phoneNum;
    int age;
    bool found = false, stateFound = false;
    User *currentUser = userHead;

    while (currentUser != nullptr)
    {
        if (currentUser->username == username)
        {
            found = true;
            cout << "Enter new password: ";
            cin >> password;
            cout << endl;
            displayStates();
            cout << "Enter new state (no spaces, e.g. PULAUPINANG): ";
            cin >> state;
            transform(state.begin(), state.end(), state.begin(), ::toupper);
            stateFound = checkState(state);

            if (stateFound)
            {
                cout << "Set Age: ";
                cin >> age;
                cout << "Set Phone Number (e.g. 0123456789): ";
                cin >> phoneNum;
                currentUser->password = password;
                currentUser->state = state;
                currentUser->age = age;
                currentUser->phoneNum = phoneNum;
                cout << "User information updated successfully\n";
            }
            else
                cout << "State not found...\n";
        }
        currentUser = currentUser->next;

        if (!found)
        {
            cout << "User not found\n";
        }
    }
    inpCheck();
}

void updateDoctor(const string &name) // to update doctor details for doctor and admin function
{
    string password, state, phoneNum;

    bool found = false, stateFound = false;
    Doctor *currentDoctor = doctorHead;

    while (currentDoctor != nullptr)
    {
        if (currentDoctor->name == name)
        {
            found = true;
            cout << "Enter new password: ";
            cin >> password;
            cout << endl;
            displayStates();
            cout << "Enter new state (no spaces, e.g. PULAUPINANG): ";
            cin >> state;
            transform(state.begin(), state.end(), state.begin(), ::toupper);
            stateFound = checkState(state);
            if (stateFound)
            {
                cout << "Set new phone number (e.g. 0123456789): ";
                cin >> phoneNum;
                currentDoctor->password = password;
                currentDoctor->state = state;
                currentDoctor->phoneNum = phoneNum;
                cout << "User information updated successfully\n";
            }
            else
                cout << "State not found...\n";

            cout << "Doctor information updated successfully\n";
            break;
        }
        currentDoctor = currentDoctor->next;
    }

    if (!found)
    {
        cout << "Doctor not found\n";
    }
    inpCheck();
}

//----------{USER FUNCTIONS}----------

void manageUserInfo() // manage, view, and update personal user details
{
    clearTerminal();
    int choice;
    User *currentUser = userHead;
    do
    {
        while (currentUser->username != loggedInUser)
        {
            currentUser = currentUser->next;
        }

        clearTerminal();
        cout << "\n----- Personal Information -----" << endl;
        cout << "Username: " << currentUser->username << endl;
        cout << "Password: " << currentUser->password << endl;
        cout << "State: " << currentUser->state << endl;
        cout << "Age: " << currentUser->age << endl;
        cout << "Phone Number: " << currentUser->phoneNum << endl;
        cout << "Account Status: " << (currentUser->isActive ? "Active" : "Inactive") << endl;
        cout << "-----------------------------" << endl;
        cout << "1. Update Information" << endl;
        cout << "2. Exit" << endl;
        cout << "Select your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            clearTerminal();
            updateUser(currentUser->username);
            break;
        }
        case 2:
            break;
        default:
            cout << invalidInput << endl;
        }
    } while (choice != 2);
}

void userDisplayCases() // to view total dengue cases based on the selected year(s) or state(s)
{
    int choice;
    do
    {
        clearTerminal();
        int numRows = 0;
        int numCols = 0;
        string **content = CSVReader::getContent("Annual number of dengue cases by state.csv", numRows, numCols);

        cout << "\nTotal Dengue Cases:" << endl;
        cout << "1. View Total Cases by Year" << endl;
        cout << "2. View Total Cases by State" << endl;
        cout << "3. Exit" << endl;
        cout << "Select your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
        {
            const int maxYear = 9999;
            int *totalCasesByYear = new int[maxYear + 1](); // Initialize all elements to 0

            // Calculate total cases by year
            for (int i = 1; i < numRows; ++i)
            {
                int year = stoi(content[i][0]);
                int total = 0;
                for (int j = 2; j < numCols; ++j)
                { // Starting from column 3 to calculate cases
                    total += stoi(content[i][j]);
                }
                totalCasesByYear[year] += total;
            }

            // Display available years
            clearTerminal();
            cout << "\nAvailable Years:" << endl;
            for (int i = 0; i <= maxYear; ++i)
            {
                if (totalCasesByYear[i] > 0)
                {
                    cout << i << endl;
                }
            }

            // Ask the user to input the year(s) to view total cases
            cout << "\nEnter the year(s) to view total cases (separated by space, e.g., 2018 2020): ";
            string input;
            getline(cin, input); // Clear the input buffer
            getline(cin, input);

            clearTerminal();
            istringstream iss(input);
            int year;
            bool found = false;
            cout << "\nYear | Total Cases" << endl;
            cout << "===================" << endl;
            while (iss >> year)
            {
                if (year >= 0 && year <= maxYear && totalCasesByYear[year] > 0)
                {
                    cout << year << "   | " << totalCasesByYear[year] << endl;
                    found = true;
                }
            }
            if (!found)
            {
                cout << "No data available for the selected year(s)." << endl;
            }

            // Clean up: deallocate the dynamic array
            for (int i = 0; i < numRows; i++)
            {
                delete[] content[i];
            }
            delete[] totalCasesByYear;
            break;
        }
        case 2:
        {
            const int numStates = numCols - 2; // Subtracting 2 for Year and Age Group columns

            // Create arrays to store the total cases for each state
            int *totalCasesByState = new int[numStates]();

            // Accumulate cases for each state
            for (int i = 2; i < numCols; ++i)
            { // Start from index 2 (skipping Year and Age Group)
                for (int j = 1; j < numRows; ++j)
                {
                    if (content[j][i].find_first_not_of("0123456789") == string::npos)
                    {
                        try
                        {
                            totalCasesByState[i - 2] += stoi(content[j][i]); // Adjusting the index
                        }
                        catch (invalid_argument const &e)
                        {
                            cerr << "Invalid argument: " << e.what() << '\n';
                            // Handle invalid data as needed
                        }
                    }
                }
            }

            clearTerminal();
            displayStates();
            // Ask the user to input the state(s) to view total cases
            string input;
            cout << "\nEnter the state(s) to view total cases (separated by space, e.g., JOHOR KEDAH): ";
            getline(cin, input); // Clear the input buffer
            getline(cin, input);

            clearTerminal();
            istringstream iss(input);
            string state;
            bool found = false;
            cout << "\nState      | Total Cases" << endl;
            cout << "=========================" << endl;
            while (iss >> state)
            {
                // Convert user input to uppercase for consistency
                transform(state.begin(), state.end(), state.begin(), ::toupper);

                for (int i = 2; i < numCols; ++i)
                {
                    string stateInData = content[0][i];
                    // Convert state in data to uppercase and remove spaces for comparison
                    string stateInDataModified = content[0][i];
                    stateInDataModified.erase(remove(stateInDataModified.begin(), stateInDataModified.end(), ' '), stateInDataModified.end());
                    transform(stateInDataModified.begin(), stateInDataModified.end(), stateInDataModified.begin(), ::toupper);

                    if (state == stateInDataModified)
                    {
                        cout << content[0][i] << " | " << totalCasesByState[i - 2] << endl;
                        found = true;
                        break;
                    }
                }
            }

            if (!found)
            {
                cout << "No data available for the selected state(s)." << endl;
            }
            // Clean up: deallocate the dynamic array
            for (int i = 0; i < numRows; i++)
            {
                delete[] content[i];
            }
            delete[] totalCasesByState;
            break;
        }
        case 3:
        {
            break;
        }
        default:
            cout << invalidInput << endl;
        }

        if (choice == 1 || choice == 2)
            inpCheck();
    } while (choice != 3);
}

void dailyCasesByState() // view daily dengue cases in descending order based on state
{
    clearTerminal();
    bool exitFlag = false;
    // Create a linked list to store state and cases pairs
    struct StateCases
    {
        string state;
        int cases;
        StateCases *next;
    };

    StateCases *stateCasesList = nullptr;

    DenguePatient *currentPatient = dengueCases;

    // Iterate through the DenguePatient linked list and count cases for each state
    while (currentPatient != nullptr)
    {
        string state = currentPatient->state;

        // Search for the state in the stateCasesList
        StateCases *stateNode = stateCasesList;
        StateCases *previousNode = nullptr;
        bool found = false;

        while (stateNode != nullptr)
        {
            if (stateNode->state == state)
            {
                stateNode->cases++;
                found = true;
                break;
            }
            previousNode = stateNode;
            stateNode = stateNode->next;
        }

        if (!found)
        {
            // State not found in the list, create a new node with 0 cases
            StateCases *newStateNode = new StateCases;
            newStateNode->state = state;
            newStateNode->cases = 1; // Initialize to 1 case
            newStateNode->next = nullptr;

            if (previousNode != nullptr)
            {
                previousNode->next = newStateNode;
            }
            else
            {
                stateCasesList = newStateNode;
            }
        }

        currentPatient = currentPatient->next;
    }

    // Display cases by state in descending order
    cout << "Daily Dengue Cases Per States" << endl;
    cout << "Cases  |  States" << endl;

    // Iterate through all possible states and check if they are in stateCasesList
    // If a state is not found, add it with 0 cases
    StateData statesData = getStates(); // Assuming you have a function to get all possible states
    for (int i = 0; i < statesData.numStates; i++)
    {
        bool stateFound = false;
        StateCases *currentStateNode = stateCasesList;
        while (currentStateNode != nullptr)
        {
            if (currentStateNode->state == statesData.stateNames[i])
            {
                cout << setw(5) << currentStateNode->cases << "  |  " << currentStateNode->state << endl;
                stateFound = true;
                break;
            }
            currentStateNode = currentStateNode->next;
        }
        if (!stateFound)
        {
            // State not found in the list, add it with 0 cases
            cout << setw(5) << "0"
                 << "  |  " << statesData.stateNames[i] << endl;
        }
    }

    // Clean up the dynamically allocated memory
    while (stateCasesList != nullptr)
    {
        StateCases *temp = stateCasesList;
        stateCasesList = stateCasesList->next;
        delete temp;
    }

    inpCheck();
}

void viewAlerts() // view alert message sent by the admin for the user's state
{
    clearTerminal();
    StateData statesData = getStates();
    int numStates = statesData.numStates;
    string *alertedStates = new string[numStates]();
    User *currentUser = userHead;
    bool foundAlert = false;

    for (int i = 0; i < numStates; ++i)
    {
        int patientsCount = 0;
        DenguePatient *currentPatient = dengueCases;

        while (currentPatient != nullptr)
        {
            if (currentPatient->state == statesData.stateNames[i])
            {
                patientsCount++;
            }
            currentPatient = currentPatient->next;
        }

        if (patientsCount >= 20)
        {
            alertedStates[i] = statesData.stateNames[i];
        }
    }

    while (currentUser != nullptr)
    {
        if (loggedInUser == currentUser->username)
        {
            for (int i = 0; i < numStates; i++)
            {
                if (alertedStates[i] == currentUser->state)
                {
                    string state = stateForDisplay(currentUser->state);
                    cout << "\nALERT! Your state, " << state << ", has a high number of dengue reports\n";
                    foundAlert = true;
                }
            }
        }
        currentUser = currentUser->next;
    }

    if (!foundAlert)
    {
        cout << "\nThere are currently no alerts for your state...\n";
    }

    delete[] alertedStates;
    inpCheck();
}

void personalDengueReports() // view the user's dengue reports based on a selected date range
{
    clearTerminal();

    // Prompt the user for the date range
    Date startDate, endDate;
    cout << "View Past Dengue Records\n";
    cout << "Enter Start Date (DD MM YYYY): ";
    cin >> startDate.day >> startDate.month >> startDate.year;

    cout << "Enter End Date (DD MM YYYY): ";
    cin >> endDate.day >> endDate.month >> endDate.year;

    DenguePatient *currentPatient = dengueCases;

    cout << "\nDengue Patients: " << endl;
    cout << setfill('=') << setw(175) << "=" << setfill(' ') << endl;
    cout << left << setw(25) << "Patient Name" << setw(25) << "Patient's State" << setw(25) << "Age" << setw(25) << "Date" << setw(25) << "Patient's Phone No." << setw(25) << "Assigned Doctor" << setw(25) << "Doctor's Phone No." << endl;
    cout << setfill('=') << setw(175) << "=" << setfill(' ') << endl;

    while (currentPatient != nullptr)
    {
        // Check if the username matches and the date is within the specified range
        if (currentPatient->username == loggedInUser &&
            isDateInRange(currentPatient->date, startDate, endDate))
        {
            // Display the patient information
            cout << left << setw(25) << currentPatient->username << setw(25) << currentPatient->state << setw(25) << to_string(currentPatient->age) << setw(25) << formatDate(currentPatient->date.day, currentPatient->date.month, currentPatient->date.year) << setw(25) << currentPatient->userNum << setw(25) << currentPatient->docName << setw(25) << currentPatient->docNum << endl;
        }
        currentPatient = currentPatient->next;
    }

    cout << setfill('=') << setw(175) << "=" << setfill(' ') << endl;
    inpCheck();
}

//----------{DOCTORS FUNCTIONS}----------

void manageDoctorInfo() // manage, view, and update personal user details
{
    clearTerminal();
    int choice;
    Doctor *currentDoctor = doctorHead;
    do
    {
        while (currentDoctor->name != loggedInUser)
        {
            currentDoctor = currentDoctor->next;
        }

        clearTerminal();
        cout << "\n----- Personal Information -----" << endl;
        cout << "Username: Dr. " << currentDoctor->name << endl;
        cout << "Password: " << currentDoctor->password << endl;
        cout << "State: " << currentDoctor->state << endl;
        cout << "Phone Number: " << currentDoctor->phoneNum << endl;
        cout << "-----------------------------" << endl;
        cout << "1. Update Information" << endl;
        cout << "2. Exit" << endl;
        cout << "Select your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            clearTerminal();
            updateDoctor(currentDoctor->name);
            break;
        }
        case 2:
            break;
        default:
            cout << invalidInput << endl;
        }
    } while (choice != 2);
}

void reportPatient() // to make new dengue reports
{
    clearTerminal();
    DenguePatient *newPatient = new DenguePatient();
    User *currentUser = userHead;
    Doctor *currentDoctor = doctorHead;
    int age;
    string username, state, userNum, docName, docNum;
    Date date;
    bool userFound = false;

    cout << "-----Report a Dengue Patient-----" << endl;
    cout << "Enter Patient Name: ";
    cin >> username;

    while (currentUser != nullptr)
    {
        if (currentUser->username == username)
        {
            state = stateForDisplay(currentUser->state);
            age = currentUser->age;
            userNum = currentUser->phoneNum;
            userFound = true;
        }
        currentUser = currentUser->next;
    }

    if (userFound)
    {
        cout << "---------Date of Report----------" << endl;
        cout << "Enter Day (1-31): ";
        cin >> date.day;

        cout << "Enter Month (1-12): ";
        cin >> date.month;

        cout << "Enter Year: ";
        cin >> date.year;

        if (isValidDate(date))
        {
            docName = loggedInUser;

            while (currentDoctor != nullptr)
            {
                if (currentDoctor->name == docName)
                {
                    docNum = currentDoctor->phoneNum;
                    break;
                }
                currentDoctor = currentDoctor->next;
            }

            newPatient->username = username;
            newPatient->state = state;
            newPatient->age = age;
            newPatient->date = date;
            newPatient->userNum = userNum;
            newPatient->docName = docName;
            newPatient->docNum = docNum;
            newPatient->next = dengueCases;
            dengueCases = newPatient;

            cout << "Patient reported successfully!" << endl;
            cout << "-----------------------------" << endl;
        }
        else
        {
            cout << "Invalid date. Please enter a valid date." << endl;
        }
    }
    else
    {
        cout << "User not found...\n";
    }
    inpCheck();
}

void displayDengueReports() // to view all of the dengue reports from the latest date
{
    clearTerminal();
    DenguePatient *currentPatient = dengueCases;

    // Count the number of patients
    int patientCount = 0;
    while (currentPatient != nullptr)
    {
        patientCount++;
        currentPatient = currentPatient->next;
    }

    // Create an array of DenguePatient pointers to hold the patient data
    DenguePatient **patientsArray = new DenguePatient *[patientCount];

    currentPatient = dengueCases;
    int index = 0;
    while (currentPatient != nullptr)
    {
        patientsArray[index++] = currentPatient;
        currentPatient = currentPatient->next;
    }

    // Bubble sort the patients by date (from most recent to least recent)
    for (int i = 0; i < patientCount - 1; ++i)
    {
        for (int j = 0; j < patientCount - i - 1; ++j)
        {
            if (patientsArray[j]->date.year < patientsArray[j + 1]->date.year ||
                (patientsArray[j]->date.year == patientsArray[j + 1]->date.year &&
                 patientsArray[j]->date.month < patientsArray[j + 1]->date.month) ||
                (patientsArray[j]->date.year == patientsArray[j + 1]->date.year &&
                 patientsArray[j]->date.month == patientsArray[j + 1]->date.month &&
                 patientsArray[j]->date.day < patientsArray[j + 1]->date.day))
            {
                // Swap patients
                DenguePatient *temp = patientsArray[j];
                patientsArray[j] = patientsArray[j + 1];
                patientsArray[j + 1] = temp;
            }
        }
    }

    // Display the sorted patients
    cout << "View Latest Dengue Cases" << endl;
    cout << "\nDengue Patients: " << endl;
    cout << setfill('=') << setw(175) << "=" << setfill(' ') << endl;
    cout << left << setw(25) << "Patient Name" << setw(25) << "Patient's State" << setw(25) << "Age" << setw(25) << "Date" << setw(25) << "Patient's Phone No." << setw(25) << "Assigned Doctor" << setw(25) << "Doctor's Phone No." << endl;
    cout << setfill('=') << setw(175) << "=" << setfill(' ') << endl;

    for (int i = 0; i < patientCount; ++i)
    {
        cout << left << setw(25) << patientsArray[i]->username << setw(25) << patientsArray[i]->state << setw(25) << to_string(patientsArray[i]->age) << setw(25) << formatDate(patientsArray[i]->date.day, patientsArray[i]->date.month, patientsArray[i]->date.year) << setw(25) << patientsArray[i]->userNum << setw(25) << ("Dr. " + patientsArray[i]->docName) << setw(25) << patientsArray[i]->docNum << endl;
    }

    cout << setfill('=') << setw(175) << "=" << setfill(' ') << endl;

    // Clean up dynamically allocated memory
    delete[] patientsArray;

    inpCheck();
}

void findDengueCasesByName() // to find a specific user's dengue reports
{
    clearTerminal();
    string name;
    DenguePatient *currentPatient = dengueCases;
    cout << "Find Dengue Cases by Patient Info:" << endl;
    cout << "Enter Patient Name: ";
    cin >> name;

    bool found = false;

    // Display the sorted patients
    cout << "\nDengue Patients: " << endl;
    cout << setfill('=') << setw(175) << "=" << setfill(' ') << endl;
    cout << left << setw(25) << "Patient Name" << setw(25) << "Patient's State" << setw(25) << "Age" << setw(25) << "Date" << setw(25) << "Patient's Phone No." << setw(25) << "Assigned Doctor" << setw(25) << "Doctor's Phone No." << endl;
    cout << setfill('=') << setw(175) << "=" << setfill(' ') << endl;

    while (currentPatient != nullptr)
    {
        if (name == currentPatient->username)
        {
            string state = stateForDisplay(currentPatient->state);
            cout << left << setw(25) << currentPatient->username << setw(25) << state << setw(25) << to_string(currentPatient->age) << setw(25) << formatDate(currentPatient->date.day, currentPatient->date.month, currentPatient->date.year) << setw(25) << currentPatient->userNum << setw(25) << ("Dr. " + currentPatient->docName) << setw(25) << currentPatient->docNum << endl;
            found = true;
        }
        currentPatient = currentPatient->next;
    }

    if (!found)
    {
        cout << "No dengue cases found for the given patient name." << endl;
    }

    cout << setfill('=') << setw(175) << "=" << setfill(' ') << endl;
    inpCheck();
}

void findCasesByAgeAndState() // to display the number of cases based on the selected age range and state(s)
{
    clearTerminal();
    string lowerAgeStr, upperAgeStr, ageRange, input;
    cout << "\nEnter Age Range (e.g., '18-30'): ";
    cin >> ageRange;

    // Check if the age range input is in the correct format
    if (ageRange.find('-') == string::npos)
    {
        cout << "\nInvalid input for age range. Please enter a valid range (e.g., '18-30')." << endl;
        inpCheck();
        return;
    }

    int lowerAge, upperAge;
    stringstream ageRangeStream(ageRange);
    getline(ageRangeStream, lowerAgeStr, '-');
    getline(ageRangeStream, upperAgeStr);
    lowerAge = stoi(lowerAgeStr);
    upperAge = stoi(upperAgeStr);

    cout << endl;
    displayStates();
    cout << "\nEnter the state(s) (separated by space, e.g., Johor PulauPinang): ";
    getline(cin, input); // Clear the input buffer
    getline(cin, input);

    istringstream iss(input);
    string state;

    DenguePatient *currentPatient = dengueCases;

    int totalCases = 0;

    while (currentPatient != nullptr)
    {
        int patientAge = currentPatient->age;

        if (patientAge >= lowerAge && patientAge <= upperAge)
        {
            string patientState = currentPatient->state;
            transform(patientState.begin(), patientState.end(), patientState.begin(), ::toupper);

            istringstream stateStream(input);
            string state;

            bool isMatched = false;
            while (stateStream >> state)
            {
                transform(state.begin(), state.end(), state.begin(), ::toupper);
                if (patientState == state)
                {
                    isMatched = true;
                    break;
                }
            }

            if (isMatched)
            {
                totalCases++;
            }
        }
        currentPatient = currentPatient->next;
    }

    cout << "\nTotal Dengue Cases under the specified Age Range and State(s): " << totalCases << endl;
    inpCheck();
}

//----------{ADMIN FUNCTIONS}----------

void addUser(const string &username, const string &password, const string &state, const int &age, const string &phoneNum) // to add new users
{
    User *newUser = new User;
    newUser->username = username;
    newUser->password = password;
    newUser->state = state;
    newUser->age = age;
    newUser->phoneNum = phoneNum;
    newUser->isActive = false;
    newUser->next = userHead;
    userHead = newUser;
}

void deleteInactiveUsers() // to delete all inactive users
{
    int choice;
    clearTerminal();
    cout << "\nAre you sure you want to delete all inactive users?\n";
    cout << "1. Yes\n";
    cout << "2. No\n";
    cout << "Select your choice: ";
    cin >> choice;

    User *currentUser = userHead;
    bool inactiveUsersFound = false;

    while (currentUser != nullptr)
    {
        if (!currentUser->isActive)
        {
            inactiveUsersFound = true;
            break;
        }
        currentUser = currentUser->next;
    }

    if (choice == 1)
    {
        if (inactiveUsersFound)
        {
            currentUser = userHead;
            User *prevUser = nullptr;

            while (currentUser != nullptr)
            {
                if (!currentUser->isActive)
                {
                    if (prevUser != nullptr)
                    {
                        prevUser->next = currentUser->next;
                        delete currentUser;
                        currentUser = prevUser->next;
                    }
                    else
                    {
                        User *temp = currentUser;
                        currentUser = currentUser->next;
                        delete temp;
                        userHead = currentUser;
                    }
                }
                else
                {
                    prevUser = currentUser;
                    currentUser = currentUser->next;
                }
            }

            cout << "Inactive users deleted successfully!" << endl;
            inpCheck();
        }
        else
        {
            cout << "No inactive users found." << endl;
            inpCheck();
        }
    }
    else if (choice == 2)
    {
        cout << "Deletion process aborted." << endl;
        inpCheck();
    }
    else
    {
        cout << invalidInput << endl;
        inpCheck();
    }
}

void removeUser(const string &username) // to remove selected users
{
    User *prevUser = nullptr;
    User *currentUser = userHead;
    bool found = false;

    while (currentUser != nullptr)
    {
        if (currentUser->username == username)
        {
            found = true;
            if (prevUser == nullptr)
            {
                userHead = currentUser->next;
            }
            else
            {
                prevUser->next = currentUser->next;
            }
            delete currentUser;
            cout << "User removed successfully" << endl;
            break;
        }
        prevUser = currentUser;
        currentUser = currentUser->next;
    }

    if (!found)
    {
        cout << "User not found" << endl;
    }
    inpCheck();
}

void displayUsers() // to display all existing users
{
    clearTerminal();

    User *current = userHead;

    cout << "\nAll Users:" << endl;
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;
    cout << left << setw(20) << "Username" << setw(20) << "State" << setw(20) << "Age" << setw(20) << "Phone Number" << setw(20) << "Active" << endl;
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;

    if (current == nullptr)
    {
        cout << "No users found" << endl;
    }
    else
    {
        while (current != nullptr)
        {
            string state = stateForDisplay(current->state);
            cout << left << setw(20) << current->username << setw(20) << state << setw(20) << to_string(current->age) << setw(20) << current->phoneNum;
            if (current->isActive)
                cout << "Yes\n";
            else
                cout << "No\n";
            current = current->next;
        }
    }
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;
    inpCheck();
}

void addDoctor(const string &name, const string &password, const string &state, const string &phoneNum) // to add new doctors
{
    Doctor *newDoctor = new Doctor;
    newDoctor->name = name;
    newDoctor->password = password;
    newDoctor->state = state;
    newDoctor->phoneNum = phoneNum;
    newDoctor->next = doctorHead;
    doctorHead = newDoctor;
}

void removeDoctor(const string &name) // to remove selected doctors
{
    Doctor *prevDoctor = nullptr;
    Doctor *currentDoctor = doctorHead;
    bool found = false;

    while (currentDoctor != nullptr)
    {
        if (currentDoctor->name == name)
        {
            found = true;
            if (prevDoctor == nullptr)
            {
                doctorHead = currentDoctor->next;
            }
            else
            {
                prevDoctor->next = currentDoctor->next;
            }
            delete currentDoctor;
            cout << "Doctor removed successfully" << endl;
            break;
        }
        prevDoctor = currentDoctor;
        currentDoctor = currentDoctor->next;
    }

    if (!found)
        cout << "User not found" << endl;
    inpCheck();
}

void displayDoctors() // to display all existing doctors
{
    clearTerminal();

    Doctor *current = doctorHead;

    // Displaying column headers
    cout << "\nAll Doctors:" << endl;
    cout << setfill('=') << setw(60) << "=" << setfill(' ') << endl;
    cout << left << setw(20) << "Name" << setw(20) << "State" << setw(20) << "Phone Number" << endl;
    cout << setfill('=') << setw(60) << "=" << setfill(' ') << endl;

    if (current == nullptr)
    {
        cout << "No doctors found" << endl;
    }
    else
    {
        while (current != nullptr)
        {
            string state = stateForDisplay(current->state); // Moved here to ensure state changes for each doctor
            cout << left << setw(20) << ("Dr. " + current->name) << setw(20) << state << setw(20) << current->phoneNum << endl;
            current = current->next; // Move to the next doctor
        }
    }
    cout << setfill('=') << setw(60) << "=" << setfill(' ') << endl;
    inpCheck();
}

void viewAnnualCases() // to view annual cases from the CSV file, in graphical view
{
    clearTerminal();
    int numRows = 0;
    int numCols = 0;

    string **content = CSVReader::getContent("Annual number of dengue cases by state.csv", numRows, numCols);

    if (content)
    {
        if (numRows < 2 || numCols < 3)
        {
            cout << "Not enough data to generate a graph." << endl;
        }
        else
        {
            // Determine the last year based on the data
            int lastYear = stoi(content[numRows - 1][0]);

            // Initialize an array to store total cases for each year
            int totalCases[6] = {0};

            // Calculate the total cases for each year
            for (int i = 1; i < numRows; i++)
            {
                int year = stoi(content[i][0]);
                if (year >= 2018 && year <= lastYear)
                {
                    for (int j = 2; j < numCols; j++)
                    {
                        totalCases[year - 2018] += stoi(content[i][j]);
                    }
                }
            }

            // Display the graph with headers
            cout << "\nYear  | Total Annual Cases" << endl;
            for (int i = 0; i <= lastYear - 2018; i++)
            {
                int year = 2018 + i;
                cout << year << "  | ";
                for (int j = 0; j < totalCases[i] / 1000; j++)
                {
                    cout << "*";
                }
                cout << endl;
            }
            cout << endl;
        }

        // Clean up: deallocate the dynamic array
        for (int i = 0; i < numRows; i++)
        {
            delete[] content[i];
        }
        delete[] content;
    }
    cin.clear();

    cout << "Press any key to proceed: ";
    cin >> inp;
}

void viewWeeklyCase() // to view weekly cases in ascending order
{
    clearTerminal();

    int numRows = 0;
    int numCols = 0;

    string **content = CSVReader::getContent("Number of dengue fever cases weekly by state.csv", numRows, numCols);

    if (content)
    {
        // Initialize the variables for storing the total cases for each week
        int maxYear = stoi(content[numRows - 1][0]); // Assuming the last row has the maximum year
        int maxWeek = stoi(content[numRows - 1][1]); // Assuming the last row has the maximum week

        WeeklyCases *weeklyTotalCases = new WeeklyCases[maxYear * maxWeek]{};
        int index = 0;

        for (int i = 1; i < numRows; ++i)
        {
            int year = stoi(content[i][0]);
            int week = stoi(content[i][1]);
            int total = 0;
            for (int j = 2; j < numCols; ++j)
            {
                total += stoi(content[i][j]);
            }
            weeklyTotalCases[index].year = year;
            weeklyTotalCases[index].week = week;
            weeklyTotalCases[index].totalCases = total;
            index++;
        }

        // Sort the weeklyTotalCases array based on total cases in ascending order using quick sort
        sortWeeklyCases(weeklyTotalCases, 0, index - 1);

        // Display the results after sorting
        cout << "View Weekly Dengue Cases" << endl;
        cout << "\n=========================" << endl;
        cout << "Year | Week | Total Cases" << endl;
        cout << "=========================" << endl;
        for (int i = 0; i < index; ++i)
        {
            cout << setw(2) << setfill('0') << weeklyTotalCases[i].year << " | "
                 << setw(2) << setfill('0') << weeklyTotalCases[i].week << "   | " << weeklyTotalCases[i].totalCases << endl;
        }
        cout << "=========================\n"
             << endl;

        // Clean up: deallocate memory
        for (int i = 0; i < numRows; i++)
        {
            delete[] content[i];
        }
        delete[] content;
        delete[] weeklyTotalCases;
    }
    inpCheck();
}

void sendAlertsToUsers() // to send alert message to users in the states with high dengue reports
{
    clearTerminal();
    StateData statesData = getStates();
    int numStates = statesData.numStates;
    string *alertedStates = new string[numStates]();
    int *patientCounts = new int[numStates]();
    bool foundAlertStates = false;

    for (int i = 0; i < numStates; ++i)
    {
        int patientsCount = 0;
        DenguePatient *currentPatient = dengueCases;

        while (currentPatient != nullptr)
        {
            if (currentPatient->state == statesData.stateNames[i])
            {
                patientsCount++;
            }
            currentPatient = currentPatient->next;
        }

        if (patientsCount >= 20)
        {
            alertedStates[i] = statesData.stateNames[i];
            patientCounts[i] = patientsCount;
            foundAlertStates = true;
        }
    }

    cout << "Alerted states with more than 20 dengue reports:" << endl;
    cout << setfill('=') << setw(40) << "=" << setfill(' ') << endl;
    cout << left << setw(20) << "Alerted States" << setw(20) << "Dengue Reports" << endl;
    cout << setfill('=') << setw(40) << "=" << setfill(' ') << endl;

    if (!foundAlertStates)
    {
        cout << "No states with more than 20 dengue patients." << endl;
    }
    else
    {
        for (int i = 0; i < numStates; ++i)
        {
            if (alertedStates[i] != "")
            {
                cout << left << setw(20) << alertedStates[i] << setw(20) << patientCounts[i] << endl;
            }
        }
    }
    cout << setfill('=') << setw(40) << "=" << setfill(' ') << endl;
    delete[] patientCounts;
    delete[] alertedStates;
    inpCheck();
}

//----------{MENU}----------

void deleteUserMenu()
{
    int choice;
    do
    {
        clearTerminal();
        cout << "\nDelete Users Menu:\n";
        cout << "1. Delete Selected User\n";
        cout << "2. Delete All Inactive Users\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
        {
            clearTerminal();
            string username;
            cout << "\nEnter username to remove user: ";
            cin >> username;
            removeUser(username);
            break;
        }
        case 2:
        {
            deleteInactiveUsers();
            break;
        }
        case 3:
            break;
        default:
            cout << invalidInput << endl;
            inpCheck();
        }
    } while (choice != 3);
}

void manageUserMenu()
{
    int choice;
    do
    {
        clearTerminal();
        cout << "\nManage User Menu:\n";
        cout << "1. Add New User\n";
        cout << "2. Update User Information\n";
        cout << "3. Delete Users\n";
        cout << "4. View Users\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
        {
            bool stateFound = false;
            bool usernameTaken = false;
            clearTerminal();
            string username, password, state, phoneNum;
            int age;
            cout << "\nAdd User:\n";
            cout << "Set username: ";
            cin >> username;
            usernameTaken = isUsernameTaken(username);
            if (usernameTaken)
            {
                cout << "Username is already taken. Please choose a different username.\n";
                inpCheck();
                break;
            }
            else
            {
                cout << "Set password: ";
                cin >> password;
                cout << endl;
                displayStates();
                cout << "Select user's state (no spaces, e.g. PULAUPINANG): ";
                cin >> state;
                transform(state.begin(), state.end(), state.begin(), ::toupper);

                stateFound = checkState(state);
                if (stateFound)
                {
                    cout << "Set Age: ";
                    cin >> age;
                    cout << "Set Phone Number (e.g. 0123456789): ";
                    cin >> phoneNum;
                    addUser(username, password, state, age, phoneNum);
                    cout << "\nUser added successfully!\n";
                    inpCheck();
                }
                else
                {
                    cout << "State not found...\n";
                    inpCheck();
                }
                break;
            }
        }
        case 2:
        {
            clearTerminal();
            string username, password, state;
            cout << "\nUpdate User:\n";
            cout << "Enter username of account to be updated: ";
            cin >> username;
            updateUser(username);
            break;
        }
        case 3:
        {
            deleteUserMenu();
            break;
        }
        case 4:
        {
            displayUsers();
            break;
        }
        case 5:
        {
            break;
        }
        default:
            cout << invalidInput << endl;
            inpCheck();
        }
    } while (choice != 5);
}

void manageDoctorMenu()
{
    int choice;
    do
    {
        clearTerminal();
        cout << "\nManage Doctor Menu:\n";
        cout << "1. Add New Doctor\n";
        cout << "2. Update Doctor Information\n";
        cout << "3. Remove doctor\n";
        cout << "4. View Doctors\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
        {
            bool stateFound = false;
            string name, password, state, phoneNum;
            clearTerminal();

            cout << "\nAdd Doctor:\n";
            cout << "Enter doctor's name: ";
            cin >> name;
            name = capitalize(name);
            cout << "Set password: ";
            cin >> password;
            cout << endl;
            displayStates();
            cout << "Select user's state (no spaces, e.g. PULAUPINANG): ";
            cin >> state;
            transform(state.begin(), state.end(), state.begin(), ::toupper);

            stateFound = checkState(state);
            if (stateFound)
            {
                cout << "Set Phone Number (e.g. 0123456789): ";
                cin >> phoneNum;
                addDoctor(name, password, state, phoneNum);
                cout << "\nDoctor added successfully\n";
                inpCheck();
            }
            else
            {
                cout << "State not found...\n";
                inpCheck();
            }

            break;
        }
        case 2:
        {
            clearTerminal();
            string name, password;
            cout << "\nUpdate Doctor:\n";
            cout << "Enter name of doctor to be updated: ";
            cin >> name;
            name = capitalize(name);
            updateDoctor(name);
            break;
        }
        case 3:
        {
            clearTerminal();
            string name;
            cout << "\nEnter doctor's name to remove doctor: ";
            cin >> name;
            name = capitalize(name);
            removeDoctor(name);
            break;
        }
        case 4:
        {
            displayDoctors();
            break;
        }
        case 5:
        {
            break;
        }
        default:
            cout << invalidInput << endl;
            inpCheck();
        }

    } while (choice != 5);
}

void userMenu()
{
    int choice;
    do
    {
        clearTerminal();
        cout << "\n"
             << loggedInUser << "'s Menu:" << endl;
        cout << "1. Manage Personal Information" << endl;
        cout << "2. Total Dengue Cases" << endl;
        cout << "3. Daily Dengue Cases" << endl;
        cout << "4. View Alert Messages" << endl;
        cout << "5. View Past Dengue Records" << endl;
        cout << "6. Logout" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            manageUserInfo();
            break;
        }
        case 2:
        {
            userDisplayCases();
            break;
        }
        case 3:
        {
            dailyCasesByState();
            break;
        }
        case 4:
        {
            viewAlerts();
            break;
        }
        case 5:
        {
            personalDengueReports();
            break;
        }
        case 6:
        {
            break;
        }
        default:
            cout << invalidInput << endl;
            inpCheck();
        }
    } while (choice != 6);
}

void doctorMenu()
{
    int choice;
    do
    {
        clearTerminal();
        cout << "\nDr. "
             << loggedInUser << "'s Menu:" << endl;
        cout << "1. Manage Personal Information" << endl;
        cout << "2. Report Infected Patient" << endl;
        cout << "3. View Latest Dengue Cases" << endl;
        cout << "4. Find A Patient's Dengue Fever Cases " << endl;
        cout << "5. Find Number of Cases Based on Age and State" << endl;
        cout << "6. Logout" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            manageDoctorInfo();
            break;
        }
        case 2:
        {
            reportPatient();
            break;
        }
        case 3:
        {
            displayDengueReports();
            break;
        }
        case 4:
        {
            findDengueCasesByName();
            break;
        }
        case 5:
        {
            findCasesByAgeAndState();
            break;
        }
        case 6:
        {
            break;
        }
        default:
            cout << invalidInput << endl;
            inpCheck();
        }
    } while (choice != 6);
}

void adminMenu()
{
    int choice;
    do
    {
        clearTerminal();
        cout << "\nAdmin Menu:\n";
        cout << "1. View Annual Dengue Cases\n";
        cout << "2. View Weekly Dengue Cases For All States\n";
        cout << "3. Send Alert Message to Users\n";
        cout << "4. Manage Doctor Account\n";
        cout << "5. Manage User Accounts\n";
        cout << "6. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
        {
            viewAnnualCases();
            break;
        }
        case 2:
        {
            viewWeeklyCase();
            break;
        }
        case 3:
        {
            sendAlertsToUsers();
            break;
        }
        case 4:
        {
            manageDoctorMenu();
            break;
        }
        case 5:
        {
            manageUserMenu();
            break;
        }
        case 6:
        {
            break;
        }
        default:
            cout << invalidInput << endl;
            inpCheck();
        }
    } while (choice != 6);
}

void adminLogin()
{

    clearTerminal();
    string username, password;
    cout << "\nAdmin Login:\n";
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;
    if (username == "admin" && password == "123")
        adminMenu();
    else
    {
        cout << loginFailedMsg << endl;
        inpCheck();
    }
}

void userLogin()
{
    bool isLoggedIn = false;

    string inputUsername, inputPassword;
    clearTerminal();
    cout << "\nUser Login:" << endl;
    cout << "Enter Username: ";
    cin >> inputUsername;
    loggedInUser = inputUsername;
    cout << "Enter Password: ";
    cin >> inputPassword;

    User *currentUser = userHead;
    while (currentUser != nullptr)
    {
        if (currentUser->username == inputUsername && currentUser->password == inputPassword)
        {
            cout << "Login successful!" << endl;
            isLoggedIn = true;
            currentUser->isActive = true; // Update isActive status
            break;                        // Exit the loop as the user is found
        }
        currentUser = currentUser->next;
    }

    if (isLoggedIn)
    {
        userMenu();
    }
    else
    {
        cout << loginFailedMsg << endl;
        inpCheck();
    }
}

void doctorLogin()
{
    bool isLoggedIn = false;

    string inputUsername, inputPassword;
    clearTerminal();
    cout << "\nDoctor Login:" << endl;
    cout << "Enter Doctor Name: ";
    cin >> inputUsername;
    inputUsername = capitalize(inputUsername);
    loggedInUser = inputUsername;
    cout << "Enter Password: ";
    cin >> inputPassword;

    Doctor *currentDoctor = doctorHead;
    while (currentDoctor != nullptr)
    {
        if (currentDoctor->name == inputUsername && currentDoctor->password == inputPassword)
        {
            cout << "Login successful!" << endl;
            isLoggedIn = true;
            break; // Exit the loop as the user is found
        }
        currentDoctor = currentDoctor->next;
    }

    if (isLoggedIn)
    {
        doctorMenu();
    }
    else
    {
        cout << loginFailedMsg << endl;
        inpCheck();
    }
}

void startMenu()
{
    int choice;
    do
    {
        clearTerminal();
        cout << "\nDENGUE FEVER SURVEILLANCE AND ALERT SYSTEM (MOH):\n";
        cout << "1. User\n";
        cout << "2. Doctor\n";
        cout << "3. Admin\n";
        cout << "4. Exit\n";
        cout << "Select user type: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
        {
            userLogin();
            break;
        }
        case 2:
        {
            doctorLogin();
            break;
        }
        case 3:
        {
            adminLogin();
            break;
        }
        case 4:
            break;
        default:
            cout << invalidInput << endl;
            inpCheck();
        }
    } while (choice != 4);
}

//----------{TO ADD MORE REPORTS FOR TESTING PURPOSES}----------

void addPatientsForTesting()
{
    DenguePatient *patients[20];

    // Details for 20 patients
    string names[] = {"rafii", "rafii", "rafii", "rafii", "ella", "frank", "grace", "henry", "isabella", "jack",
                      "katherine", "frank", "mia", "frank", "olivia", "patrick", "henry", "rachel", "samuel", "tina"};
    int ages[] = {21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40};
    int dates[][3] = {{1, 1, 2023}, {2, 1, 2023}, {3, 1, 2023}, {4, 1, 2023}, {5, 1, 2023}, {6, 1, 2023}, {7, 1, 2023}, {8, 1, 2023}, {9, 1, 2023}, {10, 1, 2023}, {11, 1, 2023}, {12, 1, 2023}, {13, 1, 2023}, {14, 1, 2023}, {15, 1, 2023}, {16, 1, 2023}, {17, 1, 2023}, {18, 1, 2023}, {19, 1, 2023}, {20, 1, 2023}};
    string userNum[] = {"1111111111", "2222222222", "3333333333", "4444444444", "5555555555",
                        "6666666666", "7777777777", "8888888888", "9999999999", "1000000000",
                        "1010101010", "2020202020", "3030303030", "4040404040", "5050505050",
                        "6060606060", "7070707070", "8080808080", "9090909090", "1234567890"};
    string doctors[] = {"Peter", "John", "Alice", "David", "Emma", "Frank", "Sophia", "Henry", "Olivia", "Jack",
                        "Lucas", "Mia", "Noah", "Olivia", "Aiden", "Sophia", "Ella", "Henry", "Mia", "Sophia"};
    string docNumbers[] = {"0111111111", "0222222222", "0333333333", "0444444444", "0555555555", "0666666666",
                           "0777777777", "0888888888", "0999999999", "1000000000", "1011111111", "1122222222",
                           "1244444444", "1366666666", "1488888888", "1611111111", "1733333333", "1855555555",
                           "1966666666", "2077777777"};

    // Generating 20 patients with the provided details
    for (int i = 0; i < 20; ++i)
    {
        patients[i] = new DenguePatient{names[i], "SELANGOR", ages[i], {dates[i][0], dates[i][1], dates[i][2]}, userNum[i], doctors[i], docNumbers[i]};
    }

    // Link the patients together in the linked list
    for (int i = 0; i < 19; ++i)
    {
        patients[i]->next = patients[i + 1];
    }

    // If dengueCases is null, set it to the first patient; otherwise, link the new patients to the existing list
    if (dengueCases == nullptr)
    {
        dengueCases = patients[0];
    }
    else
    {
        DenguePatient *current = dengueCases;
        while (current->next != nullptr)
        {
            current = current->next;
        }
        current->next = patients[0];
    }
}

//----------{MAIN}----------

int main()
{
    addUser("rafii", "123", "SELANGOR", 21, "0109675700"); // add 1 user for testing
    addDoctor("Peter", "123", "JOHOR", "0111111111"); // add 1 doctor for testing
    addPatientsForTesting(); // add 20 reports for testing
    startMenu();
}
