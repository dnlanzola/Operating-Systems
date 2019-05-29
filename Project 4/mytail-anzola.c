#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("** Error: arguments missing. **\n");
		return 1;
	}

	char *n = argv[1];
	int nLines = atoi(n);
	nLines = abs(nLines);

	int file = open(argv[2], O_RDONLY);

	int pos = lseek(file, -2, SEEK_END);

	char c;
	int i = 0;
	while (i != nLines)
	{
		read(file, &c, sizeof(c));

		if (c == '\n')
			i++;
		pos--;
		pos = lseek(file, pos, SEEK_SET);

		if (pos == -1)
		{
			printf("The file has less lines than the provided number. Printing the whole file: \n");
			break;
		}
	}

	if (pos == -1)
		pos += 1;
	else
		pos += 2;
	pos = lseek(file, pos, SEEK_SET);

	int j = read(file, &c, sizeof(c));
	while (j)
	{
		printf("%c", c);
		j = read(file, &c, sizeof(c));
	}

	close(file);

return 0;
}
