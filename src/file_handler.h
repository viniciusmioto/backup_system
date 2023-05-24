#ifndef FILE_HANFLER_H
#define FILE_HANFLER_H


#include <bits/stdc++.h>

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

#include "ConexaoRawSocket.h"


#define MAX_DATA_SIZE 63

using namespace std;

/*!
    * \brief Read a file and get its content limited by $MAX_DATA_SIZE bytes
    * \param file_name
    * \param start_position
    * \param file_size
    * \return string with file content
*/
string get_file_content(string file_name, unsigned int start_position, int file_size);

/*!
    * \brief Get the size of a file
    * \param file_name
    * \return unsigned int with file size
*/
unsigned int get_file_size(string file_name);

/*!
    * \brief Write data to a file
    * \param filename
    * \param data
    * \param append
*/
void write_to_file(string filename, unsigned char data[MAX_DATA_SIZE], bool append);

#endif