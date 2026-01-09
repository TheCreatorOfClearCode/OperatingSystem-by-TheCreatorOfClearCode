CC = gcc
CFLAGS = -m32 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -Wall -Wextra -Wno-sign-compare -Wno-incompatible-pointer-types
ASM = nasm
LD = ld

TARGET = OperatingSystem.bin
ISO = OperatingSystem-v1.3.1-by-TheCreatorOfClearCode.iso

MAIN = system/kernel/main
KERNEL_MODULES = system/kernel
DRIVERS = system/kernel/drivers
INIT = system/init

GRUB_MKRESCUE = $(shell if [ -f /etc/fedora-release ] || [ -f /etc/redhat-release ] || [ -f /etc/centos-release ]; then echo "grub2-mkrescue"; else echo "grub-mkrescue"; fi)
GRUB_PATH = /usr/lib/grub/i386-pc

OBJS = \
    boot/boot.o \
    \
    $(MAIN)/main.o \
    \
    system/about_system/SMBIOS/smbios.o \
    system/about_system/cpu_info/cpu_info.o \
    \
    $(DRIVERS)/IO/io.o \
    $(DRIVERS)/IO/config/ports_config.o \
    $(DRIVERS)/VGA/vga.o \
    $(DRIVERS)/keyboard/keyboard.o \
    $(DRIVERS)/RTC/rtc.o \
    \
    $(INIT)/init_system/init_system.o \
    $(INIT)/configs/drivers_config/drivers_config.o \
    $(INIT)/configs/plugins_config/plugins_config.o \
    \
    $(KERNEL_MODULES)/power_controller/poweroff/poweroff.o \
    $(KERNEL_MODULES)/power_controller/reboot/reboot.o \
    \
    plugins/shell/shell/shell.o \
    plugins/shell/command_interpreter/command_interpreter.o \
    plugins/power_options.o \
    plugins/about.o \
    plugins/help.o \
    plugins/datetime.o \
    plugins/installed.o \
    \
    libraries/string/string.o

ifeq ($(V),1)
    Q :=
    E := @:
else
    Q := @
    E := @echo
endif

TOTAL_FILES := $(words $(OBJS))
COMPILED_FILES := 0

define inc_counter
$(eval COMPILED_FILES=$(shell echo $$(($(COMPILED_FILES)+1))))
endef

all: $(ISO)
	@echo "========================================"
	@echo "Сборка успешно завершена!"
	@echo "Целевой файл: $(ISO)"
	@echo "========================================"

boot/boot.o: boot/boot.s
	@echo "[ASM]  $< -> $@"
	$(Q)$(ASM) -f elf32 $< -o $@
	$(call inc_counter)

%.o: %.c
	@printf "[CC]   %-40s -> %s\n" "$<" "$@"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@
	$(call inc_counter)

$(TARGET): $(OBJS)
	@echo "========================================"
	@echo "[LD]   Линковка $(words $^) файлов -> $(TARGET)"
	$(Q)$(LD) -m elf_i386 -T linker.ld $^ -o $(TARGET)
	@echo "[OK]   Ядро собрано: $(TARGET) (`stat -c%s $(TARGET)` байт)"
	@echo "========================================"

$(ISO): $(TARGET)
	@echo "[ISO]  Создание ISO образа..."
	$(Q)mkdir -p iso/boot/grub 2>/dev/null || true
	$(Q)cp $(TARGET) iso/boot/$(TARGET)
	@if [ ! -f boot/grub/grub.cfg ]; then \
		echo "[ERROR] boot/grub/grub.cfg не найден!"; \
		exit 1; \
	fi
	$(Q)cp boot/grub/grub.cfg iso/boot/grub/
	@echo "[INFO] Используется $(GRUB_MKRESCUE) с путем $(GRUB_PATH)"
	$(Q)$(GRUB_MKRESCUE) -o $(ISO) iso -d $(GRUB_PATH) 2>/dev/null
	@echo "[OK]   ISO создан: $(ISO) (`stat -c%s $(ISO)` байт)"
	@echo "========================================"

clean:
	@echo "========================================"
	@echo "[CLEAN] Удаление временных файлов..."
	$(Q)rm -f $(OBJS) $(TARGET) $(ISO) 2>/dev/null || true
	$(Q)rm -rf iso 2>/dev/null || true
	@echo "[OK]   Очистка завершена"
	@echo "========================================"

kernel: $(TARGET)
	@echo "[OK]   Ядро собрано (без создания ISO)"

iso: $(ISO)
	@echo "[OK]   ISO создан из существующего ядра"

sources:
	@echo "Исходные файлы для сборки:"
	@echo "========================================"
	@for file in $(OBJS:.o=.c) $(OBJS:.o=.s); do \
		if [ -f $$file ]; then \
			echo "  $$file"; \
		fi; \
	done
	@echo "========================================"
	@echo "Всего файлов: $(TOTAL_FILES)"

help:
	@echo "========================================"
	@echo "Использование:"
	@echo "  make           - полная сборка проекта"
	@echo "  make clean     - очистить все сгенерированные файлы"
	@echo "  make kernel    - собрать только ядро (.bin)"
	@echo "  make iso       - создать ISO из существующего ядра"
	@echo "  make sources   - показать все исходные файлы"
	@echo "  make V=1       - подробный вывод команд"
	@echo "========================================"
	@echo ""
	@echo "========================================"
	@echo "Цели сборки:"
	@echo "  $(TARGET) - ядро ОС"
	@echo "  $(ISO) - загрузочный ISO образ"
	@echo "========================================"
	@echo ""
	@echo "========================================"
	@echo "Компилятор: $(CC) $(CFLAGS)"
	@echo "Ассемблер: $(ASM)"
	@echo "Линковщик: $(LD)"
	@echo "========================================"

test:
	qemu-system-i386 -cdrom $(ISO)

.PHONY: all clean kernel iso sources help

.DEFAULT_GOAL := all
