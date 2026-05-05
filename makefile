# Deteksi OS
CC := gcc
ifeq ($(OS),Windows_NT)
    # Konfigurasi Windows
    TARGET_OS := WINDOWS
    RM := del /Q /F
    FIX_PATH = $(subst /,\,$1)
    MKDIR := mkdir
    # Linker flags untuk Windows (MinGW)
    LDFLAGS := -lraylib -lgdi32 -lwinmm
    EXT := .exe
else
    # Konfigurasi Linux
    TARGET_OS := LINUX
    RM := rm -rf
    FIX_PATH = $1
    MKDIR := mkdir -p
    # Linker flags untuk Linux
    LDFLAGS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    EXT :=
endif

# Direktori
SRC_DIR     := src
INC_DIR     := include
OBJ_DIR     := obj
BIN_DIR     := bin

# Nama Executable
TARGET      := $(BIN_DIR)/game$(EXT)

# Source dan Objects
SRCS        := $(wildcard $(SRC_DIR)/*.c)
OBJS        := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Flag Kompilasi
CFLAGS      := -Wall -Wextra -std=c11 -I$(INC_DIR)

# Rule Utama
all: prepare $(TARGET)

# Membuat direktori jika belum ada
prepare:
ifeq ($(TARGET_OS),WINDOWS)
	@if not exist $(OBJ_DIR) $(MKDIR) $(OBJ_DIR)
	@if not exist $(BIN_DIR) $(MKDIR) $(BIN_DIR)
else
	@$(MKDIR) $(OBJ_DIR) $(BIN_DIR)
endif

# Link executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Kompilasi .c ke .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Membersihkan hasil build
clean:
ifeq ($(TARGET_OS),WINDOWS)
	@if exist $(OBJ_DIR) rmdir /S /Q $(OBJ_DIR)
	@if exist $(BIN_DIR) rmdir /S /Q $(BIN_DIR)
else
	@$(RM) $(OBJ_DIR) $(BIN_DIR)
endif

# Menjalankan aplikasi
run: all
	.$(FIX_PATH)/$(TARGET)

.PHONY: all prepare clean run