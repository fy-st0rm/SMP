#include <stdio.h>
#include <time.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


int main()
{
	char key = 'r';
	printf("%d is %c\n", key, key);
    
	char key2 = '-';
	printf("%d is %c\n", key2, key2);
	
	time_t seconds;
     
    seconds = time(NULL);
    printf("Seconds since January 1, 1970 = %ld\n", seconds);

	printf("%1d\n", MAX(1/.25 - (time(NULL) - seconds), 0));

	return 0;	
}
 

