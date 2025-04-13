#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/spinlock.h>
#include <linux/kmod.h>
#include <linux/workqueue.h>

#define MAX_COUNTS 256
#define TARGET_COUNT 10 // Đếm đến 10 thì đóng Firefox
#define TIMER_INTERVAL HZ // Interrupt mỗi giây

// Biến lưu trữ số lần interrupt và buffer
static unsigned long interrupt_count = 0;
static unsigned long count_buffer[MAX_COUNTS];
static int buffer_index = 0;
static DEFINE_SPINLOCK(count_lock);
static struct proc_dir_entry *proc_entry;
static bool firefox_closed = false;

// Timer structure
static struct timer_list my_timer;

// Workqueue structure
static struct work_struct firefox_work;

// Hàm đóng Firefox từ workqueue
static void close_firefox_work(struct work_struct *work)
{
    int ret;
    // Sử dụng killall để đóng Firefox dưới user "ledat"
    char *argv[] = { "/bin/su", "ledat", "-c", "/usr/bin/killall firefox", NULL };
    char *envp[] = { "HOME=/home/ledat", "PATH=/sbin:/usr/sbin:/bin:/usr/bin", NULL };

    printk(KERN_INFO "Attempting to close Firefox...\n");
    ret = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
    if (ret != 0) {
        printk(KERN_ERR "Failed to close Firefox: %d\n", ret);
    } else {
        printk(KERN_INFO "Firefox closed successfully\n");
        firefox_closed = true;
    }
}

// Hàm xử lý interrupt từ timer
static void timer_handler(struct timer_list *t)
{
    unsigned long flags;

    // Tăng biến đếm interrupt
    interrupt_count++;

    // Lưu vào buffer
    spin_lock_irqsave(&count_lock, flags);
    if (buffer_index < MAX_COUNTS) {
        count_buffer[buffer_index++] = interrupt_count;
        printk(KERN_INFO "Timer interrupt occurred: count=%lu\n", interrupt_count);
    } else {
        printk(KERN_WARNING "Buffer full, ignoring interrupt count\n");
    }
    spin_unlock_irqrestore(&count_lock, flags);

    // Kiểm tra nếu đếm đến 10 và chưa đóng Firefox
    if (interrupt_count >= TARGET_COUNT && !firefox_closed) {
        // Lên lịch workqueue để đóng Firefox
        schedule_work(&firefox_work);
        return; // Dừng timer
    }

    // Cài đặt lại timer nếu chưa đóng Firefox
    if (!firefox_closed) {
        mod_timer(&my_timer, jiffies + TIMER_INTERVAL);
    }
}

// Hàm hiển thị số lần interrupt qua /proc
static int timer_proc_show(struct seq_file *m, void *v)
{
    int i;
    unsigned long flags;

    spin_lock_irqsave(&count_lock, flags);
    for (i = 0; i < buffer_index; i++) {
        seq_printf(m, "Interrupt %d: count=%lu\n", i + 1, count_buffer[i]);
    }
    spin_unlock_irqrestore(&count_lock, flags);

    return 0;
}

static int timer_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, timer_proc_show, NULL);
}

static const struct proc_ops timer_proc_ops = {
    .proc_open = timer_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

// Hàm khởi tạo module
static int __init timer_init(void)
{
    int ret;

    // Khởi tạo timer
    timer_setup(&my_timer, timer_handler, 0);
    
    // Khởi tạo workqueue
    INIT_WORK(&firefox_work, close_firefox_work);

    // Cài đặt timer chạy lần đầu sau 1 giây
    mod_timer(&my_timer, jiffies + TIMER_INTERVAL);

    // Tạo file /proc/timer_counts
    proc_entry = proc_create("timer_counts", 0444, NULL, &timer_proc_ops);
    if (!proc_entry) {
        printk(KERN_ERR "Failed to create /proc/timer_counts\n");
        del_timer(&my_timer);
        return -ENOMEM;
    }

    printk(KERN_INFO "Timer interrupt module loaded\n");
    return 0;
}

// Hàm dọn dẹp module
static void __exit timer_exit(void)
{
    // Xóa timer
    del_timer_sync(&my_timer);

    // Hủy workqueue
    cancel_work_sync(&firefox_work);

    // Xóa file /proc
    if (proc_entry)
        proc_remove(proc_entry);

    printk(KERN_INFO "Timer interrupt module unloaded\n");
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Example Author");
MODULE_DESCRIPTION("Timer interrupt to count and close Firefox");
