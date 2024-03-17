/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Dan Walkes
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/fs.h> // file_operations

#include "aesdchar.h"
int aesd_major =   0; // use dynamic major
int aesd_minor =   0;

MODULE_AUTHOR("Raghu Sai Phani Sriraj Vemparala"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_dev aesd_device;

int aesd_open(struct inode *inode, struct file *filp)
{
    PDEBUG("open_completed");
    struct aesd_dev *a_dev = NULL;
    /**
     * TODO: handle open
     */
    a_dev = container_of(inode->i_cdev, struct aesd_dev, cdev);
	filp->private_data = a_dev; /* for other methods */
    return 0;
}

int aesd_release(struct inode *inode, struct file *filp)
{
    PDEBUG("release");
    /**
     * TODO: handle release
     */
    return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    size_t j = 0;
    PDEBUG("read %zu bytes with offset %lld",count,*f_pos);
    struct aesd_buffer_entry *get_data;
    size_t offset = 0;
    struct aesd_dev *dev = filp->private_data;
    /**
     * TODO: handle read
     */

    if (mutex_lock_interruptible(&dev->lock)!=0) {
	    PDEBUG(KERN_ERR "Unable to acquire Mutex\n");
        retval = -ERESTARTSYS;
	    return retval;
    }
    get_data = aesd_circular_buffer_find_entry_offset_for_fpos(&dev->cir_buf, *f_pos, &offset);
    if(get_data != NULL)
    {
        size_t actual_data_size = get_data->size - offset;
        if(actual_data_size < count)
        {
            if(copy_to_user(buf, get_data->buffptr+offset,actual_data_size))
            {
                retval = -EFAULT;
                goto read_last;
            }
            *f_pos = *f_pos+actual_data_size;
            retval = actual_data_size;
            for( j = 0;j<actual_data_size;j++)
            {
                PDEBUG("If read data%cand_index%zu\n",buf[j],j);
            }
        }
        else
        {
            if(copy_to_user(buf, get_data->buffptr+offset,count))
            {
                retval = -EFAULT;
                goto read_last;
            }
            *f_pos = *f_pos+count;
            retval = count;
            for( j = 0;j<count;j++)
            {
                PDEBUG("else read data%cand_index%zu\n",buf[j],j);
            }
        }
    }
    else{
        PDEBUG("READING_NULL\n");
    }

read_last:
    mutex_unlock(&dev->lock);
    return retval;
}

ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    size_t j =0;
    const char* ret_add_entry = NULL;
    int send_data_to_cb = 0;//Set if /n is received
    int i = 0;
    struct aesd_buffer_entry entry_buffer;
    struct aesd_dev *dev = filp->private_data;
    //ssize_t retval = -ENOMEM;
    PDEBUG("write %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle write
     */
    if (mutex_lock_interruptible(&dev->lock)!=0) {
	PDEBUG(KERN_ERR "Unable to acquire Mutex\n");
	return -EFAULT;
    }
    for( j = 0;j<count;j++)
    {
        PDEBUG("bufdata %cand_count%zu\n",buf[j],count);
    }
    //That means it is a new data
    if(dev->buff_len == 0)
    {
        dev->write_buff = (char*)kmalloc(count,GFP_KERNEL);
        if(dev->write_buff == NULL)
        {
             retval = -ENOMEM;
            printk(KERN_ERR "Failed to allocate memory\n");
            goto exit_write;
        }
        dev->buff_len += count;
        if(copy_from_user(dev->write_buff, buf, count))
        {
             //Handler error case
            retval = -EFAULT;
            kfree(dev->write_buff);
            goto exit_write;
        }
        // for( j = 0;j<dev->buff_len;j++)
        // {
        //     PDEBUG("In all write %cu\n",dev->write_buff[j]);
        // }
    }
    else
    {
        dev->write_buff = krealloc(dev->write_buff, dev->buff_len+count,GFP_KERNEL);
        if(dev->write_buff == NULL)
        {
            retval = -ENOMEM;
            printk(KERN_ERR "Failed to re-allocate memory\n");
            goto exit_write;
        }
        if(copy_from_user(dev->write_buff+dev->buff_len, buf, count))//buf+dev->buff_len
        {
                         //Handler error case
            retval = -EFAULT;
            kfree(dev->write_buff);
            goto exit_write;
        }
        dev->buff_len += count;
        for( j = 0;j<dev->buff_len;j++)
        {
            PDEBUG("In reall write %cand_index%zu\n",dev->write_buff[j],dev->buff_len);
        }
    }
    //copy_from_user(dev->write_buff, buf, count);
    i = 0;
    while(i < dev->buff_len)
    {
        if(dev->write_buff[i] == '\n')
        {
            send_data_to_cb = 1;
            i = 0;
            PDEBUG("Send_data_true\n");
            break;
        }
        i++;    
    }
    retval = count;
    if(send_data_to_cb)
    {
        entry_buffer.buffptr = dev->write_buff;
        entry_buffer.size = dev->buff_len;
        ret_add_entry = aesd_circular_buffer_add_entry(&dev->cir_buf, &entry_buffer);
        retval = dev->buff_len;
        dev->buff_len = 0;//Data received /n.
        send_data_to_cb = 0;
        
    }
    if(ret_add_entry!= NULL)
    {
        kfree(ret_add_entry);
    }
exit_write:
    mutex_unlock(&dev->lock);
    return retval;
}

struct file_operations aesd_fops = {
    .owner =    THIS_MODULE,
    .read =     aesd_read,
    .write =    aesd_write,
    .open =     aesd_open,
    .release =  aesd_release,
};

static int aesd_setup_cdev(struct aesd_dev *dev)
{
    int err, devno = MKDEV(aesd_major, aesd_minor);

    cdev_init(&dev->cdev, &aesd_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &aesd_fops;
    err = cdev_add (&dev->cdev, devno, 1);
    if (err) {
        printk(KERN_ERR "Error %d adding aesd cdev", err);
    }
    return err;
}



int aesd_init_module(void)
{
    dev_t dev = 0;
    int result;
    result = alloc_chrdev_region(&dev, aesd_minor, 1,
            "aesdchar");
    aesd_major = MAJOR(dev);
    if (result < 0) {
        printk(KERN_WARNING "Can't get major %d\n", aesd_major);
        return result;
    }
    memset(&aesd_device,0,sizeof(struct aesd_dev));

    /**
     * TODO: initialize the AESD specific portion of the device
     */
    mutex_init(&aesd_device.lock);
    result = aesd_setup_cdev(&aesd_device);

    if( result ) {
        unregister_chrdev_region(dev, 1);
    }
    return result;

}

void aesd_cleanup_module(void)
{
    int num = 0;
    struct aesd_buffer_entry *clean_buff;
    dev_t devno = MKDEV(aesd_major, aesd_minor);

    cdev_del(&aesd_device.cdev);

    /**
     * TODO: cleanup AESD specific poritions here as necessary
     */
    AESD_CIRCULAR_BUFFER_FOREACH(clean_buff, &aesd_device.cir_buf, num)
    {
        if (clean_buff->buffptr != NULL)
        {
            kfree(clean_buff->buffptr);
            clean_buff->size = 0;
        }
    }
    mutex_destroy(&aesd_device.lock);
    unregister_chrdev_region(devno, 1);
}



module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
