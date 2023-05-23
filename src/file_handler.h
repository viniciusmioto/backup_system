#ifndef FILE_HANFLER_H
#define FILE_HANFLER_H


#include <bits/stdc++.h>

#include <fstream>
#include <iostream>
#include <string>

#include "ConexaoRawSocket.h"


#define MAX_DATA_SIZE 63

using namespace std;

string get_file_content(const string &file_name, unsigned int start_position, int file_size);

unsigned int get_file_size(const string &file_name);

void write_to_file(const char* filename, unsigned char data[MAX_DATA_SIZE], bool append);

#endif