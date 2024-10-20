#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "record.h"
#include "str_op.h"

Record_f *create_record(char *file_name, int fields_num)
{
	Record_f *rec = malloc(sizeof(Record_f));

	if (!rec)
	{
		printf("memmory allocation, record.c l 11.\n");
		return NULL;
	}
	rec->file_name = strdup(file_name);
	if (!rec->file_name)
	{
		printf("memory allocation file_name, record.c l 17.\n");
		free(rec);
		return NULL;
	}

	rec->fields_num = fields_num;

	rec->fields = calloc(fields_num, sizeof(Field));

	if (!rec->fields)
	{
		printf("Memory allocation fields, record.c l 26.\n");
		free(rec->file_name);
		free(rec);
		return NULL;
	}

	return rec;
}

void set_field(Record_f *rec, int index, char *field_name, ValueType type, char *value)
{
	//	printf("inside set_filed\n");
	rec->fields[index].field_name = strdup(field_name);
	rec->fields[index].type = type;

	char *endStr = NULL;
	switch (type)
	{

	case TYPE_INT:
		rec->fields[index].data.i = atoi(value);
		break;
	case TYPE_LONG:
		rec->fields[index].data.l = strtol(value, &endStr, 10);
		if (*endStr != '\0')
			printf("conversion error for TYPE_LONG");
		break;
	case TYPE_FLOAT:
		rec->fields[index].data.f = atof(value);
		break;
	case TYPE_STRING:
		rec->fields[index].data.s = strdup(value);
		break;
	case TYPE_BYTE:
		rec->fields[index].data.b = strtoul(value, &endStr, 10);
		if (*endStr != '\0')
			printf("conversion error for TYPE_STRING");
		break;
	case TYPE_DOUBLE:
		rec->fields[index].data.d = atof(value);
		break;
	default:
		printf("invalid type! type -> %d.", type);
		return;
	}
}

void clean_up(Record_f *rec, int fields_num)
{
	int i;
	if (!rec)
		return;

	for (i = 0; i < fields_num; i++)
	{
		if (rec->fields[i].field_name)
			free(rec->fields[i].field_name);

		if (rec->fields[i].type == TYPE_STRING)
		{
			if (rec->fields[i].data.s)
				free(rec->fields[i].data.s);
		}
	}

	free(rec->fields);
	free(rec->file_name);
	free(rec);
}

void print_record(int count, Record_f **recs)
{

	int i = 0, j = 0, max = 0;
	printf("#################################################################\n\n");
	printf("the Record data are: \n");

	for (j = 0; j < count; j++)
	{
		Record_f *rec = recs[j];
		for (i = 0; i < rec->fields_num; i++)
		{
			if (max < (int)strlen(rec->fields[i].field_name))
			{
				max = (int)strlen(rec->fields[i].field_name);
			}
		}

		for (i = 0; i < rec->fields_num; i++)
		{
			strip('"', rec->fields[i].field_name);
			printf("%-*s\t", max++, rec->fields[i].field_name);
			switch (rec->fields[i].type)
			{
			case TYPE_INT:
				printf("%d\n", rec->fields[i].data.i);
				break;
			case TYPE_LONG:
				printf("%ld\n", rec->fields[i].data.l);
				break;
			case TYPE_FLOAT:
				printf("%.2f\n", rec->fields[i].data.f);
				break;
			case TYPE_STRING:
				strip('"', rec->fields[i].data.s);
				printf("%s\n", rec->fields[i].data.s);
				break;
			case TYPE_BYTE:
				printf("%u\n", rec->fields[i].data.b);
				break;
			case TYPE_DOUBLE:
				printf("%.2f\n", rec->fields[i].data.d);
				break;
			default:
				break;
			}
		}
	}
	printf("\n#################################################################\n\n");
}
