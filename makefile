
build:
	xenops -f main.c -ma -p XENOPS_VERSION_ -q
	gcc -o xenops main.c -std=c89
	