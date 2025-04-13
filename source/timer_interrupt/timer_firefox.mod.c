#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xc6f46339, "init_timer_key" },
	{ 0x15ba50a6, "jiffies" },
	{ 0xc38c83b8, "mod_timer" },
	{ 0x171679eb, "proc_create" },
	{ 0x122c3a7e, "_printk" },
	{ 0xdc0e4855, "timer_delete" },
	{ 0x82ee90dc, "timer_delete_sync" },
	{ 0x3c12dfe, "cancel_work_sync" },
	{ 0x6426ed5, "proc_remove" },
	{ 0xa7eedcc4, "call_usermodehelper" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x2d3385d3, "system_wq" },
	{ 0xc5b6f236, "queue_work_on" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0x4fc73462, "seq_read" },
	{ 0xbaf2d79, "seq_lseek" },
	{ 0x6b44cf81, "single_release" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xc565dd53, "single_open" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x34db050b, "_raw_spin_lock_irqsave" },
	{ 0x3128fbbb, "seq_printf" },
	{ 0xd35cce70, "_raw_spin_unlock_irqrestore" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x2273f01b, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "91CE5AF3488E974FFF358D9");
