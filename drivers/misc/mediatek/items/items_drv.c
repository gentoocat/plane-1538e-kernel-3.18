#include <generated/autoconf.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/vmalloc.h>
#include <linux/disp_assert_layer.h>
#include <linux/semaphore.h>
#include <linux/xlog.h>
#include <asm/mach-types.h>
#include <asm/cacheflush.h>
#include <asm/io.h>

#include <mach/items.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/bootmem.h>
#include <mach/items.h>

#define MSG(evt, fmt, args...)                              \
    do {                                                    \
            printk(ANDROID_LOG_INFO, "DBG", fmt, ##args);                            \
    } while (0)

//#define MSG_FUNC_ENTER(f)   MSG(FUNC, "<items_ENTER>: %s\n", __FUNCTION__)
//#define MSG_FUNC_LEAVE(f)   MSG(FUNC, "<items_LEAVE>: %s\n", __FUNCTION__)
#define MSG_FUNC_ENTER(f)   
#define MSG_FUNC_LEAVE(f)  

#define PRNERR(fmt, args...)   printk(ANDROID_LOG_INFO, "items", fmt, ## args);

static int items_probe(struct device *dev)
{
    struct platform_device *pdev;
	
    int   r = 0;

    pdev = to_platform_device(dev);
    if (pdev->num_resources != 1) 
    {
        PRNERR("probed for an unknown device\n");
        r = -ENODEV;
		return -1;
    }

        struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		printk("items res.start=%0x,len=%0x\n",res->start,res->end - res->start + 1);

		unsigned int* va;
		unsigned int* pa;
		int ret = 0;
		*pa = res->start;
		*va = (unsigned int) ioremap_nocache(res->start, res->end - res->start + 1);
		printk("items *va=%0x\n",*va);
		
		char *item;
		item = (char*)*va;
		
		item_init(item,ITEM_SIZE_NORMAL);
	
		char str[64];
		item_string(str, "codec.power",0);
		printk("codec.power = %s\n",str);
		int bl = item_integer("bl.power", 1);
		printk("bl.power = %d\n",bl);
		printk("codec.power = %s\n",str);
		int exit ;
		exit = item_exist("bl.power");
		printk("bl.power exit = %d\n",exit);
/*		if(item_exist("bl.power")) 
			printk("--------items_probe yes bl.power\n");
		else
			printk("--------items_probe not bl.power\n");
*/
    return 0;
}

static int items_remove(struct device *dev)
{
    return 0;
}

static struct platform_driver items_driver =
{
    .driver = {
        .name    = "items",
        .bus     = &platform_bus_type,
        .probe   = items_probe,
        .remove  = items_remove,
    },
};

int __init items_init(void)
{
    int r = 0;

    MSG_FUNC_ENTER();

    /* Register the driver with items */

    if (platform_driver_register(&items_driver)) {
        PRNERR("failed to register mtkfb driver\n");
        r = -ENODEV;
        goto exit;
    }

exit:
    MSG_FUNC_LEAVE();
    return r;
}


static void __exit items_cleanup(void)
{
    MSG_FUNC_ENTER();

    platform_driver_unregister(&items_driver);
    MSG_FUNC_LEAVE();
}
module_init(items_init);
module_exit(items_cleanup);

MODULE_DESCRIPTION("MEDIATEK items driver");
MODULE_AUTHOR("Zaikuo Wang <zaikuo.wang@mediatek.com>");
MODULE_LICENSE("GPL");

