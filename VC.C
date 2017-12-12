/*  View-compr  v1.00  (Va2.EXE)
 *  Copyright (C) 1989  Jeremy Hubble
 *  by Jeremy Hubble   5/19/89  - 6/4/89  - 6/10/89 - 6/11/89 - 6/12/89
 *                     8/27/89  - 8/29/89
 *  This program will accept a compr file name from the command line
 *  or manually entered after the program is started, and then print
 *  our a list of the contents of the compr, allows wildcards, but
 *  will only print contents of .ZIPs and .LZHs.
 */
/*  6/11/89 - fixed output formatting problems */
/*  6/12/89 - added wildcard support           */
/*  8/29/89 - fixed the evil NULL pointer bug by changing *fileinfo to
 *            fileinfo, and changing around findfirst, findnext referernces */
/*  8/29/89 - added arc read capabilities */



/*  Initialization  */
#include <dos.h>
#include <stdio.h>
#include <string.h>


unsigned long int total_comprs,tsum_size, tsum_length,tnumber_files = 0;
unsigned long int sum_size,sum_length,number_files;

struct {            /* main structure that ZIP is read into */
	unsigned long int header;
	unsigned short int vers_need;
	unsigned short int bitflag;
	short int comp_method;
	unsigned short int time;
	unsigned short int date;
	unsigned long int crc32;
	unsigned long int c_size;
	unsigned long int unc_size;
	short int fname_length;
	short int extrafield_length;
	}  lfh;

struct {            /* main structure that LZH is read into */
	char compression[5];
	unsigned long int c_size;
	unsigned long int unc_size;
	unsigned short int time;
	unsigned short int date;
	char attributes;       /* single byte - usually 0x20 - attributes? */
	short int fname_length;
	}  fh;


struct  {               /* main structure arc is read into */
    unsigned long size;         /* size of compressed file */
    unsigned short date;        /* file date */
    unsigned short time;        /* file time */
    unsigned short crc;         /* cyclic redundancy check */
    unsigned long length;       /* size of uncompressed file */
  } arc_header;


/* function prototypes */

char *datestr( unsigned d, char *buf );   /* datestr prototype */
grab_compr(FILE *compr, char *fname);    /* get file, and filename(txt)  */
extern int zip_next(FILE *compr);
extern int lzh_next(FILE *lzh);
extern int arc_next(FILE *arc);

/************************  MAIN PROGRAM *****************************/

main(int argc,char **argv)
{


struct find_t fileinfo;           /* will hold results of _dos_nextfile */
extern unsigned long int sum_size,sum_length,number_compr;
short int j,compr_file_end,length;
char compr_file[128];

FILE *compr;

/* print program identification */

printf("\n\nVC V1.0 - Jeremy Hubble 8/29/89\n\n");

/*  get file name input from user  */

if (argv[1] == NULL) {        /*  nothing on command line, so input */

printf("Enter compr filename :");
gets(compr_file);
}
else                            /* something on command line, so copy it */
strcpy(compr_file,argv[1]);       /* ... to compr_file */


if ( strstr(compr_file,".") == NULL )   /* change 'filename' to 'filename.*' */
	strcat(compr_file,".*\0");


if ( (_dos_findfirst (compr_file,_A_NORMAL,&fileinfo)) != 0) {
	printf("File not found.\n");
	exit(-1);  }


do {
compr = fopen (fileinfo.name,"rb");
grab_compr(compr,fileinfo.name);
fclose (compr);
}
while ( _dos_findnext(&fileinfo) == 0);

/* if we have only one file, don't bother with printing sum of all files */
if ((total_comprs >= 2)) {

/* print out the sum sums */

printf ("======= ======= ========   ============\n");
printf ("%lu\t%lu\t\t   %u files",tsum_size,tsum_length,tnumber_files);

printf("\nContained in %lu comprs\n",total_comprs);}
}

/************************* GRAB_compr ************************************/

/* the main part to grab a compr file */
grab_compr(FILE *compr, char *fname)    /* get file, and filename(txt)  */
{
unsigned long int j;


/* check for the .ZIP extension */

if (( strstr(fname,".ZIP") ) != NULL)  {      /* it is zip, so go */
	printf ("\n\nFile: %s\n",fname);
	printf ("\nlength \tsize\tdate       Name\n");             /* print the 'header titles */
	printf ("------- ------- --------   ------------\n");
      while (zip_next(compr)); }  /* main function to print out the zip info */


/* check for .ARC extension */

if (( strstr(fname,".ARC") ) != NULL)  {      /* it is zip, so go */
	printf ("\n\nFile: %s\n",fname);
	printf ("\nlength \tsize\tdate       Name\n");             /* print the 'header titles */
	printf ("------- ------- --------   ------------\n");
      while (arc_next(compr)); }  /* main function to print out the zip info */


/* chack for .LZH extension */

if ( ( strstr(fname,".LZH") ) != NULL)   {
	printf ("\n\nFile: %s\n", fname);
	printf ("\nlength \tsize\tdate       Name\n");             /* print the 'header titles */
	printf ("------- ------- --------   ------------\n");
	while (lzh_next(compr));  /* main function to print out the compr info */
	}

if (number_files != 0) {

printf ("------- ------- --------   ------------\n");
printf ("%lu\t%lu\t\t   %u ",sum_size,sum_length,number_files);

if (number_files == 1)          /* match amount - 1 file, 2+ files */
	printf("file\n");
else
	printf("files\n");

tsum_size += sum_size;        /* add to all files counter */
tsum_length += sum_length;
tnumber_files += number_files;
++total_comprs;                   /* total number of compr files */

}       /* finish stuff done only with actual compr. */


sum_size = sum_length = number_files = 0; /* initialize to values to 0 */

}

/**********************  MAIN ZIP READ FUNCTION ****************************/


/*  Jeremy Hubble   6/10/89  */


extern zip_next(FILE *compr)
/* FILE *compr;  */

{
extern unsigned long int sum_size,sum_length,number_files;
unsigned first_date;
char filename[40],print_date[10];

fread (&lfh,30,1,compr);    /* read 30 characters from compr into lfh structure */


/* signature check */
if (lfh.header != 67324752l )   /* check for right signature! */
return(0);

/* get the date and convert it to human form */
datestr (lfh.date,print_date); /* function at end of program */


sum_size = sum_size + lfh.unc_size; /* increment total (uncompr) size counter */

/* print uncompressed & compressed sizes, and date */
printf ("%lu\t%lu\t%s   ",lfh.unc_size,lfh.c_size,print_date);

sum_length = sum_length + lfh.c_size; /* total (compr) length */

fread (filename,lfh.fname_length,1,compr);    /*  read the filename */

filename[lfh.fname_length]=0;      /*  terminate string to prevent wierd  */
printf("%s\n",filename);

fseek (compr,(long)(lfh.extrafield_length),SEEK_CUR);   /* skip extra field  */
fseek (compr,(lfh.c_size),SEEK_CUR);  /* skip past the file */

++number_files;          /* increase the number of comprs counter */

return (1);  /* hit it again, sam */
}




/**********************  MAIN lzh READ FUNCTION ****************************/


/*  Jeremy Hubble  -  8/24/89  */


extern lzh_next(FILE *lzh)

{
extern unsigned long int sum_size,sum_length,number_files;
unsigned short int first_date,sig,sig2;
char filename[40],print_date[10];


sig = fgetc(lzh);                       /* get first byte of signature */
if (sig == 0)
	return(0);                      /* 0 probably means eof */
sig2 = fgetc(lzh);                      /* get other (signature?) byte */
fseek(lzh,-1l,SEEK_CUR);                  /* skip past two chars we just got */

fread (&fh,21,1,lzh);    /* read 20 characters from lzh into fh structure */



/* get the date and convert it to human form */
datestr (fh.date,print_date); /* function at end of program */


sum_size = sum_size + fh.unc_size; /* increment total (uncompr) size counter */

/* print uncompressed & compressed sizes, and date */
printf ("%lu\t%lu\t%s   ",fh.unc_size,fh.c_size,print_date);

sum_length = sum_length + fh.c_size; /* total (compr) length */

fread (filename,fh.fname_length,1,lzh);    /*  read the filename */

filename[fh.fname_length]=0;      /*  terminate string to prevent wierd  */
printf("%s\n",filename);

fseek (lzh,((fh.c_size)+2),SEEK_CUR);  /* skip past the file */

++number_files;          /* increase the number of lzhs counter */


return (1);  /* hit it again, sam */
}


/************************* arc_next ************************************/

arc_next (FILE *arc)
{
short int arc_mark,header;
char print_date[10],filename[13];
extern unsigned long int number_files;

arc_mark = fgetc(arc);          /* is it an arc file? */
if (arc_mark != 0x1a)
	return (0);

header = fgetc(arc);            /* get the header version */
    if (header == 0)        /* end of arc mark */
	return (0);

fread(filename,13,1,arc);       /* read in filename */

    if (header == 1)  {
	fread(&arc_header,13,1,arc);
	arc_header.length = arc_header.size;   /* size = length */
	}

    else
	fread(&arc_header,17,1,arc);            /* read in normal header */


/* get the date and convert it to human form */
datestr (arc_header.date,print_date); /* function at end of program */


sum_size = sum_size + arc_header.length; /* increment total (uncompr) size counter */

/* print uncompressed & compressed sizes, and date */
printf ("%lu\t%lu\t%s   ",arc_header.length,arc_header.size,print_date);

sum_length = sum_length + arc_header.size; /* total (compr) length */

printf("%s\n",filename);

fseek (arc,(arc_header.size - 3),SEEK_CUR);  /* skip past the file */

++number_files;          /* increase the number of zips counter */

return (1);  /* hit it again, sam */
}

 /* ----------------------------------------------------------*/


/* converts MSDOS date to readable string
 *
 * datestr(date,destination)
 */

char *datestr( unsigned d, char *buf )
{
    sprintf( buf, "%2.2d/%02.2d/%02.2d",
	     (d >> 5) & 0x0f, d & 0x1f, (d >> 9) + 80 );
    return buf;
}

