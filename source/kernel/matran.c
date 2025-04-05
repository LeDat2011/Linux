#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>

int factorial(int n); 
int matmul(int p, int q, int s, int *a, int *b, int *c);
int matadd(int p, int q, int *a, int *b);
int primeBetween(int m, int n);
int maxandminmatrix(int p, int q, int *a);
int numdivisibleinmatrix(int p, int q, int *a, int s);
int primeofmat(int p, int q, int *a);
int prime(int n);
static int choice = 0;
static int p = 3, q = 3, s = 3, d;
static int a[2500], b[2500], c[2500];
static char message[2500] ;

module_param(choice, int, S_IRUGO);
module_param(d, int, S_IRUGO);
module_param(p, int, S_IRUGO);
module_param(q, int, S_IRUGO);
module_param(s, int, S_IRUGO);
module_param_array(a, int, NULL, S_IRUGO);
module_param_array(b, int, NULL, S_IRUGO);
module_param_array(c, int, NULL, S_IRUGO);

static int p02_init(void)
{
        switch (choice)
        {
        case 1:
                {
                printk(KERN_ALERT "%d! = %d\n", d, factorial(d));
                break;
                }

        case 2:
                {
                matadd(p, q, a, b);
                break;
                }

        case 3:
                {
                matmul(p, q, s, a, b, c);
                break;
                }        
        
        case 4: 
                {
                primeBetween(p, s);
                break;
                }

        case 5:
                {
                maxandminmatrix(p, q, a);
                break;
                }

        case 6:
                {
                numdivisibleinmatrix(p, q, a, s);
                break;
                }
        case 7:
                {
                primeofmat(p, q, a);
                break;
                }
        default:
                break;        
        }
        return 0;
}

int factorial(int n)
{
        if (n <= 0) {
                printk(KERN_INFO "\n=== Factorial Calculation ===\n");
                printk(KERN_INFO "Error: Cannot calculate factorial of %d\n", n);
                return 0;
        }
        if (n == 1) return 1;
        
        int gt = 2;
        int i;
        printk(KERN_INFO "\n=== Factorial Calculation ===\n");
        printk(KERN_INFO "Computing %d! = ", n);
        printk(KERN_INFO "1 × 2");
        for(i=3; i<=n; i++) {
                printk(KERN_INFO " × %d", i);
                gt *= i;
        }
        printk(KERN_INFO " = %d\n", gt);
        return gt;
}

// Sửa hàm print_matrix để hiển thị đẹp hơn cho ma trận 3x3
void print_matrix(int *matrix, int rows, int cols, const char *name) {
    int i, j;
    char buf[100];
    int pos = 0;
    
    printk(KERN_INFO "%s:\n", name);
    for (i = 0; i < rows; i++) {
        pos = 0;
        pos += sprintf(buf + pos, "| ");
        for (j = 0; j < cols; j++) {
            pos += sprintf(buf + pos, "%4d ", *(matrix + (cols * i) + j));
        }
        pos += sprintf(buf + pos, "|");
        printk(KERN_INFO "%s\n", buf);
    }
}

// Sửa hàm matadd
int matadd(int p, int q, int *a, int *b) {
    int i, j;
    for (i = 0; i < p; i++) {
        for (j = 0; j < q; j++) {
            *(c + j + i * q) = *(a + j + i * q) + *(b + j + i * q);
        }
    }
    
    printk(KERN_INFO "Matrix Addition\n");
    print_matrix(a, p, q, "Matrix A");
    print_matrix(b, p, q, "Matrix B");
    print_matrix(c, p, q, "Result");
    return 0;
}

// Sửa hàm matmul
int matmul(int p, int q, int s, int *a, int *b, int *c) {
    int i, j, l;
    // Khởi tạo mảng c về 0
    for(i = 0; i < p*s; i++) {
        *(c + i) = 0;
    }
    
    for (i=0;i<p;i++){
        for (j=0;j<s;j++){
            for (l=0;l<q;l++){
                *(c + (s * i) + j) += *(a + (q * i) + l) * *(b + (l * s) + j);
            }
        }
    }
    
    printk(KERN_INFO "Matrix Multiplication\n");
    print_matrix(a, p, q, "Matrix A");
    print_matrix(b, q, s, "Matrix B");
    print_matrix(c, p, s, "Result");
    return 0;
}

int primeBetween(int num1, int num2) {
        int i;
        for (i = num1 + 1; i < num2; ++i) {
                if(prime(i)){
                        printk(KERN_ALERT "%d\n",i);
                }
        }
        return 0;
}

// Sửa hàm maxandminmatrix
int maxandminmatrix(int p, int q, int *a) {
        int maxValue = a[0];
        int minValue = a[0];
        int i, j;
        
        printk(KERN_INFO "\n=== Matrix Analysis ===\n");
        print_matrix(a, p, q, "Input");
        
        for (i = 0; i < p; i++) {
                for (j = 0; j < q; j++) {
                        if (a[i * q + j] > maxValue) maxValue = a[i * q + j];
                        if (a[i * q + j] < minValue) minValue = a[i * q + j];
                }
        }
        
        printk(KERN_INFO "+------------------\n");
        printk(KERN_INFO "| Maximum value: %-4d |\n", maxValue);
        printk(KERN_INFO "| Minimum value: %-4d |\n", minValue);
        printk(KERN_INFO "+------------------\n");
        return 0;
}

// Sửa hàm numdivisibleinmatrix
int numdivisibleinmatrix(int p, int q, int *a, int s) {
        int i, j, count = 0;
        
        printk(KERN_INFO "\n=== Divisibility Analysis ===\n");
        print_matrix(a, p, q, "Input");
        
        for (i = 0; i < p; i++) {
                for (j = 0; j < q; j++) {
                        if (a[i * q + j] % s == 0) count++;
                }
        }
        
        printk(KERN_INFO "+-------------------------\n");
        printk(KERN_INFO "| Numbers divisible by %d: %d |\n", s, count);
        printk(KERN_INFO "+-------------------------\n");
        return 0;
}

// Sửa hàm primeofmat
int primeofmat(int p, int q, int *a) {
        int i, j, count = 0;
        
        printk(KERN_INFO "\n=== Prime Numbers Analysis ===\n");
        print_matrix(a, p, q, "Input");
        
        printk(KERN_INFO "\nPrime numbers found: ");
        for (i = 0; i < p; i++) {
                for (j = 0; j < q; j++) {
                        if (prime(a[j + q * i])) {
                                count++;
                                printk(KERN_INFO "%d ", a[j + q * i]);
                        }
                }
        }
        printk(KERN_INFO "\n");
        printk(KERN_INFO "+-------------------------\n");
        printk(KERN_INFO "| Total prime numbers: %d |\n", count);
        printk(KERN_INFO "+-------------------------\n");
        return 0;
}

int prime(int n) {
        if(n<=1)
               return 0;
        int i;
        for (i = 2; i <= n/2; i++) {
                if (n % i == 0) {
                        return 0;
                }
        }
        return 1;
}

static void p02_exit(void)
{
        printk(KERN_ALERT "Exit\n");
}

module_init(p02_init);
module_exit(p02_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("feizzvan");

