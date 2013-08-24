#ifndef EXEC_ELF_H
#define EXEC_ELF_H

typedef struct elf_object {
	void (*entry_point)(void);
	void *base_address;
	unsigned order;
} elf_object;

elf_object *load_elf(void *start);

#endif /* EXEC_ELF_H */
