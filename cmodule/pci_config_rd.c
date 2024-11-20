#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");                
MODULE_AUTHOR("LiuHao");         
MODULE_DESCRIPTION("liuhao kernel module running ...");
MODULE_VERSION("1.0"); 

typedef struct {
    u16  vendor_id;
    u16  device_id;
    u16  command;
    u16  status;
    u32  ccode_revision;
    u8   cache_line_size;
    u8   latency_timer;
    u8   header_type;
    u8   bist;
    u32  bar0;
    u32  bar1;
    u32  bar2;
    u32  bar3;
    u32  bar4;
    u32  bar5;
    u32  cardbus_cis_pointer;
    u16  subsystem_vendor_id;
    u16  subsystem_id;
    u32  expansion_rom_base_address;
    u8   capability_pointer;
    u8:  1;
    u16: 2;
    u32: 4;
    u8   interrupt_line;
    u8   interrupt_pin;
    u8   min_gnt;
    u8   max_lat;
} pci_config_64;

#if sizeof(pci_config_64) != 64
#error "the pci_config_64 is not full the requirement for pci!"
#endif

static int __init pci_config_rd_init(void)
{
    struct pci_dev *pdev;
    u8 config_space[64];
    int ret, i;

    pdev = pci_get_domain_bus_and_slot(0, 1, PCI_DEVFN(5, 0));
    if (pdev == NULL) {
        pr_info("pci_get_domain_bus_and_slot error!\n");
        return -1;
    }

    for (i = 0; i < sizeof(config_space); i ++) {
        ret = pci_read_config_byte(pdev, i , &config_space[i]);
        if (ret) {
            pr_info("pci_read_config_byte error: %d", i);
            pci_dev_put(pdev);
        }
    }

    pr_info("pci configuration space is: \n");
    for (i = 0; i < sizeof(config_space); i += 4) {
        pr_info("%02x: %02x %02x %02x %02x\n", i,
                config_space[i], config_space[i+1], config_space[i+2], config_space[i+3]);
    }

    pci_dev_put(pdev);

    return 0;
}

static void __exit pci_config_rd_exit(void)
{

    pr_info("%s end!\n", __FUNCTION__);
}

module_init(pci_config_rd_init);
module_exit(pci_config_rd_exit);