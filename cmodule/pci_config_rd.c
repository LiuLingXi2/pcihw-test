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
    u8   revision_id;
    u8   ccode;
    u16  ccode2;
    u8   cache_line_sizev;
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
    u8:  8;
    u16: 16;
    u32: 32;
    u8   interrupt_line;
    u8   interrupt_pin;
    u8   min_gnt;
    u8   max_lat;
} pci_config_64;

enum CapabilityID {
    default_id = 0,
    MSI_X = 0x11,
    ASPM, PM
};

static g_capability_addr;

static u8 parse_msi_x_structure(void)
{
    struct msi_capability {
        u8  capability_id;
        u8  next_cap_ptr;
        u16 message_control;
        u8  max_x_table_bar_indicator;
        u8  tba;
        u16 tba2;
        u8  msi_x_pending_bit_array_bar_indicator;
        u16 pba;
        u8  pba2;
    };

    u8 config_space[12];
    int ret, i;
    struct msi_capability *mc;

    pdev = pci_get_domain_bus_and_slot(0, 1, PCI_DEVFN(5, 0));
    if (pdev == NULL) {
        pr_info("pci_get_domain_bus_and_slot error!\n");
        return -1;
    }

    pr_info("%s: g_capability_addr: 0x%02x\n", __FUNCTION__, g_capability_addr);

    for (i = 0; i < sizeof(config_space); i ++) {
        ret = pci_read_config_byte(pdev, g_capability_addr ++ , &config_space[i]);
        if (ret) {
            pr_info("pci_read_config_byte error: %d", i);
            pci_dev_put(pdev);
        }
    }

    mc = (struct msi_capability *)config_space;
}

static int __init pci_config_rd_init(void)
{
    struct pci_dev *pdev;
    u8 config_space[64], capability_id;
    pci_config_64 *pc;
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

    pc = (pci_config_64 *)config_space;

    pr_info("pci configuration space is: \n");

    pr_info("Vendor ID: 0x%04x, Device ID: 0x%04x\n", pc->vendor_id, pc->device_id);

    pr_info("Revision ID: 0x%02x, Class Code: 0x%6x\n", pc->revision_id, (pc->ccode << 16U) | pc->ccode2);
    pr_info("BIST: 0x%02x, Header Type: 0x%02x, Latency Timer: 0x%02x, Cache Line Size: 0x%02x\n", pc->bist, pc->header_type, pc->latency_timer, pc->cache_line_sizev);

    pr_info("Base Address Register 0: 0x%08x\n", pc->bar0 & 0xfffffff0);
    pr_info("Base Address Register 1: 0x%08x\n", pc->bar1);
    pr_info("Base Address Register 2: 0x%08x\n", pc->bar2);
    pr_info("Base Address Register 3: 0x%08x\n", pc->bar3);
    pr_info("Base Address Register 4: 0x%08x\n", pc->bar4);
    pr_info("Base Address Register 5: 0x%08x\n", pc->bar5);

    pr_info("Cardbus CIS pointer: 0x%08x\n", pc->cardbus_cis_pointer);
    pr_info("Subsystem Vendor ID: 0x%04x, Subsystem ID: 0x%04x\n", pc->subsystem_vendor_id, pc->subsystem_id);
    pr_info("Expansion ROM Base Address: 0x%08x\n", pc->expansion_rom_base_address);
    pr_info("Capability Pointer: 0x%02x\n", pc->capability_pointer);
    pr_info("Max Lat: 0x%02x, Min Gnt: 0x%02x, Interrupt Pin: 0x%02x, Interrupt Line: 0x%02x\n", pc->max_lat, pc->min_gnt, pc->interrupt_pin, pc->interrupt_line);

    // for (i = 0; i < sizeof(config_space); i += 4) {
    //     pr_info("%02x: %02x %02x %02x %02x\n", i,
    //             config_space[i], config_space[i+1], config_space[i+2], config_space[i+3]);
    // }    

    g_capability_addr = pc->capability_pointer;
    pci_read_config_byte(pdev, pc->capability_pointer, &capability_id);


    switch(capability_id) {
        case MSI_X:
            capability_id = parse_msi_x_structure();
            break;
        default:
            pr_info("Can not recognize this capability_id: 0x%02x!\n", capability_id);
            break;
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