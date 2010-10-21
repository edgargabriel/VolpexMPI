
#include "MCFA.h"
#include "MCFA_internal.h"
#include "SL.h"

extern int SL_this_procport;
extern struct      MCFA_host_node *hostList;
extern struct      MCFA_proc_node *procList;


struct MCFA_proc_node* MCFA_add_procs(struct SL_event_msg_header *header)
{
    /*steps
    ** Adding new processes in host list
    ** Adding processes in process list
    ** spawning processes on those hosts
    **/
    
    struct      MCFA_host_node *curr = NULL, *next = NULL;
    int         hostCount = 0;
    int         port = 0,i = 0;
    struct      MCFA_proc_node *newproclist = NULL;
    char        **hostName = NULL;
    int         numprocs = 0, jobID = 1;
    struct      MCFA_process *proc = NULL;
    char        path[MAXNAMELEN];
    struct      MCFA_host *newnode=NULL;
    int         maxprocspernode = MAXPROCSPERNODE;
    int         redundancy = 1;

    numprocs = header->numprocs;
    strcpy(path,header->executable);


/*to add new process on the host with least number of processes and then proceed in round robin manner*/
    if(strcmp(header->hostfile,"")==0){
        curr = hostList;
        while(curr != NULL){
            if(curr->next != NULL){
                next = curr->next;
                if (next->hostdata.numofProcs <  curr->hostdata.numofProcs){
                    curr = next;
                    break;
                }
		
                curr = curr->next;
            }
            else
                break;
        }

        hostCount = MCFA_get_total_hosts(hostList);
        hostName = (char**)malloc(hostCount*sizeof(char*));
        if(NULL == hostName){
	    printf("ERROR: in allocating memory\n");
	    exit(-1);
        }
	
        for(i=0;i<hostCount;i++)
        {
            hostName[i] = (char *) malloc (MAXHOSTNAMELEN *sizeof(char));
            if (NULL == hostName[i]){
                printf("ERROR: in allocating memory\n");
                exit(-1);
	    }
	    
            hostName[i]= curr->hostdata.hostname;
	    if(NULL != curr->next)
                curr = curr->next;
            else
                curr = hostList;

	}
    }
    else
    {
        hostName = MCFA_allocate_func(header->hostfile,&hostCount);
        hostCount = hostCount-1;
        int num = (int)ceil((double)numprocs/(double)hostCount);
        if(maxprocspernode<num){
            maxprocspernode = num;
        }

        PRINTF(("MCFA_add_procs: adding new hosts to hostlist\n"));
        for(i=0;i<hostCount;i++)
        {
            PRINTF(("MCFA_add_procs: adding new host : %s\n ",hostName[i]));
            if((MCFA_search_hostname(hostList,hostName[i])) == NULL){       //for each host in hostfile creating an entry in hostList
                newnode = MCFA_init_hostnode(hostName[i],maxprocspernode,port);
                MCFA_add_host(&hostList, newnode);
            }
	    
        }
	
    }
    port = SL_this_procport;

    
    
    if (header->jobid != MCFA_EXISTING_JOBID){
        jobID = MCFA_get_nextjobID();
    }
    else{
        proc = MCFA_getlast_proc(procList);
        jobID = proc->jobid;
    }
    
    PRINTF(("MCFA_add_procs: Spawning newly added processes\n\n"));
    newproclist = MCFA_spawn_processes(hostName,path,port,jobID,numprocs,hostCount,redundancy, 1);
    
    PRINTF(("MCFA_add_procs: EXITING !!!!!!!!\n\n"));
    /*  for(i=0;i<hostCount;i++)
        free(hostName[i]);
	free(hostName);*/
    return newproclist;
}



struct MCFA_proc_node* MCFA_delete_proc(struct SL_event_msg_header *header)
{
    /*steps
    ** Calling SL function to delete a process
    ** Deleting processes in process list
    ** Deleting processes in host list
    ** Sending this list to all existing processes
    **/
    
    
    struct MCFA_proc_node *currproc = NULL, *list = NULL;
    struct MCFA_host_node *currhost = NULL;
    char *prochostname = NULL;
    int procid = 0;
//    int  i = 0,j = 0;
    struct MCFA_process proc ;

    currproc = procList;
    currhost = hostList;

    procid = header->procid;

    while(currproc->procdata.id != procid)
    {
        currproc = currproc->next;
    }

/*Deleting from procList*/
/*    proc = (struct MCFA_process*) malloc(sizeof(struct MCFA_process));
    if (NULL == proc){
                MCFA_printf("ERROR: in allocating memory");
                exit(-1);
    }*/
    
    
        prochostname = strdup(currproc->procdata.hostname);
        proc = currproc->procdata;
   //     MCFA_add_proc(&list,proc->id, proc->hostname, proc->portnumber, proc->jobid, proc->sock,
//		      proc->status,proc->executable, proc->fullrank);

	MCFA_add_proc(&list,proc.id, proc.hostname, proc.portnumber, proc.jobid, proc.sock,
                    proc.status,proc.executable, proc.fullrank);

        
    
/*Deleting from hostList */
/*    if(prochostname !=NULL){
        while(strcmp(currhost->hostdata.hostname, prochostname)!=0)
        {
            currhost = currhost->next;
        }
	
        for(i=0;i<currhost->hostdata.numofProcs;i++)
        {
            if(currhost->hostdata.id[i].procID == procid){      // && currhost->hostdata.id[i].jobID == jobid)
                for(j=i;j<currhost->hostdata.numofProcs;j++)
                {
                    currhost->hostdata.id[j].procID = currhost->hostdata.id[j+1].procID;
                    currhost->hostdata.id[j].jobID = currhost->hostdata.id[j+1].jobID;
                }
                currhost->hostdata.numofProcs--;
//              strcpy(currhost->hostdata.executable,  "");
//              currhost->hostdata.status = 0;
            }
        }
    }*/

    free(prochostname);
    return list;
}



struct MCFA_proc_node* MCFA_delete_job(struct SL_event_msg_header *header, int *num)
{
    /*steps
    ** Deleting processes in host list
    ** Deleting processes in process list
    ** Sending this list to all existing processes
    **/


    struct MCFA_proc_node *currproc = NULL;
    struct MCFA_host_node *currhost = NULL;
    int procid  = 0, jobid = 0 ,i = 0,j = 0;
    struct MCFA_process* proc = NULL;
    struct MCFA_proc_node *list = NULL;
    int count = 0;

    currproc = procList;
    currhost = hostList;

    procid = header->procid;
    jobid = header->jobid;

/*Deleting from procList*/
    proc = (struct MCFA_process*) malloc(sizeof(struct MCFA_process));
    if (NULL == proc){
                printf("ERROR: in allocating memory\n");
                exit(-1);
    }


    while(currproc!=NULL)
    {
        if(currproc->procdata.jobid == jobid){
            *proc = currproc->procdata;
            MCFA_add_proc(&list,proc->id, proc->hostname, proc->portnumber, proc->jobid,
                                proc->sock, proc->status,proc->executable, proc->fullrank);
            MCFA_proc_close(procList,proc->id);
            count++;
        }
        currproc = currproc->next;
    }




/*Deleting from hostList */
    while(currhost!= NULL)
    {
        i = 0;
        while(i<=currhost->hostdata.numofProcs)
        {
            if(currhost->hostdata.id[i].jobID == jobid){
                for(j=i;j<currhost->hostdata.numofProcs;j++)
                {
                    currhost->hostdata.id[j].procID = currhost->hostdata.id[j+1].procID;
                    currhost->hostdata.id[j].jobID = currhost->hostdata.id[j+1].jobID;
                }
                currhost->hostdata.numofProcs--;
            }
            else
                i++;
        }
        currhost = currhost->next;
    }
    *num = count;
    return list;
}








