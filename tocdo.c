#include <stdio.h>
#include "config.h"

#define CLI_IMPLEMENTATION
#include "CLIH/CLI.h"

void* add_tocdo(cli_cmd_group *m, cli_cmd_group *c){
	for(int i = 0; i < c->arg_head->item.type_block.n; i++){
		printf("[%s]", c->arg_head->result->ls[i]);
	}
	printf("\n");
}

void* del_tocdo(cli_cmd_group *m, cli_cmd_group *c){
	for(int i = 0; i < c->arg_head->item.type_block.n; i++){
		printf("[%s]", c->arg_head->result->ls[i]);
	}
	printf("\n");
}

void* edit_tocdo(cli_cmd_group *m, cli_cmd_group *c){
	for(int i = 0; i < c->arg_head->item.type_block.n; i++){
		printf("[%s]", c->arg_head->result->ls[i]);
	}
	printf("\n");
}

void* show_tocdo(cli_cmd_group *m, cli_cmd_group *c){
	for(int i = 0; i < c->arg_head->item.type_block.n; i++){
		printf("[%s]", c->arg_head->result->ls[i]);
	}
	printf("\n");
}

void* configurate_tocdo(cli_cmd_group *m, cli_cmd_group *c){
	for(int i = 0; i < c->arg_head->item.type_block.n; i++){
		printf("[%s]", c->arg_head->result->ls[i]);
	}
	printf("\n");
}

cli_list* arg_parser(int argc, char **argv){
	char* program_descr = "CLI Tool for todo.txt implementation";
	cli_list *list = cli_init(program_descr, NULL);
	cli_cmd_group *main = list->opt_arg_grp;
	//cli_add_opt(main, (cli_opt_item){"-f", "--file", "Location of the todo.txt file.", STRING, 0, 0});
	//cli_set_default(*main, "--file", (cli_result) "~/todo.txt");

	char *help_cg_add[2] = {"-H", "--HELP"};
	cli_cmd_group *cg_add = cli_add_cmd_grp(list, "add", "Add todo entry.", help_cg_add, add_tocdo);
	cli_grp_add_arg(cg_add, (cli_arg_item){"TODO_ENTRY", "Todo entry which should be added to the todo.txt file.", STRING, -1, 1});

	cli_cmd_group *cg_del = cli_add_cmd_grp(list, "del", "Delete todo entry.", NULL, del_tocdo);
	cli_grp_add_arg(cg_del, (cli_arg_item){"TODO_ENTRY", "Todo entry which should be deleted from the todo.txt file.", STRING, -1, 1});

	cli_cmd_group *cg_edit = cli_add_cmd_grp(list, "edit", "Edit", NULL, edit_tocdo);
	cli_grp_add_arg(cg_edit, (cli_arg_item){"TODO_ENTRY", "Edit", STRING, -1, 1});

	cli_cmd_group *cg_show = cli_add_cmd_grp(list, "show", "Show", NULL, show_tocdo);
	cli_grp_add_arg(cg_show, (cli_arg_item){"TODO_ENTRY", "Show", STRING, -1, 1});

	cli_cmd_group *cg_config = cli_add_cmd_grp(list, "config", "Config", NULL, configurate_tocdo);
	cli_grp_add_arg(cg_config, (cli_arg_item){"TODO_ENTRY", "Config", STRING, -1, 1});

	return list;
}

int main(int argc, char **argv){
	cli_list *list = arg_parser(argc, argv);
	config_tocdo *conf = config_init();	

	int res = cli_execute(list, argc, argv);
	return !res;
}
