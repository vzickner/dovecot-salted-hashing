/**
 * Copyright 2013 Valentin Zickner zickner(at)querformatik(dot)de
 *                Querformatik UG (haftungsbeschraenkt)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "checkpassword.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <my_global.h>
#include <mysql.h>
#include <crypt.h>
#include <getopt.h>
#ifdef ENABLE_MD5
#include <openssl/md5.h>
#endif

int main(int argc, char **argv) {
	int maxLength = MAX_LENGTH_OF_DATA;
	char *username = (char *)malloc(maxLength * sizeof(char));
	char *password = (char *)malloc(maxLength * sizeof(char));
	char *correctPassword = (char *)malloc(maxLength * sizeof(char));
	char *encryptedPassword;


	if (argc < 2) {
		logMessage("ERR: Argument is missing");
		return 2;
	}

	// Read data from dovecot
	if (readUserData(username, password, maxLength) < 2) {
		logMessage("ERR: Reading account data failed");
		return 2;
	}

	// Check data against mysql
	if (proveUserData(username, correctPassword, maxLength) != 0) {
		logMessage("ERR: Database connection or SQL error.");
		return 1;
	}

#ifdef ENABLE_MD5
	if (index(correctPassword, '$') != NULL) {
#endif
		// Encrypt password from user, use correct password as configuration (salt)
		encryptedPassword = crypt(password, correctPassword);
		logMessage("INFO: Password is salted");
#ifdef ENABLE_MD5
	} else {
		// Encrypt password from user as md5
		encryptedPassword = md5enc(password, strlen(password));
		logMessage("INFO: Password is md5 hash");
	}
#endif

	// Check password against correct password.
	if (strlen(encryptedPassword) == strlen(correctPassword) &&
		strncmp(encryptedPassword, correctPassword, strlen(correctPassword)) == 0) {

		int status = 0;
		pid_t processId = fork();
		switch (processId) {
			case -1: // Failed
				 break;
			case 0: 
				setenv("AUTHORIZED", "2", 1);
				execl(argv[1], "");
				exit(0);
			default:
				wait(&status);
				logMessage("INFO: Login success");
				break;
		}
		return 0;
	}
        
	logMessage("ERR: Authentication failed");
	return 1;
}

/**
 * Read user data from descriptor 3 and pass to username and password.
 *
 * @param string username Location to write username in
 * @param string password Location to write password in
 * @param integer maxLength Maximum of username and password should be.
 */
int readUserData(char *username, char *password, int maxLength) {
	int pointer = 0;
	int position = 0;
	FILE *input = fdopen(3, "r");
	if (input) {
		while (!feof(input)) {
			char read = fgetc(input);
			if (pointer == 0 && position < maxLength) {
				username[position] = read;
				position++;
			}
			if (pointer == 1 && position < maxLength) {
				password[position] = read;
				position++;
			}

			if (read == '\0') {
				pointer++;
				position = 0;
			}
		}
        
		fclose(input);
	}
	return pointer;
}

/**
 * Prove data from user login, use username for database query.
 *
 * @param string username Username to prove against database.
 * @param string password The password stored in database will be in this.
 * @param integer maxLength Maximum length of username and password.
 */
int proveUserData(char *username, char *password, int maxLength) {
	MYSQL *connection;
	MYSQL_RES *result;
	MYSQL_ROW row;
	unsigned long *lengths;
	
	connection = mysql_init(NULL);
	if (!mysql_real_connect(connection, MYSQL_HOSTNAME, MYSQL_USERNAME,
			MYSQL_PASSWORD, MYSQL_DATABASE, 0, NULL, 0)) {
		return 1;
	}

	char *usernameEscaped = malloc(maxLength* sizeof(char));

	mysql_real_escape_string(connection, usernameEscaped, username, min(strlen(username), maxLength));
	char *selectQuery = malloc((strlen(usernameEscaped)+strlen(MYSQL_QUERY))*sizeof(char));
	sprintf(selectQuery, MYSQL_QUERY, usernameEscaped);

	if (mysql_query(connection, selectQuery) != 0) {
		return 2;
	}

	result = mysql_store_result(connection);
	if (mysql_num_fields(result) < 1) {
		return 3;
	}
	if (mysql_num_rows(result) < 1) {
		return 4;
	}
	row = mysql_fetch_row(result);
	lengths = mysql_fetch_lengths(result);

	strncpy(password, row[0], min(maxLength, lengths[0]));
	
	mysql_free_result(result);
	mysql_close(connection);
	return 0;
}

#ifdef ENABLE_MD5
/**
 * Encrypt text using md5.
 *
 * @param string text Text to encrypt.
 * @param integer length Length of the text to encrypt.
 */
char *md5enc(char *text, int length) {
	int i;
	char *encrypted = (char *)malloc((2*MD5_DIGEST_LENGTH+1) * sizeof(char));
	unsigned char result[MD5_DIGEST_LENGTH];
	MD5(text, length, result);
	for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
		char tempString[3];
		sprintf(tempString, "%02x",result[i]);
		encrypted[i*2]   = tempString[0];
		encrypted[(i*2)+1] = tempString[1];
	}
	encrypted[2*MD5_DIGEST_LENGTH] = '\0';
	return encrypted;
}
#endif

#ifdef ENABLE_LOGGING
int logMessage(char *message) {
	FILE *log = fopen(LOGFILE, "a"); 
	fputs(message, log);
	fputc('\n', log);
	fclose(log);
}
#endif
