#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "pstat.h"
//#define MAXTICKETS 150
//#define MINTICKETS 10

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
int minpassnew=10;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}



/* ************************************************************************************************
int findmin(void)
{
	struct proc *p;
	int min_pass = 2000000000;
	
	for(p=ptable.proc;p<&ptable.proc[NPROC];p++)
	{
		if(p->state != RUNNABLE)
		{
			continue;
		}
		else
		{
			if(p->pass < min_pass)
			{
			  	min_pass = p->pass;
			}
		}		
		
	}
        minpassnew=min_pass;
	return min_pass;
	
}


************************************************************************************************ */







// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;
  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  p->tickets = 10;
  p->stride = (277200)/p->tickets; //default value
  p->pass = 0;
  p->n_schedule=0;
  if(minpassnew!=0&&minpassnew!=2000000000)
	p->pass=minpassnew;
  else
  	p->pass = 0;
  
  release(&ptable.lock);

  // Allocate kernel stack if possible.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;
  
  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;
  
  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}

// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];
  
  p = allocproc();
  acquire(&ptable.lock);
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  p->state = RUNNABLE;

//process stride and pass value

  p->tickets = 10;
  p->stride = (277200)/ p->tickets;
  p->pass = 0;
  p->n_schedule=0;
  release(&ptable.lock);
}



// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  
  sz = proc->sz;
  if(n > 0){
    if((sz = allocuvm(proc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(proc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  proc->sz = sz;
  switchuvm(proc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;

  // Allocate process.
  if((np = allocproc()) == 0)
    return -1;

  // Copy process state from p.
  if((np->pgdir = copyuvm(proc->pgdir, proc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = proc->sz;
  np->parent = proc;
  *np->tf = *proc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(proc->ofile[i])
      np->ofile[i] = filedup(proc->ofile[i]);
  np->cwd = idup(proc->cwd);
 
  pid = np->pid;
  np->state = RUNNABLE;
  safestrcpy(np->name, proc->name, sizeof(proc->name));
  np->tickets = 10;
  np->stride = (277200)/ np->tickets;
  
  //np->pass = 0;
  np->n_schedule=0;
  
  //np->stride = (MAXTICKETS+MINTICKETS -(np->tickets))/10; //default value
 if(minpassnew!=0&&minpassnew!=2000000000)
	np->pass=minpassnew;
  else
  	np->pass = 0;

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *p;
  int fd;

  if(proc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(proc->ofile[fd]){
      fileclose(proc->ofile[fd]);
      proc->ofile[fd] = 0;
    }
  }

  iput(proc->cwd);
  proc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(proc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == proc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
  proc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;

  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for zombie children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != proc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->state = UNUSED;
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || proc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(proc, &ptable.lock);  //DOC: wait-sleep
  }
}

// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.

struct proc* stride_scheduler()
{
	struct proc *p,*pselected = NULL;
	int min_pass = 2000000000;
	for(p=ptable.proc;p<&ptable.proc[NPROC];p++)
	{
		if(p->state != RUNNABLE)
		{
			continue;
		}
		else
		{
			if(p->pass < min_pass)
			{
			  	pselected = p;
				min_pass = p->pass;
			}
		}
	}
        minpassnew=min_pass;
	return pselected;
}



void setpasszero(void)
{
	struct proc *p;
	for(p=ptable.proc;p<&ptable.proc[NPROC];p++)
	{
		p->pass=0;
		
	}
	
}

void
scheduler(void)
{
  struct proc *p;

  acquire(&ptable.lock);
  
  for(p=ptable.proc;p<&ptable.proc[NPROC];p++)
  {
	p->pass=0;	
  }
  minpassnew=0;
  setpasszero();
  release(&ptable.lock);

  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    
	p  = stride_scheduler();
	if(p!=NULL)
	{
		if(p->pass>=2000000000)
		{
			setpasszero();
		}
		// Switch to chosen process.  It is the process's job
      		// to release ptable.lock and then reacquire it
      		// before jumping back to us.
		//cprintf("minpass value in Scheduler %d\n",minpassnew);
		proc = p;
      		switchuvm(p);
     		p->state = RUNNING;
      		swtch(&cpu->scheduler, proc->context);
      		switchkvm();

		p->n_schedule++;
		p->pass += p->stride;// update the pass value		
		//update the # times process is scheduled
		// Process is done running for now.
     		// It should have changed its p->state before coming back.
      		
   	}
	proc = 0;

    release(&ptable.lock);

  }
}


/*
void
scheduler(void)
{
  struct proc *p;

  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      proc = p;
      switchuvm(p);
      p->state = RUNNING;
      swtch(&cpu->scheduler, proc->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      proc = 0;
    }
    release(&ptable.lock);

  }
}
*/




// Count number of process

int cmp(char a1[], char a2[])
{
	int j;
	for(j=0;a2[j]!='\0';j++)
	{
		if(a1[j]==a2[j])
			return 0;
	}
	return 1;
}

int num_process(void)
{
	int count=0;
	struct proc *p;
	int flag1=0;
	int flag2=0;
	acquire(&ptable.lock);
	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(!(p->state==UNUSED||p->state==RUNNING||p->state==RUNNABLE||p->state==EMBRYO||p->state==SLEEPING||p->state==ZOMBIE))
			return -1;	
// UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE	
		if(p->state != UNUSED)
		{
			count++;
			if(cmp(p->name,"init"))
				flag1=1;
			 if(cmp(p->name,"sh"))
                                flag2=1;
			//cprintf("\n%s",p->name);
		}
      	}
	release(&ptable.lock);
	if(flag1==0 && flag2==0)
		return -1;
	return count;
}


// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state.
void
sched(void)
{
  int intena;

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(cpu->ncli != 1)
    panic("sched locks");
  if(proc->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = cpu->intena;
  swtch(&proc->context, cpu->scheduler);
  cpu->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  proc->state = RUNNABLE;
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);
  
  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  if(proc == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }

  // Go to sleep.
  proc->chan = chan;
  proc->state = SLEEPING;
  sched();

  // Tidy up.
  proc->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;
  //int x;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if(p->state == SLEEPING && p->chan == chan)
    {
      p->state = RUNNABLE;
      if(minpassnew!=0&&minpassnew!=2000000000)
		p->pass=minpassnew;
  	else
  		p->pass = 0;
  
     /*if(minpassnew!=2000000000&&minpassnew!=0)
		p->pass=minpassnew;*/
	//cprintf("%d\n",minpassnew);
    }
  }
  
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];
  
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

int 
fill_pstat(struct pstat* p)
{
	struct proc* currproc;
	int i=0;
	acquire(&ptable.lock);
    	for(currproc=ptable.proc; currproc<&ptable.proc[NPROC];currproc++)
	{
		if(currproc->state!=UNUSED)
		{
			p[i].inuse = 1;
		}
		else
		{
			p[i].inuse = 0;
		}
		p[i].pid = currproc->pid;
		safestrcpy(p[i].name,currproc->name,sizeof(p[i].name));
		p[i].tickets = currproc->tickets;
		p[i].stride = currproc->stride;
		p[i].pass = currproc->pass;
		p[i].n_schedule = currproc->n_schedule;
		//cprintf("%s\n %d",p[i].name,p[i].pid);
		i++;
	}
	release(&ptable.lock);
	return 0;
}

