#include "ft_ping.h"

unsigned short	ft_checksum(const void *data, size_t len)
{
	const unsigned short	*w = data;
	unsigned long			sum = 0;

	while (len > 1)
	{
		sum += *w++;
		len -= 2;
	}
	if (len == 1)
		sum += *(const unsigned char *)w;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return ((unsigned short)~sum);
}
