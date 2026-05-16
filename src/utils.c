#include <stdio.h>
#include "../include/utils.h"

int get_conn_count()
{
	FILE *f = fopen("/tmp/conn_count.txt", "r");
	int count = 0;

	if (f)
	{
	    fscanf(f, "%d", &count);
	    fclose(f);
	}

	return count;

}

void update_conn_count(int count)
{
        FILE *f = fopen("/tmp/conn_count.txt", "r");
	if (f)
        {
            fscanf(f, "%d", &count);
            fclose(f);
        }
}
