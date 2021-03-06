#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>
#include <linux/sched.h>

#include "pbs_entities.h"

// Kernel Page Error https://www.dedoimedo.com/computers/crash-analyze.html#mozTocId782257

   // Required for the copy to user function
#define  DEVICE_NAME "run"    ///< The device will appear at /dev/ebbchar using this value
#define  CLASS_NAME  "pbs_run"        ///< The device class -- this is a character device driver

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("ml");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("base module");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static struct class*  dev_class  = NULL; ///< The device-driver class struct pointer
static struct device* dev_device = NULL; ///< The device-driver device struct pointer

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

long print_counter = 0;
extern void (*handle_prediction_failure)(void);
//runs a tick for prediction_failure_hanlding
void run_predication_failure_handling(void);

struct PBS_Plan* plan_ptr;

extern struct PBS_Plan* get_pbs_plan(void);
//extern void handle_prediction_failure(struct PBS_Plan*);//renamed in level2
extern void pbs_run_timer_tick(struct PBS_Plan*);
void check_correct_setup(void *);

// --- OPEN ----
static int dev_open(struct inode *inodep, struct file *filep){
   return 0;
}


// --- READ ----
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    int i;
    printk("[PBS_run_module]: Running 100 timer ticks for plan-based-scheduler");
    for (i = 0; i < 100; i++){
            pbs_run_timer_tick(plan_ptr);
    }
  return 0;

}

// --- WRITE ----
//TODO: Run ix schedule according to parameter
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
    return 0;
}


// --- CLOSE ----
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO ": Device successfully closed\n");
   return 0;
}


static int __init init_global_module(void){
    void (*old_func_ptr)(void) = handle_prediction_failure;

    // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "[PBS_run_module] failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "[PBS_run_module]: registered correctly with major number %d\n", majorNumber);

   // Register the device class
   dev_class = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(dev_class)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(dev_class);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "[PBS_run_module]: device class registered correctly\n");

   // Register the device driver
   dev_device = device_create(dev_class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(dev_device)){               // Clean up if there is an error
      class_destroy(dev_class);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(dev_device);
   }
    plan_ptr = get_pbs_plan();
    handle_prediction_failure = &run_predication_failure_handling;
    check_correct_setup(old_func_ptr);
    return 0;
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit exit_global_module(void){
   device_destroy(dev_class, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(dev_class);                          // unregister the device class
   class_destroy(dev_class);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "[PBS_run_module]: Goodbye from the LKM!\n");
}


void run_predication_failure_handling(){
    if(print_counter % 10000000 == 0){
        printk(KERN_INFO "[PBS_run_predication_failure_handling] Called from kernel %ld times\n", print_counter);
    }
    pbs_run_timer_tick(plan_ptr);
    print_counter++;
}
EXPORT_SYMBOL(run_predication_failure_handling);

void check_correct_setup(void *func_ptr_handle){
    // check if pointer was set properly
    printk(KERN_INFO "[PBS_run_module]: handle_prediction_failure(old): %p, new: %p, run_prediction_failure: %p", func_ptr_handle, handle_prediction_failure, run_predication_failure_handling);
    printk(KERN_INFO "[PBS_run_module]: first == NULL; second == third");

    printk(KERN_INFO "[PBS_run_module]: pbs_run_timer_tick: %p", pbs_run_timer_tick);
    printk(KERN_INFO "[PBS_run_module]: should be != NULL");

    printk(KERN_INFO "[PBS_run_module]: plan_ptr: %p, 1st_task: %p, 1st_task_id: %ld, 1st_task_length: %ld", plan_ptr, &plan_ptr->plan_ptr[0], plan_ptr->plan_ptr[0].task_id,  plan_ptr->plan_ptr[0].instructions_planned);
    printk(KERN_INFO "[PBS_run_module]: first != NULL; 1st_task != NULL; 1st_task_id == 0");

    printk(KERN_INFO "[PBS_run_module]: cur_task: %p, cur_process: %p", plan_ptr->cur_task, plan_ptr->cur_process);
    printk(KERN_INFO "[PBS_run_module]: cur_task != NULL; cur_task != NULL");

}

module_init(init_global_module);
module_exit(exit_global_module);
