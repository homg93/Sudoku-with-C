#include <stdio.h>
#include <stdlib.h>//exit(1)
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#define SIZE 4 //버퍼의 사이즈


void myprint(int x[][9]){//반복문을 통해 2차원 배열의 sudoku표를 출력
	int i,j;

	printf("＼열┏1━2━3━┳━4━5━6━┳━7━8━9━┓\n");
	printf("행  ┏━━━━━━┳━━━━━━━┳━━━━━━━┓\n");

	for(i=0; i<9;i++){
		if( i != 0 && i%3 == 0){
			printf("    ┣━━━━━━╋━━━━━━━╋━━━━━━━┫\n");
		}
		for(j=0; j<9; j++){
			if(j == 0){
				printf("(%d) ┃", i+1);
			}
			else if( j%3 ==0 ){
				printf("┃ ");
			}
			if ( x[i][j] == 0){
				printf("  ");
			}
			else	printf("%d ",x[i][j]);
			if(j == 8){
				printf("┃");
			}
		}
		printf("\n");	
	}
	printf("    ┗━━━━━━┻━━━━━━━┻━━━━━━━┛\n");


}
void insert(int x[][9],int data,int row, int col){ //sudoku표의 원하는 행과 열에 데이터를 입력
	if( data <1 || data >9){
		printf("ERR :: 1~9사이의 수를 입력하세요\n");
	}
	else{
		x[row-1][col-1] = data;
	}
}
void delete(int x[][9],int row, int col){ //sudoku표의 원하는 행과 열의 데이터를 삭제
	
	x[row-1][col-1] = 0;
	
}
int rtest(int x[][9],int row){ 
// 매개변수 row에 입력받은 행번호를 불러와 입력된 행에 대한 중복검사실행
// 만약 중복이 되었다면 0을 리턴
	int rtest[9] = {1,2,3,4,5,6,7,8,9};
	int i,j, k;
	for(i = row-1; i<row; i ++){
		int save[] = {0,0,0,0,0,0,0,0,0};
		for(j=0; j<9 ; j++){
			for( k =0 ; k < 9 ; k++){
				if(rtest[j] == x[i][k])
					save[j] = save[j] + 1;
				if(save[j] >= 2){
					printf("%d 행에 %d을(를) 중복입력 하였습니다\n", i+1, j+1);
					return 0;
				}
			}
		}
	}
	return 1;
}

int ltest(int x[][9],int col){
// 매개변수 col에 입력받은 열번호를 불러와 입력된 열에 대한 중복검사실행
// 만약 중복이 되었다면 0을 리턴
	int ltest[9] = {1,2,3,4,5,6,7,8,9};
	int i,j, k;
	for(i = col-1; i<col; i ++){
		int save[] = {0,0,0,0,0,0,0,0,0};
		for(j=0; j<9 ; j++){
	
			for( k =0 ; k < 9 ; k++){
				if(ltest[j] == x[k][i])
					save[j] = save[j] + 1;
				if(save[j] >= 2){
					printf("%d 열에 %d을(를) 중복입력 하였습니다\n", i+1, j+1);
					return 0;
				}
			}
		}
	}
	return 1;
}

int small_triangle(int x[][9],int num,int rStart,int rEnd,int cStart,int cEnd){
//소격자에 대한 스도쿠표 중복검사를 진행. 만약 중복이 되었다면 0을 리턴
//매개변수 설명 : num = 소격자 번호, rStart = 행반복시작값, rEnd = 행반복종료값
//cStart = 열반복시작값, cEnd = 열반복종료값
	int st1[9] = {1,2,3,4,5,6,7,8,9};
	int i,j, k;
	int save[] = {0,0,0,0,0,0,0,0,0};
	for(i = 0; i<9; i ++){		
		for(j=rStart; j<rEnd ; j++){
			for( k =cStart ; k < cEnd ; k++){
				if(st1[i] == x[j][k])
					save[i] = save[i] + 1;
				if(save[i] >= 2){
					printf("%d 소격자에 %d을(를) 중복입력 하였습니다\n", num, i+1);
					return 0;
				}
			}
		}
	}
	return 1;
}

// 1~9 소격자 검사 예시
//small_triangle(sdq,1,0,3,0,3)//small_triangle(sdq,2,0,3,3,6)//small_triangle(sdq,3,0,3,6,9)
//small_triangle(sdq,4,3,6,0,3)//small_triangle(sdq,5,3,6,3,6)//small_triangle(sdq,6,3,6,6,9)
//small_triangle(sdq,7,6,9,0,3)//small_triangle(sdq,8,6,9,3,6)//small_triangle(sdq,9,6,9,6,9)

void multi_thread_pipe(int sdq[][9],int x, int row, int col){
//멀티 스레드을 이용해 각각의 스레드에서 스도쿠의 행검사, 열검사를 실행한다.
//부모와 자식은 pipe를 통하여 행,열, 입력받은 숫자를 주고받는다.
// 만약 숫자가 중복이 될 경우 자식스레드는 맨 마지막 버퍼에 ERR를 write한다.
// ERR를 read한 부모프로세스는 입력받은 데이터를 삭제한다.
	char arg[3][3];
	sprintf(arg[0], "%d", row);	sprintf(arg[1], "%d", col);	sprintf(arg[2], "%d", x);
	char buffer[SIZE];
	char buffer2[SIZE] = {"ERR"};
	int pipes[2], i;
	int pipes2[2];
	int status;
	pid_t pid[2];
	int mybool = 0;
	int tmt[3];
	int run = 0;

	if(pipe(pipes) == -1) {
		perror("pipe failed");
		exit(1);
	}
	if(pipe(pipes2) ==-1) {
		perror("pipe2 failed");
		exit(1);
	}
	while(run < 2){
		pid[run] = fork();
		if(pid[run] > 0){//부모 프로세스
			printf("\n부모 parent %ld, child %ld\n", (long)getpid(), (long)pid);
			for(i =0; i<3; i++){
				write(pipes[1], arg[i], SIZE);
				printf("parent process write to pipe: arg[%d] = %s\n",i, arg[i]);
				
				if(read(pipes2[0], buffer, SIZE)){
					printf("parent process read from pipe: arg[%d] = %s\n",i, buffer);
					if(i == 2 && *buffer == *buffer2){
					//자식스레드에서 ERR를 전송받았다면 해당 행열의 값을 삭제
						sdq[row-1][col-1] = 0;	
					}
				}
			}
		//sleep(3);
		}
		else if(pid[run] == 0){// 자식스레드 일 때
			printf("자식 parent %ld, child %ld\n", (long)getppid(), (long)getpid());
	
			if(run == 0){
				for(i=0; i<3; i++){
					read(pipes[0], buffer, SIZE);
					printf("child process read from pipe: arg[%d] = %s\n",i, buffer);
					tmt[i] = atoi(buffer);
					if(!rtest(sdq,row)&& i ==2){
						char buffer2[SIZE] = {"ERR"};
						write(pipes2[1],buffer2 , SIZE);//중복이 되었다면 ERR를 전송	
						printf("child process write to pipe:arg[%d] = %s\n",i, buffer);	
						sdq[tmt[0]-1][tmt[1]-1] = 0;						
						mybool = 1;		
					}
					if(mybool == 0){
					write(pipes2[1],buffer , SIZE);
					printf("child process write to pipe:arg[%d] = %s\n",i, buffer);
					}
				}
			}

			if(run == 1){
				for(i=0; i<3; i++){
					read(pipes[0], buffer, SIZE);
					printf("child process read from pipe: arg[%d] = %s\n",i, buffer);
					tmt[i] = atoi(buffer);
					if(!ltest(sdq,col)&& i ==2){
						char buffer2[SIZE] = {"ERR"};
						write(pipes2[1],buffer2 , SIZE);//중복이 되었다면 ERR를 전송	
						printf("child process write to pipe:arg[%d] = %s\n",i, buffer);	
						sdq[tmt[0]-1][tmt[1]-1] = 0;						
						mybool = 1;		
					}
					if(mybool == 0){
					write(pipes2[1],buffer , SIZE);
					printf("child process write to pipe:arg[%d] = %s\n",i, buffer);
					}
				}
			}
			exit(0); //검사를 완료했다면 스레드 종료
		}
		run++;
	}//while end
}
void main(){
	int sdq[9][9]={   {5,3,0,0,7},
		{6,0,0,1,9,5},
		{0,9,8,0,0,0,0,6},
		{8,0,0,0,6,0,0,0,3},
		{4,0,0,8,0,3,0,0,1},
		{7,0,0,0,2,0,0,0,6},
		{0,6,0,0,0,0,2,8},
		{0,0,0,4,1,9,0,0,5},
		{0,0,0,0,8,0,0,7,9}};
	int i,j;
	int num=0;
	int row = 9;
	int col= 9;
	int data = 0;
//입력받은 숫자가 위치한 소격자 검사를 위한 변수들
	int st_row = 0; 
	int st_col =0;
	int end_row = 0;
	int end_col = 0;
	int triangle_num = 0;

	myprint(sdq);

	while(1) {
		printf("============================메뉴============================\n");
		printf("1)추가\n");
		printf("2)삭제\n");
		printf("3)출력\n");
		printf("7)종료\n");
		printf(">>번호를 입력하시오 : ");
		scanf("%d", &num);
		
		switch (num)
		{
		case 1:
			printf("행번호를 입력하세요 : ");
			scanf("%d", &row);
			if( row <1 || row >9){
				printf("ERR :: 1~9사이의 수를 입력하세요\n");
				break;
			}
			printf("열번호를 입력하세요 : ");
			scanf("%d", &col);
			if( col <1 || col >9){
				printf("ERR :: 1~9사이의 수를 입력하세요\n");
				break;
			}
			printf("%d행, %d열에 넣을 숫자를 입력하세요 : ",row,col);
			scanf("%d", &data);

			insert(sdq,data,row,col);
			multi_thread_pipe(sdq,data,row,col);

			//소격자 검사를 위한 조건문
			// 이 조건문을 통하여 입력받은 데이터에 대한 소격자 번호를 알 수 있다.
			if(row >=1 && row <=3){
				st_row = 0;
				end_row =3;
				triangle_num =1;
			}else if(row >=4 && row <=6){
				st_row = 3;
				end_row =6;
				triangle_num =4;
			}else if(row >=7 && row <=9){
				st_row = 6;
				end_row =9;
				triangle_num =7;
			}
			//소격자 검사를 위한 조건문
			if(col >=1 && col <=3){
				st_col = 0;
				end_col =3;
				triangle_num = triangle_num + 0;
			}else if(col >=4 && col <=6){
				st_col = 3;
				end_col =6;
				triangle_num = triangle_num + 1;
			}else if(col >=7 && col <=9){
				st_col = 6;
				end_col =9;
				triangle_num = triangle_num + 2;
			}
			//입력받은 숫자가 위치한 소격자 검사
			if(!small_triangle(sdq,triangle_num,st_row,end_row,st_col,end_col)){
				sdq[row-1][col-1] = 0;						
			}else{
				printf("%d번 소격자 이상없음 \n",triangle_num);
			}
			break;

		case 2://sudoku 배열의 데이터 삭제
			printf("행번호를 입력하세요 : ");
			scanf("%d", &row);
			if( row <1 || row >9){
				printf("ERR :: 1~9사이의 수를 입력하세요\n");
				break;
			}
			printf("열번호를 입력하세요 : ");
			scanf("%d", &col);
			if( col <1 || col >9){
				printf("ERR :: 1~9사이의 수를 입력하세요\n");
				break;
			}
			delete(sdq,row,col);			
			break;

		case 3://sudoku 배열 출력
			myprint(sdq);
			break;

		case 7://프로그램 종료
			printf("프로그램을 종료합니다.\n");
			exit(0);
			exit(1);

			break;
		default:
			printf("Index range ERR\n");
			break;
		}

	}

}


