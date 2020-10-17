#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/gpio.h>                       // for the GPIO functions 
#include <linux/interrupt.h>                  // for the IRQ code
#include <linux/fs.h>

#define  DEVICE_MAJOR 239         ///< Requested device node major number or 0 for dynamic allocation
#define  DEVICE_NAME "buttonled"   ///< In this implementation, the device name has nothing to do with the name of the device in /dev. You must use mknod to create the device node in /dev

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Felix Meyer-Veit"); 
MODULE_DESCRIPTION("A Button/LED test driver for the RPi"); 
MODULE_VERSION("0.1");

static unsigned int gpioLED = 17;             // pin 11 (GPIO17) 
static unsigned int gpioButton = 27;          // pin 13 (GPIO27) 
static unsigned int irqNumber;                // share IRQ num within file 
static unsigned int numberPresses = 0;        // store number of presses 
static bool         ledOn = 0;                // used to invert state of LED
static int          majorNumber;              ///< Stores the device number -- determined automatically
static int          numberOpens = 0;          ///< Counts the number of times the device is opened
static ssize_t      buttonvalue = 0;          // value of button
static int          result = 0;


static int     gpio_open(struct inode *, struct file *);
static ssize_t gpio_read(struct file *, char *, size_t, loff_t *);
static ssize_t gpio_write(struct file *, const char *, size_t, loff_t *);
static int     gpio_release(struct inode *, struct file *);

static struct file_operations fops =
{
   .open = gpio_open,
   .write = gpio_write,
   .read = gpio_read,
   .release = gpio_release,
};

static ssize_t gpio_write(struct file * file, const char *buf, size_t count, loff_t *ppos)
{
    // alternative: constantly write a value to the LED
    //printk("GPIO_TEST: Set Led to %zu\n", count);
    //gpio_set_value(gpioLED, count);              // turn the LED on/off

    // only write LED when button was pressed
    numberPresses++;                         // global counter
    ledOn = !ledOn;                          // invert the LED state   
    gpio_set_value(gpioLED, ledOn);          // set LED accordingly
    return count;
}

static ssize_t gpio_read(struct file * file, char *buf, size_t count, loff_t *ppos)
{
    buttonvalue = gpio_get_value(gpioButton);
    //printk("GPIO_TEST: Read value of button: %zu\n", count);
    return buttonvalue;                         // return the button value
}


// prototype for the custom IRQ handler function, function below 
static irq_handler_t  erpi_gpio_irq_handler(unsigned int irq, 
                                            void *dev_id, struct pt_regs *regs);

static int __init erpi_gpio_init(void) 
{
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber<0){
        printk(KERN_ALERT "GPIO_TEST failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "GPIO_TEST: registered correctly with major number %d\n", majorNumber);




    printk(KERN_INFO "GPIO_TEST: Initializing the GPIO_TEST LKM\n");

    if (!gpio_is_valid(gpioLED)) 
    {
        printk(KERN_INFO "GPIO_TEST: invalid LED GPIO\n");
        return -ENODEV;
    }   

    ledOn = true;

    gpio_request(gpioLED, "sysfs");          // request LED GPIO
    gpio_direction_output(gpioLED, ledOn);   // set in output mode and on 
    gpio_set_value(gpioLED, false);              // not reqd - see line above
    gpio_export(gpioLED, false);             // appears in /sys/class/gpio
                                             // false prevents in/out change   
    gpio_request(gpioButton, "sysfs");       // set up gpioButton   
    gpio_direction_input(gpioButton);        // set up as input   
    gpio_set_debounce(gpioButton, 200);      // debounce delay of 200ms
    gpio_export(gpioButton, false);          // appears in /sys/class/gpio

    printk(KERN_INFO "GPIO_TEST: button value is currently: %d\n", 
           gpio_get_value(gpioButton));

    irqNumber = gpio_to_irq(gpioButton);     // map GPIO to IRQ number
    printk(KERN_INFO "GPIO_TEST: button mapped to IRQ: %d\n", irqNumber);

    // This next call requests an interrupt line      
    result = request_irq(irqNumber,          // interrupt number requested            
        (irq_handler_t) erpi_gpio_irq_handler,   // handler function            
        IRQF_TRIGGER_RISING,                     // on rising edge (press, not release)            
        "erpi_gpio_handler",                     // used in /proc/interrupts
        NULL);                                   // *dev_id for shared interrupt lines

    printk(KERN_INFO "GPIO_TEST: IRQ request result is: %d\n", result);
    return result;
}

static void __exit erpi_gpio_exit(void) 
{
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "GPIO_TEST: button value is currently: %d\n", 
           gpio_get_value(gpioButton));

    printk(KERN_INFO "GPIO_TEST: pressed %d times\n", numberPresses);
    gpio_set_value(gpioLED, 0);              // turn the LED off
    gpio_unexport(gpioLED);                  // unexport the LED GPIO   
    free_irq(irqNumber, NULL);               // free the IRQ number, no *dev_id   
    gpio_unexport(gpioButton);               // unexport the Button GPIO   
    gpio_free(gpioLED);                      // free the LED GPIO
    gpio_free(gpioButton);                   // free the Button GPIO
    printk(KERN_INFO "GPIO_TEST: Goodbye from the LKM!\n"); 
}

static irq_handler_t erpi_gpio_irq_handler(unsigned int irq, 
                                           void *dev_id, struct pt_regs *regs) 
{   
    ledOn = !ledOn;                          // invert the LED state   
    gpio_set_value(gpioLED, ledOn);          // set LED accordingly  
    printk(KERN_INFO "GPIO_TEST: Interrupt! (button is %d)\n", 
           gpio_get_value(gpioButton));
    numberPresses++;                         // global counter
    return (irq_handler_t) IRQ_HANDLED;      // announce IRQ handled 
}

static int gpio_open(struct inode *inodep, struct file *filep)
{
   numberOpens++;
   printk(KERN_INFO "GPIO_TEST: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

static int gpio_release(struct inode *inodep, struct file *filep)
{
   printk(KERN_INFO "GPIO_TEST: Device successfully closed\n");
   return 0;
}


module_init(erpi_gpio_init);
module_exit(erpi_gpio_exit);