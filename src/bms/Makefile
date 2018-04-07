#=============================================================================#
# ARM makefile
#
# author: Freddie Chopin, http://www.freddiechopin.info/
# last change: 2012-01-08
#
# this makefile is based strongly on many examples found in the network
#=============================================================================#

SAVAGE_MODE = false

#=============================================================================#
# hash based versioning
#=============================================================================#

GIT_HASH = $(shell git rev-parse --verify HEAD | cut -c 1-8)
SRC_HASH = $(shell tar c src inc Makefile | sha1sum | cut -c 1-8)
HASH = G$(GIT_HASH)S$(SRC_HASH)

AUTHOR = $(shell git config user.name)

#=============================================================================#
# toolchain configuration
#=============================================================================#

TOOLCHAIN = arm-none-eabi-

CC = $(TOOLCHAIN)gcc
AS = $(TOOLCHAIN)gcc -x assembler-with-cpp
OBJCOPY = $(TOOLCHAIN)objcopy
OBJDUMP = $(TOOLCHAIN)objdump
SIZE = $(TOOLCHAIN)size
RM = rm -f

#=============================================================================#
# test configuration
#=============================================================================#

UNITY_BASE = ../Unity
CC_TEST = gcc
AS_TEST = gcc -x assembler-with-cpp
SIZE_TEST = size
LINT = oclint

#=============================================================================#
# project configuration
#=============================================================================#

# project name
PROJECT = ltc_battery_controller

# core type
CORE = cortex-m0

# linker script
LD_SCRIPT = gcc.ld

# output folder (absolute or relative path, leave empty for in-tree compilation)
OUT_DIR = bin

# include directories (absolute or relative paths to additional folders with
# headers, current folder is always included)
INC_DIRS_CROSS = inc/ ../../lib/lpc11cx4-library/lpc_chip_11cxx_lib/inc ../../lib/lpc11cx4-library/evt_lib/inc/ ../../lib/CANlib

# additional directories with source files (absolute or relative paths to
# folders with source files, current folder is always included)
SRCS_DIRS = src/ ../../lib/lpc11cx4-library/lpc_chip_11cxx_lib/src  ../../lib/lpc11cx4-library/evt_lib/src/ ../../lib/CANlib

SPECIAL_FSAE_FLAGS = -DFSAE_DRIVERS -DCAN_ARCHITECTURE_ARM

# C definitions
C_DEFS = -DSAVAGE_MODE=$(SAVAGE_MODE) -DHASH="\"$(HASH)\"" -DAUTHOR="\"$(AUTHOR)\"" -DCORE_M0 -DDEBUG_ENABLE $(SPECIAL_FSAE_FLAGS) -DCAN_ARCHITECTURE_LPC11CX4

# ASM definitions
AS_DEFS = -D__STARTUP_CLEAR_BSS -D__START=main

# library directories (absolute or relative paths to additional folders with
# libraries)
LIB_DIRS =

# libraries (additional libraries for linking, e.g. "-lm -lsome_name" to link
# math library libm.a and libsome_name.a)
LIBS =


# extension of C files
C_EXT = c

# wildcard for C source files (all files with C_EXT extension found in current
# folder and SRCS_DIRS folders will be compiled and linked)
C_SRCS = $(wildcard $(patsubst %, %/*.$(C_EXT), . $(SRCS_DIRS)))

# extension of ASM files
AS_EXT = S

# wildcard for ASM source files (all files with AS_EXT extension found in
# current folder and SRCS_DIRS folders will be compiled and linked)
AS_SRCS = $(wildcard $(patsubst %, %/*.$(AS_EXT), . $(SRCS_DIRS)))

# optimization flags ("-O0" - no optimization, "-O1" - optimize, "-O2" -
# optimize even more, "-Os" - optimize for size or "-O3" - optimize yet more)
OPTIMIZATION = -O2

# set to 1 to optimize size by removing unused code and data during link phase
REMOVE_UNUSED = 1

# define warning options here
C_WARNINGS = -Wall -Wstrict-prototypes -Wextra

# C language standard ("c89" / "iso9899:1990", "iso9899:199409",
# "c99" / "iso9899:1999", "gnu89" - default, "gnu99")
C_STD = gnu99

#=============================================================================#
# Unit Testing Configuration
#=============================================================================#

# test out folder
OUT_DIR_TEST = testbin

# include directories for test
INC_DIRS_TEST = $(INC_DIRS_CROSS) $(SRCS_DIRS) test $(UNITY_BASE)/src $(UNITY_BASE)/extras/fixture/src

# directories for testing sources
TEST_SRCS_DIRS = test $(UNITY_BASE)/src $(UNITY_BASE)/extras/fixture/src

# c files for testing
C_SRCS_TEST = $(wildcard $(patsubst %, %/*.$(C_EXT), . $(TEST_SRCS_DIRS))) src/charge.c src/ssm.c src/discharge.c src/bms_utils.c src/board.c src/error_handler.c src/cell_temperatures.c

#=============================================================================#
# Write Configuration
#=============================================================================#

COMPORT ?= $(word 1, $(wildcard /dev/tty.usbserial-*) $(wildcard /dev/ttyUSB*))
BAUDRATE = 57600
CLOCK_OSC = 0

#=============================================================================#
# Lint Configuration
#=============================================================================#

MAX_LINE_SIZE = 80

#=============================================================================#
# set the VPATH according to SRCS_DIRS
#=============================================================================#

VPATH = $(SRCS_DIRS) test $(UNITY_BASE)/extras/fixture/src $(UNITY_BASE)/src devices

#=============================================================================#
# when using output folder, append trailing slash to its name
#=============================================================================#

ifeq ($(strip $(OUT_DIR)), )
	OUT_DIR_F =
else
	OUT_DIR_F = $(strip $(OUT_DIR))/
endif

#=============================================================================#
# when using output folder, append trailing slash to its name
#=============================================================================#

ifeq ($(strip $(OUT_DIR_TEST)), )
	OUT_DIR_TEST_F =
else
	OUT_DIR_TEST_F = $(strip $(OUT_DIR_TEST))/
endif

#=============================================================================#
# various compilation flags
#=============================================================================#

# core flags
CORE_FLAGS = -mcpu=$(CORE) -mthumb

# flags for C compiler
C_FLAGS = -fdiagnostics-color=always -std=$(C_STD) -g -ggdb3 -fverbose-asm -Wa,-ahlms=$(OUT_DIR_F)$(notdir $(<:.$(C_EXT)=.lst)) -DUART_BAUD=$(BAUDRATE)
#			add diagnostic colors		c standard	debug(?) extra comments

# flags for assembler
AS_FLAGS = -g -ggdb3 -Wa,-amhls=$(OUT_DIR_F)$(notdir $(<:.$(AS_EXT)=.lst))

# flags for linker
LD_FLAGS = -T$(LD_SCRIPT) -g -nostartfiles -Wl,-Map=$(OUT_DIR_F)$(PROJECT).map,--cref

# flags for lint
LINT_FLAGS = -rc LONG_LINE=$(MAX_LINE_SIZE)

# process option for removing unused code
ifeq ($(REMOVE_UNUSED), 1)
	# enable garbage collection of unused sections
	LD_FLAGS += -Wl,--gc-sections
	# put functions and data into their own sections
	OPTIMIZATION += -ffunction-sections -fdata-sections
endif

#=============================================================================#
# do some formatting
#=============================================================================#

C_OBJS_TEST = $(addprefix $(OUT_DIR_TEST_F), $(notdir $(C_SRCS_TEST:.$(C_EXT)=.o)))
AS_OBJS_TEST = $(addprefix $(OUT_DIR_TEST_F), $(notdir $(AS_SRCS_TEST:.$(AS_EXT)=.o)))

TEST_OBJS = $(AS_OBJS_TEST) $(C_OBJS_TEST)

C_OBJS = $(addprefix $(OUT_DIR_F), $(notdir $(C_SRCS:.$(C_EXT)=.o)))
AS_OBJS = $(addprefix $(OUT_DIR_F), $(notdir $(AS_SRCS:.$(AS_EXT)=.o)))
OBJS = $(AS_OBJS) $(C_OBJS) $(USER_OBJS)
DEPS = $(OBJS:.o=.d)
INC_DIRS_F = -I. $(patsubst %, -I %, $(INC_DIRS_CROSS))
LIB_DIRS_F = $(patsubst %, -L %, $(LIB_DIRS))

INC_DIRS_F_TEST = -I. $(patsubst %, -I %, $(INC_DIRS_TEST))

ELF = $(OUT_DIR_F)$(PROJECT).elf
HEX = $(OUT_DIR_F)$(PROJECT).hex
BIN = $(OUT_DIR_F)$(PROJECT).bin
LSS = $(OUT_DIR_F)$(PROJECT).lss
DMP = $(OUT_DIR_F)$(PROJECT).dmp

TEST_TARGET = $(OUT_DIR_TEST_F)$(PROJECT)

# format final flags for tools, request dependancies for C and asm
C_FLAGS_F_CROSS = $(CORE_FLAGS) $(OPTIMIZATION) $(C_WARNINGS) $(C_FLAGS) $(C_DEFS) -MD -MP -MF $(OUT_DIR_F)$(@F:.o=.d) $(INC_DIRS_F)
AS_FLAGS_F_CROSS = $(CORE_FLAGS) $(AS_FLAGS) $(AS_DEFS) -MD -MP -MF $(OUT_DIR_F)$(@F:.o=.d) $(INC_DIRS_F)
LD_FLAGS_F_CROSS = $(CORE_FLAGS) $(LD_FLAGS) $(LIB_DIRS_F_CROSS)

# format final flags for tools, request dependancies for C and asm
C_FLAGS_F = $(CORE_FLAGS) $(OPTIMIZATION) $(C_WARNINGS) $(C_FLAGS) $(C_DEFS) -MD -MP -MF $(OUT_DIR_F)$(@F:.o=.d) $(INC_DIRS_F)
AS_FLAGS_F = $(CORE_FLAGS) $(AS_FLAGS) $(AS_DEFS) -MD -MP -MF $(OUT_DIR_F)$(@F:.o=.d) $(INC_DIRS_F)
LD_FLAGS_F = $(CORE_FLAGS) $(LD_FLAGS) $(LIB_DIRS_F)

C_FLAGS_F_TEST =  $(OPTIMIZATION) $(C_WARNINGS) $(C_DEFS) -MD -MP -MF $(OUT_DIR_F)$(@F:.o=.d) $(INC_DIRS_F_TEST) -DTEST_HARDWARE
AS_FLAGS_F_TEST = $(AS_FLAGS) $(AS_DEFS) -MD -MP -MF $(OUT_DIR_F)$(@F:.o=.d) $(INC_DIRS_F_TEST)
# LD_FLAGS_F_TEST = $(LIB_DIRS_F_TEST)

#contents of output directory
GENERATED = $(wildcard $(patsubst %, $(OUT_DIR_F)*.%, bin d dmp elf hex lss lst map o)) $(wildcard $(OUT_DIR_TEST_F)*)

#=============================================================================#
# make all
#=============================================================================#

all : make_output_dir $(ELF) $(LSS) $(DMP) $(HEX) $(BIN) print_size

test : CC 			= $(CC_TEST)
test : AS 			= $(AS_TEST)
test : OBJCOPY 	= $(OBJCOPY_TEST)
test : OBJDUMP 	= $(OBJDUMP_TEST)
test : SIZE 		= $(SIZE_TEST)
test : C_FLAGS_F 	= $(C_FLAGS_F_TEST)
test : AS_FLAGS_F 	= $(AS_FLAGS_F_TEST)
test : LD_FLAGS_F 	= $(LD_FLAGS_F_TEST)

.PHONY: test
test : make_test_output_dir $(TEST_TARGET)
	./$(TEST_TARGET)

test_writeflash: AS_DEFS = -D__STARTUP_CLEAR_BSS -D__START=hardware_test
test_writeflash: writeflash

# make object files dependent on Makefile
$(OBJS) : Makefile
$(TEST_OBJS) : Makefile
# make .elf file dependent on linker script
$(ELF) : $(LD_SCRIPT)

#-----------------------------------------------------------------------------#
# test_linking - objects -> elf
#-----------------------------------------------------------------------------#
$(TEST_TARGET) : $(TEST_OBJS)
	@$(CC) $(TEST_OBJS) $(LIBS) -o $@
	@echo ' '

#-----------------------------------------------------------------------------#
# linking - objects -> elf
#-----------------------------------------------------------------------------#

$(ELF) : $(OBJS)
	@echo 'Linking target: $(ELF)'
	$(CC) $(LD_FLAGS_F) $(OBJS) $(LIBS) -o $@
	@echo ' '

#-----------------------------------------------------------------------------#
# compiling - C source -> objects
#-----------------------------------------------------------------------------#

$(OUT_DIR_F)%.o : %.$(C_EXT)
	@echo 'Compiling file: $<'
	$(CC) -c $(C_FLAGS_F) $< -o $@
	@echo ' '

$(OUT_DIR_TEST_F)%.o : %.$(C_EXT)
	@echo 'Compiling file: $<'
	$(CC) -c $(C_FLAGS_F_TEST) $< -o $@
	@echo ' '

#-----------------------------------------------------------------------------#
# assembling - ASM source -> objects
#-----------------------------------------------------------------------------#

$(OUT_DIR_F)%.o : %.$(AS_EXT)
	@echo 'Assembling file: $<'
	$(AS) -c $(AS_FLAGS_F) $< -o $@
	@echo ' '

#-----------------------------------------------------------------------------#
# memory images - elf -> hex, elf -> bin
#-----------------------------------------------------------------------------#

$(HEX) : $(ELF)
	@echo 'Creating IHEX image: $(HEX)'
	$(OBJCOPY) -O ihex $< $@
	@echo ' '

$(BIN) : $(ELF)
	@echo 'Creating binary image: $(BIN)'
	$(OBJCOPY) -O binary $< $@
	@echo ' '

#-----------------------------------------------------------------------------#
# memory dump - elf -> dmp
#-----------------------------------------------------------------------------#

$(DMP) : $(ELF)
	@echo 'Creating memory dump: $(DMP)'
	$(OBJDUMP) -x --syms $< > $@
	@echo ' '

#-----------------------------------------------------------------------------#
# extended listing - elf -> lss
#-----------------------------------------------------------------------------#

$(LSS) : $(ELF)
	@echo 'Creating extended listing: $(LSS)'
	$(OBJDUMP) -S $< > $@
	@echo ' '

#-----------------------------------------------------------------------------#
# print the size of the objects and the .elf file
#-----------------------------------------------------------------------------#

print_size :
	@echo 'Size of modules:'
	$(SIZE) -B -t --common $(OBJS) $(USER_OBJS)
	@echo ' '
	@echo 'Size of target .elf file:'
	$(SIZE) -B $(ELF)
	@echo ' '

#-----------------------------------------------------------------------------#
# create the desired output directory
#-----------------------------------------------------------------------------#

make_output_dir :
	$(shell mkdir $(OUT_DIR_F) 2>/dev/null)

make_test_output_dir :
	$(shell mkdir $(OUT_DIR_TEST_F) 2>/dev/null)

#-----------------------------------------------------------------------------#
# Perform static analysis with lint
#-----------------------------------------------------------------------------#

lint: $(C_SRCS)
	oclint $^ $(LINT_FLAGS) -- $(C_FLAGS_F_CROSS) -I/usr/local/Cellar/gcc-arm-none-eabi/20140805/arm-none-eabi/include/


#-----------------------------------------------------------------------------#
# Write to flash of chip
#-----------------------------------------------------------------------------#

flash: all
	@echo "Writing to" $(COMPORT)
	lpc21isp -NXPARM -control $(HEX) $(COMPORT) $(BAUDRATE) $(CLOCK_OSC)

writeflash: flash

#-----------------------------------------------------------------------------#
# Open up in picocom
#-----------------------------------------------------------------------------#

com:
	@echo "Opening" $(COMPORT)
	lpc21isp -NXPARM -control -termonly $(HEX) $(COMPORT) $(BAUDRATE) $(CLOCK_OSC)


#-----------------------------------------------------------------------------#
# Flash and then open in com
#-----------------------------------------------------------------------------#

fc: flash com

#=============================================================================#
# make clean
#=============================================================================#

clean:
ifeq ($(strip $(OUT_DIR_F)), )
	@echo 'Removing all generated output files'
else
	@echo 'Removing all generated output files from output directory: $(OUT_DIR_F)'
endif
ifneq ($(strip $(GENERATED)), )
	$(RM) $(GENERATED)
else
	@echo 'Nothing to remove...'
endif

#=============================================================================#
# global exports
#=============================================================================#

.PHONY: all clean dependents

.SECONDARY:

# include dependancy files
-include $(DEPS)
