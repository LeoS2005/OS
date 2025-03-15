#include <kernel/gdt.h>
#include <kernel/tty.h>
#include <kernel/multiboot.h>
#include <stdint.h>
#include <stdio.h>

void parse_multiboot2_tags(uint32_t* multiboot_info_addr) {
    //printf("%s%d\n","size_of_multiboot_info_adr: ", *multiboot_info_addr);
    uint8_t* tag_ptr = (uint8_t*) multiboot_info_addr + 8;
    printf("--------------------------------------------------------------------------------");
    while (((struct multiboot_tag*) tag_ptr)->type != MULTIBOOT_TAG_TYPE_END) {
        //printf("%s%d\n", "tag_type: ", ((struct multiboot_tag*) tag_ptr)->type );
        switch (((struct multiboot_tag*) tag_ptr)->type) {
            case MULTIBOOT_TAG_TYPE_CMDLINE: 
            {
                printf("%d) ", MULTIBOOT_TAG_TYPE_CMDLINE);
                printf("MULTIBOOT_TAG_TYPE_CMDLINE: %s\n", ((struct multiboot_tag_with_data*) tag_ptr)->data);
                printf("--------------------------------------------------------------------------------");
                break;
            }
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            {
                printf("%d) ", MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME);
                printf("MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME: %s\n", ((struct multiboot_tag_with_data*) tag_ptr)->data);
                printf("--------------------------------------------------------------------------------");
                break;
            }
            case MULTIBOOT_TAG_TYPE_MODULE:
            {
                printf("%d) ", MULTIBOOT_TAG_TYPE_MODULE);
                printf("MULTIBOOT_TAG_TYPE_MODULE: %s\n", ((struct multiboot_tag_with_data*) tag_ptr)->data);
                printf("--------------------------------------------------------------------------------");
                break;
            }
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO: 
            {
                printf("%d) ", MULTIBOOT_TAG_TYPE_BASIC_MEMINFO);
                struct multiboot_tag_basic_meminfo* basic_meminfo_ptr = ((struct multiboot_tag_basic_meminfo*) tag_ptr);
                printf("MULTIBOOT_TAG_TYPE_BASIC_MEMINFO: %d %d\n", basic_meminfo_ptr->mem_lower, basic_meminfo_ptr->mem_upper);
                printf("--------------------------------------------------------------------------------");
                break;
            }
            case MULTIBOOT_TAG_TYPE_BOOTDEV: // последние два некорректны
            {
                printf("%d) ", MULTIBOOT_TAG_TYPE_BOOTDEV);
                struct multiboot_tag_bootdev* type_bootdev_ptr = ((struct multiboot_tag_bootdev*) tag_ptr);
                printf("MULTIBOOT_TAG_TYPE_BOOTDEV: %d %d %d\n",type_bootdev_ptr->biosdev, type_bootdev_ptr->slice, type_bootdev_ptr->part);
                printf("--------------------------------------------------------------------------------");
                break;
            }
            case MULTIBOOT_TAG_TYPE_MMAP:
            {
                printf("%d) ", MULTIBOOT_TAG_TYPE_MMAP);
                struct multiboot_tag_mmap* mmap_tag_ptr = ((struct multiboot_tag_mmap*) tag_ptr);
                printf("MULTIBOOT_TAG_TYPE_MMAP: %d, %d\n", mmap_tag_ptr->entry_size, mmap_tag_ptr->entry_version);
                printf("entries:\n");
                printf("addr: %d, len %d, type %d\n", mmap_tag_ptr->entries->addr, mmap_tag_ptr->entries->len, mmap_tag_ptr->entries->type);
                printf("--------------------------------------------------------------------------------");
                break;
            }
            case MULTIBOOT_TAG_TYPE_VBE: // не отображается
            {
                printf("%d) ", MULTIBOOT_TAG_TYPE_VBE);
                struct multiboot_tag_vbe* vbe_tag_ptr = ((struct multiboot_tag_vbe*) tag_ptr);
                printf("MULTIBOOT_TAG_TYPE_VBE\n");
                printf("vbe_mode: %d", vbe_tag_ptr->vbe_mode);
                printf("--------------------------------------------------------------------------------");
                break;
            }
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
            {
                printf("%d) ", MULTIBOOT_TAG_TYPE_FRAMEBUFFER);
                struct multiboot_tag_framebuffer* framebuffer_tag_ptr = ((struct multiboot_tag_frambuffer*) tag_ptr);
                printf("MULTIBOOT_TAG_TYPE_FRAMEBUFFER\n");
                printf("framebuffer_addr: %d\n", framebuffer_tag_ptr->framebuffer_addr);
                printf("framebuffer_type: %d\n", framebuffer_tag_ptr->framebuffer_type);
                printf("--------------------------------------------------------------------------------");
                break;
            }
            case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
            {
                printf("%d) ", MULTIBOOT_TAG_TYPE_ELF_SECTIONS);
                struct multiboot_tag_elf_sections* framebuffer_tag_ptr = ((struct multiboot_tag_elf_sections*) tag_ptr);
                printf("MULTIBOOT_TAG_TYPE_ELF_SECTIONS\n");
                printf("--------------------------------------------------------------------------------");
                break;
            }
            case MULTIBOOT_TAG_TYPE_APM:
            {
                printf("%d) ", MULTIBOOT_TAG_TYPE_APM);
                struct multiboot_tag_apm* apm_tag_ptr = ((struct multiboot_tag_apm*) tag_ptr);
                printf("MULTIBOOT_TAG_TYPE_APM\n");
                printf("apm_version: %d\n", apm_tag_ptr->version);
                printf("--------------------------------------------------------------------------------");
                break;
            }
            case MULTIBOOT_TAG_TYPE_EFI32: // не отображается 
            {
                printf("%d) ", MULTIBOOT_TAG_TYPE_EFI32);
                struct multiboot_tag_efi32* efi32_tag_ptr = ((struct multiboot_efi32*) tag_ptr);
                printf("MULTIBOOT_TAG_TYPE_EFI32\n");
                printf("efi32_structure_pointer: %d\n", efi32_tag_ptr->pointer);
                printf("--------------------------------------------------------------------------------");
                break;
            }
            case MULTIBOOT_TAG_TYPE_EFI64: // не отображается 
            {
                printf("%d) ", MULTIBOOT_TAG_TYPE_EFI64);
                struct multiboot_tag_efi64* efi64_tag_ptr = ((struct multiboot_efi64*) tag_ptr);
                printf("MULTIBOOT_TAG_TYPE_EFI64\n");
                printf("efi32_structure_pointer: %d\n", efi64_tag_ptr->pointer);
                printf("--------------------------------------------------------------------------------");
                break;
            }
            case MULTIBOOT_TAG_TYPE_SMBIOS: // не отображается
            {
                printf("%d) ", MULTIBOOT_TAG_TYPE_SMBIOS);
                struct multiboot_tag_smbios* smbios_tag_ptr = ((struct multiboot_smbios*) tag_ptr);
                printf("MULTIBOOT_TAG_TYPE_SMBIOS\n");
                printf("--------------------------------------------------------------------------------");
                break;
            }
        }



        tag_ptr += ((struct multiboot_tag*) tag_ptr)->size; 
        if ((uintptr_t)tag_ptr % 8 != 0) {
            tag_ptr += 8 - ((uintptr_t)tag_ptr % 8);
        }

    }
}


void kernel_main(uint32_t magic, uint32_t* multiboot_info_addr) {
    terminal_initialize();
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        printf("incorrect_magic\n");
        abort();
    }
    init_descriptor_tables();
    parse_multiboot2_tags(multiboot_info_addr);
}
