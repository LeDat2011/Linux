/**
 * hotkey_process_terminator.c - Kernel module để kết thúc tiến trình bằng tổ hợp phím tùy chỉnh
 * 
 * Module này thay thế chức năng Alt+F4 bằng tổ hợp phím tùy chỉnh (Ctrl+Alt+K)
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/sched/signal.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Module để kết thúc tiến trình bằng tổ hợp phím tùy chỉnh");
MODULE_VERSION("0.1");

/* Định nghĩa các phím */
#define KEY_CTRL 29
#define KEY_ALT 56
#define KEY_K 37

/* Biến để theo dõi trạng thái các phím */
static int ctrl_pressed = 0;
static int alt_pressed = 0;
static pid_t target_pid = -1;

/* Work queue để xử lý việc kết thúc tiến trình */
static struct workqueue_struct *process_term_wq;
struct work_struct process_term_work;

/* Proc FS entries */
static struct proc_dir_entry *proc_entry;
#define PROC_NAME "hotkey_process_terminator"
#define BUFFER_SIZE 128
static char proc_buffer[BUFFER_SIZE];

/* Danh sách các tiến trình hệ thống cần bỏ qua */
static const char *system_processes[] = {
    "systemd", "kthreadd", "kworker", "ksoftirqd", "vmtoolsd",
    "accounts-daemon", "VGAuthService", "rsyslogd", "systemd-timesyn",
    "cron", "dbus", "NetworkManager", "polkitd", "udisksd"
};

/* Kiểm tra xem có phải tiến trình hệ thống không */
static bool is_system_process(const char *name) {
    int i;
    for (i = 0; i < sizeof(system_processes) / sizeof(char *); i++) {
        if (strstr(name, system_processes[i]) != NULL) {
            return true;
        }
    }
    return false;
}

/* Hàm tìm và kết thúc tiến trình mục tiêu */
static void find_and_terminate_process(void) {
    struct task_struct *task;
    struct kernel_siginfo info;
    
    if (target_pid <= 0) {
        printk(KERN_INFO "Hotkey Terminator: Chưa cấu hình PID. Hãy chạy: echo [pid] > /proc/hotkey_process_terminator\n");
        return;
    }
    
    /* Tìm tiến trình theo PID */
    task = pid_task(find_vpid(target_pid), PIDTYPE_PID);
    if (!task) {
        printk(KERN_INFO "Hotkey Terminator: Không tìm thấy tiến trình với PID %d\n", target_pid);
        return;
    }
    
    printk(KERN_INFO "Hotkey Terminator: Đang kết thúc tiến trình %s (PID: %d)\n", 
           task->comm, task->pid);
    
    /* Thiết lập thông tin tín hiệu SIGTERM */
    memset(&info, 0, sizeof(struct kernel_siginfo));
    info.si_signo = SIGTERM;
    info.si_code = SI_KERNEL;
    info._sifields._kill._pid = task->pid;
    
    /* Gửi tín hiệu SIGTERM đến tiến trình */
    if (send_sig_info(SIGTERM, &info, task) < 0) {
        printk(KERN_INFO "Hotkey Terminator: Không thể gửi SIGTERM đến tiến trình %d\n", task->pid);
        /* Thử gửi SIGKILL nếu SIGTERM không thành công */
        info.si_signo = SIGKILL;
        if (send_sig_info(SIGKILL, &info, task) < 0) {
            printk(KERN_INFO "Hotkey Terminator: Không thể gửi SIGKILL đến tiến trình %d\n", task->pid);
        } else {
            printk(KERN_INFO "Hotkey Terminator: Đã gửi SIGKILL đến tiến trình %d\n", task->pid);
        }
    } else {
        printk(KERN_INFO "Hotkey Terminator: Đã gửi SIGTERM đến tiến trình %d\n", task->pid);
    }
}

/* Hàm work queue */
static void terminate_process(struct work_struct *work) {
    find_and_terminate_process();
}

/* Proc FS operations */
static int proc_show(struct seq_file *m, void *v) {
    seq_printf(m, "PID hiện tại: %d\n", target_pid);
    seq_printf(m, "Cách sử dụng:\n");
    seq_printf(m, "1. Xem PID của tiến trình: ps aux | grep [tên tiến trình]\n");
    seq_printf(m, "2. Cấu hình PID: echo [pid] > /proc/%s\n", PROC_NAME);
    seq_printf(m, "3. Nhấn Ctrl+Alt+K để kết thúc tiến trình\n");
    return 0;
}

static int proc_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_show, NULL);
}

static ssize_t proc_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos) {
    int new_pid;
    
    if (count >= BUFFER_SIZE)
        return -EFAULT;
    
    if (copy_from_user(proc_buffer, ubuf, count))
        return -EFAULT;
    
    proc_buffer[count] = '\0';
    
    if (kstrtoint(proc_buffer, 10, &new_pid) < 0)
        return -EINVAL;
    
    target_pid = new_pid;
    printk(KERN_INFO "Hotkey Terminator: Đã cập nhật PID thành %d\n", target_pid);
    
    return count;
}

static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_write = proc_write,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

/* Callback function cho keyboard notifier */
static int keyboard_notifier_callback(struct notifier_block *nblock, 
                                     unsigned long code, void *_param) {
    struct keyboard_notifier_param *param = _param;
    
    if (code != KBD_KEYCODE)
        return NOTIFY_OK;
    
    if (param->value == KEY_CTRL)
        ctrl_pressed = param->down ? 1 : 0;
    else if (param->value == KEY_ALT)
        alt_pressed = param->down ? 1 : 0;
    
    if (ctrl_pressed && alt_pressed && param->value == KEY_K && param->down) {
        printk(KERN_INFO "Hotkey Terminator: Phát hiện tổ hợp phím Ctrl+Alt+K\n");
        queue_work(process_term_wq, &process_term_work);
        return NOTIFY_STOP;
    }
    
    return NOTIFY_OK;
}

/* Notifier block */
static struct notifier_block keyboard_nb = {
    .notifier_call = keyboard_notifier_callback
};

/* Hàm khởi tạo module */
static int __init hotkey_process_terminator_init(void) {
    int ret;
    
    printk(KERN_INFO "Hotkey Terminator: Đang khởi tạo module\n");
    
    /* Khởi tạo work queue */
    process_term_wq = create_singlethread_workqueue("process_term_wq");
    if (!process_term_wq) {
        printk(KERN_ERR "Hotkey Terminator: Không thể tạo workqueue\n");
        return -ENOMEM;
    }
    
    /* Khởi tạo work */
    INIT_WORK(&process_term_work, terminate_process);
    
    /* Đăng ký keyboard notifier */
    ret = register_keyboard_notifier(&keyboard_nb);
    if (ret) {
        printk(KERN_ERR "Hotkey Terminator: Không thể đăng ký keyboard notifier\n");
        destroy_workqueue(process_term_wq);
        return ret;
    }
    
    /* Tạo proc entry */
    proc_entry = proc_create(PROC_NAME, 0666, NULL, &proc_fops);
    if (!proc_entry) {
        printk(KERN_ERR "Hotkey Terminator: Không thể tạo proc entry\n");
        unregister_keyboard_notifier(&keyboard_nb);
        destroy_workqueue(process_term_wq);
        return -ENOMEM;
    }
    
    printk(KERN_INFO "Hotkey Terminator: Module đã khởi tạo thành công\n");
    printk(KERN_INFO "Hotkey Terminator: Cấu hình PID qua: echo [pid] > /proc/%s\n", PROC_NAME);
    
    return 0;
}

/* Hàm giải phóng module */
static void __exit hotkey_process_terminator_exit(void) {
    printk(KERN_INFO "Hotkey Terminator: Đang giải phóng module\n");
    
    /* Xóa proc entry */
    remove_proc_entry(PROC_NAME, NULL);
    
    /* Hủy đăng ký keyboard notifier */
    unregister_keyboard_notifier(&keyboard_nb);
    
    /* Hủy và làm sạch work queue */
    cancel_work_sync(&process_term_work);
    flush_workqueue(process_term_wq);
    destroy_workqueue(process_term_wq);
    
    printk(KERN_INFO "Hotkey Terminator: Module đã giải phóng thành công\n");
}

module_init(hotkey_process_terminator_init);
module_exit(hotkey_process_terminator_exit);
