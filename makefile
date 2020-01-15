# For building the current version (or a version without version increase) use make BUILD
# If releasing a new version use the RELEASE option


release: version build

version:
	xenops -f main.c -ma -mi -p XENOPS_VERSION_ -q

build:
	gcc -o xenops main.c -std=c89	