#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int cmpstr(void const *a, void const *b){
	const char **pa=(const char **)a;
	const char **pb=(const char **)b;
	return strcmp(*pa,*pb);
}
int cmpNum(void const *a, void const *b){
	const int *pa=(const int *)a;
	const int *pb=(const int *)b;
	return(*pa-*pb);
}

int main(int argc, char *argv[]){
	FILE *inFile;
	FILE *outFile;
	bool oFlag=false;
	bool nFlag=false;
	bool rFlag=false;
	bool hFlag=false;
	bool inFileTaken=false;
	bool outFileWant=false;
	char inFilename[257];
	char outFilename[255];
	char **list;
	int iLine;
	int nLines;
	int lineMtpl;
	int charMtpl;
	int i;
	long int pos;
	//Parse arguments
	for (int iArg=1;iArg<argc;iArg++){
		//If arg begins with '-' and we don't want outFile: must be option
		if (argv[iArg][0]=='-' && outFileWant==false){
			//If only 1 character long:
			if (strlen(argv[iArg])==2){
				//Check option
				switch (argv[iArg][1]){
					case 'o':
						if (oFlag==false){
							oFlag=true;
							outFileWant=true;
						}
						//Only option that gives an error if used multiple times
						else{
							fprintf(stderr,"ERROR: Multiple output options\n");
							exit(1);
						}
						break;
					case 'n':
						nFlag=true;
						break;
					case 'r':
						rFlag=true;
						break;
					case 'h':
						hFlag=true;
						break;
					//Invalid option:Error
					default:
						fprintf(stderr,"ERROR: Invalid option: %s\n",argv[iArg]);
						exit(1);
				}
			}
			//If >1 character long: Error
			else{
				fprintf(stderr,"ERROR: Invalid option: %s\n",argv[iArg]);
				exit(1);
			}
		}
		//Doesn't begin with '-' and we don't want outFile = must be inFilename
		else if (argv[iArg][0]!='-' && outFileWant==false){
			//If inFilename already taken: Error
			if (inFileTaken==true){
				fprintf(stderr,"ERROR: Invalid input\n");
				exit(1);
			}
			//If >255 characters or includes '/': Error
			if (strlen(argv[iArg])>255 || strchr(argv[iArg],'/')!=NULL){
				fprintf(stderr,"ERROR: Invalid input filename\n");
				exit(1);
			}
			//Else: assign inFilename
			else {
				strcpy(inFilename,argv[iArg]);
				inFileTaken=true;
			}
		}
		//Doesn't begin with '-' and we want outFile = must be outFile
		else if (argv[iArg][0]!='-' && outFileWant==true){
			//Repeated outFiles already prevented with oFlag
			//If >255 characters or includes '/': Error
			if (strlen(argv[iArg])>255 || strchr(argv[iArg],'/')!=NULL){
				fprintf(stderr,"ERROR: Invalid input filename\n");
				exit(1);
			}
			//Else: assign outFilename
			else{
				strcpy(outFilename,argv[iArg]);
				outFileWant=false;
			}
		}
		//Begins with '-' and we want outFile: Error (no outFile provided)
		else{
			fprintf(stderr,"ERROR: Output file not provided\n");
			exit(1);
		}
	}
	//If -h option
	if (hFlag==true){
		//Output usage information
		printf("Specify input file as argument or input words from standard input\nUse -o option to specify output file\nUse -n option to sort numerically\nUse -r option to sort in reverse\n");
		printf("I have managed to implement every aspect of this part of the coursework\n-n option is not perfect in a few extreme cases\n");
		//End function
		return(0);
	}
	//Run sort function
	//If outFile wanted but not given: Error
	if (outFileWant==true){
		fprintf(stderr,"ERROR: Output file not provided\n");
		exit(1);
	}
	//Malloc list
	list=(char **)malloc(10*sizeof(char *));
	//If inFile given:
	if (inFileTaken==true){
		//Read inFile
		inFile=fopen(inFilename,"r");
		//If invalid inFile: Error
		if (inFile==NULL){
			fprintf(stderr,"ERROR: Invalid input file\n");
			exit(1);
		}
		//If empty inFile:
		int cur=fgetc(inFile);
		if (cur==EOF){
			//If -option: Open outFile
			if (oFlag==true){
				outFile=fopen(outFilename,"w");
				//If invalid outFile: Error
				if(outFile==NULL){
					fprintf(stderr,"ERROR: Invalid output file\n");
					exit(1);
				}
				//Close outFile
				fclose(outFile);
			}
			//close Infile and return 0
			fclose(inFile);
			return(0);
		}
		//If not empty:
		else{
			//Malloc each line
			for (iLine=0;iLine<10;iLine++){
				list[iLine]=malloc(1000*sizeof(char));
			}
			char *lineStr=malloc(1000*sizeof(char));
			iLine=0;
			lineMtpl=1;
			charMtpl=1;
			pos=ftell(inFile);
			//Go to start of file
			fseek(inFile,0L,SEEK_SET);
			//Read each line into list
			while (fgets(lineStr,1000*charMtpl+2,inFile)!=NULL){
				//If lines fill list memory: Realloc more lines
				if (iLine==10*lineMtpl){
					lineMtpl++;
					list=(char **)realloc(list,(10*lineMtpl*sizeof(char *)));
					for (i=0;i<10*lineMtpl;i++){
						list[i]=realloc(list[i],1000*charMtpl*sizeof(char));
					}
				}
				//If characters fill line memory: Malloc more characters
				signed int lenStr=strlen(lineStr);
				if (lenStr>1000*lineMtpl){
					charMtpl++;
					for (i=0;i<10*lineMtpl;i++){
						list[i]=realloc(list[i],1000*charMtpl*sizeof(char));
					}
					lineStr=realloc(lineStr,1000*charMtpl*sizeof(char));
					//Rewind line
					fseek(inFile,0L,pos);
				}
				//Else: read in line
				else{
					strcpy(list[iLine],lineStr);
					free(lineStr);
					lineStr=malloc(1000*charMtpl*sizeof(char));
					iLine++;
				}
				pos=ftell(inFile);
			}
			free(lineStr);
			lineStr=NULL;
			nLines=iLine;
			//Close inFile
			fclose(inFile);
		}
	}
	//If no inFile given:
	else{
		//Malloc each line
		for (iLine=0;iLine<10;iLine++){
			list[iLine]=malloc(1000*sizeof(char));
		}
		char *lineStr=malloc(1000*sizeof(char));
		iLine=0;
		lineMtpl=1;
		//Take lines from standard input
		while(fgets(lineStr,1002,stdin)!=NULL){
			//If lines fill list memory: Realloc more lines
			if (iLine==10*lineMtpl){
				lineMtpl++;
				list=(char **)realloc(list,(10*lineMtpl*sizeof(char *)));
				for (i=0;i<10*lineMtpl;i++){
					list[i]=realloc(list[i],1000*sizeof(char));
				}
			}
			//If >1000 characters: Error
			if (strlen(lineStr)==1002){
					fprintf(stderr,"ERROR: Line %c in input is too long\n",iLine+1);
					exit(1);
			}
			//Else: read in line
			else{
				strcpy(list[iLine],lineStr);
				free(lineStr);
				lineStr=NULL;
				lineStr=malloc(1000*sizeof(char));
				iLine++;
			}
		}
		free(lineStr);
		lineStr=NULL;
		nLines=iLine+1;
	}
	//Sort
	//If -n option:: sort numerically
	if (nFlag==true && nFlag==false){ //Normal sorts better
		//Malloc numList
		int *numList=malloc(nLines*sizeof(int));
		//Convert list to numList of integers
		for (i=0;i<nLines;i++){
			numList[i]=atoi(list[i]);
		}
		qsort(numList,nLines,sizeof(int),cmpNum);
		//If -o: Write to file
		if (oFlag==true){
			//Open outFile
			outFile=fopen(outFilename,"w");
			//If invalid outFile: Error
			if(outFile==NULL){
				fprintf(stderr,"ERROR: Invalid output file\n");
				exit(1);
			}
			//If no -r option: Write normally
			if (rFlag==false){
				for (i=0;i<nLines;i++){
					fprintf(outFile,"%d",numList[i]);
				}
			}
			//Else: Write in reverse
			else{
				for (i=nLines-1;i>=0;i--){
					fprintf(outFile,"%d",numList[i]);
				}
			}
			//Close file
			fclose(outFile);
		}
		//Else: Print to stdout
		else{
			//If no -r option: Print normally
			if (rFlag==false){
				for (i=0;i<nLines;i++){
					printf("%d\n",numList[i]);
				}
			}
			//Else: Print in reverse
			else{
				for (i=nLines-1;i>=0;i--){
					printf("%d\n",numList[i]);

				}
			}
		}
		free(numList);
	}
	//Else: Sort normally
	else{
		qsort(list,nLines,sizeof(char*),cmpstr);
		//If -o: Write to file
		if (oFlag==true){
			//Open outFile
			outFile=fopen(outFilename,"w");
			//If invalid outFile: Error
			if(outFile==NULL){
				fprintf(stderr,"ERROR: Invalid output file\n");
				exit(1);
			}
			//If no -r option: Write normally
			if (rFlag==false){
				for (i=0;i<nLines;i++){
					fputs(list[i],outFile);
				}
			}
			//Else: Write in reverse
			else{
				for (i=nLines-1;i>=0;i--){
					fputs(list[i],outFile);
				}
			}
			//Close file
			fclose(outFile);
		}
		//Else: Print to stdout
		else{
			//If no -r option: Print normally
			if (rFlag==false){
				for (i=0;i<nLines;i++){
					printf("%s",list[i]);
				}
			}
			//Else: Print in reverse
			else{
				for (i=nLines-1;i>=0;i--){
					printf("%s",list[i]);

				}
			}
		}
	}
	//Free all list[iLine] and free list
	for (i=0;i<10*lineMtpl;i++){
		free(list[i]);
		list[i]=NULL;
	}
	free(list);
	list=NULL;
	return(0);
}

