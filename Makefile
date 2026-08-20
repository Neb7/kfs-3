NAME		:= kernel.bin
ISO			:= kfs01.iso

CC			:= gcc
ASM			:= nasm
LD			:= ld

# Flags gcc: 32-bit, sans stdlib, sans dépendances hôte
INCLUDE		:= -I kernel/includes -I kernel
CFLAGS		:= -m32 -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Wall -Wextra $(INCLUDE)

# Flag nasm: format ELF 32-bit (obligatory for the linker)
ASMFLAGS 	:= -f elf32

LDFLAGS		:= -m elf_i386 -T linker.ld


SRCS_DIR	= kernel/

SRCS_DIR_C	= srcs/
SRC			= idt.c \
			  kernel.c \
			  vga.c \
			  kprintf.c \
			  kprintk.c \
			  pic.c \
			  gdt.c

SRCS_DIR_LIBFT	= libft/
SRC_LIBFT	= ft_atoi.c \
			  ft_bzero.c \
			  ft_isalnum.c \
			  ft_isalpha.c \
			  ft_isascii.c \
			  ft_isdigit.c \
			  ft_isprint.c \
			  ft_memchr.c \
			  ft_memcmp.c \
			  ft_memcpy.c \
			  ft_memmove.c \
			  ft_memset.c \
			  ft_strchr.c \
			  ft_striteri.c \
			  ft_strlcat.c \
			  ft_strlcpy.c \
			  ft_strlen.c \
			  ft_strncmp.c \
			  ft_strnstr.c \
			  ft_strrchr.c \
			  ft_tolower.c \
			  ft_toupper.c

SRC_A		= boot.asm

SRC_C		= $(addprefix $(SRCS_DIR_C), $(SRC))
SRC_C		+= $(addprefix $(SRCS_DIR_LIBFT), $(SRC_LIBFT))

SRCS_C		= $(addprefix $(SRCS_DIR), $(SRC_C))

SRC_ASM		= $(addprefix $(SRCS_DIR), $(SRC_A))


OBJS_DIR	:= .objs/
OBJ_C		:= $(addprefix ${OBJS_DIR}, ${SRC_C:.c=.o})
OBJ_ASM		:= $(addprefix ${OBJS_DIR}, ${SRC_A:.asm=.o})
OBJ			:= $(OBJ_C) $(OBJ_ASM)	# boot.o has to be first

# RULES

all: $(NAME)

# link all .o -> kernel.bin wiht custom linker
$(NAME): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

# compile .c -> .o
${OBJS_DIR}%.o: ${SRCS_DIR}%.c | ${OBJS_DIR}
				@mkdir -p $(dir $@)
				@${CC} ${CFLAGS} -c $< -o $@

# compile .asm -> .o
${OBJS_DIR}%.o: ${SRCS_DIR}%.asm | ${OBJS_DIR}
				@mkdir -p $(dir $@)
				@${ASM} ${ASMFLAGS} $< -o $@

# create .ISO bootable with GRUB
iso: $(NAME)
	cp $(NAME) iso/boot/
	grub-mkrescue -o $(ISO) iso/

# launch in QEMU / KVM
run: iso
	qemu-system-i386 -cdrom $(ISO) -no-reboot

clean: 
	rm -rf $(OBJS_DIR)
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME) $(ISO) iso/boot/$(NAME)

re: fclean all

$(OBJS_DIR):
	mkdir -p $@

.PHONY: all iso run clean fclean re
