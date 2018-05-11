
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

enum Header {
	NAME = 0, MODE = 100, UID = 108, GID = 116, SIZE = 124,
	CHK = 148, TYPE = 156, LINK = 157,  END = 512
};

enum Type {
	REG = '0', DIRECTORY = '5'
};

void chksum(const char b[END], char *chk) {
	unsigned sum = 0, i;
	for(i = 0; i<END; i++)
		sum += (i >= CHK && i < CHK+8) ? ' ' : b[i];
	snprintf(chk, 9, "0%5o", sum);
}

int extract(char *fname, int l, char b[END]){
	static char lname[101] = {0}, chk[9] = {0};
	int r = 0;
	FILE *f = NULL;

	memcpy(lname, b+LINK, 100);
	unlink(fname);
	switch(b[TYPE]) {
	case REG:
		r = !(f = fopen(fname, "w")) ||
			chmod(fname,strtoul(b + MODE, 0, 8));
		break;
	case DIRECTORY:
		r = mkdir(fname,(mode_t) strtoull(b + MODE,0,8));
		break;
	default:
		fprintf(stderr,"%s: unsupported filetype %c\n", fname, b[TYPE]);
	}
	/*
	if(r || (getuid() == 0 && chown(fname, strtoul(b + UID, 0, 8),
					strtoul(b + GID, 0, 8))))
		perror(fname);
	*/
	chksum(b, chk);
	//printf("checksum: '%.8s'='%s'\n", b+CHK, chk);
	if(strncmp(b+CHK, chk, 8))
		fprintf(stderr, "%s: chksum failed\n", fname);

	for(; l > 0; l -= END){
		fread(b, END, 1, stdin);
		if(f) fwrite(b, MIN(l, 512), 1, f);
	}
	if(f) fclose(f);
	return 0;
}

int untar() {
	int l;
	char b[END], fname[101] = { 0 };

	while(fread(b, END, 1, stdin)){
		if(*b == '\0')
			break;
		memcpy(fname, b, 100);
		l = strtol(b+SIZE, 0, 8);
		extract(fname, l, b);
	}
	return EXIT_SUCCESS;
}


int main(int argc, char *argv[]) {
		return untar();
}
