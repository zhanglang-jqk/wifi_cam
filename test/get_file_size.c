#include <stdio.h>
#include <sys/stat.h>

unsigned long get_file_size(const char *path)
{
    unsigned long filesize = -1;
    struct stat statbuff;
    if (stat(path, &statbuff) < 0)
    {
        return filesize;
    }
    else
    {
        filesize = statbuff.st_size;
    }
    return filesize;
}

int main()
{
    // FILE *f = fopen("README");
    // printf("file size is %d byte \r\n", f->_blksize);
    printf("file size is %d byte \r\n", get_file_size("README"));
    return 0;
}