#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Communication.hpp"
#include <glob.h>

/*!
 * \brief Sends the file data to the socket
 * \param socket
 * \param fileName: name of the file to be sent
 * \param msgCounter: sequence number of the message
 */
void sendFileData(int socket, string fileName, int &msgCounter);

/*!
 * \brief Sends one file to the socket
 * \param socket
 */
void sendOneFile(int socket);

/*!
 * \brief 
 */
void getGroupOfFiles();

vector<string> getGlobResults(const string& pattern);

#endif