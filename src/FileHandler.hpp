#ifndef FILE_HANFLER_H
#define FILE_HANFLER_H


#include <bits/stdc++.h>

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

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

#endif