# ============================================================
# ⚙️ SynapseOS v2.5 Advanced Makefile
# - Auto-detects new kernel files
# - Supports boot_splash, mini_printf, system_info, GUI
# - Colorized output + safe build flow
# ============================================================

BUILD_DIR := build
ISO_DIR := isodir/boot/grub
ISO := synapseos.iso
KERNEL_BIN := $(BUILD_DIR)/kernel.bin
PSF := kernel/font.psf
FONT_OBJ := $(BUILD_DIR)/font_psf.o

# ============================================================
# 🧠 Toolchain Configuration
# ============================================================

CC := i686-elf-gcc
LD := i686-elf-ld
NASM := nasm
OBJCPY := objcopy

CFLAGS := -ffreestanding -O2 -Wall -Wextra -I kernel -m32
ASFLAGS := -f elf32
LDFLAGS := -m elf_i386 -T linker.ld -nostdlib

# ============================================================
# 🧩 Source Auto-Detection
# ============================================================

C_SRCS := $(wildcard kernel/*.c)
ASM_SRCS := $(wildcard kernel/*.asm)
BOOT_SRC := boot/boot.asm

OBJS := $(patsubst kernel/%.c,$(BUILD_DIR)/%.o,$(C_SRCS))
ASM_OBJS := $(patsubst kernel/%.asm,$(BUILD_DIR)/%.o,$(ASM_SRCS))
BOOT_OBJ := $(BUILD_DIR)/boot.o

# ============================================================
# 🎨 Output Formatting
# ============================================================

YELLOW=\033[1;33m
GREEN=\033[1;32m
RED=\033[1;31m
BLUE=\033[1;34m
RESET=\033[0m

OK = @echo "$(GREEN)[ OK ]$(RESET)"
STEP = @echo "$(BLUE)--->$(RESET)"
WARN = @echo "$(YELLOW)[ WARN ]$(RESET)"
ERR = @echo "$(RED)[ ERROR ]$(RESET)"

# ============================================================
# 🏗️ Build Targets
# ============================================================

.PHONY: all clean run debug rebuild iso info

all: $(ISO)

# ------------------------------------------------------------
# Build Directory
# ------------------------------------------------------------
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# ------------------------------------------------------------
# Assemble boot.asm (Multiboot2 loader)
# ------------------------------------------------------------
$(BOOT_OBJ): $(BOOT_SRC) | $(BUILD_DIR)
	$(STEP) "Assembling bootloader..."
	$(NASM) $(ASFLAGS) $< -o $@
	$(OK)

# ------------------------------------------------------------
# Assemble kernel .asm files
# ------------------------------------------------------------
$(BUILD_DIR)/%.o: kernel/%.asm | $(BUILD_DIR)
	$(STEP) "Assembling $<..."
	$(NASM) $(ASFLAGS) $< -o $@
	$(OK)

# ------------------------------------------------------------
# Compile all kernel C files
# ------------------------------------------------------------
$(BUILD_DIR)/%.o: kernel/%.c | $(BUILD_DIR)
	$(STEP) "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@
	$(OK)

# ------------------------------------------------------------
# Embed PSF font
# ------------------------------------------------------------
$(FONT_OBJ): $(PSF) | $(BUILD_DIR)
	$(STEP) "Embedding PSF font..."
	$(OBJCPY) -I binary -O elf32-i386 -B i386 $(PSF) $(FONT_OBJ)
	$(OK)

# ------------------------------------------------------------
# Link the final kernel
# ------------------------------------------------------------
$(KERNEL_BIN): $(BOOT_OBJ) $(ASM_OBJS) $(OBJS) $(FONT_OBJ)
	$(STEP) "Linking kernel..."
	$(LD) $(LDFLAGS) -o $@ $^
	@echo "$(GREEN)✅ Kernel linked successfully!$(RESET)"
	@echo "$(YELLOW)→ Output: $(KERNEL_BIN)$(RESET)"

# ------------------------------------------------------------
# ISO Creation
# ------------------------------------------------------------
$(ISO): $(KERNEL_BIN)
	$(STEP) "Creating ISO image..."
	mkdir -p $(ISO_DIR)
	cp $(KERNEL_BIN) isodir/boot/kernel.bin
	printf 'set timeout=1\nset default=0\nmenuentry "SynapseOS" {\n  multiboot2 /boot/kernel.bin\n  boot\n}\n' > $(ISO_DIR)/grub.cfg
	grub-mkrescue -o $(ISO) isodir >/dev/null 2>&1
	@echo "$(GREEN)💿 ISO created: $(ISO)$(RESET)"

# ============================================================
# 🖥️ Run / Debug Commands
# ============================================================

run: $(ISO)
	$(STEP) "Booting SynapseOS in QEMU..."
	qemu-system-i386 -cdrom $(ISO) -m 512M -vga std -boot d
	$(OK)

debug: $(ISO)
	$(STEP) "Launching QEMU (debug mode)..."
	qemu-system-i386 -cdrom $(ISO) -s -S -m 512M -vga std

# ============================================================
# 🧹 Maintenance
# ============================================================

clean:
	$(STEP) "Cleaning build files..."
	rm -rf $(BUILD_DIR) isodir $(ISO)
	$(OK)
	@echo "$(GREEN)🧹 Clean complete.$(RESET)"

rebuild: clean all

info:
	@echo "$(BLUE)📦 SynapseOS Build Information$(RESET)"
	@echo "Detected C files: $(words $(C_SRCS))"
	@echo "Detected ASM files: $(words $(ASM_SRCS))"
	@echo "Output binary: $(KERNEL_BIN)"
