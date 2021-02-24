//creating proc called seconds
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

int init_jiffies, term_jiffies;
const int hz = HZ;

//function protoypes
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
};


// loading module
int proc_init(void){
    // creates the /proc/seconds entry
    init_jiffies = jiffies;
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
	return 0;
}


//removing module
void proc_exit(void){
    // removes the /proc/seconds entry
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}


///proc/seconds is read
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        if (completed) {
            completed = 0;
            return 0;
        }

        term_jiffies = jiffies;
        completed = 1;
        rv = sprintf(buffer, "The number of seconds elapsed was %d\n", ((term_jiffies - init_jiffies)/hz));
        copy_to_user(usr_buf, buffer, rv);
        return rv;
}


module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("Bradley Diep");