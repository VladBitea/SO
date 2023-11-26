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
#include <sys/wait.h>
 
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
 
 
  char buffer[512];
  snprintf(buffer, sizeof(buffer), "Nume director: %s/n Identficatorul utilizatorului: %d\n Drepturi de acces user: %s\n Drepturi de acces grup: %s\n Drepturi de acces altii: %s\n \n", basename(dirname), dir_stat.st_uid, user_perm, group_perm, others_perm);
 
  if(write(output_file, buffer, strlen(buffer)) < 0)
    {
      error("Eroare scriere statistica director\n");
    }
 
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
 
 
  char buffer[512];
  snprintf(buffer, sizeof(buffer), "Nume legatura: %s/n Dimensiune legatura: %ld\n Dimensiune fisier target: %ld\n Drepturi de acces user: %s\n Drepturi de acces grup: %s\n Drepturi de acces altii: %s\n \n", basename(filename), link_stat.st_size, target_stat.st_size, user_perm, group_perm, others_perm);
 
  if(write(output_file, buffer, strlen(buffer)) < 0)
    {
      error("Eroare scriere statistica legatura\n");
    }
 
  close(fd_in);
 
 
}
 
void process_file_BMP(char *filename, int output_file)
{
 
  int fd_in = open(filename, O_RDWR);
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
 
 
 
  char buffer[512];
  snprintf(buffer, sizeof(buffer), "Nume fisier: %s/n Inaltime: %d\n Lungime: %d\n Dimensiune: %d\n Identficatorul utilizatorului: %d\n Timpul ultimei modificari: %s\n Contorul de legaturi: %ld\n Drepturi de acces user: %s\n Drepturi de acces grup: %s\n Drepturi de acces altii: %s\n \n", basename(filename), inaltime, lungime, dimensiune, uid, time_str, file_stat.st_nlink, user_perm, group_perm, others_perm);
 
  if(write(output_file, buffer, strlen(buffer)) < 0)
    {
      error("Eroare scriere statistica director\n");
    }
 
  pid_t childPID = fork();
 
  if(childPID == -1)
    {
      error("Eroare proces");
      close(fd_in);
    }
 
  if(childPID == 0)
    {
      char pixel[3];
      ssize_t byte;
 
      while((byte = read(fd_in, pixel, sizeof(pixel))) > 0)
	{
	  char grayscale = 0.299 * pixel[0] + 0.587 * pixel[1] + 0.114 * pixel[2];
	  memset(pixel, grayscale, sizeof(pixel));
	  lseek(fd_in, -byte, SEEK_CUR);
	  write(fd_in, pixel, sizeof(pixel));
	}
      close(fd_in);
      exit(0);
    }
  else
    {
      int status;
      waitpid(childPID, &status, 0);
 
      if (WIFEXITED(status))
	{
            printf("S-a încheiat procesul cu pid-ul %d și codul %d\n", childPID, WEXITSTATUS(status));
        }
      else
	{
            printf("Procesul cu pid-ul %d nu s-a încheiat normal\n", childPID);
        }
    }
 
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
 
  char buffer[512];
  snprintf(buffer, sizeof(buffer), "Nume fisier: %s/n Dimensiune: %ld\n Identficatorul utilizatorului: %d\n Timpul ultimei modificari: %s\n Contorul de legaturi: %ld\n Drepturi de acces user: %s\n Drepturi de acces grup: %s\n Drepturi de acces altii: %s\n \n", basename(filename), file_stat.st_size, file_stat.st_uid, time_str, file_stat.st_nlink, user_perm, group_perm, others_perm);
 
  if(write(output_file, buffer, strlen(buffer)) < 0)
    {
      error("Eroare scriere statistica director\n");
    }
 
 
  close(fd_in);
 
}
 
int main(int argc, char **argv)
{
 
  if(argc != 3)
    {
      printf("Numar invalid argumente!");
      exit(1);
    }
 
  DIR *dir_in = opendir(argv[1]);
  if(!dir_in)
    {
      error("Eroare deschidere director intrare");
    }
 
  char *dir_out = argv[2];
 
  char stat_file[PATH_MAX];
  snprintf(stat_file, PATH_MAX, "%s/statistica8.txt", argv[1]);
  int stat_fd = open(stat_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if(stat_fd < 0)
    {
      error("Eroare fisier statistica");
    }
 
  struct dirent *entry;
  while ((entry = readdir(dir_in)) != NULL)
    {
 
      if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0)
	{
	  continue;
	}
 
      char full_path[PATH_MAX];
      snprintf(full_path, sizeof(full_path), "%s/%s", argv[1], entry->d_name);
 
      pid_t childPID = fork();
      if(childPID == -1)
	{
	  error("Eroare proces fiu");
	}
 
      if(childPID == 0)
	{
	  char output_file[PATH_MAX];
	  snprintf(output_file, PATH_MAX, "%s/%s_statistica8.txt", argv[2], entry->d_name);
 
	  int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
 
 
	  if (output_fd < 0)
	    {
	      error("Eroare la deschiderea fisierului de iesire");
	    }
 
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
      close(output_fd);
	}
      else
	{
	  int status = 0;
	  waitpid(childPID, &status, 0);
 
	  if (WIFEXITED(status))
	    {
	      printf("S-a încheiat procesul cu pid-ul %d și codul %d\n", childPID, WEXITSTATUS(status));
	    }
	  else
	    {
	      printf("Procesul cu pid-ul %d nu s-a încheiat normal\n", childPID);
	    }
 
 
 
	}
    }
 
  closedir(dir_in);
  close(stat_fd);
 
  return 0;
}