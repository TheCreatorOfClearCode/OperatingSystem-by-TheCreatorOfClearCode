CC = gcc
CFLAGS = -m32 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -Wall -Wextra -Wno-sign-compare -Wno-incompatible-pointer-types
ASM = nasm
LD = ld

TARGET = OperatingSystem.bin
ISO = OperatingSystem-v1.3.2-by-TheCreatorOfClearCode.iso

GRUB_MKRESCUE = $(shell if [ -f /etc/fedora-release ] || [ -f /etc/redhat-release ] || [ -f /etc/centos-release ]; then echo "grub2-mkrescue"; else echo "grub-mkrescue"; fi)
GRUB_PATH = /usr/lib/grub/i386-pc

ASM_SOURCES := $(shell find . -name "*.s" -not -path "./iso/*" -not -path "./test/*" -not -path "./tests/*")
C_SOURCES := $(shell find . -name "*.c" -not -path "./iso/*" -not -path "./test/*" -not -path "./tests/*")

ASM_OBJECTS := $(ASM_SOURCES:.s=.o)
C_OBJECTS := $(C_SOURCES:.c=.o)

OBJS := $(ASM_OBJECTS) $(C_OBJECTS)

OBJS := $(sort $(filter-out ./test/%, $(filter-out ./tests/%, $(OBJS))))

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
	@echo "Всего скомпилировано файлов: $(COMPILED_FILES)"
	@echo "========================================"

%.o: %.s
	@printf "[ASM]  %-40s -> %s\n" "$<" "$@"
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
	@echo "[OK]   Ядро собрано: $(TARGET) (`stat -c%s $(TARGET) 2>/dev/null || echo "?"` байт)"
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
	$(Q)$(GRUB_MKRESCUE) -o $(ISO) iso -d $(GRUB_PATH) 2>/dev/null || echo "[WARN] Не удалось создать ISO, проверьте установку grub"
	@if [ -f "$(ISO)" ]; then \
		echo "[OK]   ISO создан: $(ISO) (`stat -c%s $(ISO)` байт)"; \
	else \
		echo "[ERROR] Не удалось создать ISO образ"; \
	fi
	@echo "========================================"

clean:
	@echo "========================================"
	@echo "[CLEAN] Удаление временных файлов..."
	$(Q)find . -name "*.o" -type f -delete 2>/dev/null || true
	$(Q)rm -f $(TARGET) $(ISO) 2>/dev/null || true
	$(Q)rm -rf iso 2>/dev/null || true
	@echo "[OK]   Очистка завершена"
	@echo "========================================"

kernel: $(TARGET)
	@echo "[OK]   Ядро собрано (без создания ISO)"

iso: $(ISO)
	@echo "[OK]   ISO создан из существующего ядра"

sources:
	@echo "Найдены исходные файлы:"
	@echo "========================================"
	@echo "Ассемблерные файлы (.s):"
	@for file in $(ASM_SOURCES); do echo "  $$file"; done
	@echo ""
	@echo "C файлы (.c):"
	@for file in $(C_SOURCES); do echo "  $$file"; done
	@echo "========================================"
	@echo "Всего: $(words $(ASM_SOURCES)) .s файлов, $(words $(C_SOURCES)) .c файлов"

tree:
	@echo "Структура проекта:"
	@find . -type f -name "*.s" -o -name "*.c" -o -name "*.h" | sort | grep -v "./iso" | grep -v "./test"

deps:
	@echo "Проверка зависимостей..."
	@which $(CC) >/dev/null && echo "[OK]   Компилятор C: $(CC)" || echo "[ERROR] Компилятор C не найден"
	@which $(ASM) >/dev/null && echo "[OK]   Ассемблер: $(ASM)" || echo "[ERROR] Ассемблер не найден"
	@which $(LD) >/dev/null && echo "[OK]   Линковщик: $(LD)" || echo "[ERROR] Линковщик не найден"
	@which $(GRUB_MKRESCUE) >/dev/null 2>&1 && echo "[OK]   GRUB: $(GRUB_MKRESCUE)" || echo "[WARN] GRUB не найден, создание ISO будет недоступно"

help:
	@echo "========================================"
	@echo "Использование:"
	@echo "  make           - полная сборка проекта"
	@echo "  make clean     - очистить все сгенерированные файлы"
	@echo "  make kernel    - собрать только ядро (.bin)"
	@echo "  make iso       - создать ISO из существующего ядра"
	@echo "  make sources   - показать все исходные файлы"
	@echo "  make tree      - показать структуру проекта"
	@echo "  make deps      - проверить зависимости"
	@echo "  make V=1       - подробный вывод команд"
	@echo "  make test      - запустить в QEMU (если установлен)"
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
	@echo "Автоматический поиск файлов включен"
	@echo "========================================"

test:
	@if command -v qemu-system-i386 >/dev/null; then \
		if [ -f "$(ISO)" ]; then \
			echo "[TEST] Запуск в QEMU..."; \
			qemu-system-i386 -cdrom $(ISO); \
		else \
			echo "[ERROR] Сначала соберите ISO: make"; \
		fi; \
	else \
		echo "[ERROR] QEMU не установлен"; \
	fi

.PHONY: all clean kernel iso sources tree deps help test

.DEFAULT_GOAL := all
