#include "mpi.h"
#include "SL_proc.h"


SL_array_t *Volpex_proc_array;
SL_array_t *Volpex_comm_array;
extern int SL_this_procid;
extern char *hostip;
extern char *hostname;
extern int Volpex_numprocs;
extern int redundancy;
extern int Volpex_numcomms;


int Volpex_insert_reuseval(int procid,int reuseval)
{
	Volpex_proc *proc=NULL;
	
	proc = Volpex_get_proc_byid(procid);
	proc->reuseval = reuseval;
//	if (reuseval>10)
//	printf("[%d]:Reuseval :%d for proc %d\n", SL_this_procid,reuseval, proc->id);
	return 0;
}







int Volpex_dest_src_locator(int rank, int comm, char *myfullrank, int **target,
                            int *numoftargets, int msglen, int msgtype)
{
    int i, j=0 ;
    int *tar=NULL;
    int numtargets=0;
    Volpex_comm *communicator=NULL;
    Volpex_proc *proc=NULL;

    Volpex_proclist *plist=NULL;

    communicator = Volpex_get_comm_byid ( comm );
    plist        = &communicator->plist[rank];


    for(i=0; i< plist->num; i++){
        proc = Volpex_get_proc_byid (plist->ids[i]);
        if (proc->state == VOLPEX_PROC_CONNECTED){
            numtargets ++;
        }
    }
    
    tar = (int *) malloc ( numtargets * sizeof(int));
    if ( NULL == tar ) {
        return SL_ERR_NO_MEMORY;
    }

    for(i=0; i<proc->plist->num; i++){
	proc = Volpex_get_proc_byid (proc->plist->ids[i]);
        if (proc->state == VOLPEX_PROC_CONNECTED){
		tar[j] = proc->plist->ids[i];
		j++;
	}
    }	
    *target = tar;
    *numoftargets = numtargets;

    return MPI_SUCCESS;

}




int Volpex_change_target(int rank, int comm)
{
    int i,j;
    Volpex_comm *communicator=NULL;
    Volpex_proc *proc=NULL, *tproc=NULL;
    Volpex_proclist *plist=NULL;
    int tmp;
    
    communicator = Volpex_get_comm_byid ( comm );
    plist        = &communicator->plist[rank];
    
    
    tmp = plist->ids[0];
    
    proc = Volpex_get_proc_byid(tmp);
  



    PRINTF(("[%d]: Volpex_change_target: changing target from %d to %d\n", SL_this_procid,proc->plist->ids[0], proc->plist->ids[1]));
    tmp = proc->plist->ids[0];
    for (i=1; i<proc->plist->num; i++)
    {
 	if (proc->state == VOLPEX_PROC_CONNECTED)	
	    proc->plist->ids[i-1] = proc->plist->ids[i] ;
    }
    proc->plist->ids[i-1] = tmp;
   
    
    for(i=0;i<proc->plist->num;i++){
	tproc = Volpex_get_proc_byid(proc->plist->ids[i]);
	for(j=0; j<proc->plist->num;j++)
	{
	    tproc->plist->ids[j] = proc->plist->ids[j];
	}

 }   

//    Volpex_print_procplist();
    return proc->plist->ids[0];   
}

int Volpex_set_newtarget(int newtarget, int rank, int comm)
{
	int i;
    Volpex_comm *communicator=NULL;
    Volpex_proc *proc=NULL ;
    Volpex_proc	*tproc=NULL; int j;
    Volpex_proclist *plist=NULL ;
    int id,tmp;

    communicator = Volpex_get_comm_byid ( comm );
    plist        = &communicator->plist[rank];


    tmp = plist->ids[0];
    proc = Volpex_get_proc_byid(tmp);


    PRINTF(("[%d]:Volpex_set_newtarget: target:%d\n", SL_this_procid,newtarget));





    id = proc->plist->ids[0];
    for (i=1; i<proc->plist->num; i++)
    {
        if (proc->plist->ids[i] == newtarget)
                proc->plist->ids[i] = id ;
    }
    proc->plist->ids[0] = newtarget;
	for(i=0;i<proc->plist->num;i++){
                        tproc = Volpex_get_proc_byid(proc->plist->ids[i]);
                        for(j=0; j<proc->plist->num;j++)
                        {
                                tproc->plist->ids[j] = proc->plist->ids[j];
                        }
                }


//    Volpex_print_procplist();
    
    return MPI_SUCCESS;



}







