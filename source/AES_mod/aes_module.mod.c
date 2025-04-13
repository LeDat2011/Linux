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
	{ 0xe1684bdb, "misc_deregister" },
	{ 0x51ed29c1, "crypto_destroy_tfm" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xd0760fc0, "kfree_sensitive" },
	{ 0xeef598b6, "crypto_skcipher_setkey" },
	{ 0x43babd19, "sg_init_one" },
	{ 0xa6d3cdb6, "crypto_skcipher_decrypt" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xcd5585e4, "crypto_skcipher_encrypt" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x62376bf8, "misc_register" },
	{ 0xf3b7aee3, "crypto_alloc_skcipher" },
	{ 0x122c3a7e, "_printk" },
	{ 0x2273f01b, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "76520AC2B90E38E4F21D09F");
