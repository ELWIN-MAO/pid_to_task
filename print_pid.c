#include <linux/module.h>    
#include <linux/init.h>    
#include <linux/kernel.h>    
#include <linux/fs.h>    
#include <linux/miscdevice.h>    
#include <asm/uaccess.h>    
#include <linux/pid.h>
#include <linux/sched.h>
     
#define DEVICE_NAME "wordcount"         //  定义设备文件名    
static unsigned char mem[10000];                //  保存向设备文件写入的数据    
static char read_flag = 'y';                    //  y：已从设备文件读取数据   n：未从设备文件读取数据    
static int written_count = 0;                   // 向设备文件写入数据的字节数    
    int apid;
struct task_struct *p;
     
//  从设备文件读取数据时调用该函数    
//  file：指向设备文件、buf：保存可读取的数据   count：可读取的字节数  ppos：读取数据的偏移量    
static ssize_t word_count_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)    
{       
    //  如果还没有读取设备文件中的数据，可以进行读取    
    if(read_flag == 'n')    
    {       
        //  将内核空间的数据复制到用户空间，buf中的数据就是从设备文件中读出的数据    
        copy_to_user(buf, (void*) mem, written_count+1);    
        //  向日志输出已读取的字节数    
        printk("read count:%d\n", (int) written_count);    
        //  设置数据已读状态    
        read_flag = 'y';    
        return written_count;    
    }    
    //  已经从设备文件读取数据，不能再次读取数据    
    else   
    {       
        return 0;    
    }    
}    
//  向设备文件写入数据时调用该函数    
//  file：指向设备文件、buf：保存写入的数据   count：写入数据的字节数  ppos：写入数据的偏移量    
static ssize_t word_count_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)    
{       
    //  将用户空间的数据复制到内核空间，mem中的数据就是向设备文件写入的数据    
    copy_from_user(mem, buf, count);    
    mem[count+1]='\0';    
    //  设置数据的未读状态    
    read_flag = 'n';    
    //  保存写入数据的字节数   
    written_count = count;    
    //  向日志输出已写入的字节数    
    printk("written count:%d\n", (int)count);  
    printk("content:%s\n", mem);	
    sscanf(mem,"%d", &apid);
    printk("int_content:%d\n", apid);
    p = pid_task(find_vpid(apid), PIDTYPE_PID);
    if(p!=NULL)
    {
    printk("%d,%d---->%s\n",p->pid,p->tgid,p->comm);
    memset ((void*) mem, 0,1000);
    sprintf(mem,"%d,%s\n",p->tgid,p->comm);
    written_count = strlen(mem);
    }
    else
    {
    printk("%d,%d---->%s\n",apid,0,"NULL");
    memset ((void*) mem, 0,1000);
    sprintf(mem,"%d , %s\n",0,"NULL");
    written_count = strlen(mem);
    }
	
    return count;    
}    
//  描述与设备文件触发的事件对应的回调函数指针    
//  需要设置read和write成员变量，系统才能调用处理读写设备文件动作的函数    
static struct file_operations dev_fops =    
{ .owner = THIS_MODULE, .read = word_count_read, .write = word_count_write };    
     
//  描述设备文件的信息    
static struct miscdevice misc =    
{ .minor = MISC_DYNAMIC_MINOR, .name = DEVICE_NAME, .fops = &dev_fops };    
     
//  初始化Linux驱动    
static int word_count_init(void)    
{    
    int ret;    
    //  建立设备文件    
    ret = misc_register(&misc);    
    //  输出日志信息    
    printk("word_count_init_success\n");    
    return ret;    
}    
     
// 卸载Linux驱动    
static void word_count_exit(void)    
{    
    //  删除设备文件    
    misc_deregister(&misc);    
    //  输出日志信息    
    printk("word_init_exit_success\n");    
}    
     
//  注册初始化Linux驱动的函数    
module_init( word_count_init);    
//  注册卸载Linux驱动的函数    
module_exit( word_count_exit);    
     
MODULE_AUTHOR("lining");    
MODULE_DESCRIPTION("statistics of word count.");    
MODULE_ALIAS("word count module.");    
MODULE_LICENSE("GPL");
