#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

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
	{ 0x4cf819e6, "module_layout" },
	{ 0x7aa1756e, "kvfree" },
	{ 0xb3f0559, "class_destroy" },
	{ 0x2c5fa45b, "device_destroy" },
	{ 0x599fb41c, "kvmalloc_node" },
	{ 0x7ac22d2b, "device_create" },
	{ 0x52ea150d, "__class_create" },
	{ 0x11eb121f, "cdev_del" },
	{ 0x646eac6, "cdev_add" },
	{ 0xd90cd7e6, "cdev_init" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x4e517eb9, "cdev_alloc" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xf906c78f, "remap_pfn_range" },
	{ 0x71a152b9, "module_refcount" },
	{ 0x93ffbc00, "current_task" },
	{ 0x92997ed8, "_printk" },
	{ 0x4c9d28b0, "phys_base" },
	{ 0x97651e6c, "vmemmap_base" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "EB7F77C1D240A8C368B3225");
