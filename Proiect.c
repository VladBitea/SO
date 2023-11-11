#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <libgen.h>
 
void error(char *msg)
{
  perror(msg);
  exit(1);
}
 
void process_directory(char *dirname, int output_file)
{
  DIR *dir;
  if((dir=opendir(dirname))==NULL)
    {
      error("Eroare deschidere director");
    }
  struct stat dir_stat;
  if(stat(dirname, &dir_stat) < 0)
    {
      error("Eroare obtinere informatii director");
    }
 
  char user_perm[4], group_perm[4], others_perm[4];
  sprintf(user_perm, "%c%c%c", (dir_stat.st_mode & S_IRUSR) ? 'R' : '-', (dir_stat.st_mode & S_IWUSR) ? 'W' : '-', (dir_stat.st_mode & S_IXUSR) ? 'X' : '-');
  sprintf(group_perm, "%c%c%c", (dir_stat.st_mode & S_IRGRP) ? 'R' : '-', (dir_stat.st_mode & S_IWGRP) ? 'W' : '-', (dir_stat.st_mode & S_IXGRP) ? 'X' : '-');
  sprintf(others_perm, "%c%c%c", (dir_stat.st_mode & S_IROTH) ? 'R' : '-', (dir_stat.st_mode & S_IWOTH) ? 'W' : '-', (dir_stat.st_mode & S_IXOTH) ? 'X' : '-');
 
 
  dprintf(output_file, "nume director: %s\n", basename(dirname));
  dprintf(output_file, "identificatorul utilizatorului: %d\n", dir_stat.st_uid);
  dprintf(output_file, "drepturi de acces user: %s\n", user_perm);
  dprintf(output_file, "drepturi de acces grup: %s\n", group_perm);
  dprintf(output_file, "drepturi de acces altii: %s\n", others_perm);
  dprintf(output_file, "\n");
 
  closedir(dir);
 
 
}
 
void process_symlink(char *filename, int output_file)
{
 
   int fd_in = open(filename, O_RDONLY);
   if(fd_in < 0)
    {
      error("Eroare deschidere fisier intrare!");
    }
 
  struct stat link_stat;
  if(lstat(filename, &link_stat) < 0)
    {
      error("Eroare obtinere informatii legatura simbolica");
    }
 
  struct stat target_stat;
  if(fstat(fd_in, &target_stat) < 0)
    {
      error("Eroare obtinere informatii despre target-ul legaturii simbolice");
    }
 
  char user_perm[4], group_perm[4], others_perm[4];
  sprintf(user_perm, "%c%c%c", (link_stat.st_mode & S_IRUSR) ? 'R' : '-', (link_stat.st_mode & S_IWUSR) ? 'W' : '-', (link_stat.st_mode & S_IXUSR) ? 'X' : '-');
  sprintf(group_perm, "%c%c%c", (link_stat.st_mode & S_IRGRP) ? 'R' : '-', (link_stat.st_mode & S_IWGRP) ? 'W' : '-', (link_stat.st_mode & S_IXGRP) ? 'X' : '-');
  sprintf(others_perm, "%c%c%c", (link_stat.st_mode & S_IROTH) ? 'R' : '-', (link_stat.st_mode & S_IWOTH) ? 'W' : '-', (link_stat.st_mode & S_IXOTH) ? 'X' : '-');
 
 
  dprintf(output_file, "nume legatura: %s\n", basename(filename));
  dprintf(output_file, "dimensiune legatura: %ld\n", link_stat.st_size);
  dprintf(output_file, "dimensiune fisier target: %ld\n", target_stat.st_size);
  dprintf(output_file, "drepturi de acces user: %s\n", user_perm);
  dprintf(output_file, "drepturi de acces grup: %s\n", group_perm);
  dprintf(output_file, "drepturi de acces altii: %s\n", others_perm);
  dprintf(output_file, "\n");
 
  close(fd_in);
 
 
}
 
void process_file_BMP(char *filename, int output_file)
{
 
  int fd_in = open(filename, O_RDONLY);
   if(fd_in < 0)
    {
      error("Eroare deschidere fisier intrare!");
    }
 
  char bmp_header[54];
  if(read(fd_in, bmp_header, sizeof(bmp_header)) != sizeof(bmp_header))
    {
      error("Eroare citire header BMP");
    }
 
  int lungime = *(int *)&bmp_header[18];
  int inaltime = *(int *)&bmp_header[22];
  int dimensiune = *(int *)&bmp_header[2];
  int uid = *(int *)&bmp_header[6];
 
  struct stat file_stat;
 
  if(fstat(fd_in, &file_stat) < 0)
    {
      error("Eroare obtinere statistica!");
    }
 
  char time_str[20];
  struct tm *tm_info;
  tm_info = localtime(&file_stat.st_mtime);
  strftime(time_str, 20, "%d.%m.%Y", tm_info);
 
  char user_perm[4], group_perm[4], others_perm[4];
  sprintf(user_perm, "%c%c%c", (file_stat.st_mode & S_IRUSR) ? 'R' : '-', (file_stat.st_mode & S_IWUSR) ? 'W' : '-', (file_stat.st_mode & S_IXUSR) ? 'X' : '-');
  sprintf(group_perm, "%c%c%c", (file_stat.st_mode & S_IRGRP) ? 'R' : '-', (file_stat.st_mode & S_IWGRP) ? 'W' : '-', (file_stat.st_mode & S_IXGRP) ? 'X' : '-');
  sprintf(others_perm, "%c%c%c", (file_stat.st_mode & S_IROTH) ? 'R' : '-', (file_stat.st_mode & S_IWOTH) ? 'W' : '-', (file_stat.st_mode & S_IXOTH) ? 'X' : '-');
 
 
 
  dprintf(output_file, "nume fisier: %s\n", basename(filename));
  dprintf(output_file, "inaltime: %d\n", inaltime);
  dprintf(output_file, "lungime: %d\n", lungime);
  dprintf(output_file, "dimensiune: %d\n", dimensiune);
  dprintf(output_file, "identificatorul utilizatorului: %d\n", uid);
  dprintf(output_file, "timpul ultimei modificari: %s\n", time_str);
  dprintf(output_file, "contorul de legaturi: %ld\n", file_stat.st_nlink);
  dprintf(output_file, "drepturi de acces user: %s\n", user_perm);
  dprintf(output_file, "drepturi de acces grup: %s\n", group_perm);
  dprintf(output_file, "drepturi de acces altii: %s\n", others_perm);
  dprintf(output_file, "\n");
 
  close(fd_in);
 
}
 
void process_file(char *filename, int output_file)
{
 
  int fd_in = open(filename, O_RDONLY);
   if(fd_in < 0)
    {
      error("Eroare deschidere fisier intrare!");
    }
 
 
  struct stat file_stat;
 
  if(fstat(fd_in, &file_stat) < 0)
    {
      error("Eroare obtinere statistica!");
    }
 
  char time_str[20];
  struct tm *tm_info;
  tm_info = localtime(&file_stat.st_mtime);
  strftime(time_str, 20, "%d.%m.%Y", tm_info);
 
  char user_perm[4], group_perm[4], others_perm[4];
  sprintf(user_perm, "%c%c%c", (file_stat.st_mode & S_IRUSR) ? 'R' : '-', (file_stat.st_mode & S_IWUSR) ? 'W' : '-', (file_stat.st_mode & S_IXUSR) ? 'X' : '-');
  sprintf(group_perm, "%c%c%c", (file_stat.st_mode & S_IRGRP) ? 'R' : '-', (file_stat.st_mode & S_IWGRP) ? 'W' : '-', (file_stat.st_mode & S_IXGRP) ? 'X' : '-');
  sprintf(others_perm, "%c%c%c", (file_stat.st_mode & S_IROTH) ? 'R' : '-', (file_stat.st_mode & S_IWOTH) ? 'W' : '-', (file_stat.st_mode & S_IXOTH) ? 'X' : '-');
 
 
 
  dprintf(output_file, "nume fisier: %s\n", basename(filename));
  dprintf(output_file, "dimensiune: %ld\n", file_stat.st_size);
  dprintf(output_file, "identificatorul utilizatorului: %d\n", file_stat.st_uid);
  dprintf(output_file, "timpul ultimei modificari: %s\n", time_str);
  dprintf(output_file, "contorul de legaturi: %ld\n", file_stat.st_nlink);
  dprintf(output_file, "drepturi de acces user: %s\n", user_perm);
  dprintf(output_file, "drepturi de acces grup: %s\n", group_perm);
  dprintf(output_file, "drepturi de acces altii: %s\n", others_perm);
  dprintf(output_file, "\n");
 
  close(fd_in);
 
}
 
int main(int argc, char **argv)
{
 
  if(argc != 2)
    {
      printf("Numar invalid argumente!");
      exit(1);
    }
 
  DIR *dir = opendir(argv[1]);
  if(!dir)
    {
      error("Eroare deschidere director");
    }
 
  char output_file[] = "statistica2.txt";
  int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if(output_fd < 0)
    {
      error("Eroare fisier statistica");
    }
 
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
    {
 
      if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0)
	{
	  continue;
	}
 
      char full_path[PATH_MAX];
      snprintf(full_path, sizeof(full_path), "%s/%s", argv[1], entry->d_name);
 
      if(entry->d_type == DT_LNK)
	{
	  process_symlink(full_path, output_fd);
	}
      else if(entry->d_type == DT_REG)
	{
	  char *dot = strrchr(entry->d_name, '.');
	  if(dot && strcmp(dot, ".bmp") == 0)
	    {
 
	      process_file_BMP(full_path, output_fd);
	    }
	  else
	    {
	      process_file(full_path, output_fd);
	    }
	}
      else if(entry->d_type == DT_DIR)
	{
	  process_directory(full_path, output_fd);
	}
    }
 
  closedir(dir);
  close(output_fd);
 
  return 0;
}