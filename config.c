#include <stdlib.h>
#include <string.h>
#include "config.h"

void create_config_file(FILE *conf_fd){
	fprintf(conf_fd, "# Default configuration file for tocdo.\n");
	fprintf(conf_fd, "\n");
	fprintf(conf_fd, "tocdo_todo_location=%s\n", getenv("HOME"));
	fprintf(conf_fd, "notifications_enabled=0\n");
	// (PRIO:A..B&DUE:7d)|(PRIO:C..D&DUE:2d)|(CONTAINS:"+exam")
	fprintf(conf_fd, "# The setting 'notifications_filter' have the following options:\n");
	fprintf(conf_fd, "# PRIO - Can have a prio set from A-Z and a range for example 'PRIO:A..C\n");
	fprintf(conf_fd, "# DUE - 3D, 3W, 3M, 3Y, 3s, 3m, 3h\n");
	fprintf(conf_fd, "# CONTAINS - Any string inside the todo msg of the entry\n");
	fprintf(conf_fd, "# You can concat as followed: '(PRIO:A..B&DUE:7d)|(PRIO:C..D&DUE:2d)|(CONTAINS:\"very important\"')\n");
	fprintf(conf_fd, "notifications_filter=\n");
	fprintf(conf_fd, "\n");
	fprintf(conf_fd, "# Settings for smtp server to reach. Need to activate as 'notifications_enabled=1' for this to take effect.\n");
	fprintf(conf_fd, "smtp_server=\n");
	fprintf(conf_fd, "smtp_username=\n");
	fprintf(conf_fd, "smtp_password=\n");
}

void load_config_file(config_tocdo *conf, char* config_path){
	FILE *conf_fd = fopen(config_path, "r");
	char conf_char;
	char buffer[5000] = {0};

	for(int i = 0; (conf_char = fgetc(conf_fd)) != EOF && i < 5000; i++){
		if(conf_char == '#'){
			while((conf_char = fgetc(conf_fd)) != '\n'){}
		}else{
			printf("%c", conf_char);
		}
	}
}

void config_init(config_tocdo *conf){
	const char *home = getenv("HOME");
	const char *conf_dir = "/.tocdo/";
	const char *conf_file = "config";
	char *config_path = malloc(strlen(home) + strlen(conf_dir) + strlen(conf_file));
	strncpy(config_path, home, strlen(home));
	strncat(config_path, conf_dir, strlen(conf_dir));
	mkdir(config_path, 0755);
	strncat(config_path, conf_file, strlen(conf_file));

	FILE *conf_fd = fopen(config_path, "r");
	if(conf_fd == NULL){
		conf_fd = fopen(config_path, "wb");
		create_config_file(conf_fd);
	}
	fclose(conf_fd);

	load_config_file(conf, config_path);
}
