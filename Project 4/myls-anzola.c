#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
	struct dirent *dirEntry;
	DIR *dr;
	char currentDir[1024];
	bool check = 0;

	struct stat fileStat;


	// NO DIRECTORY AND NO FLAG
	if (argc == 1)
	{
		getcwd(currentDir, sizeof(currentDir));
		dr = opendir(currentDir);
	}

	// NO DIRECTORY AND FLAG
	else if (argc == 2 && (strcmp(argv[1],"-l") == 0))
	{
		printf("%s\n", argv[1]);
		getcwd(currentDir, sizeof(currentDir));
		dr = opendir(currentDir);
		check = 1;
	}

	// DIRECTORY AND NO FLAG
	else if (argc == 2 && (strcmp(argv[1],"-l") != 0))
	{
		printf("%s\n", argv[1]);
		dr = opendir(argv[1]);
	}

	// DIRECTORY AND FLAG
	else if (argc == 3 && (strcmp(argv[1],"-l") == 0))
	{
		dr = opendir(argv[2]);
		check = 1;
	}
	// ERROR
	else
	{
		printf("Error: arguments missing.\n");
		return 1;
	}


	// IF DIRECTORY EXISTS
	if (dr == NULL)
	{
		printf("Cannot open directory '%s'\n", argv[1]);
		return 1;
	}

	while ((dirEntry = readdir(dr)) != NULL)
	{
		if (check)
		{
			stat(dirEntry->d_name, &fileStat);
			printf("%s:\n", dirEntry->d_name);
			printf("Size: %d bytes.\n", fileStat.st_size);
			printf("Blocks Allocated: %d blocks.\n", fileStat.st_blocks);
			printf("File Link Count: %d\n", fileStat.st_nlink);
			printf("File Permissions: ");
			printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
			printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
			printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
			printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
			printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
			printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
			printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
			printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
			printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
			printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
			printf("\n\n");
		}
		else
		printf("%s\n", dirEntry->d_name);
	}
	printf("\n");

	closedir(dr);

return 0;
}
