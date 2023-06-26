#include "FileHandler.hpp"

using namespace std;

string get_file_content(string fileName, unsigned int startPosition, int fileSize, int *bytesRead) {
    ifstream file(fileName, ios::binary); // Open the file in binary mode

    if (file.is_open()) {
        file.seekg(startPosition); // Set the starting position
        string content(MAX_DATA_SIZE, '\0');
        file.read(&content[0], MAX_DATA_SIZE);
        *bytesRead = file.gcount();
        file.close(); // Close the file
        return content;
    } else {
        cerr << "\033[0;35m ### ERROR: Failed to open the file. \033[0m" << endl;
        return "";
    }
}

unsigned int get_file_size(string fileName) {
    ifstream arq(fileName);

    if (!arq) {
        cerr << "\033[0;35m ### ERROR: Failed to open the file.\033[0m" << endl;
        return 0;
    }

    stringstream buffer;
    buffer << arq.rdbuf(); // Read the file buffer into the stringstream

    return buffer.str().size(); // Convert the stringstream to a string
}

void write_to_file(string fileName, unsigned char data[MAX_DATA_SIZE], bool append, size_t dataSize) {
    ofstream file;

    // check if the data will be appended
    if (append) {
        file.open(fileName, ios::binary | ios::app);
    } else { // otherwise, overwrite the file
        file.open(fileName, ios::binary);
    }

    if (file.is_open()) {

        file.write((const char *)(data), dataSize);
        file.close();

    } else {
        cerr << "\033[0;35m ### ERROR: Could not write file  " << fileName << ".\033[0m" << endl;
    }
}

string calculateMD5(string filename) {
    std::ifstream file(filename, std::ios::binary); // Open the file in binary mode

    if (!file) { // Check if the file was successfully opened
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }

    // Create a new MD5 context
    EVP_MD_CTX *md5Context = EVP_MD_CTX_new();
    // Initialize the MD5 context with the MD5 algorithm
    EVP_DigestInit_ex(md5Context, EVP_md5(), nullptr);

    constexpr int bufferSize = 1024; // Define the buffer size for reading the file
    char buffer[bufferSize];         // Create a buffer to read file data

    // Read the file in chunks
    while (file.read(buffer, bufferSize))
        // Update the MD5 context with the chunk of data
        EVP_DigestUpdate(md5Context, buffer, bufferSize);
    // Update the MD5 context with the remaining data
    EVP_DigestUpdate(md5Context, buffer, file.gcount());

    // Create an array to store the MD5 digest
    unsigned char digest[MD5_DIGEST_LENGTH];
    // Finalize the MD5 calculation and store the digest
    EVP_DigestFinal_ex(md5Context, digest, nullptr);
    // Free the MD5 context
    EVP_MD_CTX_free(md5Context);

    // Create a string stream for converting the digest to a string
    std::stringstream ss;
    // Set the stream to output hexadecimal values and pad with zeroes
    ss << std::hex << std::setfill('0');

    // Iterate over each byte in the digest
    for (unsigned char byte : digest)
        // Convert each byte to hexadecimal and append to the string stream
        ss << std::setw(2) << static_cast<unsigned int>(byte);

    // Convert the string stream to a string and
    // return the MD5 hash as a hexadecimal string
    return ss.str();
}

bool verifyFileIntegrity(string localFileMD5, string serverFileMD5) {

    if (localFileMD5.empty()) {
        cerr << "\033[0;33m Warning: Unable to calculate MD5 hash of the LOCAL file. \033[0m" << endl;
        return false;
    }

    if (serverFileMD5.empty()) {
        cerr << "\033[0;33m Warning: Unable to calculate MD5 hash of the SERVER file. \033[0m" << endl;
        return false;
    }

    // Compare the MD5 hashes
    if (localFileMD5 == serverFileMD5) {
        cout << "\033[0;32m  The files have the same MD5 hash. Integrity verified. \033[0m" << endl;
        return true;
    } else {
        cout << "\033[0;33m Warning: The files have different MD5 hashes. Integrity verification failed. \033[0m" << endl;
        return false;
    }
}

bool fileExists(string fileName) {
    ifstream file(fileName);
    return file.good();
}

vector<string> getGlobResults(string pattern) {
    vector<string> results;

    glob_t globResult;
    int globStatus = glob(pattern.c_str(), GLOB_TILDE, nullptr, &globResult);

    if (globStatus == 0) {
        // iterate over the matched file paths and add them to the results vector
        for (size_t i = 0; i < globResult.gl_pathc; ++i) {
            results.push_back(globResult.gl_pathv[i]);
        }
    }

    // free the memory allocated by glob() by calling globfree()
    globfree(&globResult);

    return results;
}

vector<string> getGroupOfFiles(string filePatterns) {
    vector<string> matched_files;

    istringstream iss(filePatterns);
    string pattern;

    // process each pattern individually
    while (iss >> pattern) {
        // call the getGlobResults function to get the matching file paths for each pattern
        vector<string> patternResults = getGlobResults(pattern);

        // append the pattern matched_files to the overall matched_files vector
        matched_files.insert(matched_files.end(), patternResults.begin(), patternResults.end());
    }

    return matched_files;
}
