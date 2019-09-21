#include <stdlib.h>
#include <stdio.h>

#define SERIAL_MMIO 0x1000
#define GPO_MMIO	0x1001
#define GPI_MMIO	0x1002

char string[] = "Hello world\n";

void write_gpio(int gpioNum, int value)
{
	char *pGpio = (char*) GPO_MMIO;
	if (value != 0)
	{
		*pGpio |= 1<<gpioNum;
	}
	else
	{
		*pGpio &= (0xFF ^ (1<<gpioNum));
	}
}

char read_gpio(int gpioNum)
{
	return ((*((char *) GPI_MMIO)) & 1<<gpioNum) > 0;
}

void send_serial(char c)
{
	char *pSerial = (char*) SERIAL_MMIO;
	*pSerial = c;
	while(*pSerial != 0) {}
}

void main(void)
{
	int i;
	for (i = 0; string[i] != 0; i++)
	{
		send_serial(string[i]);
	}
	write_gpio(0, 1);
	write_gpio(2, 1);
	i = 0;
    while(1)
    {
		if (i == 250000)
		{
			write_gpio(2, 0);
		}
		else if (i == 500000)
		{
			write_gpio(2, 1);
			i = 0;
		}
		i++;
		write_gpio(1, read_gpio(1));
	}
}


