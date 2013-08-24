#include <exec_elf.h>

#include <alloc.h>
#include <page_alloc.h>

/* e_ident */
#define EI_MAG0    0
#define EI_MAG1    1
#define EI_MAG2    2
#define EI_MAG3    3
#define EI_CLASS   4
#define EI_DATA    5
#define EI_VERSION 6
#define EI_PAD     7
#define EI_NIDENT 16

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define ELFCLASSNONE 0
#define ELFCLASS32   1
#define ELFCLASS64   2

#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

/* e_type */
#define ET_NONE 0
#define ET_REL  1
#define ET_EXEC 2
#define ET_DYN  3
#define ET_CORE 4
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff

/* e_machine */
#define EM_ARM 40

/* e_version */
#define EV_NONE    0
#define EV_CURRENT 1

typedef struct Elf32_Ehdr {
	unsigned char  e_ident[EI_NIDENT];
	unsigned short e_type;
	unsigned short e_machine;
	unsigned       e_version;
	void *         e_entry;
	unsigned       e_phoff;
	unsigned       e_shoff;
	unsigned       e_flags;
	unsigned short e_ehsize;
	unsigned short e_phentsize;
	unsigned short e_phnum;
	unsigned short e_shentsize;
	unsigned short e_shnum;
	unsigned short e_shstrndx;
} Elf32_Ehdr;

/* p_type */
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7fffffff

typedef struct Elf32_Phdr {
	unsigned       p_type;
	unsigned       p_offset;
	void *         p_vaddr;
	void *         p_paddr;
	unsigned       p_filesz;
	unsigned       p_memsz;
	unsigned       p_flags;
	unsigned       p_align;
} Elf32_Phdr;

static inline int check_magic(unsigned char ident[]) {
	return ident[EI_MAG0] == ELFMAG0 &&
		ident[EI_MAG1] == ELFMAG1 &&
		ident[EI_MAG2] == ELFMAG2 &&
		ident[EI_MAG3] == ELFMAG3;
}

static inline void *on_offset(void *base, unsigned offset) {
	return (void *)((unsigned)base + offset);
}

static inline unsigned min(unsigned a, unsigned b) {
	return a < b ? a : b;
}

static inline unsigned max(unsigned a, unsigned b) {
	return a > b ? a : b;
}

elf_object *load_elf(void *start) {
	Elf32_Ehdr *elf_header = start;
	if (!check_magic(elf_header->e_ident)) {
		return 0;
	}

	Elf32_Phdr *program_header = on_offset(start, elf_header->e_phoff);
	unsigned max_image_addr = 0x00000000;
	unsigned min_image_addr = 0xffffffff;
	for (int i = 0; i < elf_header->e_phnum; ++i) {
		if (program_header->p_type != PT_LOAD) {
			program_header = on_offset(program_header,
					elf_header->e_phentsize);
			continue;
		}

		min_image_addr = min(min_image_addr, (unsigned)program_header->p_vaddr);
		max_image_addr = max(max_image_addr, (unsigned)program_header->p_vaddr +
				program_header->p_memsz);

		program_header = on_offset(program_header,
				elf_header->e_phentsize);
	}

	unsigned min_start_page = min_image_addr/PAGE_SIZE;
	unsigned max_end_page = (max_image_addr - 1)/PAGE_SIZE + 1;
	unsigned page_count = max_end_page - min_start_page + 1;
	unsigned start_empty = min_start_page * PAGE_SIZE;

	unsigned tmp = page_count >> 1;
	unsigned order = 0;
	while (tmp) {
		tmp >>= 1;
		++order;
	}

	elf_object *obj = malloc(sizeof(*obj));
	obj->base_address = page_alloc(order);
	obj->entry_point = (unsigned)elf_header->e_entry - start_empty +
		(unsigned)obj->base_address;
	obj->order = order;

	program_header = on_offset(start, elf_header->e_phoff);
	for (int i = 0; i < elf_header->e_phnum; ++i) {
		if (program_header->p_type != PT_LOAD) {
			program_header = on_offset(program_header,
					elf_header->e_phentsize);
			continue;
		}

		unsigned from_size = program_header->p_filesz;
		unsigned to_size = program_header->p_memsz;

		char *from = on_offset(start, program_header->p_offset);
		char *to = (unsigned)obj->base_address - start_empty +
			(unsigned)program_header->p_vaddr;

		for (; from_size; --from_size, --to_size) {
			*to = *from;
			++from; ++to;
		}
		for (; to_size; --to_size) {
			*to = 0;
			++to;
		}

		program_header = on_offset(program_header,
				elf_header->e_phentsize);
	}

	return obj;
}
