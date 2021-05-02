# For building the current version (or a version without version increase) use make BUILD
# If releasing a new version use the RELEASE option

INCDIRS	 := include
PROJDIRS := src
SRCFILES = $(shell find $(PROJDIRS) -type f -name "*.c")

CC := gcc

# Update build number using xenops stable
# and build the new executable
build:
	xenops -q -f $(INCDIRS)/version.h
	$(CC) -o xenops $(SRCFILES) -std=c99

release: version build

version:
	xenops -f main.c -ma -mi -p XENOPS_VERSION_ -q	