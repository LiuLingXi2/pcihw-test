#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/iommu.h>

MODULE_LICENSE("GPL");                
MODULE_AUTHOR("LiuHao");         
MODULE_DESCRIPTION("liuhao kernel module running ...");
MODULE_VERSION("1.0");            


static int __init smmu_test_init(void)
{


    return 0;
}

static void __exit smmu_test_exit(void)
{
    pr_info("%s end!\n", __FUNCTION__);
}

module_init(smmu_test_init);
module_exit(smmu_test_exit);