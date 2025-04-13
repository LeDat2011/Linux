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
	{ 0x8c03d20c, "destroy_workqueue" },
	{ 0x9ed554b3, "unregister_keyboard_notifier" },
	{ 0xc565dd53, "single_open" },
	{ 0x3128fbbb, "seq_printf" },
	{ 0xc5b6f236, "queue_work_on" },
	{ 0x5564d286, "find_vpid" },
	{ 0x828a571a, "pid_task" },
	{ 0xb1b9b07f, "send_sig_info" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xabc89c1e, "remove_proc_entry" },
	{ 0x3c12dfe, "cancel_work_sync" },
	{ 0x9166fc03, "__flush_workqueue" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x8c8569cb, "kstrtoint" },
	{ 0x4fc73462, "seq_read" },
	{ 0xbaf2d79, "seq_lseek" },
	{ 0x6b44cf81, "single_release" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0x49cd25ed, "alloc_workqueue" },
	{ 0x96554810, "register_keyboard_notifier" },
	{ 0x171679eb, "proc_create" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x2273f01b, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "A5334276E8DA11635D39E1D");
