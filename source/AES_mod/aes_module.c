#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/crypto.h>
#include <crypto/skcipher.h>
#include <linux/scatterlist.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define DEVICE_NAME "aes_dev"
#define AES_KEY_SIZE 16 // AES-128
#define AES_BLOCK_SIZE 16

// Định nghĩa các lệnh ioctl
#define AES_ENCRYPT 0
#define AES_DECRYPT 1

static struct crypto_skcipher *skcipher;

static int aes_dev_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int aes_dev_release(struct inode *inode, struct file *file)
{
    return 0;
}

static long aes_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    char key[AES_KEY_SIZE];
    char input[AES_BLOCK_SIZE], output[AES_BLOCK_SIZE];
    struct skcipher_request *req_local;
    struct scatterlist sg_in, sg_out;
    int ret;

    // Cấp phát skcipher_request cho mỗi lần gọi
    req_local = skcipher_request_alloc(skcipher, GFP_KERNEL);
    if (!req_local) {
        printk(KERN_ERR "Failed to allocate skcipher_request\n");
        return -ENOMEM;
    }

    // Lấy dữ liệu từ user space: key + input
    if (copy_from_user(key, (void *)arg, AES_KEY_SIZE)) {
        ret = -EFAULT;
        goto free_req;
    }
    if (copy_from_user(input, (void *)(arg + AES_KEY_SIZE), AES_BLOCK_SIZE)) {
        ret = -EFAULT;
        goto free_req;
    }

    // Thiết lập khóa
    if (crypto_skcipher_setkey(skcipher, key, AES_KEY_SIZE)) {
        printk(KERN_ERR "Failed to set key\n");
        ret = -EINVAL;
        goto free_req;
    }

    // Thiết lập scatterlist cho mã hóa/giải mã
    sg_init_one(&sg_in, input, AES_BLOCK_SIZE);
    sg_init_one(&sg_out, output, AES_BLOCK_SIZE);

    // Thiết lập yêu cầu (không cần IV với ECB)
    skcipher_request_set_crypt(req_local, &sg_in, &sg_out, AES_BLOCK_SIZE, NULL);

    // Thực hiện mã hóa hoặc giải mã dựa trên cmd
    if (cmd == AES_ENCRYPT) {
        ret = crypto_skcipher_encrypt(req_local);
        if (ret) {
            printk(KERN_ERR "Encryption failed: %d\n", ret);
            ret = -EIO;
            goto free_req;
        }
        printk(KERN_INFO "Encryption completed\n");
    } else if (cmd == AES_DECRYPT) {
        ret = crypto_skcipher_decrypt(req_local);
        if (ret) {
            printk(KERN_ERR "Decryption failed: %d\n", ret);
            ret = -EIO;
            goto free_req;
        }
        printk(KERN_INFO "Decryption completed\n");
    } else {
        printk(KERN_ERR "Invalid ioctl command: %u\n", cmd);
        ret = -EINVAL;
        goto free_req;
    }

    // Gửi kết quả về user space
    if (copy_to_user((void *)(arg + AES_KEY_SIZE + AES_BLOCK_SIZE), output, AES_BLOCK_SIZE)) {
        ret = -EFAULT;
        goto free_req;
    }

    // Giải phóng skcipher_request trước khi thoát
    skcipher_request_free(req_local);
    return 0;

free_req:
    skcipher_request_free(req_local);
    return ret;
}

static struct file_operations aes_fops = {
    .owner = THIS_MODULE,
    .open = aes_dev_open,
    .release = aes_dev_release,
    .unlocked_ioctl = aes_dev_ioctl,
};

static struct miscdevice aes_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &aes_fops,
};

static int __init aes_module_init(void)
{
    int ret;

    // Đăng ký thiết bị
    ret = misc_register(&aes_dev);
    if (ret) {
        printk(KERN_ERR "Failed to register misc device\n");
        return ret;
    }

    // Khởi tạo Crypto API với ECB mode
    skcipher = crypto_alloc_skcipher("ecb(aes)", 0, 0);
    if (IS_ERR(skcipher)) {
        printk(KERN_ERR "Failed to allocate skcipher: %ld\n", PTR_ERR(skcipher));
        misc_deregister(&aes_dev);
        return PTR_ERR(skcipher);
    }

    printk(KERN_INFO "AES module (ECB mode) loaded\n");
    return 0;
}

static void __exit aes_module_exit(void)
{
    crypto_free_skcipher(skcipher);
    misc_deregister(&aes_dev);
    printk(KERN_INFO "AES module unloaded\n");
}

module_init(aes_module_init);
module_exit(aes_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ledat");
MODULE_DESCRIPTION("AES encryption/decryption module with ECB mode");
