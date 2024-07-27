#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "build.h"
#include "hash_tbl.h"
#include "helper.h"
#include "file.h"
#include "str_op.h"
#include "debug.h"

unsigned char build_from_txt_file(char *file_path, char *txt_f)
{
	FILE *fp = fopen(txt_f, "r");
	if (!fp)
	{
		printf("file open failed, build.c l %d.\n", __LINE__ - 2);
		return 0;
	}

	/*get record number and buffer for the longest line*/
	int line_buf = get_number_value_from_txt_file(fp);
	int recs = get_number_value_from_txt_file(fp);
	if (line_buf == 0 || recs == 0)
	{
		printf("no number in file txt, build.c:%d.\n", __LINE__ - 4);
		return 0;
	}

	/*load the txt file in memory */
	char buf[line_buf];
	char **lines = calloc(recs, sizeof(char *));
	if (!lines)
	{
		printf("calloc failed, build.c:%d", __LINE__ - 2);
		return 0;
	}

	printf("copying file system...\nthis might take a few minutes\n");
	int i = 0;

	while (fgets(buf, sizeof(buf), fp))
	{
		buf[strcspn(buf, "\n")] = '\0';
		lines[i] = strdup(buf);
		i++;
	}

	fclose(fp);
	//	printf("%s\n%s\ni is %d\n", lines[i-1], lines[1],i);
	//	getchar();

	/*creates two file for our system */
	int fd_index = -1, fd_data = -1;
	char **files = two_file_path(file_path);

	fd_index = create_file(files[0]);
	fd_data = create_file(files[1]);

	/* create target file */
	if (!create_empty_file(fd_data, fd_index, recs))
	{
		printf("create empty file failed, build.c:%d.\n", __LINE__ - 1);
		close_file(2, fd_index, fd_data);
		delete_file(2, files[0], files[1]);
		free(files[0]), free(files[1]), free(files);
		return 0;
	}

	HashTable ht = {0, NULL, write_ht};

	begin_in_file(fd_index);
	if (!read_index_file(fd_index, &ht))
	{
		printf("file pointer failed, build.c:%d.\n", __LINE__ - 1);
		close_file(2, fd_index, fd_data);
		delete_file(2, files[0], files[1]);
		free(files[0]), free(files[1]), free(files);
		return 0;
	}

	char *sv_b = NULL;
	int end = i;
	for (i = 0; i < end; i++)
	{
		char *str = strtok_r(lines[i], "|", &sv_b);
		char *k = strtok_r(NULL, "|", &sv_b);
		// printf("%s\n%s",str,k);
		// getchar();

		if (!k || !str)
		{
			printf("%s\t%s\ti is %d\n", str, k, i);
			continue;
		}

		if (append_to_file(fd_data, fd_index, file_path, k, str, files, &ht) == 0)
		{
			printf("%s\t%s\n i is %d\n", str, k, i);

			begin_in_file(fd_index);
			if (!ht.write(fd_index, &ht))
			{
				printf("could not write index file. build.c l %d.\n", __LINE__ - 1);
				destroy_hasht(&ht);
				free_strs(recs, 1, lines);
				free(files[0]), free(files[1]), free(files);
				close_file(2, fd_index, fd_data);
				return 0;
			}

			close_file(2, fd_index, fd_data);
			destroy_hasht(&ht);
			free(files[0]), free(files[1]), free(files);
			free_strs(recs, 1, lines);
			return 0;
		}
	}

	begin_in_file(fd_index);
	if (!ht.write(fd_index, &ht))
	{
		printf("could not write index file. build.c l %d.\n", __LINE__ - 1);
		destroy_hasht(&ht);
		free_strs(recs, 1, lines);
		free(files[0]), free(files[1]), free(files);
		close_file(2, fd_index, fd_data);
		return 0;
	}

	destroy_hasht(&ht);
	free_strs(recs, 1, lines);
	free(files[0]), free(files[1]), free(files);
	close_file(2, fd_index, fd_data);
	return 1;
}

int get_number_value_from_txt_file(FILE *fp)
{

	char buffer[250];
	unsigned short i = 0;
	unsigned char is_num = 1;
	if (fgets(buffer, sizeof(buffer), fp))
	{
		buffer[strcspn(buffer, "\n")] = '\0';

		for (i = 0; i < buffer[i] == '\0'; i++)
			if (!isdigit(buffer[i]))
			{
				is_num = 0;
				break;
			}
	}

	if (!is_num)
	{
		printf("first line is not a number, build.c:%d", __LINE__ - 1);
		return 0;
	}

	return atoi(buffer);
}
