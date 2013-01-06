all:
	gcc -I/path/to/openssl/ -lcrypto -lcrypt `mysql_config --cflags --libs` -o checkpassword checkpassword.c
