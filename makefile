# For building the current version (or a version without version increase) use make BUILD
# If releasing a new version use the RELEASE option

INCDIRS	 := include
PROJDIRS := src
SRCFILES = $(shell find $(PROJDIRS) -type f -name "*.c")

CC := gcc

# _XOPEN_SOURCE is for unistd ( ftruncate() )
FLAGS := -D_XOPEN_SOURCE=500 -std=c99

# Update build number using xenops stable
# and build the new executable
build:
	@xenops -q -f $(INCDIRS)/version.h -p XENOPS_VERSION_
	@$(CC) -o xenops $(SRCFILES) $(FLAGS)

release: version build

version:
	@xenops -f $(INCDIRS)/version.h -mi -p XENOPS_VERSION_ -q	