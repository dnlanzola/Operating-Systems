#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

void mysearch(char *dirName, int level);

int main(int argc, char **argv)
{
	struct dirent *dirEntry;
	DIR *dr;
	DIR *auxDr;
	char currentDir[1024];
	int lvl = 0;

	// DIRECTORY GIVEN
	if (argc == 2)
		dr = opendir(argv[1]);
	// NO DIRECTORY GIVEN
	else 
    {
        getcwd(currentDir, sizeof(currentDir));
		dr = opendir(currentDir);
    }

	
	while ((dirEntry = readdir(dr)) != NULL)
	{
		printf("%s\n", dirEntry->d_name);
		
		if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
		{
			auxDr = opendir(dirEntry->d_name);
			if (auxDr != NULL)
				mysearch(dirEntry->d_name, lvl);
			closedir(auxDr);
		}
		printf("\n");
	}

	closedir(dr);

return 0;
}

void mysearch(char *dirName, int level)
{
	struct dirent *dirEntryS;
	DIR *dirPtr = opendir(dirName);
	DIR *dirAux;

    level++;
	
	while ((dirEntryS = readdir(dirPtr)) != NULL)
	{
		int i;
		for (i = 0; i < level; i++)
			printf("\t");

		printf("%s\n", dirEntryS->d_name);
		
		if (strcmp(dirEntryS->d_name, ".") != 0 && strcmp(dirEntryS->d_name, "..") != 0)
		{
			dirAux = opendir(dirEntryS->d_name);
			if (dirAux != NULL)
				mysearch(dirEntryS->d_name, level);
			closedir(dirAux);
		}
		printf("\n");
	}
	
	closedir(dirPtr);
}
