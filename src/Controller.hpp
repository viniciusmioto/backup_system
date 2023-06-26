#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Communication.hpp"

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
 * \brief Sends a group of files to the socket
 * \param socket: socket to send the files
 * \param filesPatterns: patterns to be searched separated by space;
 * example: "*.txt *.pdf myFile.png"
 */
void sendGroupOfFiles(int socket, string filesPatterns, int &msgCounter);

/*!
 * \brief Receives the file name and open file to write (overwrite)
 * \param interface: network interface
 * \param recvMessage: message received
 * \return file name
 */
string getFileName(char interface[], Message recvMessage);

/*!
 * \brief Receives one from the socket
 * \param socket: socket to receive the file
 * \param interface: network interface
 * \param recvMessage: message received
 * \param msgCounter: sequence number of the message
 */
void receiveOneFile(int socket, char interface[], int &msgCounter);

/*!
 * \brief Receives a group of files from the socket
 * \param socket: socket to receive the files
 * \param interface: network interface
 * \param msgCounter: sequence number of the message
 */
void receiveGroupOfFiles(int socket, char interface[], int &msgCounter);

/*!
 * \brief Gets the current directory
 * \return current directory
 */
string getCurrentDirectory();

/*!
 * \brief Changes the current directory to the path
 * \param path: path of the directory
 * \return true if the directory exists, false otherwise
 */
bool changeDirectory(const string &path);

/*!
 * \brief Send to change server working directory
 * \param socket: socket to send the directory
 * \param path: path of the directory
 * \param msgCounter: sequence number of the message
 */
void sendServerDirectory(int socket, string path, int &msgCounter);

/*!
 * \brief Receives to change the server working directory
 * \param socket: socket to receive the directory
 * \param recvMessage: message received
 * \param msgCounter: sequence number of the message
 */
void receiveServerDirectory(int socket, Message recvMessage, int &msgCounter);

/*!
 * \brief Gets the server working directory
 * \param socket: socket to receive the directory
 * \param msgCounter: sequence number of the message
 */
void getServerWorkingDirectory(int socket, int &msgCounter);

/*!
 * \brief Sends the server working directory
 * \param socket: socket to send the directory
 * \param msgCounter: sequence number of the message
 */
void sendServerWorkingDirectory(int socket, int &msgCounter);

/*!
 * \brief Receive file from the server
 * \param socket: socket to receive the file
 * \param interface: network interface
 * \param fileName: name of the file to restore
 * \param msgCounter: sequence number of the message
 */
void restoreOneFile(int socket, char interface[], string fileName, int &msgCounter);

/*!
 * \brief Receive group of files from the server
 * \param socket: socket to receive the files
 * \param interface: network interface
 * \param filesPattern: patterns to be searched separated by space;
 * example: "*.txt *.pdf myFile.png"
 * \param msgCounter: sequence number of the message
 */
void restoreGroupOfFiles(int socket, char *interface, string filesPattern, int &msgCounter);

void verifyBackup(int socket, string fileName, int &msgCounter);

void sendVerifyBackup(int socket, char *interface, string fileName, int &msgCounter);

#endif