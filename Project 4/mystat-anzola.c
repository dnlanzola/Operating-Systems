#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
	// CHECK FOR INPUT FILE
	if (argc != 2)
	{
		printf("** No input file! **\n");
		return 1;
	}

	struct stat fileStat;
	if (stat(argv[1], &fileStat) < 0)
		return 1;

	printf("File Size: %d bytes.\n", fileStat.st_size);
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
	printf("\n");
	printf("File iNode: %d\n", fileStat.st_ino);

return 0;
}
