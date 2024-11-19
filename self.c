#include <linux/module.h> 
#include <linux/kernel.h>   
#include <linux/init.h>    


MODULE_LICENSE("GPL");                
MODULE_AUTHOR("LiuHao");         
MODULE_DESCRIPTION("liuhao kernel module running ...");
MODULE_VERSION("1.0");            

static int __init self_init(void)
{

    return 0;
}

static void __exit self_exit(void)
{

}

module_init(self_init);
module_exit(self_exit);
