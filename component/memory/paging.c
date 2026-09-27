// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------


#include "paging.h"

__attribute__((aligned(4096))) static uint32_t page_directory[1024];
__attribute__((aligned(4096))) static uint32_t ram_page_tables[32][1024];
__attribute__((aligned(4096))) static uint32_t vbe_page_tables[16][1024];

static uint32_t vbe_pt_idx = 0;

static void map_4mb_identity(uint32_t pde_idx, uint32_t *pt) {
    uint32_t phys_base = pde_idx * 0x400000;
    for (int i = 0; i < 1024; i++) {
        pt[i] = (phys_base + (i * 0x1000)) | 3;
    }
    page_directory[pde_idx] = ((uint32_t)pt) | 3;
}

static void map_vram_region(uint32_t base_addr) {
    uint32_t start_pde = base_addr >> 22;
    for (int k = 0; k < 4; k++) {
        uint32_t pde = start_pde + k;
        if (pde < 1024 && (page_directory[pde] & 1) == 0) {
            if (vbe_pt_idx < 16) {
                map_4mb_identity(pde, vbe_page_tables[vbe_pt_idx]);
                vbe_pt_idx++;
            }
        }
    }
}

void paging_init(uint32_t vbe_lfb_phys) {
    vbe_pt_idx = 0;

    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002;
    }

    for (int i = 0; i < 32; i++) {
        map_4mb_identity(i, ram_page_tables[i]);
    }

    if (vbe_lfb_phys != 0) {
        map_vram_region(vbe_lfb_phys);
    }
    
    map_vram_region(0xE0000000);
    map_vram_region(0xFD000000);
    map_vram_region(0xC0000000);

    asm volatile("mov %0, %%cr3" : : "r"(page_directory));

    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
}