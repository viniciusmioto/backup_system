#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Communication.hpp"
#include <glob.h>

/*!
 * \brief Sends the file data to the socket
 * \param socket: socket to send the file
 * \param fileName: name of the file to be sent
 * \param msgCounter: sequence number of the message
 */
void sendFileData(int socket, string fileName, int &msgCounter);

/*!
 * \brief Sends one file to the socket
 * \param socket: socket to send the file
 * \param fileName: name of the file to be sent
 */
void sendOneFile(int socket, string fileName, int &msgCounter);

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

/*!
 * \brief Sends a group of files to the socket
 * \param socket: socket to send the files
 * \param filesPatterns: patterns to be searched separated by space;
 * example: "*.txt *.pdf myFile.png"
 */
void sendGroupOfFiles(int socket, string filesPatterns, int &msgCounter);

string getFileName(int socket, char sock[], Message recvMessage, int &msgCounter);

void receiveOneFile(int socket, char sock[], int &msgCounter);

void receiveGroupOfFiles(int socket, char sock[], int &msgCounter);

#endif