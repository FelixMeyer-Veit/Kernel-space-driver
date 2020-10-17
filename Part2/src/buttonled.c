/**********************************************************************
 * Function:    buttonledTask
 * Description: This task toggles the led when pressing the button 
 * Notes:        
 * Returns:     None.
 *  **********************************************************************/
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

void *buttonledTask(void *param)
{
    char out_buffer [] = ""; // text that will be sended to the kernel
    int count = 0; // bytes of message
    int buttonvalue = 0;

    int fd = open("/dev/buttonled", O_RDWR);   // fd = reference to the device
    while (1)    
    {    
        buttonvalue = read(fd, out_buffer, count);           
        usleep(100000); // = 100 ms
        
        // toggle state of led when button was pressed
        if(buttonvalue > 0)
        {
            int ret = write(fd, out_buffer, buttonvalue); // write to kernel predifend message
            if(ret < 0) printf("Error");  // if return of write() is negative, an error occurred
        }
        
        // alternative: turn led on only when button is pressed
        //int ret = write(fd, out_buffer, buttonvalue); // write to kernel predifend message
        //if(ret < 0) printf("Error");  // if return of write() is negative, an error occurred
    }
    close(fd);
}