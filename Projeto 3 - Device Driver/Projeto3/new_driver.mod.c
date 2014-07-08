#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

MODULE_INFO(intree, "Y");

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x89e12248, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xf7dadb04, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xbab10cdd, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x4eb45420, __VMLINUX_SYMBOL_STR(kmem_cache_alloc) },
	{ 0x3dbeeecc, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x99b5a21, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xe6d8fbeb, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x7b450bee, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x390a3fa1, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x74d1667e, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xb5419b40, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0xc671e369, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

