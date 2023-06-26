#ifndef FILE_HANFLER_H
#define FILE_HANFLER_H


#include <bits/stdc++.h>

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <iomanip>
#include <openssl/md5.h>
#include <glob.h>
#include <openssl/evp.h>
#include <unistd.h>

#include "RawSocketConnection.hpp"


#define MAX_DATA_SIZE 63

using namespace std;

/*!
    * \brief Read a file and get its content limited by $MAX_DATA_SIZE bytes
    * \param fileName
    * \param start_position: position of the file to start reading
    * \param fileSize: in bytes
    * \return string with file content
*/
string get_file_content(string fileName, unsigned int start_position, int fileSize, int* bytesRead);

/*!
    * \brief Get the size of a file
    * \param fileName
    * \return unsigned int with file size
*/
unsigned int get_file_size(string fileName);

/*!
    * \brief Write data to a file
    * \param fileName
    * \param data: char array with data to be written
    * \param append: if true, append data to the end of the file; otherwise, overwrite the file
    * \param dataSize: size of the data to be written
*/
void write_to_file(string fileName, unsigned char data[MAX_DATA_SIZE], bool append, size_t dataSize);

/*!
 * \brief Calculates the MD5 of the file
 * \param filename: name of the file
 * \return MD5 hash of the file
 */
string calculateMD5(string filename);

/*!
 * \brief Verifies the integrity of the file (MD5)
 * \param localFile: name of the local file
 * \param serverFile: name of the server file
 * \return true if the files are equal, false otherwise
 */
bool verifyFileIntegrity(string localFile, string serverFile);

/*!
 * \brief Checks if the file exists
 * \param fileName: name of the file
 * \return true if the file exists, false otherwise
 */
bool fileExists(string fileName);

/*!
 * \brief Gets the glob results from the pattern
 * \param pattern: one pattern to be searched;
 * example: "*.txt"
 * \return vector of strings with the results (file names)
 */
vector<string> getGlobResults(string pattern);

/*!
 * \brief Gets the group of files from the patterns
 * \param filesPattern: patterns to be searched separated by space;
 * example: "*.txt *.pdf *.png"
 * \return vector of strings with the results (file names)
 */
vector<string> getGroupOfFiles(string filesPattern);

#endif