# ConcurrencyAssignment
62588 Operating systems Fall 22 - Concurrency Assignment


## What is the idea of the application:
The idea of this application is to create, use and understand threads.
From here we need to solve the problems that can happen when working with threads.

For our application we chose to create a bank ATM application, where you can deposit, withdraw, transfer and so on. This would supply the needed problems revolving concurrency, for us to solve.

## Why concurrency is needed:
Concurrency is needed to make the process for the user go smoothly. 
But more important it's also needed since the bank could have multiple ATM's, all trying to access the same account, there for it's important to control when each ATM are making chances in the main data base. 

## What could be the potential issue specifically:
Specifically a potential issue could be as mentioned above. Having multiple ATM's in the bank, trying to for example deposit and withdraw from the account at the precise same time. This could end up with the account, having the wrong balance after both processes are done, along with the processes maybe not executing using the actual balance amount.

## Address race conditions:
Race condition happens when multiple threads are writing on the same variables, and the loser of the race will determine what the final value of the variable should be.

In ATM's it's very important to address how you need to chance the values of variables when two ATM's are trying to either read or write to an file,
It can be a problem if two ATM are trying to withdraw from the same account at the same time, where the loser of the race sets the new value on the account, all in all this makes the account holder get free money from the ATM.

## Solution for race condition:
One solution for race condition could be to make an system where only one process are allowed to read and write on an account at the same time.

Another solution could be to allow all ATM's to read an accounts resources, but make an que for writing to an account. For this que to work its also needed for the que to be able to decline requestes in the que if the value changed between the read and request, so the ATM request went to be illegal.

## Address deadlocks and starvation:
A deadlock is when a several processes are waiting on one or more processes to finish, to be able to continue the process.  So what happens is a process is waiting on another process, which is waiting on another process, so neither of the processes will be able to continue. 

A deadlock can happen if the following things are happening at the same time:
if there is Mutual exclusion(usually a resource being in a state where it cannot be shared), resource holding, where one of the processes is having one resource locked and want's another one that is being used by another process, No preemption, where a resource can only be unlocked by the process that is currently using it, and circular wait where one process is waiting for another one, which is waiting for another one and thereby creating a circle of processes waiting for eachother.

Starvation is when one of the processes is denied a required resource for the process to continue and complete its process, starvation might happen because of errors in mutual exclusion, or if there is a resource leak. So starvation is happening usually because of bad scheduling.

## Solution for deadlocks and starvation:
A solution for deadlocks in our program could be to set an timer on our mutex lock. This is because of if we have multiple ATM's who are trying to change an accounts resources, we are locking the secound thread till the first thread are complete. This would fix any problem if there are problems with an thread and it therefor cant end properly. 

 
## Resources
https://en.wikipedia.org/wiki/Deadlock

https://en.wikipedia.org/wiki/Starvation_(computer_science)