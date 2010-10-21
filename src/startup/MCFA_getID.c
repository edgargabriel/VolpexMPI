#include "MCFA.h"
#include "MCFA_internal.h"
#include "SL.h"


int MCFA_get_nextID()
{
    static int id=0;
    return id++;
}

int MCFA_get_nextjobID()
{
    static int jobid = 1;
    return jobid++;
}

/*function to get absolute path of executable file */
void MCFA_get_abs_path(char *arg, char **path1)
{
	

    char temparg[BUFFERSIZE];
    char currdir[BUFFERSIZE];
    char path[MAXHOSTNAMELEN];
    
    getcwd(currdir,BUFFERSIZE);

    if( (path[0]!='/') && (path[0]!='.' ))
    {
        strcpy(temparg,"/");
        strcat(temparg,path);
        strcpy(path,temparg);

    }
    if(arg[0]=='/')
    {
        strcat(arg,"/");
        strcat(arg,path);
        strcpy(path,arg);
    }
    else
    {
        strcat(currdir,"/");
        strcat(currdir,arg);
        strcat(currdir,"/");
        strcat (currdir,path);
        strcpy(path,currdir);

    }

    *path1 = strdup(path);
    MCFA_printf("%s\n",path);

}

/*function to get absolute path of executable file */
void MCFA_get_path(char *arg, char **path1)
{

    char path[BUFFERSIZE];
    getcwd(path, BUFFERSIZE);

    if( (arg[0]!='/') && (arg[0]!='.' ))
    {
        strcat(path,"/");
        strcat (path, arg);
    }
    else
    {
        strcat(path,"/");
        strcat (path, arg);
    }
    *path1 = strdup(path);
//      MCFA_printf("%s\n\n",path);
}

/*function to allocate contents of hostfile to an array and counting the number of elements*/
char** MCFA_allocate_func(char fileName[MAXHOSTNAMELEN], int *num)
{
    int i=0;
    char **name =NULL;
    FILE *inputFile = NULL;
    inputFile=fopen(fileName,"r");

    if (inputFile == NULL){
        MCFA_printf("Cannot open file : %s \n",fileName);
        return 0;
    }
    name = (char**)malloc(100 * sizeof(char *));//******************TO BE CORRECTED********************************************
    if(name == NULL){
        printf("ERROR: in allocating memory\n");
        exit(-1);
    }

    while(!feof(inputFile))
    {
        name[i] = (char *) malloc (MAXHOSTNAMELEN * sizeof(char ));
        if(name[i] == NULL){
            printf("ERROR: in allocating memory\n");
            exit(-1);
        }

        fscanf(inputFile,"%s",name[i]);  /* constructing an array by scanning the inputfile */
        i++;
    }
    *num=i;
    fclose(inputFile);
    return name;
}


int MCFA_get_exec_name(char *path, char *filename)
{


        char* filenameStart = strrchr(path ,'/');

       if(!filenameStart )
             filenameStart[0] = path[1];
       else
             filenameStart++;
//	filename = strdup(filenameStart);

                strcpy(filename,filenameStart);

        return 0;
}