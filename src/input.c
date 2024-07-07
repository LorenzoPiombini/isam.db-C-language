#include <stdio.h>
#include <stdarg.h>
#include "input.h"
#include "str_op.h"


void print_usage(char *argv[]){
        printf("Usage: %s -n -f <database file>\n", argv[0]);
        printf("\t -n - create a new database file\n");
        printf("\t -f - [required] path to file (file name)\n");
        printf("\t -r - look for the record key provided in the specified file. \n");
        printf("\t -d - variables name and type <variableName>:TYPE_INT:12.\n");
        printf("\t -D - delete the record  provided for specified file.\n");
	printf("\t -k - specify the record id, the program will save, retrice and delete the record based on this id.\n");
	printf("\t -t - list of available types.");

        return;
}


int check_input_and_values(char* file_path,char* data_to_add, char*fileds_and_type, char* key, char* argv[], int del){	


        if(!file_path){
                print_usage(argv);
                return 0;
        }

        if(!is_file_name_valid(file_path)){
                printf("file name or path not valid");
                return 0;
        }

        if(data_to_add != NULL && fileds_and_type != NULL){
                printf("you can`t use both option -a and -d.");
                return 0;
        }

        if((data_to_add || fileds_and_type ) && !key){
                printf("option -k is required.\n\n");
                print_usage(argv);
                return 0;
        }
	
	return 1;
}