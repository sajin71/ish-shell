#include <stdio.h>
#include <string.h>
#include "parse.h"

extern void do_job(job*);

int main(int argc, char *argv[])
{
    char s[LINELEN];
    job *curr_job;

    while(get_line(s, LINELEN)) {
	if(!strcmp(s, "exit\n")) 
	    break;

	curr_job = parse_line(s);

        do_job(curr_job);

	free_job(curr_job);
    }

    return 0;
}
