#ifndef PARSE_H
#define PARSE_H

#include "record.h"

#define HEADER_ID_SYS 0x657A3234
#define VS 1
#define MAX_FIELD_LT 32
#define MAX_HD_SIZE 7232
 
typedef struct {
	unsigned short fields_num;
	char** fields_name;
	ValueType* types;

}Schema;

typedef struct {
	unsigned int id_n;
	unsigned short version;
	Schema sch_d;
}Header_d;

Record_f* parse_d_flag_input(char* file_path,int fields_num, char* buffer, char* buf_t, char* buf_v, Schema *sch,                              int check_sch);
void clean_schema(Schema *sch);
int create_header(Header_d *hd);
int write_header(int fd, Header_d *hd);
int read_header(int fd, Header_d *hd);
int check_schema(int fields_n, char* buffer, char* buf_t, Header_d hd);
#endif
