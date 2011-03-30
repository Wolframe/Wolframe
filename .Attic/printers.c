#include <stdio.h>
#include <cups/cups.h>


int main(void)
{
	int		i;
	cups_dest_t	*dests, *dest;
	int num_dests = cupsGetDests(&dests);

	for ( i = num_dests, dest = dests; i > 0; i --, dest ++ )	{
		if ( dest->instance )
			printf( "%s/%s\n", dest->name, dest->instance );
		else
			puts(dest->name);
	}
	return (0);
}
