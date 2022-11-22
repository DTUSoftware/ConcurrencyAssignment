# ConcurrencyAssignment
62588 Operating systems Fall 22 - Concurrency Assignment

## How to run

To compile the application, make sure you have to make and gcc installed.
To run the program, run the following command:
```bash
make run
```
Since the code uses shared memory, you can run several instances of the same program since we have used shared memory.
Just open up a new/second terminal and run `make run` again and again and again!

To run our self-made tests on the program, run the following:
```bash
make test
```
(If you want faster tests, you can change the `BASE_SLEEP_MICRO_SECONDS` variable in `main.h`, but keep in mind that even though transactions get way faster, you lose the "overview" of what is happening. You do NOT need to adjust the base sleep to be above the value of deadlocking since there is a separate test in place to check for deadlocks to see whether they work.)

If you want to clean up, run
```bash
make clean
```

## What is the idea of the application:
This application aims to create, use and understand threads and how interactions between threads need to be moderated. This is done via us making an ATM where multiple threads can access an account (which, without any moderation, can end up in missing money somewhere!).

From here, we need to solve the problems that can happen when working with threads.

We created a bank ATM application for our application, where you can deposit, withdraw, transfer, and so on. This would supply the needed problems revolving around concurrency for us to solve.

## Why concurrency is needed:
Concurrency is needed to make the process for the user go smoothly. 
However, more importantly, it is also needed since the bank could have multiple ATMs, all trying to access the same account, so it is essential to control when each ATM is making changes in the central database. 

## What could be the potential issue:
Specifically, a potential issue could be as mentioned above. We have multiple ATMs in the bank, trying to, for example, deposit and withdraw from the account simultaneously. This could result in the account having the wrong balance after both processes are done, and the processes may not execute using the actual balance amount.

## Address race conditions:
Race condition happens when multiple threads are written on the same variables, and the race's loser will determine the variable's final value.

In an ATM, it is essential to address how you need to change the values of variables when two ATMs are trying to either read or write to a file,
It can be a problem if two ATMs are trying to withdraw from the same account at the same time, where the race loser sets the new value on the account; all in all, this makes the account holder get free money from the ATM.

## Solution for race condition:
One solution for race conditions could be to create a system where only one process can read and write on an account simultaneously.

Another solution could be to allow all ATMs to read the resources of an account but make a queue for writing to an account. For this queue to work, it is also needed for the queue to be able to decline requests in the queue if the value changes between the read and request, so the ATM request went to be illegal.

## Address deadlocks and starvation:
A deadlock is when several processes are waiting for one or more to finish continuing the process. So what happens is that a process is waiting on another process, which is waiting on another process, so neither of the processes will be able to continue. 

A deadlock can happen if the following things are happening at the same time:
- Mutual exclusion (usually a resource being in a state where it cannot be shared), resource holding, where one of the processes has one resource locked and wants another one that is being used by another process.
 - No preemption, where a resource can only be unlocked by the process that is currently using it, and circular wait, where one process is waiting for another one, which is waiting for another one and thereby creating a circle of processes waiting for each other.

Starvation is when one of the processes is denied a required resource for the process to continue and complete its process; starvation might happen because of errors in mutual exclusion or if there is a resource leak. So starvation is usually happening because of bad scheduling.

## Solution for deadlocks and starvation:
A solution for our program's deadlocks could be setting a timer on our mutex lock. This is because if we have multiple ATMs trying to change an account's resources, we lock the second thread until the first thread is complete. This would fix any problem if there are problems with a thread and it, therefore, cannot end properly.

This is also what we have implemented as a part of a Housekeeping task, where we check whether a mutex lock has been locked for a considerable amount of time (> 15 sec). If this is true, then we assume that the process which locked the IO probably has crashed. We will then revert the balance to what it was before anything happened to it and release the mutex lock.

 
## Resources
https://en.wikipedia.org/wiki/Deadlock  
https://en.wikipedia.org/wiki/Starvation_(computer_science)  
https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-pthread-mutex-trylock-attempt-lock-mutex-object  
https://users.cs.cf.ac.uk/Dave.Marshall/C/node27.html  
https://www.ibm.com/docs/en/zos/2.1.0?topic=functions-shmat-shared-memory-attach-operation  
https://man7.org/linux/man-pages/man2/shmget.2.html  
https://www.ibm.com/docs/en/zos/2.1.0?topic=functions-shmget-get-shared-memory-segment  
https://linux.die.net/man/2/shmat  
https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-pthread-mutex-init-initialize-mutex-object  
https://stackoverflow.com/a/3068420  
https://www.geeksforgeeks.org/clear-console-c-language/  
https://gist.github.com/btmills/4201660  
