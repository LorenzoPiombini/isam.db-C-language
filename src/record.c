#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include "record.h"
#include "debug.h"
#include "str_op.h"

struct Record_f *create_record(char *file_name, int fields_num)
{
	struct Record_f *rec = malloc(sizeof(struct Record_f));

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

	rec->fields = calloc(fields_num, sizeof(struct Field));
	if (!rec->fields)
	{
		printf("Memory allocation fields, record.c l 26.\n");
		free(rec->file_name);
		free(rec);
		return NULL;
	}

	return rec;
}

unsigned char set_field(struct Record_f *rec, int index, char *field_name, enum ValueType type, char *value)
{
	rec->fields[index].field_name = strdup(field_name);
	if (!rec->fields[index].field_name)
	{
		fprintf(stderr, "strdup() failed %s:%d.\n", F, L - 3);
		return 0;
	}
	rec->fields[index].type = type;

	switch (type)
	{
	case TYPE_INT:
	case TYPE_ARRAY_INT:
	{
		if (type == TYPE_ARRAY_INT)
		{
			if (!rec->fields[index].data.v.elements.i)
			{
				rec->fields[index].data.v.insert = insert_element;
				rec->fields[index].data.v.destroy = free_dynamic_array;
			}

			char *t = strtok(value, ",");
			while (t)
			{

				if (!is_integer(t))
				{
					printf("invalid value for integer type: %s.\n", value);
					return 0;
				}

				int range = 0;
				if ((range = is_number_in_limits(t)) == 0)
				{
					printf("integer value not allowed in this system.\n");
					return 0;
				}

				if (range == IN_INT)
				{
					/*convert the value to long and then cast it to int */
					/*i do not want to use atoi*/
					long n = strtol(t, NULL, 10);
					if (n == ERANGE || n == EINVAL)
					{
						printf("conversion ERROR type int %s:%d.\n", F, L - 2);
						return 0;
					}

					int num = (int)n;
					rec->fields[index].data.v.insert((void *)&num,
													 &rec->fields[index].data.v,
													 type);
				}
				else
				{
					printf("the integer value is not valid for this system.\n");
					return 0;
				}

				t = strtok(NULL, ",");
			}
		}
		else
		{

			if (!is_integer(value))
			{
				printf("invalid value for integer type: %s.\n", value);
				return 0;
			}

			int range = 0;
			if ((range = is_number_in_limits(value)) == 0)
			{
				printf("integer value not allowed in this system.\n");
				return 0;
			}

			if (range == IN_INT)
			{
				/*convert the value to long and then cast it to int */
				/*i do not want to use atoi*/
				long n = strtol(value, NULL, 10);
				if (n == ERANGE || n == EINVAL)
				{
					printf("conversion ERROR type int %s:%d.\n", F, L - 2);
					return 0;
				}

				rec->fields[index].data.i = (int)n;
			}
			else
			{
				printf("the integer value is not valid for this system.\n");
				return 0;
			}
		}
		break;
	}
	case TYPE_LONG:
	case TYPE_ARRAY_LONG:
	{

		if (type == TYPE_ARRAY_LONG)
		{
			if (!rec->fields[index].data.v.elements.l)
			{
				rec->fields[index].data.v.insert = insert_element;
				rec->fields[index].data.v.destroy = free_dynamic_array;
			}

			char *t = strtok(value, "t");
			while (t)
			{
				if (!is_integer(t))
				{
					printf("invalid value for long integer type: %s.\n", value);
					return 0;
				}

				int range = 0;
				if ((range = is_number_in_limits(t)) == 0)
				{
					printf("long value not allowed in this system.\n");
					return 0;
				}

				if (range == IN_INT || range == IN_LONG)
				{
					long n = strtol(t, NULL, 10);
					if (n == ERANGE || n == EINVAL)
					{
						printf("conversion ERROR type long %s:%d.\n", F, L - 2);
						return 0;
					}
					rec->fields[index].data.v.insert((void *)&n,
													 &rec->fields[index].data.v,
													 type);
				}
				t = strtok(NULL, ",");
			}
		}
		else
		{

			if (!is_integer(value))
			{
				printf("invalid value for long integer type: %s.\n", value);
				return 0;
			}

			int range = 0;
			if ((range = is_number_in_limits(value)) == 0)
			{
				printf("long value not allowed in this system.\n");
				return 0;
			}

			if (range == IN_INT || range == IN_LONG)
			{
				long n = strtol(value, NULL, 10);
				if (n == ERANGE || n == EINVAL)
				{
					printf("conversion ERROR type long %s:%d.\n", F, L - 2);
					return 0;
				}
				rec->fields[index].data.l = n;
			}
			break;
		}
	}
	case TYPE_FLOAT:
	case TYPE_ARRAY_FLOAT:
	{
		if (type == TYPE_ARRAY_FLOAT)
		{
			if (!rec->fields[index].data.v.elements.f)
			{
				rec->fields[index].data.v.insert = insert_element;
				rec->fields[index].data.v.destroy = free_dynamic_array;
			}

			char *t = strtok(value, ",");
			while (t)
			{
				if (!is_floaintg_point(t))
				{
					printf("invalid value for float type: %s.\n", value);
					return 0;
				}

				int range = 0;
				if ((range = is_number_in_limits(t)) == 0)
				{
					printf("float value not allowed in this system.\n");
					return 0;
				}

				if (range == IN_FLOAT)
				{
					float f = strtof(t, NULL);
					if (f == ERANGE || f == EINVAL)
					{
						printf("conversion ERROR type float %s:%d.\n", F, L - 2);
						return 0;
					}

					rec->fields[index].data.v.insert((void *)&f,
													 &rec->fields[index].data.v,
													 type);
				}
				t = strtok(NULL, ",");
			}
		}
		else
		{

			if (!is_floaintg_point(value))
			{
				printf("invalid value for float type: %s.\n", value);
				return 0;
			}

			int range = 0;
			if ((range = is_number_in_limits(value)) == 0)
			{
				printf("float value not allowed in this system.\n");
				return 0;
			}

			if (range == IN_FLOAT)
			{
				float f = strtof(value, NULL);
				if (f == ERANGE || f == EINVAL)
				{
					printf("conversion ERROR type float %s:%d.\n", F, L - 2);
					return 0;
				}

				rec->fields[index].data.f = f;
			}
		}
		break;
	}
	case TYPE_STRING:
	case TYPE_ARRAY_STRING:
	{
		if (type == TYPE_ARRAY_STRING)
		{
			if (!rec->fields[index].data.v.elements.s)
			{
				rec->fields[index].data.v.insert = insert_element;
				rec->fields[index].data.v.destroy = free_dynamic_array;
			}

			char *t = strtok(value, ",");
			while (t)
			{
				rec->fields[index].data.v.insert((void *)&t,
												 &rec->fields[index].data.v,
												 type);
				t = strtok(NULL, "");
			}
		}
		else
		{
			rec->fields[index].data.s = strdup(value);
			if (!rec->fields[index].data.s)
			{
				fprintf(stderr,
						"strdup() failed %s:%d.\n",
						F, L - 4);
				return 0;
			}
		}
		break;
	}
	case TYPE_BYTE:
	case TYPE_ARRAY_BYTE:
	{
		if (type == TYPE_ARRAY_BYTE)
		{
			if (!rec->fields[index].data.v.elements.b)
			{
				rec->fields[index].data.v.insert = insert_element;
				rec->fields[index].data.v.destroy = free_dynamic_array;
			}

			char *t = strtok(value, ",");
			while (t)
			{
				if (!is_integer(t))
				{
					printf("invalid value for byte type: %s.\n", value);
					return 0;
				}

				unsigned long un = strtoul(t, NULL, 10);
				if (un == ERANGE || un == EINVAL)
				{
					printf("conversion ERROR type float %s:%d.\n", F, L - 2);
					return 0;
				}

				if (un > UCHAR_MAX)
				{
					printf("byte value not allowed in this system.\n");
					return 0;
				}

				unsigned char num = (unsigned char)un;
				rec->fields[index].data.v.insert((void *)&num,
												 &rec->fields[index].data.v,
												 type);
				t = strtok(NULL, ",");
			}
		}
		else
		{

			if (!is_integer(value))
			{
				printf("invalid value for byte type: %s.\n", value);
				return 0;
			}

			unsigned long un = strtoul(value, NULL, 10);
			if (un == ERANGE || un == EINVAL)
			{
				printf("conversion ERROR type float %s:%d.\n", F, L - 2);
				return 0;
			}
			if (un > UCHAR_MAX)
			{
				printf("byte value not allowed in this system.\n");
				return 0;
			}
			rec->fields[index].data.b = (unsigned char)un;
		}
		break;
	}
	case TYPE_DOUBLE:
	case TYPE_ARRAY_DOUBLE:
	{
		if (type == TYPE_ARRAY_DOUBLE)
		{
			if (!rec->fields[index].data.v.elements.d)
			{
				rec->fields[index].data.v.insert = insert_element;
				rec->fields[index].data.v.destroy = free_dynamic_array;
			}

			char *t = strtok(value, ",");
			while (t)
			{
				if (!is_floaintg_point(t))
				{
					printf("invalid value for double type: %s.\n", value);
					return 0;
				}

				int range = 0;
				if ((range = is_number_in_limits(t)) == 0)
				{
					printf("float value not allowed in this system.\n");
					return 0;
				}

				if (range == IN_DOUBLE)
				{
					double d = strtod(t, NULL);
					if (d == ERANGE || d == EINVAL)
					{
						printf("conversion ERROR type double %s:%d.\n", F, L - 2);
						return 0;
					}

					rec->fields[index].data.v.insert((void *)&d,
													 &rec->fields[index].data.v,
													 type);
				}

				t = strtok(NULL, ",");
			}
		}
		else
		{

			if (!is_floaintg_point(value))
			{
				printf("invalid value for double type: %s.\n", value);
				return 0;
			}

			int range = 0;
			if ((range = is_number_in_limits(value)) == 0)
			{
				printf("float value not allowed in this system.\n");
				return 0;
			}

			if (range == IN_DOUBLE)
			{
				double d = strtod(value, NULL);
				if (d == ERANGE || d == EINVAL)
				{
					printf("conversion ERROR type double %s:%d.\n", F, L - 2);
					return 0;
				}
				rec->fields[index].data.d = d;
			}
		}
		break;
	}
	default:
		printf("invalid type! type -> %d.", type);
		return 0;
	}

	return 1;
}

void free_record(struct Record_f *rec, int fields_num)
{
	int i;
	if (!rec)
		return;

	for (i = 0; i < fields_num; i++)
	{
		if (rec->fields[i].field_name)
			free(rec->fields[i].field_name);

		switch (rec->fields[i].type)
		{
		case TYPE_INT:
		case TYPE_LONG:
		case TYPE_FLOAT:
		case TYPE_BYTE:
		case TYPE_OFF_T:
		case TYPE_DOUBLE:
			break;
		case TYPE_STRING:
			if (rec->fields[i].data.s)
				free(rec->fields[i].data.s);
			break;
		case TYPE_ARRAY_INT:
		case TYPE_ARRAY_LONG:
		case TYPE_ARRAY_FLOAT:
		case TYPE_ARRAY_STRING:
		case TYPE_ARRAY_BYTE:
		case TYPE_ARRAY_DOUBLE:
			rec->fields[i].data.v.destroy(&rec->fields[i].data.v, rec->fields[i].type);
			break;
		default:
			fprintf(stderr, "type not supported.\n");
			return;
		}
	}

	free(rec->fields);
	free(rec->file_name);
	free(rec);
}

void print_record(int count, struct Record_f **recs)
{

	int i = 0, j = 0, max = 0;
	printf("#################################################################\n\n");
	printf("the Record data are: \n");

	for (j = 0; j < count; j++)
	{
		struct Record_f *rec = recs[j];
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
			case TYPE_ARRAY_INT:
			{
				int k;
				for (k = 0; k < rec->fields[i].data.v.size; k++)
				{
					if (rec->fields[i].data.v.size - k > 1)
					{
						if (!rec->fields[i].data.v.elements.i[k])
							continue;

						printf("%d, ", *rec->fields[i].data.v.elements.i[k]);
					}
				}

				break;
			}
			case TYPE_ARRAY_LONG:
			{
				int k;
				for (k = 0; k < rec->fields[i].data.v.size; k++)
					if (rec->fields[i].data.v.size - k > 1)
						printf("%ld, ", *rec->fields[i].data.v.elements.l[k]);
				printf("%ld.\n", *rec->fields[i].data.v.elements.l[k]);
				break;
			}
			case TYPE_ARRAY_FLOAT:
			{
				int k;
				for (k = 0; k < rec->fields[i].data.v.size; k++)
					if (rec->fields[i].data.v.size - k > 1)
						printf("%.2f, ", *rec->fields[i].data.v.elements.f[k]);
				printf("%.2f.\n", *rec->fields[i].data.v.elements.f[k]);
				break;
			}
			case TYPE_ARRAY_STRING:
			{
				int k;
				for (k = 0; k < rec->fields[i].data.v.size; k++)
				{
					if (rec->fields[i].data.v.size - k > 1)
					{
						strip('"', rec->fields[i].data.v.elements.s[k]);
						printf("%s, ", rec->fields[i].data.v.elements.s[k]);
					}
				}
				strip('"', rec->fields[i].data.v.elements.s[k]);
				printf("%s.\n", rec->fields[i].data.v.elements.s[k]);
				break;
			}
			case TYPE_ARRAY_BYTE:
			{
				int k;
				for (k = 0; k < rec->fields[i].data.v.size; k++)
					if (rec->fields[i].data.v.size - k > 1)
						printf("%u, ", *rec->fields[i].data.v.elements.b[k]);
				printf("%u.\n", *rec->fields[i].data.v.elements.b[k]);
				break;
			}
			case TYPE_ARRAY_DOUBLE:
			{
				int k;
				for (k = 0; k < rec->fields[i].data.v.size; k++)
					if (rec->fields[i].data.v.size - k > 1)
						printf("%.2f, ", *rec->fields[i].data.v.elements.d[k]);
				printf("%.2f.\n", *rec->fields[i].data.v.elements.d[k]);
				break;
			}
			default:
				break;
			}
		}
	}
	printf("\n#################################################################\n\n");
}

void free_record_array(int len, struct Record_f ***recs)
{
	int i = 0;
	for (i = 0; i < len; i++)
	{
		if (*recs)
		{
			if ((*recs)[i])
			{
				free_record((*recs)[i], (*recs)[i]->fields_num);
			}
		}
	}
	free(*recs);
}

/* this parameters are:
	- int len => the length of the array() Record_f***
	- Record_f*** array => the arrays of record arrays
	- int* len_ia => length inner array,  an array that keeps track of all the found records array sizes
	- int size_ia => the actual size of len_ia,
*/
void free_array_of_arrays(int len, struct Record_f ****array, int *len_ia, int size_ia)
{
	if (!*array)
		return;

	if (!size_ia)
	{
		free(*array);
		return;
	}

	int i = 0;
	for (i = 0; i < len; i++)
	{
		if ((*array)[i])
		{
			if (i < size_ia)
			{
				if (len_ia[i] != 0)
					free_record_array(len_ia[i], &(*array)[i]);
			}
			else
			{
				free((*array)[i]);
				return;
			}
		}
	}

	free(*array);
}

unsigned char copy_rec(struct Record_f *src, struct Record_f **dest)
{
	*dest = create_record(src->file_name, src->fields_num);
	if (!*dest)
	{
		printf("create_record failed %s:%d.\n", F, L - 3);
		return 0;
	}

	int i = 0;
	char data[30];
	for (i = 0; i < src->fields_num; i++)
	{
		switch (src->fields[i].type)
		{
		case TYPE_INT:
			if (snprintf(data, 30, "%d", src->fields[i].data.i) < 0)
			{
				printf("snprintf failed %s:%d.\n", F, L - 2);
				free_record((*dest), (*dest)->fields_num);
				return 0;
			}
			if (!set_field(*dest, i, src->fields[i].field_name,
						   src->fields[i].type, data))
			{
				printf("set_fields() failed %s:%d.\n", F, L - 2);
				free_record((*dest), (*dest)->fields_num);
				return 0;
			}
			break;
		case TYPE_STRING:
			if (!set_field(*dest, i, src->fields[i].field_name,
						   src->fields[i].type, src->fields[i].data.s))
			{
				printf("set_field() failed %s:%d.\n", F, L - 2);
				free_record((*dest), (*dest)->fields_num);
				return 0;
			}
			break;
		case TYPE_LONG:
			if (snprintf(data, 30, "%ld", src->fields[i].data.l) < 0)
			{
				printf("snprintf failed %s:%d.\n", F, L - 2);
				free_record((*dest), (*dest)->fields_num);
				return 0;
			}
			if (!set_field(*dest, i, src->fields[i].field_name,
						   src->fields[i].type, data))
			{
				printf("set_field() failed %s:%d.\n", F, L - 2);
				free_record((*dest), (*dest)->fields_num);
				return 0;
			}
			break;
		case TYPE_FLOAT:
			if (snprintf(data, 30, "%.2f", src->fields[i].data.f) < 0)
			{
				printf("snprintf failed %s:%d.\n", F, L - 2);
				free_record((*dest), (*dest)->fields_num);
				return 0;
			}
			if (!set_field(*dest, i, src->fields[i].field_name,
						   src->fields[i].type, data))
			{
				printf("set_field() failed %s:%d.\n", F, L - 2);
				free_record((*dest), (*dest)->fields_num);
				return 0;
			}
			break;
		case TYPE_BYTE:
			if (snprintf(data, 30, "%d", src->fields[i].data.b) < 0)
			{
				printf("snprintf failed %s:%d.\n", F, L - 2);
				free_record((*dest), (*dest)->fields_num);
				return 0;
			}
			if (!set_field(*dest, i, src->fields[i].field_name,
						   src->fields[i].type, data))
			{
				printf("set_field() failed %s:%d.\n", F, L - 2);
				free_record((*dest), (*dest)->fields_num);
				return 0;
			}
			break;
		case TYPE_DOUBLE:
			if (snprintf(data, 30, "%.2f", src->fields[i].data.d) < 0)
			{
				printf("snprintf failed %s:%d.\n", F, L - 2);
				free_record((*dest), (*dest)->fields_num);
				return 0;
			}
			if (!set_field(*dest, i, src->fields[i].field_name,
						   src->fields[i].type, data))
			{
				printf("set_field() failed %s:%d.\n", F, L - 2);
				free_record((*dest), (*dest)->fields_num);
				return 0;
			}
			break;
		default:
			printf("type not supported, %s:%d.\n", F, L - 27);
			return 0;
		}
	}

	return 1;
}

unsigned char get_index_rec_field(char *field_name, struct Record_f **recs,
								  int recs_len, int *field_i_r, int *rec_index)
{
	size_t field_name_len = strlen(field_name);
	for (int i = 0; i < recs_len; i++)
	{
		for (int j = 0; j < recs[i]->fields_num; j++)
		{
			if (strncmp(field_name, recs[i]->fields[j].field_name, field_name_len) == 0)
			{
				*field_i_r = j;
				*rec_index = i;
				return 1;
			}
		}
	}

	return 0;
}

int init_array(struct array **v, enum ValueType type)
{
	(*(*v)).size = DEF_SIZE;
	switch (type)
	{
	case TYPE_ARRAY_INT:
	{
		(*(*v)).elements.i = calloc(DEF_SIZE, sizeof(int *));
		if (!(*(*v)).elements.i)
		{
			__er_calloc(F, L - 2);
			return -1;
		}

		break;
	}
	case TYPE_ARRAY_LONG:
	{
		(*(*v)).elements.l = calloc(DEF_SIZE, sizeof(long *));
		if (!(*(*v)).elements.l)
		{
			__er_calloc(F, L - 2);
			return -1;
		}
		break;
	}
	case TYPE_ARRAY_FLOAT:
	{
		(*(*v)).elements.f = calloc(DEF_SIZE, sizeof(float *));
		if (!(*(*v)).elements.f)
		{
			__er_calloc(F, L - 2);
			return -1;
		}
		break;
	}
	case TYPE_ARRAY_STRING:
	{
		(*(*v)).elements.s = calloc(DEF_SIZE, sizeof(char *));
		if (!(*(*v)).elements.s)
		{
			__er_calloc(F, L - 2);
			return -1;
		}
		break;
	}
	case TYPE_ARRAY_BYTE:
	{
		(*(*v)).elements.b = calloc(DEF_SIZE, sizeof(unsigned char *));
		if (!(*(*v)).elements.b)
		{
			__er_calloc(F, L - 2);
			return -1;
		}
		break;
	}
	case TYPE_ARRAY_DOUBLE:
	{
		(*(*v)).elements.d = calloc(DEF_SIZE, sizeof(double *));
		if (!(*(*v)).elements.d)
		{
			__er_calloc(F, L - 2);
			return -1;
		}
		break;
	}
	default:
		fprintf(stderr, "type not supported.\n");
		return -1;
	}
	return 0;
}

int insert_element(void *element, struct array *v, enum ValueType type)
{
	/*check if the array has been initialized */
	if (!(*v).elements.i)
	{
		if (init_array(&v, type) == -1)
		{
			fprintf(stderr, "init array failed.\n");
			return -1;
		}
	}

	/*check if there is enough space for new item */

	switch (type)
	{
	case TYPE_ARRAY_INT:
	{
		if (!(*v).elements.i[(*v).size - 1])
		{
			for (int i = 0; i < (*v).size; i++)
			{
				if ((*v).elements.i[i])
					continue;

				(*v).elements.i[i] = malloc(sizeof(int));
				if (!(*v).elements.i[i])
				{
					__er_malloc(F, L - 2);
					return -1;
				}
				*((*v).elements.i[i]) = *(int *)element;
				return 0;
			}
		}

		/*not enough space, increase the size */
		int **elements_new = realloc((*v).elements.i, ++(*v).size * sizeof(int *));
		if (!elements_new)
		{
			__er_realloc(F, L - 2);
			return -1;
		}

		(*v).elements.i = elements_new;
		(*v).elements.i[(*v).size - 1] = malloc(sizeof(int));
		if (!(*v).elements.i[(*v).size - 1])
		{
			__er_malloc(F, L - 2);
			return -1;
		}

		*((*v).elements.i[(*v).size - 1]) = *(int *)element;
		return 0;
	}
	case TYPE_ARRAY_LONG:
	{
		if (!(*v).elements.l[(*v).size - 1])
		{
			for (int i = 0; i < (*v).size; i++)
			{
				if ((*v).elements.l[i])
					continue;

				(*v).elements.l[i] = malloc(sizeof(long));
				if (!(*v).elements.l[i])
				{
					__er_malloc(F, L - 2);
					return -1;
				}
				*((*v).elements.l[i]) = *(long *)element;
				return 0;
			}
		}
		/*not enough space, increase the size */
		long **elements_new = realloc((*v).elements.l, ++(*v).size * sizeof(long *));
		if (!elements_new)
		{
			__er_realloc(F, L - 2);
			return -1;
		}

		(*v).elements.l = elements_new;
		(*v).elements.l[(*v).size - 1] = malloc(sizeof(long));
		if (!(*v).elements.l[(*v).size - 1])
		{
			__er_malloc(F, L - 2);
			return -1;
		}

		*((*v).elements.i[(*v).size - 1]) = *(long *)element;
		return 0;
	}
	case TYPE_ARRAY_FLOAT:
	{
		if (!(*v).elements.f[(*v).size - 1])
		{
			for (int i = 0; i < (*v).size; i++)
			{
				if ((*v).elements.f[i])
					continue;

				(*v).elements.f[i] = malloc(sizeof(float));
				if (!(*v).elements.f[i])
				{
					__er_malloc(F, L - 2);
					return -1;
				}
				*((*v).elements.f[i]) = *(float *)element;
				break;
			}
		}
		/*not enough space, increase the size */
		float **elements_new = realloc((*v).elements.f, ++(*v).size * sizeof(float *));
		if (!elements_new)
		{
			__er_realloc(F, L - 2);
			return -1;
		}

		(*v).elements.f = elements_new;
		(*v).elements.f[(*v).size - 1] = malloc(sizeof(float));
		if (!(*v).elements.f[(*v).size - 1])
		{
			__er_malloc(F, L - 2);
			return -1;
		}

		*((*v).elements.i[(*v).size - 1]) = *(float *)element;
		return 0;
	}
	case TYPE_ARRAY_STRING:
	{
		if (!(*v).elements.s[(*v).size - 1])
		{
			for (int i = 0; i < (*v).size; i++)
			{
				if ((*v).elements.s[i])
					continue;

				size_t len = strlen((char *)element) + 1;
				(*v).elements.s[i] = malloc(len * sizeof(char));
				if (!(*v).elements.s[i])
				{
					__er_malloc(F, L - 2);
					return -1;
				}
				(*v).elements.s[i] = (char *)element;
				return 0;
			}
		}

		/*not enough space, increase the size */
		char **elements_new = realloc((*v).elements.s, ++(*v).size * sizeof(char *));
		if (!elements_new)
		{
			__er_realloc(F, L - 2);
			return -1;
		}

		(*v).elements.s = elements_new;

		size_t len = strlen((char *)element) + 1;
		(*v).elements.s[(*v).size - 1] = malloc(len * sizeof(char));
		if (!(*v).elements.s[(*v).size - 1])
		{
			__er_malloc(F, L - 2);
			return -1;
		}

		(*v).elements.s[(*v).size - 1] = (char *)element;
		return 0;
	}
	case TYPE_ARRAY_BYTE:
	{
		if (!(*v).elements.b[(*v).size - 1])
		{
			for (int i = 0; i < (*v).size; i++)
			{
				if ((*v).elements.b[i])
					continue;

				(*v).elements.b[i] = malloc(sizeof(unsigned char));
				if (!(*v).elements.b[i])
				{
					__er_malloc(F, L - 2);
					return -1;
				}
				*((*v).elements.b[i]) = *(unsigned char *)element;
				return 0;
			}
		}

		/*not enough space, increase the size */
		unsigned char **elements_new = realloc((*v).elements.b, ++(*v).size * sizeof(unsigned char *));
		if (!elements_new)
		{
			__er_realloc(F, L - 2);
			return -1;
		}

		(*v).elements.b = elements_new;
		(*v).elements.b[(*v).size - 1] = malloc(sizeof(unsigned char));
		if (!(*v).elements.i[(*v).size - 1])
		{
			__er_malloc(F, L - 2);
			return -1;
		}

		*((*v).elements.b[(*v).size - 1]) = *(unsigned char *)element;
		return 0;
	}
	case TYPE_ARRAY_DOUBLE:
	{
		if (!(*v).elements.d[(*v).size - 1])
		{
			for (int i = 0; i < (*v).size; i++)
			{
				if ((*v).elements.d[i])
					continue;

				(*v).elements.d[i] = malloc(sizeof(double));
				if (!(*v).elements.d[i])
				{
					__er_malloc(F, L - 2);
					return -1;
				}
				*((*v).elements.d[i]) = *(double *)element;
				return 0;
			}
		}

		/*not enough space, increase the size */
		double **elements_new = realloc((*v).elements.d, ++(*v).size * sizeof(double *));
		if (!elements_new)
		{
			__er_realloc(F, L - 2);
			return -1;
		}

		(*v).elements.d = elements_new;
		(*v).elements.d[(*v).size - 1] = malloc(sizeof(double));
		if (!(*v).elements.d[(*v).size - 1])
		{
			__er_malloc(F, L - 2);
			return -1;
		}

		*((*v).elements.d[(*v).size - 1]) = *(double *)element;
		return 0;
	}
	default:
		fprintf(stderr, "type not supperted");
		return -1;
	}

	return 0; /*success*/
}
void free_dynamic_array(struct array *v, enum ValueType type)
{
	switch (type)
	{
	case TYPE_ARRAY_INT:
	{
		for (int i = 0; i < v->size; i++)
			free(v->elements.i[i]);
		free(v->elements.i);
		break;
	}
	case TYPE_ARRAY_LONG:
	{
		for (int i = 0; i < v->size; i++)
			free(v->elements.l[i]);
		free(v->elements.l);
		break;
	}
	case TYPE_ARRAY_FLOAT:
	{
		for (int i = 0; i < v->size; i++)
			free(v->elements.f[i]);
		free(v->elements.f);
		break;
	}
	case TYPE_ARRAY_STRING:
	{
		for (int i = 0; i < v->size; i++)
			free(v->elements.s[i]);
		free(v->elements.s);
		break;
	}
	case TYPE_ARRAY_BYTE:
	{
		for (int i = 0; i < v->size; i++)
			free(v->elements.b[i]);
		free(v->elements.b);
		break;
	}
	case TYPE_ARRAY_DOUBLE:
	{
		for (int i = 0; i < v->size; i++)
			free(v->elements.d[i]);
		free(v->elements.d);
		break;
	}
	default:
		fprintf(stderr, "array type not suported.\n");
		return;
	}
}
