#include <stdio.h>

int main(void)
{
	int i = 0;
	for (i = 1; i < 5; i++)
	{
		char *addr1 = (char *)(0x800000 + (i - 1)*0x100000);
		char *addr2 = (char *)(0x800000 + (i - 1)*0x200000);
		printf("addr 1: int = %d\t hex = %x\n", addr1, addr1);
		printf("addr 2: int = %d\t hex = %x\n", addr2, addr2);
	}
}
