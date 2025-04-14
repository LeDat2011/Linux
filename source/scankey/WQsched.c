#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#define MY_WORK_QUEUE_NAME "WQsched.c"

static struct workqueue_struct *my_workqueue;

struct my_work {
    struct work_struct work;
    unsigned char scancode;
};

static struct my_work task;
static void got_char(struct work_struct *work)
{
    struct my_work *my_work = container_of(work, struct my_work, work);
    printk(KERN_INFO "Scan Code %x %s.\n",
           (int)my_work->scancode & 0x7F,
           my_work->scancode & 0x80 ? "Released" : "Pressed");
}

irqreturn_t irq_handler(int irq, void *dev_id)
{
    unsigned char status;
    status = inb(0x64);
    task.scancode = inb(0x60);
    INIT_WORK(&task.work, got_char);
    queue_work(my_workqueue, &task.work);
    return IRQ_HANDLED;
}
int init_module()
{
    my_workqueue = create_workqueue(MY_WORK_QUEUE_NAME);
    free_irq(1, NULL);
    return request_irq(1, irq_handler, IRQF_SHARED, "test_keyboard_irq_handler", (void *)(irq_handler));
}
void cleanup_module()
{
    free_irq(1, NULL);
}
MODULE_LICENSE("GPL");

