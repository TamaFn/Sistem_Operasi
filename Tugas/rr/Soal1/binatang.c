#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <wait.h>
#include <time.h>
#include <dirent.h>

char *folderName[] = {"HewanDarat","HewanAmphibi","HewanAir"};


void MoveFile(char *src, char *flr1, char *flr2,char *flr3){
 
  int status;
  pid_t child_id;
  struct dirent *dp;
  DIR *ed;
  ed = opendir(src);
 
  if(ed != NULL){
    while ((dp = readdir(ed)) != NULL){
     	   if (strstr(dp->d_name, "darat") != NULL){
     	     if((child_id = fork()) == 0) {
       	       char *argv[] = {"mv", dp->d_name, flr1, NULL};
               execv("/bin/mv", argv);
             }
     	   }
     	   while(wait(&status) > 0);
    	 
     	   if (strstr(dp->d_name, "amphibi") != NULL){
     	     if((child_id = fork()) == 0) {
       	       char *argv[] = {"mv", dp->d_name, flr1, NULL};
               execv("/bin/mv", argv);
             }
     	   }
     	   while(wait(&status) > 0);
   	 
     	   if (strstr(dp->d_name, "air") != NULL){
     	     if((child_id = fork()) == 0) {
       	       char *argv[] = {"mv", dp->d_name, flr1, NULL};
               execv("/bin/mv", argv);
             }
     	   }
     	   while(wait(&status) > 0);
     }
    closedir(ed);
  }
}


void MakeDirectory(){
  pid_t child_id;
  int status;
  if((child_id = fork()) == 0) {
    execlp("mkdir","-p",folderName[0],folderName[1],folderName[2],NULL);
  }
  while(wait(&status) > 0);
}

void ZipFolder(){

  pid_t child_id;
  int status;
  
  if((child_id = fork()) == 0) {
    char *argv[] = {"zip","-r","HewanDarat.zip","HewanDarat",NULL};
    execv("/usr/bin/zip", argv);
  }
  
  while(wait(&status) > 0);

  if((child_id = fork()) == 0) {
    char *argv[] = {"zip","-r","HewanAmphibi.zip","HewanAmphibi",NULL};
    execv("/usr/bin/zip", argv);
  }
  
  while(wait(&status) > 0);
  
  if((child_id = fork()) == 0) {
    char *argv[] = {"zip","-r","HewanAir.zip","HewanAir",NULL};
    execv("/usr/bin/zip", argv);
  }
  
  while(wait(&status) > 0);
}



void DownloadFile() {
  char *url = {
    "https://drive.google.com/uc?export=download&id=1oDgj5kSiDO0tlyS7-20uz7t20X3atwrq"
  };

  pid_t child_id;
  int status;

  while(wait(&status) > 0);
  
  if((child_id = fork()) == 0) {
    execlp("wget", "--no-check-certificate", url, "-O","Binatang.zip", "-q", NULL);
  }

  while(wait(&status) > 0);
  
  if((child_id = fork()) == 0) {
    execlp("unzip","-q","Binatang.zip",NULL);
  }
  
  while(wait(&status) > 0);
  
  if((child_id = fork()) == 0) {
    execlp("rm","-rf","Binatang.zip",NULL);
  }
  while(wait(&status) > 0);
  
 }
 
void MakeRandomFile(char *src){
  int status;
  char *path[100];
  struct dirent *dp;
  DIR *ed;
  ed = opendir(src);
  int i = 0;
  
  if (!ed) return;
 
     while ((dp = readdir(ed)) != NULL){
     
       if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 && strstr(dp->d_name,  ".jpg") != NULL){
      	   char *str = dp->d_name;
      	   path[i] = str;
      	   i++;
       }
     }
    
      int size = sizeof(*path);
      int random = rand() % size;

      FILE *file;
      file = fopen("penjaga.txt", "w");
      fprintf(file, "Hewan Yang Dijaga : %s", strtok(path[random], "."));
      fclose(file);
      
      
      closedir(ed);
}





void RemoveFolder(){
  
  pid_t child_id;
  int status;
  
  if((child_id = fork()) == 0) {
    char *argv[] = {"rm","-rf","HewanDarat",NULL};
    execv("/usr/bin/rm", argv);
  }
  while(wait(&status) > 0);
  
  if((child_id = fork()) == 0) {
    char *argv[] = {"rm","-rf","HewanAmphibi",NULL};
    execv("/usr/bin/rm", argv);
  }
  while(wait(&status) > 0);
  
  if((child_id = fork()) == 0) {
    char *argv[] = {"rm","-rf","HewanAir",NULL};
    execv("/usr/bin/rm", argv);
  }
  while(wait(&status) > 0);

}
 


int main() {
  pid_t pid, sid;
  int status;
  pid = fork();

  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  else if (pid == 0) {
    DownloadFile();
    MakeDirectory();
    MakeRandomFile(".");
    MoveFile(".",folderName[0],folderName[1],folderName[2]);
    ZipFolder();
  }else{
    while(wait(&status) > 0);
    RemoveFolder();
  }

  
}
