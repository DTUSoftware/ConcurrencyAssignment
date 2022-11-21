# ConcurrencyAssignment
62588 Operating systems Fall 22 - Concurrency Assignment

## How to run

To compile the application, make sure that you have make and gcc installed.
To run the program run the following command:
```bash
make run
```
Since the code uses shared memory, you can run several instances of the same program since we have used shared memory.
Just open up a new/second terminal and run `make run` again and again and again!

To run our self-made tests on the program run:
```bash
make test
```
(If you want faster tests, you can change the `BASE_SLEEP_MICRO_SECONDS` variable in `main.h`, but do keep in mind that even though transactions get way faster, you lose the "overview" of what's happening. You do NOT need to adjust the base sleep to be above the value of deadlocking, since there is a separate test in place to check for deadlocks to whether they work.)

If you want to clean up run
```bash
make clean
```

## What is the idea of the application:
The idea of this application is to create, use and understand threads and how interactions between threads need to be moderated. This is done via us making an ATM where multiple threads can access an account (which, without any moderation, can end up in missing money somewhere!).

From here we need to solve the problems that can happen when working with threads.

For our application we chose to create a bank ATM application, where you can deposit, withdraw, transfer and so on. This would supply the needed for problems revolving concurrency, for us to solve.

## Why concurrency is needed:
Concurrency is needed to make the process for the user go smoothly. 
But more important it's also needed since the bank could have multiple ATM's, all trying to access the same account, there for it is important to control when each ATM are making chances in the main database. 

## What could be the potential issue specifically:
Specifically a potential issue could be as mentioned above. Having multiple ATM's in the bank, trying to for example deposit and withdraw from the account at the precise same time. This could end up with the account, having the wrong balance after both processes are done, along with the processes maybe not executing using the actual balance amount.

## Address race conditions:
Race condition happens when multiple threads are writing on the same variables, and the loser of the race will determine what the final value of the variable should be.

In ATM's it's very important to address how you need to chance the values of variables when two ATMs are trying to either read or write to a file,
It can be a problem if two ATM are trying to withdraw from the same account at the same time, where the loser of the race sets the new value on the account, all in all this makes the account holder get free money from the ATM.

## Solution for race condition:
One solution for race condition could be to make a system where only one process are allowed to read and write on an account at the same time.

Another solution could be to allow all ATM's to read an accounts resources, but make a queue for writing to an account. For this queue to work it's also needed for the queue to be able to decline requests in the queue if the value changed between the read and request, so the ATM request went to be illegal.

## Address deadlocks and starvation:
A deadlock is when a several processes are waiting on one or more processes to finish, to be able to continue the process.  So what happens is a process is waiting on another process, which is waiting on another process, so neither of the processes will be able to continue. 

A deadlock can happen if the following things are happening at the same time:
if there is Mutual exclusion(usually a resource being in a state where it cannot be shared), resource holding, where one of the processes is having one resource locked and want's another one that is being used by another process, No preemption, where a resource can only be unlocked by the process that is currently using it, and circular wait where one process is waiting for another one, which is waiting for another one and thereby creating a circle of processes waiting for each other.

Starvation is when one of the processes is denied a required resource for the process to continue and complete its process, starvation might happen because of errors in mutual exclusion, or if there is a resource leak. So starvation is happening usually because of bad scheduling.

## Solution for deadlocks and starvation:
A solution for deadlocks in our program could be to set a timer on our mutex lock. This is because of if we have multiple ATM's who are trying to change an accounts resources, we are locking the second thread till the first thread is complete. This would fix any problem if there are problems with a thread, and it therefore can't end properly.

This is also what we have implemented as a part of a Housekeeping task, where we check whether a mutex lock has been locked for a considerable amount of time (> 15 sec). If this is true, then we assume that the process which locked the IO probably has crashed. We will then revert the balance back to what it was before anything happened to it, and release the mutex lock.

 
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
