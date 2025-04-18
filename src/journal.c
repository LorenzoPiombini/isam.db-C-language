#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <byteswap.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "hash_tbl.h"
#include "journal.h"
#include "file.h"

static char p[] ="db";

static void error(char *msg,int line);

int journal(int caller_fd, off_t offset, void *key, int key_type, int operation)
{
	int create = 0;
	int fd = open_file(JINX,0);
	if(fd == -1){
		create = 1;
		fd = create_file(JINX);
		if(fd == -1){
			fprintf(stderr,"(%s): can't create or open '%s'.\n",p,J_DEL);
			return -1;
		}
	}

	/*
	 * each journal record will store : 
	 * - timestamp
	 * - the file path
	 * - off_t
	 * - key   
	 * */
	
	struct stack index = {0};
	struct Node_stack  node = {0};
	if(create){
		index.capacity++;
	} else {

		if (read_journal_index(fd, &index) == -1) {
			fprintf(stderr,"(%s): read index from '%s' failed, %s:%d",
					p,JINX,__FILE__,__LINE__-1);
			close(fd);
			return -1;
		}
	}
	
	/*get the file name from the caller file descriptor */
	ssize_t buf_size = 0;
	char path[1024] = {0};
	char file_name[1024] = {0};
	
	if(snprintf(path,1024,PROC_PATH,fd) < 0){
		error("snpritnf() failed",__LINE__ - 1);
		close(fd);
		return -1;
	}

	if((buf_size = readlink(path,file_name,1024-1)) == -1){
		error("cannot get full path.",__LINE__-1);
		close(fd);
		return -1;
	}
	
	if(buf_size == 1024){
		error("file name is not completed",__LINE__-7);
		close(fd);
		return -1;
	}
	
	node.
	switch(key_type){
	case STR:
	{	
		size_t l = strlen((char *) key)+1;
		uint64_t l_ne = bswap_64(l);
		char buff[l];
		memset(buff,0,l);
		strncpy(buff,(char *)key,l);

		if(write(fd,&l_ne,sizeof(l_ne)) == -1 ||
			write(fd,buff,l) == -1){
			fprintf(stderr,
				"(%s): write to '%s' failed, %s:%d.\n",
				p,J_DEL,__FILE__,__LINE__-1);
			close_file(2,fd,fd_inx);
			return -1;
		}
		break;
	}
	case UINT:
		uint32_t k_ne = htonl(*(uint32_t *)key);
		if(write(fd,&k_ne,sizeof(k_ne)) == -1){
			fprintf(stderr,
				"(%s): write to '%s' failed, %s:%d.\n",
				p,J_DEL,__FILE__,__LINE__-1);
			close_file(2,fd,fd_inx);
			return -1;
		}
		break;
	default:
		close_file(2,fd,fd_inx);
		return -1;
	}


	/*write timestamp and offt of the new journal entry */
	

	/* load all indexes in memory */
	
	struct Node_stack node = {time(NULL),eof,NULL};
	if(push(&index,node) == -1){
		error("failed to push on journal stack",__LINE__-1);
		close_file(2,fd,fd_inx);
		return -1;
	}

	/* write the index file */
	if (write_journal_index(&fd_inx, &index) == -1) {
		printf("write to file failed, %s:%d",__FILE__,__LINE__ - 2);
		close_file(2,fd,fd_inx);
		return -1;
	}
	
	close_file(2,fd,fd_inx);
	return 0;
}

 
int push(struct stack *index, struct Node_stack node)
{
	if(index->capacity < MAX_STACK_CAP ){
		index->elements[index->capacity] = node; 
		index->capacity++;
		return 0;
	}
	
	if(index->dynamic_capacty == 0) {
		index->dynamic_elements = calloc(1,sizeof(struct Node_stack));
		if(!index->dynamic_elements){
			fprintf(stderr,"(%s): calloc failed.\n",p);
			return -1;
		}
		index->dynamic_elements->timestamp = node.timestamp;
		index->dynamic_elements->offset = node.offset;
		index->dynamic_capacty++;
		return 0;
	}
	
	struct Node_stack *nd = calloc(1,sizeof(struct Node_stack));
	if(!nd){
		fprintf(stderr,"(%s): calloc failed.\n",p);
		return -1;
	}
	
	
	nd->timestamp = node.timestamp;
	nd->offset = node.offset;
	index->dynamic_elements->next = nd;
	index->dynamic_capacty++;
	return 0;
}
int pop(struct stack *index){
	if(index->capacity <= MAX_STACK_CAP && index->dynamic_capacty == 0){
		memset(&index->elements[index->capacity - 1],0,sizeof(struct Node_stack));
		index->capacity--;
		return 0;
	}
	

	struct Node_stack *temp = index->dynamic_elements;
	while(temp) {

		if(!temp->next->next) {
			struct Node_stack *temp_s = temp; 
			temp_s->next = NULL;
		}

		temp = temp->next;
	}

	free(temp);
	index->dynamic_capacty--;
	return 0;
}

int peek(struct stack *index, struct Node_stack *node)
{
	if(index->capacity < MAX_STACK_CAP){
		*node = index->elements[index->capacity-1];
		return 0;
	}
	
	struct Node_stack *temp = index->dynamic_elements;
	while(temp) temp = temp->next;

	node->timestamp = temp->timestamp;
	node->offset = temp->offset;

	return 0;
}

int is_empty(struct stack *index)
{
	return index->capacity == 0;

}

void free_stack(struct stack *index)
{
	if(index->dynamic_capacty == 0 ) return;
	
	while(index->dynamic_capacty > 0) pop(index);
}

int write_journal_index(int *fd,struct stack *index)
{
	if(index->capacity == 0) return -1;
	/*  
	 * check if capacity is bigger then MAX_STACK_CAP
	 * if true, you have to 
	 * write 400 entry to an HISTORY-index
	 * and write the most recent 100 to a new truncated (so you erase the content)
	 * journal index file,
	 * so you can keep the system efficient, and avoid to use memory allocations
	 * extensevely
	 * */

	if(index->capacity == MAX_STACK_CAP){
		int fd_hst = open_file(JHST,0);
		if(fd_hst == -1){
			fd_hst = create_file(JHST);
			if(fd_hst == -1){
				error("can't create history file.",__LINE__-2);
				return -1;
			}
		}

		int hst_cap = 0;
		int nw_cap = 0;
		if(index->dynamic_capacty == 0){

			hst_cap = index->capacity -100;
		}	

		if(index->dynamic_capacty > 0){
			nw_cap = 100 + index->dynamic_capacty;
			hst_cap = index->capacity-100;
		}

		uint32_t cap_ne = htonl(hst_cap);
		if(write(fd_hst,&cap_ne,sizeof(cap_ne)) == -1) {
			error("can't write to journal index file.",__LINE__-1);
			close(fd_hst);
			return -1;
		}

		for(int i = 0; i < hst_cap;i++){
			uint64_t ts_ne = bswap_64((index->elements[i].timestamp));
			if(write(fd_hst,&ts_ne,sizeof(ts_ne)) == -1){
				error("can't write to journal index file.",__LINE__-1);
				close(fd_hst);
				return -1;
			}
				
			uint64_t ot_ne = bswap_64((index->elements[i].offset));
			if(write(fd_hst,&ot_ne,sizeof(ot_ne)) == -1){
				error("can't write to journal index file.",__LINE__-1);
				close(fd_hst);
				return -1;
			}
		}

		close_file(2,*fd,fd_hst);
		if(nw_cap == 0)
			nw_cap = 100;

		/*open with O_TRUNC*/
		*fd = open_file(JINX,1); 
		if(file_error_handler(1,*fd) > 0) return -1;

		uint32_t nw_cap_ne = htonl(nw_cap);
		if(write(*fd,&nw_cap_ne,sizeof(nw_cap_ne)) == -1){
			error("can't write to journal index file.",__LINE__-1);
			return -1;
		}
		
		if(nw_cap == 100)
			nw_cap = MAX_STACK_CAP;

		for(int i = hst_cap ; i < nw_cap;i++){
			
			if(i >= MAX_STACK_CAP){
				struct Node_stack *temp = index->dynamic_elements;
				while(temp){
					uint64_t ts_ne = bswap_64((temp->timestamp));
					if(write(*fd,&ts_ne,sizeof(ts_ne)) == -1){
						error("can't write to journal index file.",__LINE__-1);
						return -1;
					}

					uint64_t ot_ne = bswap_64((temp->offset));
					if(write(*fd,&ot_ne,sizeof(ot_ne)) == -1){
						error("can't write to journal index file.",__LINE__-1);
						return -1;
					}

					temp = temp->next;
				}
				close(*fd);
				*fd = open_file(JINX,0);
				if(file_error_handler(1,*fd) != 0) return -1;

				return 0;
			}

			uint64_t ts_ne = bswap_64((index->elements[i].timestamp));
			if(write(*fd,&ts_ne,sizeof(ts_ne)) == -1){
				error("can't write to journal index file.",__LINE__-1);
				return -1;
			}
				
			uint64_t ot_ne = bswap_64((index->elements[i].offset));
			if(write(*fd,&ot_ne,sizeof(ot_ne)) == -1){
				error("can't write to journal index file.",__LINE__-1);
				return -1;
			}
		}

		close(*fd);
		*fd = open_file(JINX,0);
		if(file_error_handler(1,*fd) != 0) return -1;

		return 0;
	}


	uint32_t cap_ne = htonl(index->capacity);
	if(write(*fd,&cap_ne,sizeof(cap_ne)) == -1){
		fprintf(stderr,"(%s): can't write journal index file.",p);
		return -1;
	}

	
	for(int i = 0; i < index->capacity; i++){
		uint64_t ts_ne = bswap_64((index->elements[i].timestamp));
		if(write(*fd,&ts_ne,sizeof(ts_ne)) == -1){
			error("can't write to journal index file.",__LINE__-1);
			return -1;
		}

		uint64_t ot_ne = bswap_64((index->elements[i].offset));
		if(write(*fd,&ot_ne,sizeof(ot_ne)) == -1){
			error("can't write to journal index file.",__LINE__-1);
			return -1;
		}
	}
	close(*fd);
	*fd = open_file(JINX,0);
	if(file_error_handler(1,*fd) != 0) return -1;

	return 0;
}

int read_journal_index(int fd,struct stack *index)
{
	uint32_t cap_ne = 0;
	if(read(fd,&cap_ne,sizeof(cap_ne)) == -1){
		fprintf(stderr,"(%s): can't write journal index file.",p);
		return -1;
	}


	int cap = (int)ntohl(cap_ne);
	if(cap == 0 || cap >= MAX_STACK_CAP) {
		return cap == 0 ? -1 : EJCAP;
	}

	for(int i = 0; i < cap; i++){
		
		uint64_t ts_ne = 0;
		if(read(fd,&ts_ne,sizeof(ts_ne)) == -1){
			error("read journal index failed",__LINE__-1);
			return -1;
		}
		
		index->elements[i].timestamp = (time_t) bswap_64(ts_ne);
		
		uint64_t os_ne = 0;
		if(read(fd,&os_ne,sizeof(os_ne)) == -1){
			error("read journal index failed",__LINE__-1);
			return -1;
		}
		
		index->elements[i].offset = (off_t) bswap_64(os_ne);
		index->capacity++;
	}

	return 0;
}
static void error(char *msg,int line)
{
	fprintf(stderr,"(%s): %s, %s:%d.\n",p,msg,__FILE__,line);
}
