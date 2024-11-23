#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/io.h>

MODULE_LICENSE("GPL");                
MODULE_AUTHOR("LiuHao");         
MODULE_DESCRIPTION("liuhao kernel module running ...");
MODULE_VERSION("1.0"); 

#define MSI_X_ENTRY_SIZE 16

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
    PMI = 0x1,
    MSI = 0x5,
    PCII = 0x10,
    MSI_X = 0x11,
    ASPM
};

struct msi_entry {
    u8 lower_addr;
    u8 upper_addr;
    u8 vector_data;
    u8 vector_control;
};

struct msi_entry *g_msi_table[65];

static u8 parse_pmi_structure(struct pci_dev *pdev, u8 pointer_addr)
{
    return 0;
}

static u8 parse_pcii_structure(struct pci_dev *pdev, u8 pointer_addr)
{
    return 0;
}

/**
 * @brief msi-x structure parse
 * 
 * @param [in] pdev  device info
 * @param [in] pointer_addr current capability structure address
 * 
 * @return u8 next capability id 
 */
static u8 parse_msi_x_structure(struct pci_dev *pdev, u8 pointer_addr)
{
    struct msi_capability {
        u8  capability_id;
        u8  next_cap_ptr;
        u16 message_control;
        u8  tbl_bir_35;
        u8  tbl0;
        u16 tbl1;
        u8  pba_bir_35;
        u8  pba0;
        u16 pba1;
    };

    u8 msi_x_capability[12], next_capability_id;
    int ret, i;
    struct msi_capability *mc;
    void __iomem *bar_addr;
    u32 msi_x_size, lower_addr, upper_addr;
    u32 tbl_addr_offset, pba_addr_offset;
    u32 *msi_x_entry;

    pr_info("%s: pointer_addr: 0x%02x\n", __FUNCTION__, pointer_addr);

    for (i = 0; i < sizeof(msi_x_capability); i ++) {
        ret = pci_read_config_byte(pdev, pointer_addr ++ , &msi_x_capability[i]);
        if (ret) {
            pr_info("pci_read_config_byte error: %d", i);
            pci_dev_put(pdev);
        }
    }

    mc = (struct msi_capability *)msi_x_capability;

    pr_info("capability id: 0x%02x\n", mc->capability_id);
    pr_info("next cap ptr: 0x%02x\n", mc->next_cap_ptr);
    pr_info("message control: 0x%02x\n", mc->message_control);

    pr_info("msi-x tbl bir: 0x%01x\n", mc->tbl_bir_35 & 0x7);
    tbl_addr_offset = (mc->tbl_bir_35) | (mc->tbl0 << 8U) | (mc->tbl1 << 16U);
    pr_info("tbl address: 0x%08x\n", tbl_addr_offset);

    pr_info("msi-x pba bir: 0x%01x\n", mc->pba_bir_35 & 0x7);
    pba_addr_offset = (mc->pba_bir_35) | (mc->pba0 << 8U) | (mc->pba1 << 16U);
    pr_info("pba address: 0x%08x\n", pba_addr_offset);

    // bar_addr = pci_ioremap_bar(pdev, 0);
    bar_addr = ioremap(pci_resource_start(pdev, 0), pci_resource_len(pdev, 0));
    if (bar_addr == NULL) {
        pr_info("pci_ioremap_bar error\n");
        pci_dev_put(pdev);
    }

    pr_info("bar_addr 0x%x\n", (unsigned int)bar_addr);

    msi_x_size = 65;
    for (i = 0; i < msi_x_size; i ++) {
        msi_x_entry = (u32 *)(bar_addr + tbl_addr_offset + (i * MSI_X_ENTRY_SIZE));
        lower_addr = msi_x_entry[0];
        upper_addr = msi_x_entry[1];
        g_msi_table[i] = (struct msi_entry *)msi_x_entry;
        pr_info("lower_addr %x, upper_addr %x, vec_data %x, vec_control %x\n", lower_addr, upper_addr, msi_x_entry[2], msi_x_entry[3]);
    }

    ret = pci_read_config_byte(pdev, mc->next_cap_ptr, &next_capability_id);
    if (ret) {
        pr_info("pci_read_config_byte error: 0x%02x", mc->next_cap_ptr);
        pci_dev_put(pdev);
    }
    return next_capability_id;
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

    ret = pci_read_config_byte(pdev, pc->capability_pointer, &capability_id);
    if (ret) {
        pr_info("pci_read_config_byte error: 0x%02x", pc->capability_pointer);
        pci_dev_put(pdev);
    }

    while (1) {
        switch(capability_id) {
            case MSI_X:
                capability_id = parse_msi_x_structure(pdev, pc->capability_pointer);
                break;
            case PMI:
                capability_id = parse_pmi_structure(pdev, pc->capability_pointer);
                break;
            case PCII:
                capability_id = parse_pcii_structure(pdev, pc->capability_pointer);
                break;
            default:
                pr_info("Can not recognize this capability_id: 0x%02x!\n", capability_id);
                goto br;
                break;
        }
br:
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