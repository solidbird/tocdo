#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <sys/stat.h>

typedef struct {
	char* todo_file_location;
	int notifications_enabled;
	char* notifications_filter;
	char* smtp_server;
	char* smtp_username;
	char* smtp_password;
} config_tocdo;

void create_config_file(FILE *conf_fd);
void load_config_file(config_tocdo *conf, char* config_path);
void config_init(config_tocdo *conf);

#endif //CONFIG_H
