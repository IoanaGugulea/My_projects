#include <bits/stdc++.h>
#include <stdexcept>
#include <regex>
#include <chrono>
#include <cctype>

using namespace std;

//gestionare exceptii cu ajutorul clasei de baza exception
class CustomException : public exception {
public:
    CustomException(const string& message) : message(message) {}
//implementez metoda suprascrisa "what" din clasa de baza exception
//noexcept -- functia nu arunca exceptii
    const char* what() const noexcept override {
        return message.c_str(); //am furnizat un pointer catre sirul de caractere
    }

private:
    string message;
};

//imi creez o structura de chei RSA
struct RSAkey {
    int n;
    int publicKey;
    int privateKey;
};

//verific daca un nr este prim
bool isPrime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

//functie cmmdc
int cmmd(int a, int b) {
    if (b == 0) return a;
    return cmmd(b, a % b);
}

// functie pt a gasi un nr prim compatibil
int findCoprime(int phi) {
    for (int e = 2; e < phi; ++e) {
        if (cmmd(e, phi) == 1) {
            return e;
        }
    }
    return -1; //nu e posibil in mod real
}

// modular exponentiation (a^b % mod)
int mod_exp(int baza, int exponent, int mod) {
    int rezultat = 1;
    baza = baza % mod;

    while (exponent > 0) {
        if (exponent % 2 == 1) {
            rezultat = (rezultat * baza) % mod;
        }

        exponent = exponent >> 1; //asemanator cu exponent /= 2
        baza = (baza * baza) % mod;
    }

    return rezultat;
}

//functie care sa genereze perechea de chei RSA
RSAkey generareRSAkeys() {
    RSAkey keys;

    // Choose two prime numbers (p and q)
    int p = 17;
    int q = 19;

    // Calculate n = p * q
    keys.n = p * q;

    // Calculate Euler's totient function (phi)
    int phi = (p - 1) * (q - 1);

    // Choose a public key (e) that is coprime with phi
    keys.publicKey = findCoprime(phi);

    // Calculate the private key (d) such that (e * d) % phi = 1
    keys.privateKey = 0;
    while ((keys.publicKey * keys.privateKey) % phi != 1) {
        ++keys.privateKey;
    }

    return keys;
}

//incriptez parola folosind RSA
string encryptPassword(const string& password, const RSAkey& publicKey) {
    string encryptedPassword = "";

    //codul se executa pentru fiecare caracter din parola
    for (char ch : password) {
        int encryptedChar = mod_exp(ch, publicKey.publicKey, publicKey.n);
        encryptedPassword += to_string(encryptedChar) + " ";
    }

    return encryptedPassword;
}

//functie de decriptare a parolei folosind RSA
string decryptPassword(const string& encryptedPassword, const RSAkey& privateKey) {
    string decryptedPassword = "";
    string tempEncryptedPassword = encryptedPassword;
    size_t pos = 0;

    // Process each token in the encrypted password
    while ((pos = encryptedPassword.find(' ')) != string::npos) {
        // Extract token from the beginning to the space character
        string token = encryptedPassword.substr(0, pos);

        // Erase the token and the space character from the encrypted password
        tempEncryptedPassword.erase(0, pos + 1);

        // Convert the token to an integer
        try {
            int encryptedChar = stoi(token);

            // Decrypt the character using RSA modular exponentiation
            int decryptedChar = mod_exp(encryptedChar, privateKey.privateKey, privateKey.n);

            // Append the decrypted character to the result
            decryptedPassword += static_cast<char>(decryptedChar);
        } catch (const invalid_argument& e) {
            // Handle the case where stoi fails to convert the token to an integer
            cerr << "Error converting token to integer: " << e.what() << endl;
            // You may choose to handle this error in a way that suits your application
        } catch (const out_of_range& e) {
            // Handle the case where the converted integer is out of range
            cerr << "Error: Converted integer is out of range: " << e.what() << endl;
            // You may choose to handle this error in a way that suits your application
        }
    }

    return decryptedPassword;
}

//verific daca calatoria mai este valabila
bool isTripAvailable(const string &date, const string &city) {
    ifstream inFile("calatorii.csv");
    string data, oras;

    while (getline(inFile, data, ',')) {
        getline(inFile, oras);

        if (data == date && oras == city) {
            inFile.close();
            return true;
        }
    }

    inFile.close();
    return false;
}

class Operator {
public:
    bool login(const string& username, const string& password, const RSAkey& publicKey)
    {
        string storedusername = "Operator";
        string storedpassword = encryptPassword("operator_password", publicKey);

        if (username != storedusername || password != storedpassword) {
            throw runtime_error("Credentialele operatorului sunt invalide");
        }

        return true;
    }

    void adaugareCursa(const string& date, const string& city) {
        //verific daca formatul datei si data sunt valide
        if (!isValidDateFormat(date) || !isValidDate(date) || !isValidCity(city)) {
            throw invalid_argument("Detaliile calatoriei sunt invalide");
        }

        // adaug o calatorie in fisier
        ofstream outFile("calatorii.csv", ios::app);
        outFile << date << "," << city << endl;
        outFile.close();

        cout << "Calatorie adaugata cu succes: Data " << date << ", Orasul " << city << endl;
    }

    void stergereCursa(const string& date, const string& city) {
        // Check date format and date validity
        if (!isValidDateFormat(date) || !isValidDate(date) || !isValidCity(city)) {
            throw invalid_argument("Detalii invalide ale calatoriei");
        }
    }
    bool isTripExists(const string& date, const string& city) {
       ifstream inFile("calatorii.csv");
       string data, oras;

       while (getline(inFile, data, ',')) {
            getline(inFile, oras);

            if (data == date && oras == city) {
                inFile.close();
                return true;
        }
    }

        inFile.close();
        return false;

        if (!isTripExists(date, city)) {
            throw runtime_error("Calatoria nu exista!");
        }
    }

     void removeTripFromDatabase(const string& date, const string& city) {
    // Open the input file for reading
    ifstream inFile("calatorii.csv");
    if (!inFile) {
        cerr << "Error opening input file: calatorii.csv\n";
        return;
    }

    // Open a temporary file for writing
    ofstream tempFile("temp_calatorii.csv");
    if (!tempFile) {
        cerr << "Error opening temporary file: temp_calatorii.csv\n";
        inFile.close();
        return;
    }

    string data, oras;

    // Read data from the input file, omitting the specified trip
    while (getline(inFile, data, ',')) {
        getline(inFile, oras);

        if (data == date && oras == city) {
            // Skip this line (omit the specified trip)
            cout << "Trip removed: Date " << date << ", City " << city << endl;
        } else {
            // Write the non-matching trip to the temporary file
            tempFile << data << "," << oras << endl;
        }
    }

    // Close the input and temporary files
    inFile.close();
    tempFile.close();

    // Remove the original file
    if (remove("calatorii.csv") != 0) {
        cerr << "Error removing original file: calatorii.csv\n";
        return;
    }

    // Rename the temporary file to the original file
    if (rename("temp_calatorii.csv", "calatorii.csv") != 0) {
        cerr << "Error renaming temporary file\n";
    }

        removeTripFromDatabase(date, city);

        cout << "Calatorie stearsa cu succes: Data " << date << ", Oras " << city << endl;
     }

private:

     bool isValidDateFormat(const string& date) {
    // Define a regular expression for "YYYY-MM-DD" format
    regex dateFormat("^\\d{4}-\\d{2}-\\d{2}$");

    // Use regex_match to check if the date matches the pattern
    return regex_match(date, dateFormat);
     }

    //verific daca data este valida
    bool isValidDate(const string& date) {
    // Parse the provided date string into a tm structure
    tm tmDate = {};
    istringstream(date) >> get_time(&tmDate, "%Y-%m-%d");

    if (tmDate.tm_year < 0 || tmDate.tm_mon < 0 || tmDate.tm_mday <= 0) {
        // Invalid date format
        return false;
    }

    // Get the current time
    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    tm tmNow = *localtime(&now);

    // Convert both dates to chrono::system_clock::time_point
    auto datePoint = chrono::system_clock::from_time_t(mktime(&tmDate));
    auto nowPoint = chrono::system_clock::from_time_t(now);

    // Check if the date is in the future
    return datePoint > nowPoint;
}

    bool isValidCity(const string& city) {
    // Define a regular expression for city name validation
    regex cityRegex("^[A-Za-z\\s]+$");

    // Use regex_match to check if the city matches the pattern
    return regex_match(city, cityRegex);
}
};

class User {
public:

bool registerUser(const string& username, const string& email, const string& password, const RSAkey& publicKey) {
    // Check email format
    if (!isValidEmail(email)) {
        throw invalid_argument("Invalid email format");
    }

    // Check password strength
    if (password.length() < 8 || !isValidPassword(password)) {
        throw invalid_argument("Invalid password");
    }

    // Encrypt the user's password using the RSA public key
    string encryptedPassword = encryptPassword(password, publicKey);

    // Add user to the database (not implemented in this example)
    ofstream outFile("utilizatori.csv", ios::app);
    outFile << username << "," << email << "," << encryptedPassword << endl;
    outFile.close();

    return true;
}

     bool login(const string& email, const string& password, const RSAkey& publicKey) {

        string storedEmail = "user1@example.com";
        string storedEncryptedPassword = "encrypted_user_password";

        string storedPassword = decryptPassword(storedEncryptedPassword, publicKey);

        // Compare email and hashed password with stored data
        if (email != storedEmail || password != storedPassword) {
            throw runtime_error("Credentialele utilizatorului nu sunt valide");
        }

        return true;
    }

    void searchTrip(const string &date, const string &city) {

        if (isTripAvailable(date, city)) {
            cout << "Calatorie gasita: Data " << date << ", Orasul " << city << endl;
        } else {
            cout << "Calatoria nu a fost gasita.\n";
        }
    }

    void bookTrip(const string &date, const string &city) {
        //adaug calatoria in fisierul csv, daca este valabila
        if (isTripAvailable(date, city)) {
            cout << "Trip booked: Date " << date << ", City " << city << endl;

            ofstream outFile("turistinfo.csv", ios::app);
            outFile << date << "," << city << "," << "user1@example.com" << endl;
            outFile.close();
        } else {
            cout << "Nu am putut adauga calatoria. Cautare invalida\n";
        }
    }

private:

bool isValidEmail(const string& email) {
    // Define a regular expression for basic email validation
    regex emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Z|a-z]{2,}$");

    // Use regex_match to check if the email matches the pattern
    return regex_match(email, emailRegex);
}


    bool isValidPassword(const string& password) {
    // Check minimum length
    if (password.length() < 8) {
        return false;
    }

    // Check for at least one uppercase letter
    bool hasUppercase = false;
    // Check for at least one lowercase letter
    bool hasLowercase = false;
    // Check for at least one digit
    bool hasDigit = false;

    for (char ch : password) {
        if (isupper(ch)) {
            hasUppercase = true;
        } else if (islower(ch)) {
            hasLowercase = true;
        } else if (isdigit(ch)) {
            hasDigit = true;
        }
    }

    // Return true only if all criteria are met
    return hasUppercase && hasLowercase && hasDigit;
}
};

int main()
{
    Operator operator1;
    User user1;

    // Generate RSA key pair
    RSAkey rsaKeyPair = generareRSAkeys();

    try {
        //activitate operator
        if (operator1.login("operator", "operator_password", rsaKeyPair)) {
            operator1.adaugareCursa("2023-12-01", "CityA");
            operator1.stergereCursa("2023-12-02", "CityB");
        } else {
            cout << "Operator login failed.\n";
        }

        //activitate user
        if (user1.registerUser("user1", "user1@example.com", "strongPassword", rsaKeyPair)) {
            if (user1.login("user1@example.com", "strongPassword", rsaKeyPair)) {
                user1.searchTrip("2023-12-01", "CityA");
                user1.bookTrip("2023-12-01", "CityA");
            } else {
                cout << "User login failed.\n";
            }
        } else {
            cout << "User registration failed.\n";
        }
    } catch (const exception& e) {
        cerr << "Exception caught: " << e.what() << endl;
    }

    return 0;
}
