#include <linux/module.h> 
#include <linux/kernel.h>   
#include <linux/init.h>   
#include <linux/errno.h> 
#include <linux/pci.h>

MODULE_LICENSE("GPL");                
MODULE_AUTHOR("LiuHao");         
MODULE_DESCRIPTION("liuhao kernel module running ...");
MODULE_VERSION("1.0");            

static int __init pci_init(void)
{
    struct pci_dev *dev = NULL;
    // unsigned char configure_space[64];
    // int ret;

    for_each_pci_dev(dev) {
        if (dev && dev->bus && dev->devfn)
            pr_info("bus: %02x dev: %02x fun: %02x", dev->bus->number, PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn));
    }

    pr_info("%s start!\n", __FUNCTION__);
    return 0;
}

static void __exit pci_exit(void)
{
    pr_info("%s end!\n", __FUNCTION__);
}

module_init(pci_init);
module_exit(pci_exit);
