#include <stdio.h>
#include <time.h>
#include <regex.h>
#include "config.h"

#define CLI_IMPLEMENTATION
#include "CLIH/CLI.h"

typedef struct file_offset_list {
	int index;
	long file_offset;
	struct file_offset_list *next;
	struct file_offset_list *prev;
} file_offset_list;

void today_date_str(char *date_str, int delta_year, int delta_month, int delta_day){
	time_t now;
	struct tm *tm_info;

	time(&now);
	tm_info = localtime(&now);
	tm_info->tm_year+=delta_year;
	tm_info->tm_mon+=delta_month;
	tm_info->tm_mday+=delta_day;
	mktime(tm_info);
	strftime(date_str, 100, "%Y-%m-%d", tm_info);
}

int match_at(char *string, const char *pattern, int *start, int *end){
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

int match(char *string, const char *pattern){
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

void due_key_update(char *build_entry){
	const char *due_tag_pattern = "((due)\\:([0-9])+([d|w|m|y])(,([0-9])+([d|w|m|y]))*)";
	char *token_context;
	char *tmp = malloc(strlen(build_entry));
	char *tmp_cut = malloc(strlen(build_entry));	
	char *build_entry_cut_end = malloc(strlen(build_entry));

	int s = 0, e = 0;
	char *build_tmp = malloc(strlen(build_entry));
	strcpy(tmp_cut, build_entry);
	strcpy(build_tmp, build_entry);
	if(!match_at(build_tmp, due_tag_pattern, &s, &e)){
		char *due_value = strtok_r(build_tmp + s, ": ", &token_context);
		due_value = strtok_r(NULL, ": ", &token_context);

		strcpy(tmp_cut + s, token_context);
		char *token_context_within;
		char *due_value_within = strtok_r(due_value, ",", &token_context_within);
		char transform_date[100] = {0};
		int y = 0, m = 0, d = 0;
		while(due_value_within != NULL){
			char time_suffix = due_value_within[strlen(due_value_within)-1];
			due_value_within[strlen(due_value_within)-1] = '\0';

			switch(time_suffix){
				case 'd':
					d += atoi(due_value_within);
				break;
				case 'w':
					d += (atoi(due_value_within)) * 7;
				break;
				case 'm':
					m += atoi(due_value_within);
				break;
				case 'y':
					y += atoi(due_value_within);
				break;
				default:
				break;
			}	
			due_value_within = strtok_r(NULL, ",", &token_context_within);
		}

		//TODO: make  and put that at the place where
		//the due:... dotted value should be. Replace it.
		today_date_str(transform_date, y, m, d);
		build_entry_cut_end = malloc(strlen(build_entry));
		memset(build_entry_cut_end, 0, strlen(build_entry));
		if(build_entry[e + 1] != '\0'){
			strncpy(build_entry_cut_end, build_entry + e + 1, strlen(build_entry + e + 1));
		}
		strncpy(build_entry + s + 4, transform_date, strlen(transform_date));
		build_entry[s + 4 + strlen(transform_date)] = '\0';
		strncat(build_entry, build_entry_cut_end, strlen(build_entry_cut_end));
		//strcpy(build_entry, tmp);
	}
	free(tmp);
	free(tmp_cut);
	free(build_tmp);
	free(build_entry_cut_end);
}

void prio_key_update(char *build_entry){
	const char *special_tag_pattern = "((prio)\\:([A-Z])+)";
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
	}
	free(tmp);
	free(tmp_cut);
}

char* update_tocdo_entry_date(char **entries, size_t n){
	const char *done_pattern = "^(X|x)";
	const char *prio_pattern = "^(\\([A-Z]\\))";
	const char *date_pattern = "^([0-9]{4}-[0-9]{2}-[0-9]{2})";
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
	char *build_entry_tmp = malloc(total_size);
	char date[100] = {0};
	today_date_str(date,0,0,0);
	concat_word_entries(build_entry_tmp, entries, 0, n);

	if(!match(build_entry_tmp, done_pattern)){
		return parse_tocdo_done_task(entries, n);
	}

	if(!match(build_entry_tmp, date_pattern)){
		strncpy(build_entry, date, strlen(date));
		strncat(build_entry, &build_entry_tmp[strlen(date)], strlen(build_entry_tmp)-strlen(date));

		return build_entry;
	}

	if(!match(build_entry_tmp, prio_pattern)){
		strncpy(build_entry, build_entry_tmp, strlen("(X)"));
		strcat(build_entry, " ");
		strncat(build_entry, date, strlen(date));		
		strcat(build_entry, " ");

		int offset = strlen("(X)") + 1;
		if(!match(&build_entry_tmp[offset], date_pattern)) offset += strlen(date) + 1; 
		strncat(build_entry, &(build_entry_tmp[offset]), strlen(build_entry_tmp)-offset);

		return build_entry;
	}
	
	strncpy(build_entry, date, strlen(date));
	strcat(build_entry, " ");
	concat_word_entries(build_entry, entries, 0, n);

	return build_entry;
}

void* add_tocdo(cli_cmd_group *m, cli_cmd_group *c, void *void_args){
	FILE *entry_file = (FILE *)void_args;
	
	char **entries = c->arg_head->result->ls;
	size_t size = c->arg_head->item.type_block.n;
	
	char* todo_entry;
	if(todo_entry = update_tocdo_entry_date(entries, size)){
		prio_key_update(todo_entry);
		due_key_update(todo_entry);

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

int del_file_offset(file_offset_list **fol_head, int index){
	file_offset_list *tmp = (*fol_head);
	while(tmp != NULL){
		if(tmp->index != index){
			tmp = tmp->next;
			continue;
		}

		//TODO: if node alone make just delete, if only prev null set the next prev to null, if only next null etc...

		if(tmp->prev){
			tmp->prev->next = tmp->next;
		}else{
			*fol_head = tmp->next;
		}
		if(tmp->next) tmp->next->prev = tmp->prev;
		free(tmp);
		return 1;
	}
	return 0;
}

void add_file_offset(file_offset_list **fol_head, FILE *todo_file){
	if((*fol_head) == NULL){
		(*fol_head) = malloc(sizeof(file_offset_list));
		(*fol_head)->index = 1;
		(*fol_head)->file_offset = ftell(todo_file);
		(*fol_head)->next = NULL;
		(*fol_head)->prev = NULL;
	}else{
		file_offset_list *tmp = (*fol_head);
		while(tmp->next != NULL){
			(tmp) = (tmp)->next;
		}
		(tmp)->next = malloc(sizeof(file_offset_list));
		(tmp)->next->file_offset = ftell(todo_file);
		(tmp)->next->index = (tmp)->index + 1;
		(tmp)->next->prev = (tmp);
	}
}

void print_file_offset(file_offset_list *fol, FILE *todo_file){
	file_offset_list *tmp = fol;
	while(tmp != NULL){
		fseek(todo_file, tmp->file_offset, SEEK_SET);
		char read;
		printf("[%d] ", tmp->index);
		while((read = fgetc(todo_file)) != EOF){
			printf("%c", read);
			if(read == '\n') break;
		}
		tmp = tmp->next;
	}
}

void filter_str_file_offset(file_offset_list **fol_head, FILE *file, char *str_filter){
	file_offset_list *tmp = (*fol_head);
	char read;
	while(tmp != NULL){
		int buffer_size = 1;
		char *build_line = malloc(buffer_size);
		fseek(file, tmp->file_offset, SEEK_SET);
		while((read = fgetc(file)) != EOF){
			if(read == '\n') break;
			build_line[buffer_size - 1] = read;
			build_line = realloc(build_line, ++buffer_size);
		}
		if(match(build_line, str_filter)) del_file_offset(fol_head, tmp->index);
		//if(!strstr(build_line, str_filter)) del_file_offset(fol_head, tmp->index);
		memset(build_line, 0, buffer_size);
		free(build_line);
		tmp = tmp->next;
	}
}

void* show_tocdo(cli_cmd_group *m, cli_cmd_group *c, void *void_args){
	//TODO: allow filter with expr and sort options
	FILE *todo_file = (FILE*) void_args;
	char file_chr;
	file_offset_list *fol = NULL;

	while((file_chr = fgetc(todo_file)) != EOF){
		if(file_chr != '\n') continue;
		add_file_offset(&fol, todo_file);
	}

	cli_opt_list *search_opt = find_opt_name(c->opt_head, "--search");
	if(search_opt->result) filter_str_file_offset(&fol, todo_file, search_opt->result->s);
	rewind(todo_file);
	print_file_offset(fol, todo_file);
}

void* configurate_tocdo(cli_cmd_group *m, cli_cmd_group *c, void *void_args){
	for(int i = 0; i < c->arg_head->item.type_block.n; i++){
		printf("[%s]", c->arg_head->result->ls[i]);
	}
	printf("\n");
}

cli_list* arg_parser(int argc, char **argv, config_tocdo *conf, FILE *todo_file){
	char* program_descr = "CLI Tool for todo.txt implementation";
	cli_list *list = cli_init(program_descr, NULL);
	cli_cmd_group *main = list->opt_arg_grp;
	//cli_add_opt(main, (cli_opt_item){"-f", "--file", "Location of the todo.txt file.", STRING, 0, 0});
	//cli_set_default(*main, "--file", (cli_result) "~/todo.txt");

	char *help_cg_add[2] = {"-H", "--HELP"};
	cli_cmd_group *cg_add = cli_add_cmd_grp(list, "add", "Add todo entry.", help_cg_add, add_tocdo, todo_file);
	cli_grp_add_arg(cg_add, (cli_arg_item){"TODO_ENTRY", "Todo entry which should be added to the todo.txt file.", STRING, -1, 1});

	cli_cmd_group *cg_del = cli_add_cmd_grp(list, "del", "Delete todo entry.", NULL, del_tocdo, NULL);
	cli_grp_add_arg(cg_del, (cli_arg_item){"TODO_ENTRY", "Todo entry which should be deleted from the todo.txt file.", STRING, -1, 1});

	cli_cmd_group *cg_edit = cli_add_cmd_grp(list, "edit", "Edit", NULL, edit_tocdo, NULL);
	cli_grp_add_arg(cg_edit, (cli_arg_item){"TODO_ENTRY", "Edit", STRING, -1, 1});

	//FILE *entry_file = fopen("/home/ich/todo.txt", "r");
	cli_cmd_group *cg_show = cli_add_cmd_grp(list, "show", "Show", NULL, show_tocdo, todo_file);
	cli_grp_add_opt(cg_show, (cli_opt_item){"-s", "--search", "Search string within the todos.", STRING, 0, 0});

	cli_cmd_group *cg_config = cli_add_cmd_grp(list, "config", "Config", NULL, configurate_tocdo, NULL);
	cli_grp_add_arg(cg_config, (cli_arg_item){"TODO_ENTRY", "Config", STRING, -1, 1});

	return list;
}

int main(int argc, char **argv){
	config_tocdo *conf = config_init();	
	char *todo_file = malloc(strlen(conf->todo_file_location) + strlen("/todo.txt"));
	strcpy(todo_file, conf->todo_file_location);
	strcat(todo_file, "/todo.txt");
	FILE *entry_file = fopen(todo_file, "a+");

	cli_list *list = arg_parser(argc, argv, conf, entry_file);

	int res = cli_execute(list, argc, argv);
	if(!res){
		fprintf(stderr, "FAIL");
	}
	return !res;
}
