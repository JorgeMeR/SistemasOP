//
// descriptor_tables.c - Initialises the GDT and IDT, and defines the
// default ISR and IRQ handler.
// Based on code from Bran's kernel development tutorials.
// Rewritten for JamesM's kernel development tutorials.
//

#include "common.h"
#include "descriptor_tables.h"

// Lets us access our ASM functions from our C code.
extern void gdt_flush(u32int);

// Internal function prototypes.
static void init_gdt();
static void gdt_set_gate(s32int,u32int,u32int,u8int,u8int);

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

// Initialisation routine - zeroes all the interrupt service routines,
// initialises the GDT and IDT.
void init_descriptor_tables()
{
   // Initialise the global descriptor table.
   init_gdt();
}

static void init_gdt()
{
   gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
   gdt_ptr.base  = (u32int)&gdt_entries;

   gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
   gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
   gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
   gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
   gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

   gdt_flush((u32int)&gdt_ptr);
}

// Set the value of one GDT entry.
static void gdt_set_gate(s32int num, u32int base, u32int limit, u8int access, u8int gran)
{
   gdt_entries[num].base_low    = (base & 0xFFFF);
   gdt_entries[num].base_middle = (base >> 16) & 0xFF;
   gdt_entries[num].base_high   = (base >> 24) & 0xFF;

   gdt_entries[num].limit_low   = (limit & 0xFFFF);
   gdt_entries[num].granularity = (limit >> 16) & 0x0F;

   gdt_entries[num].granularity |= gran & 0xF0;
   gdt_entries[num].access      = access;
} 

[GLOBAL gdt_flush]    ; Allows the C code to call gdt_flush().

gdt_flush:
   mov eax, [esp+4]  ; Get the pointer to the GDT, passed as a parameter.
   lgdt [eax]        ; Load the new GDT pointer

   mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
   mov ds, ax        ; Load all data segment selectors
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov ss, ax
   jmp 0x08:.flush   ; 0x08 is the offset to our code segment: Far jump!
.flush:
   ret

extern void idt_flush(u32int);
...
static void init_idt();
static void idt_set_gate(u8int,u32int,u16int,u8int);
...
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;
...
void init_descriptor_tables()
{
  init_gdt();
  init_idt();
}
...
static void init_idt()
{
   idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
   idt_ptr.base  = (u32int)&idt_entries;

   memset(&idt_entries, 0, sizeof(idt_entry_t)*256);

   idt_set_gate( 0, (u32int)isr0 , 0x08, 0x8E);
   idt_set_gate( 1, (u32int)isr1 , 0x08, 0x8E);
   ...
   idt_set_gate(31, (u32int)isr32, 0x08, 0x8E);

   idt_flush((u32int)&idt_ptr);
}

static void idt_set_gate(u8int num, u32int base, u16int sel, u8int flags)
{
   idt_entries[num].base_lo = base & 0xFFFF;
   idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

   idt_entries[num].sel     = sel;
   idt_entries[num].always0 = 0;
   // We must uncomment the OR below when we get to using user-mode.
   // It sets the interrupt gate's privilege level to 3.
   idt_entries[num].flags   = flags /* | 0x60 */;
}

