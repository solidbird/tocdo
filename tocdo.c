#include <stdio.h>
#include <time.h>
#include <regex.h>
#include "config.h"

#define CLI_IMPLEMENTATION
#include "CLIH/CLI.h"

void today_date_str(char *date_str){
	time_t now;
	struct tm *tm_info;

	time(&now);
	tm_info = localtime(&now);
	strftime(date_str, 100, "%Y-%m-%d", tm_info);
}

int match(char *string, char *pattern){
	int result;
	regex_t reg;

	if(regcomp(&reg, pattern, REG_EXTENDED | REG_NOSUB) != 0) return -1;
	result = regexec(&reg, string, 0, 0, 0);
	regfree(&reg);

	return result;
}

int parse_tocdo_entry(char **entries, size_t n){
	if(!match(entries[0], "[0-9]{4}-[0-9]{2}-[0-9]{2}")){
		today_date_str(entries[0]);
	}

	for(int i = 0; i < n; i++){
		printf("[%s]", entries[i]);
	}
	printf("\n");
	return 1;
}

void* add_tocdo(cli_cmd_group *m, cli_cmd_group *c, void *void_args){
	config_tocdo *conf = (config_tocdo *)void_args;
	
	char **entries = c->arg_head->result->ls;
	size_t size = c->arg_head->item.type_block.n;
	
	if(parse_tocdo_entry(entries, size)){
		char *todo_file = malloc(strlen(conf->todo_file_location) + strlen("/todo.txt"));
		strcpy(todo_file, conf->todo_file_location);
		strcat(todo_file, "/todo.txt");

		//char date_str[100];
		//today_date_str(date_str);

		FILE *entry_file = fopen(todo_file, "a+");
		//fwrite(date_str, sizeof(char), strlen(date_str), entry_file);
		//fputc(' ', entry_file);

		for(int i = 0; i < size; i++){
			fwrite(entries[i], sizeof(char), strlen(entries[i]), entry_file);
			if(i != size - 1) fputc(' ', entry_file);
		}
		fputc('\n', entry_file);
	}
}

void* del_tocdo(cli_cmd_group *m, cli_cmd_group *c, void *void_args){
	for(int i = 0; i < c->arg_head->item.type_block.n; i++){
		printf("[%s]", c->arg_head->result->ls[i]);
	}
	printf("\n");
}

void* edit_tocdo(cli_cmd_group *m, cli_cmd_group *c, void *void_args){
	for(int i = 0; i < c->arg_head->item.type_block.n; i++){
		printf("[%s]", c->arg_head->result->ls[i]);
	}
	printf("\n");
}

void* show_tocdo(cli_cmd_group *m, cli_cmd_group *c, void *void_args){
	for(int i = 0; i < c->arg_head->item.type_block.n; i++){
		printf("[%s]", c->arg_head->result->ls[i]);
	}
	printf("\n");
}

void* configurate_tocdo(cli_cmd_group *m, cli_cmd_group *c, void *void_args){
	for(int i = 0; i < c->arg_head->item.type_block.n; i++){
		printf("[%s]", c->arg_head->result->ls[i]);
	}
	printf("\n");
}

cli_list* arg_parser(int argc, char **argv, config_tocdo *conf){
	char* program_descr = "CLI Tool for todo.txt implementation";
	cli_list *list = cli_init(program_descr, NULL);
	cli_cmd_group *main = list->opt_arg_grp;
	//cli_add_opt(main, (cli_opt_item){"-f", "--file", "Location of the todo.txt file.", STRING, 0, 0});
	//cli_set_default(*main, "--file", (cli_result) "~/todo.txt");

	char *help_cg_add[2] = {"-H", "--HELP"};
	cli_cmd_group *cg_add = cli_add_cmd_grp(list, "add", "Add todo entry.", help_cg_add, add_tocdo, conf);
	cli_grp_add_arg(cg_add, (cli_arg_item){"TODO_ENTRY", "Todo entry which should be added to the todo.txt file.", STRING, -1, 1});

	cli_cmd_group *cg_del = cli_add_cmd_grp(list, "del", "Delete todo entry.", NULL, del_tocdo, NULL);
	cli_grp_add_arg(cg_del, (cli_arg_item){"TODO_ENTRY", "Todo entry which should be deleted from the todo.txt file.", STRING, -1, 1});

	cli_cmd_group *cg_edit = cli_add_cmd_grp(list, "edit", "Edit", NULL, edit_tocdo, NULL);
	cli_grp_add_arg(cg_edit, (cli_arg_item){"TODO_ENTRY", "Edit", STRING, -1, 1});

	cli_cmd_group *cg_show = cli_add_cmd_grp(list, "show", "Show", NULL, show_tocdo, NULL);
	cli_grp_add_arg(cg_show, (cli_arg_item){"TODO_ENTRY", "Show", STRING, -1, 1});

	cli_cmd_group *cg_config = cli_add_cmd_grp(list, "config", "Config", NULL, configurate_tocdo, NULL);
	cli_grp_add_arg(cg_config, (cli_arg_item){"TODO_ENTRY", "Config", STRING, -1, 1});

	return list;
}

int main(int argc, char **argv){
	config_tocdo *conf = config_init();	
	cli_list *list = arg_parser(argc, argv, conf);

	int res = cli_execute(list, argc, argv);
	return !res;
}
