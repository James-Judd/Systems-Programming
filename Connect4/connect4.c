//You can include any of headers defined in the C11 standard here. They are:
//assert.h, complex.h, ctype.h, errno.h, fenv.h, float.h, inttypes.h, iso646.h, limits.h, locale.h, math.h, setjmp.h, signal.h, stdalign.h, stdarg.h, stdatomic.h, stdbool.h, stddef.h, stdint.h, stdio.h, stdlib.h, stdnoreturn.h, string.h, tgmath.h, threads.h, time.h, uchar.h, wchar.h or wctype.h
//You may not include any other headers.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "connect4.h"

struct board_structure{
	char **boardPointer;
	int nRows;
	int nColsFirstRow;
	int nx;
	int no;
	bool xWin;
	bool oWin;
};

board setup_board(){
	board u=malloc(sizeof(struct board_structure));
	return(u);
}

void cleanup_board(board u){
	for (int curRow=0; curRow < u->nRows ;curRow++){
		free(u->boardPointer[curRow]);
		u->boardPointer[curRow]=NULL;
	}
	free(u->boardPointer);
	u->boardPointer=NULL;
	free(u);
	u=NULL;
}

void read_in_file(FILE *infile, board u){
	int nRows=0;
	int nColsFirstRow=0;
	int nColsCurRow=0;
	int nx=0;
	int no=0;
	bool FirstRowChecked=false;
	bool prevNewLine=false;
	//If invalid infile: Error
	if (infile==NULL){
		fprintf(stderr,"ERROR: Invalid input file");
		exit(1);
	}
	//If empty file: Error
	char curChar=getc(infile);
	if (curChar==EOF){
		fprintf(stderr,"ERROR: Empty input file\n");
		exit(1);
	}
	//Go to start of file
	fseek(infile,0L,SEEK_SET);
	//Iterate through cells
	while(curChar!=EOF){
		curChar=getc(infile);
		switch(curChar){
			case 'x':
				nx++;
				nColsCurRow++;
				prevNewLine=false;
				break;
			case 'o':
				no++;
				nColsCurRow++;
				prevNewLine=false;
				break;
			case '.':
				nColsCurRow++;
				break;
			case '\n':
				//If extra blank line: Error
				if (prevNewLine==true){
					fprintf(stderr,"ERROR: Unexpected blank line in input file\n");
					exit(1);
				}
				//Set 1st row
				if (FirstRowChecked==false){
					nColsFirstRow=nColsCurRow;
					FirstRowChecked=true;
				}
				//If non-uniform rows: Error
				if (nColsCurRow!=nColsFirstRow){
					fprintf(stderr,"ERROR: Non-uniform row length in input file\n");
					exit(1);
				}
				nRows++;
				nColsCurRow=0;
				break;
			case EOF:
				break;
			default:
				fprintf(stderr,"ERROR: Invalid character in input file\n");
				exit(1);
		}
		//If >512 columns: Error
		if (nColsCurRow>512){
			fprintf(stderr,"ERROR: Input file exceeds 512 column limit\n");
			exit(1);
		}
	}
	//If <4 columns: Error
	if (nColsFirstRow<4){
		fprintf(stderr,"ERROR: Input file below 4 column limit");
		exit(1);
	}
	//Return to start of file
	fseek(infile,0L,SEEK_SET);
	//Build the board
	char **board=(char **)malloc(nRows*sizeof(char *));
	for (int curRow=0;curRow<nRows;curRow++){
		board[curRow]=(char *)malloc(nColsFirstRow*sizeof(char));
	}
	//Fill the board
	char* charLine=malloc(nColsFirstRow * sizeof(char));
	for (int curRow=0;curRow<nRows;curRow++){
		fscanf(infile,"%s",charLine);
		for (int curCol=0;curCol<nColsFirstRow;curCol++){
			board[curRow][curCol]=charLine[curCol];
		}
	}
	free(charLine);
	charLine=NULL;
	//Gravity check:
	//Iterate through cells, starting bottom+1 left
	for (int curRow=nRows-2;curRow>=0;curRow--){
		for (int curCol=0;curCol<nColsFirstRow;curCol++){
			if (board[curRow][curCol]!='.'){
				if (board[curRow+1][curCol]=='.'){
					fprintf(stderr,"ERROR: Invalid input file (gravity)");
					exit(1);
				}
			}
		}
	}
	u->boardPointer=board;
	u->nRows=nRows;
	u->nColsFirstRow=nColsFirstRow;
	u->nx=nx;
	u->no=no;
	u->xWin=false;
	u->oWin=false;
	//If input has already won: Error
	if (current_winner(u)!='.'){
		fprintf(stderr,"ERROR:Invalid input file (already won)");
		exit(1);
	}
}

void write_out_file(FILE *outfile, board u){
	//If invalid output file: Error
	if (outfile==NULL){
		fprintf(stderr,"ERROR:Invalid output file\n");
		exit(1);
	}
	//If board was not won on previous call: check
	if (u->xWin==false && u->oWin==false){
		current_winner(u);
	}
	//Write
	for (int curRow=0;curRow< u->nRows;curRow++){
		for(int curCol=0;curCol < u->nColsFirstRow;curCol++){
			fputc(u->boardPointer[curRow][curCol],outfile);
		}
		fputc('\n',outfile);
	}
}

char next_player(board u){
	if (u->nx > u->no){
		return('o');
	}
	else{
		return('x');
	}
}

char current_winner(board u){
	int nRows= u->nRows;
	int nCols= u->nColsFirstRow;
	bool zeroDots=true;
	//If board was not won on previous call:
	if (u->xWin==false && u->oWin==false){
		//Check every cell, starting bottom left
		for (int curRow=nRows-1;curRow>=0;curRow--){
			for (int curCol=0;curCol<nCols;curCol++){
				char curChar= u->boardPointer[curRow][curCol];
				switch(curChar){
					case 'x':
						//If no win already (to save iterations):
						if (u->xWin==false){
							//Check next 3 to right
							if (u->boardPointer[curRow][(curCol+1)%nCols]=='x'){
								if (u->boardPointer[curRow][(curCol+2)%nCols]=='x'){
									if (u->boardPointer[curRow][(curCol+3)%nCols]=='x'){
										//Mark as win
										u->xWin=true;
										//Capitalise winning characters
										u->boardPointer[curRow][curCol]='X';
										u->boardPointer[curRow][(curCol+1)%nCols]='X';
										u->boardPointer[curRow][(curCol+2)%nCols]='X';
										u->boardPointer[curRow][(curCol+3)%nCols]='X';
										break;
									}
								}
							}
							//If below 3rd row:
							if (curRow>2){
								//Check next 3 to up-right
								if (u->boardPointer[curRow-1][(curCol+1)%nCols]=='x'){
									if (u->boardPointer[curRow-2][(curCol+2)%nCols]=='x'){
										if (u->boardPointer[curRow-3][(curCol+3)%nCols]=='x'){
											u->xWin=true;
											u->boardPointer[curRow][curCol]='X';
											u->boardPointer[curRow-1][(curCol+1)%nCols]='X';
											u->boardPointer[curRow-2][(curCol+2)%nCols]='X';
											u->boardPointer[curRow-3][(curCol+3)%nCols]='X';
											break;
										}
									}
								}
								//Check next 3 above
								if (u->boardPointer[curRow-1][curCol]=='x'){
									if (u->boardPointer[curRow-2][curCol]=='x'){
										if (u->boardPointer[curRow-3][curCol]=='x'){
											u->xWin=true;
											u->boardPointer[curRow][curCol]='X';
											u->boardPointer[curRow-1][curCol]='X';
											u->boardPointer[curRow-2][curCol]='X';
											u->boardPointer[curRow-3][curCol]='X';
											break;
										}
									}
								}
								//Check next 3 to up-left
								if (u->boardPointer[curRow-1][(curCol+nCols-1)%nCols]=='x'){
									if (u->boardPointer[curRow-2][(curCol+nCols-2)%nCols]=='x'){
										if (u->boardPointer[curRow-3][(curCol+nCols-3)%nCols]=='x'){
											u->xWin=true;
											u->boardPointer[curRow][curCol]='X';
											u->boardPointer[curRow-1][(curCol+nCols-1)%nCols]='X';
											u->boardPointer[curRow-2][(curCol+nCols-2)%nCols]='X';
											u->boardPointer[curRow-3][(curCol+nCols-3)%nCols]='X';
											break;
										}
									}
								}
							}
						}
						break;
					case 'o':
						//If no win already (to save iterations):
						if (u->oWin==false){
							//Check next 3 to right
							if (u->boardPointer[curRow][(curCol+1)%nCols]=='o'){
								if (u->boardPointer[curRow][(curCol+2)%nCols]=='o'){
									if (u->boardPointer[curRow][(curCol+3)%nCols]=='o'){
										u->oWin=true;
										u->boardPointer[curRow][curCol]='O';
										u->boardPointer[curRow][(curCol+1)%nCols]='O';
										u->boardPointer[curRow][(curCol+2)%nCols]='O';
										u->boardPointer[curRow][(curCol+3)%nCols]='O';
										break;
									}
								}
							}
							//If below 3rd row:
							if (curRow>2){
								//Check next 3 to up-right
								if (u->boardPointer[curRow-1][(curCol+1)%nCols]=='o'){
									if (u->boardPointer[curRow-2][(curCol+2)%nCols]=='o'){
										if (u->boardPointer[curRow-3][(curCol+3)%nCols]=='o'){
											u->oWin=true;
											u->boardPointer[curRow][curCol]='O';
											u->boardPointer[curRow-1][(curCol+1)%nCols]='O';
											u->boardPointer[curRow-2][(curCol+2)%nCols]='O';
											u->boardPointer[curRow-3][(curCol+3)%nCols]='O';
											break;
										}
									}
								}
								//Check next 3 above
								if (u->boardPointer[curRow-1][curCol]=='o'){
									if (u->boardPointer[curRow-2][curCol]=='o'){
										if (u->boardPointer[curRow-3][curCol]=='o'){
											u->oWin=true;
											u->boardPointer[curRow][curCol]='O';
											u->boardPointer[curRow-1][curCol]='O';
											u->boardPointer[curRow-2][curCol]='O';
											u->boardPointer[curRow-3][curCol]='O';
											break;
										}
									}
								}
								//Check next 3 to up-left
								if (u->boardPointer[curRow-1][(curCol+nCols-1)%nCols]=='o'){
									if (u->boardPointer[curRow-2][(curCol+nCols-2)%nCols]=='o'){
										if (u->boardPointer[curRow-3][(curCol+nCols-3)%nCols]=='o'){
											u->oWin=true;
											u->boardPointer[curRow][curCol]='O';
											u->boardPointer[curRow-1][(curCol+nCols-1)%nCols]='O';
											u->boardPointer[curRow-2][(curCol+nCols-2)%nCols]='O';
											u->boardPointer[curRow-3][(curCol+nCols-3)%nCols]='O';
											break;
										}
									}
								}
							}
						}
						break;
					default:
						if (zeroDots==true){
							zeroDots=false;
						}
				}
			}
		}
	}
	//Return winner
	if (u->xWin==false && u->oWin==false){
		if (zeroDots==true){
			return('d');
		}
		else{
			return('.');
		}
	}
	if (u->xWin==true && u->oWin==true){
		return('d');
	}
	if (u->xWin==true && u->oWin==false){
		return('x');
	}
	if(u->xWin==false && u->oWin==true){
		return('o');
	}
	return(1);
}

struct move read_in_move(board u){
	struct move m;
	char line[22];
	char *ptr;
	int column;
	int row;
  printf("Player %c enter column to place your token: ",next_player(u)); //Do not edit this line
	//Take input from command line
	fgets(line,22,stdin);
	//Strip excess from int
	column=strtol(line,&ptr,10);
	//If excess exists: Error
	if (strlen(ptr)!=1){
		fprintf(stderr,"ERROR: Invalid user input\n");
		exit(1);
	}
	//If column=0 and input was empty(not 0): Error
	if (column==0 && line[0]==10){
		fprintf(stderr, "ERROR: Invalid user input\n");
		exit(1);
	}
  printf("Player %c enter row to rotate: ",next_player(u)); // Do not edit this line
	//Same for row
	fgets(line,22,stdin);
	if (strlen(line)==0){
	}
	if (line[0]==0){
		fprintf(stderr,"ERROR: Invalid user input\n");
		exit(1);
	}
	row=strtol(line,&ptr,10);
	if (strlen(ptr)!=1){
		fprintf(stderr,"ERROR: Invalid user input\n");
		exit(1);
	}
		if (row==0 && line[0]==10){
		fprintf(stderr, "ERROR: Invalid user input\n");
		exit(1);
	}
	m.column=column;
	m.row=row;
	return(m);
}

int is_valid_move(struct move m, board u){
	int column=m.column;
	int row=m.row;
	int nCols= u->nColsFirstRow;
	int nRows= u->nRows;
	//If out of bounds: Return 0
	if (column<1 || column>nCols){
		return(0);
	}
	if (row<-nRows || row>nRows){
		return(0);
	}
	//If column is full: Return 0
	if (u->boardPointer[0][column-1]!='.'){
		return(0);
	}
	else{
		return(1);
	}
}

char is_winning_move(struct move m, board u){
	//Build copy of board
	int nRows=u->nRows;
	int nCols=u->nColsFirstRow;
	board v=setup_board();
	char **nextBoard=(char **)malloc(nRows*sizeof(char *));
	for (int row=0;row<nRows;row++){
		nextBoard[row]=(char *)malloc(nCols*sizeof(char));
	}
	for (int row=0;row<nRows;row++){
		for (int col=0;col<nCols;col++){
			nextBoard[row][col]=u->boardPointer[row][col];
		}
	}
	v->boardPointer=nextBoard;
	v->nRows=u->nRows;
	v->nColsFirstRow=u->nColsFirstRow;
	v->nx=u->nx;
	v->no=u->no;
	v->xWin=u->xWin;
	v->oWin=u->oWin;
	//Play move
	play_move(m,v);
	//Find winner
	char winner=current_winner(v);
	//Clean board
	cleanup_board(v);
	return(winner);
}

void play_move(struct move m, board u){
	char playChar=next_player(u);
	int nRows=u->nRows;
	int nCols=u->nColsFirstRow;
	int playRow=0;
	int playCol=(m.column)-1;
	int twistRow=nRows-abs(m.row);
	int twist=((m.row>0)-(m.row<0));
	//Play & Gravity
	//While above the bottom row
	while (playRow!=nRows-1){
		//Check if cell below is free & iterate downwards
		if (u->boardPointer[playRow+1][playCol]=='.'){
			playRow++;
		}
		//If cell below is not free: break loop
		else{
			break;
		}
	}
	//Fill cell
	u->boardPointer[playRow][playCol]=playChar;
	//Twist
	if (twist!=0){
		char firstChar=u->boardPointer[twistRow][0];
		char lastChar=u->boardPointer[twistRow][nCols-1];
		switch(twist){
			//Right
			case 1:
				for (int col=nCols-1;col>0;col--){
					u->boardPointer[twistRow][col]=u->boardPointer[twistRow][col-1];
				}
				u->boardPointer[twistRow][0]=lastChar;
				break;
			//Left
			default:
				for (int col=0;col<nCols-1;col++){
					u->boardPointer[twistRow][col]=u->boardPointer[twistRow][col+1];
				}
				u->boardPointer[twistRow][nCols-1]=firstChar;
		}
		//Gravity on each column of twist row
		for (int col=0;col<nCols;col++){
			playRow=twistRow;
			char gravChar=u->boardPointer[twistRow][col];
			//If twist row character is a '.':
			if (gravChar=='.'){
				//Iterate upwards from twist row
				for (int row=twistRow-1;row>=0;row--){
					//Drop x/o characters by 1 and replace them with '.'
					if (u->boardPointer[row][col]!='.'){
						u->boardPointer[row+1][col]=u->boardPointer[row][col];
						u->boardPointer[row][col]='.';
					}
					//Until a '.'
					else{
						break;
					}
				}
			}
			//If twist row character is x/o:
			else {
				//While above bottom row
				while (playRow!=nRows-1){
					//Check if cell below is free, iterate downwards
					if (u->boardPointer[playRow+1][col]=='.'){
						playRow++;
					}
					//If cell below is not free: break loop
					else{
						break;
					}
				}
				//If drop:
				if (playRow!=twistRow){
					//Drop and replace with '.'
					u->boardPointer[playRow][col]=u->boardPointer[twistRow][col];
					u->boardPointer[twistRow][col]='.';
				}
			}
		}
	}
	//Increase character count
	if (playChar=='x'){
		u->nx++;
	}
	else{
		u->no++;
	}
}
