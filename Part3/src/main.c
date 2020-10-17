/********************************************************************** 
 *Function:    main 
 *Description: Main routine for calling buttonledTask
 *  
 * Notes:        
 * 
 * Returns:     0.
 * **********************************************************************/
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

/* declare functions of other files and point to them */
void *buttonledTask(void *param);

/* Declare the task variables. */
pthread_t buttonledTaskObj;

int main(void)
{   
   /* Create the sayhellotask using the default task     
   * attributes. Do not pass in any parameters to the tasks. */    
  pthread_create(&buttonledTaskObj, NULL, buttonledTask, NULL);  
  
  /* Allow the tasks to run. */    
  pthread_join(buttonledTaskObj, NULL);   
  
  return 0;
}