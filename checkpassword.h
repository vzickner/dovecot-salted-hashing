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

#define MYSQL_HOSTNAME		"localhost"
#define MYSQL_USERNAME		"root"
#define MYSQL_PASSWORD		""
#define MYSQL_DATABASE		"typo3"
#define MYSQL_QUERY		"SELECT `password` FROM `fe_users` WHERE `username` = '%s' LIMIT 1"
#define MAX_LENGTH_OF_DATA	256
#define ENABLE_MD5
#define ENABLE_LOGGING		1
#define LOGFILE			"/var/log/checkpassword.log"

int readUserData(char *username, char *password, int maxLength);
int proveUserData(char *username, char *password, int maxLength);
char *md5enc(char *text, int length);
int logMessage(char *message);
