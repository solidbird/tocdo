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

int match_at(char *string, char *pattern, int *start, int *end){
	int result;
	regex_t reg;
	regmatch_t match;

	if(regcomp(&reg, pattern, REG_EXTENDED) != 0) return -1;
	result = regexec(&reg, string, 1, &match, 0);
	regfree(&reg);

	*start = match.rm_so;
	*end = match.rm_eo-1;

	return result;
}

int match(char *string, char *pattern){
	int result;
	regex_t reg;

	if(regcomp(&reg, pattern, REG_EXTENDED | REG_NOSUB) != 0) return -1;
	result = regexec(&reg, string, 0, 0, 0);
	regfree(&reg);

	return result;
}

char* parse_tocdo_done_task(char **entries, size_t n){
	//TODO
	return NULL;
}

void concat_word_entries(char *build_entry, char** entries, size_t start, size_t end){
	for(size_t i = start; i < end; i++){
		strncat(build_entry, entries[i], strlen(entries[i]));
		if(i < end-1) strcat(build_entry, " ");
	}
}

void prio_key_update(char *build_entry, size_t n){
	char *special_tag_pattern = "((prio)\\:([A-Z])+)";
	char *token_context;
	char *tmp = malloc(strlen(build_entry));
	char *tmp_cut = malloc(strlen(build_entry));	

	int s = 0, e = 0;
	strcpy(tmp_cut, build_entry);
	if(!match_at(build_entry, special_tag_pattern, &s, &e)){
		char *prio_value = strtok_r(build_entry + s, ": ", &token_context);
		prio_value = strtok_r(NULL, ": ", &token_context);

		strcpy(tmp_cut + s, token_context);
		sprintf(tmp, "(%c) %s", prio_value[0], tmp_cut);
		strcpy(build_entry, tmp);
		free(tmp);
		free(tmp_cut);
	}
}

char* update_tocdo_entry_date(char **entries, size_t n){
	char *done_pattern = "^(X|x)";
	char *prio_pattern = "^(\\([A-Z]\\))";
	char *date_pattern = "^([0-9]{4}-[0-9]{2}-[0-9]{2})";
	// "x? (\([A-Z]\))? ([0-9]{4}-[0-9]{2}-[0-9]{2})? ([0-9]{4}-[0-9]{2}-[0-9]{2})? (.)*"
	// ----------------------------------------------------------------------------------
	// ^x									- 	done
	// ^(\([A-Z]\))							- 	prio
	// ([0-9]{4}-[0-9]{2}-[0-9]{2})?		- 	completion date
	// ([0-9]{4}-[0-9]{2}-[0-9]{2})?		- 	creation date
	// (.)*									-	description

	int total_size = 20 + n;
	for(size_t i = 0; i < n; i++){
		total_size += strlen(entries[i]);
	}

	char *build_entry = malloc(total_size);
	char date[100] = {0};
	today_date_str(date);

	if(!match(entries[0], done_pattern)){
		return parse_tocdo_done_task(entries, n);
	}

	if(!match(entries[0], date_pattern)){
		today_date_str(date);
		strncpy(build_entry, date, strlen(date));
		if(n == 1){
			strncat(build_entry, &entries[0][strlen(date)], strlen(entries[0])-strlen(date));
		}else{
			strcat(build_entry, " ");
			concat_word_entries(build_entry, entries, 1, n);
		}

		return build_entry;
	}

	if(!match(entries[0], prio_pattern)){
		strncpy(build_entry, entries[0], strlen("(X)"));
		strcat(build_entry, " ");
		strncat(build_entry, date, strlen(date));		
		strcat(build_entry, " ");
		if(n == 1){
			int offset = strlen("(X)") + 1;
			if(!match(&entries[0][offset], date_pattern)) offset += strlen(date) + 1; 
			strncat(build_entry, &(entries[0][offset]), strlen(entries[0])-offset);
		}else{
			size_t start = 1;
			if(!match(entries[1], date_pattern)) start++; 
			concat_word_entries(build_entry, entries, start, n);
		}

		return build_entry;

	}
	
	strncpy(build_entry, date, strlen(date));
	strcat(build_entry, " ");
	concat_word_entries(build_entry, entries, 0, n);

	return build_entry;
}

void* add_tocdo(cli_cmd_group *m, cli_cmd_group *c, void *void_args){
	config_tocdo *conf = (config_tocdo *)void_args;
	
	char **entries = c->arg_head->result->ls;
	size_t size = c->arg_head->item.type_block.n;
	
	char* todo_entry;
	if(todo_entry = update_tocdo_entry_date(entries, size)){
		prio_key_update(todo_entry, size);
		char *todo_file = malloc(strlen(conf->todo_file_location) + strlen("/todo.txt"));
		strcpy(todo_file, conf->todo_file_location);
		strcat(todo_file, "/todo.txt");

		FILE *entry_file = fopen(todo_file, "a+");
		fwrite(todo_entry, sizeof(char), strlen(todo_entry), entry_file);
		fputc('\n', entry_file);
	}
	free(todo_entry);
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
	if(!res){
		fprintf(stderr, "FAIL");
	}
	return !res;
}
