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

