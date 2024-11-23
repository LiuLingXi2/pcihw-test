#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");                
MODULE_AUTHOR("LiuHao");         
MODULE_DESCRIPTION("liuhao kernel module running ...");
MODULE_VERSION("1.0"); 

#define ITS_BASE_ADDR 0x08080000
#define TABLE_SIZE 0x2000
#define GITS_BASER(n) (0x100 + (n) * 8)

#define NVME1_EVENTID 0x10

static void __iomem *its_base;
static void __iomem **device_table;

static void read_its_table(void)
{
    int i;
    u64 device_table_addr, collec_table_addr;
    u32 device_table_size, collec_table_size;
    u64 device_table_entry;

    device_table_addr = readq(its_base + GITS_BASER(0)) & 0xfffffffff000;
    collec_table_addr = readq(its_base + GITS_BASER(1)) & 0xfffffffff000;

    pr_info("device_table_addr: %x\n", device_table_addr);
    pr_info("collec_table_addr: %x\n", collec_table_addr);

    device_table_size = readq(its_base + GITS_BASER(0)) & 0xff;
    collec_table_size = readq(its_base + GITS_BASER(1)) & 0xff;

    device_table = ioremap(device_table_addr, device_table_size);
    if (device_table == NULL) {
        pr_info("device_table error!\n");
        return ;
    }

    for (i == 0; i < NVME1_EVENTID; i ++) {
        pr_info("itt_addr%d: %x\n", i, (unsigned int)device_table[i]);
    }
}

static int __init gic_its_init(void)
{
    its_base = ioremap(ITS_BASE_ADDR, TABLE_SIZE);

    if (its_base == NULL) {
        pr_info("its_base error!\n");
        return -1;
    }

    pr_info("its_base: %x\n", (unsigned int)its_base);
    read_its_table();
    return 0;
}

static void __exit gic_its_exit(void)
{
    pr_info("%s end!\n", __FUNCTION__);
}

module_init(gic_its_init);
module_exit(gic_its_exit);