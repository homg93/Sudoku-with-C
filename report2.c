#include <stdio.h>
#include <stdlib.h>//exit(1)
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#define TOTALFORK 11 //총 생성해야할 스레드 수
#define SIZE 4


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
void insert(int x[][9],int data,int row, int col){//sudoku표의 원하는 행과 열에 데이터를 입력
	if( data <1 || data >9){
		printf("ERR :: 1~9사이의 수를 입력하세요\n");
	}
	else{
		x[row-1][col-1] = data;
	}
}
void delete(int x[][9],int row, int col){
//sudoku표의 원하는 행과 열의 데이터를 삭제
	x[row-1][col-1] = 0;
	printf("%d 행 %d열의 값이 삭제되었습니다.\n",row, col);

	
}
int rtest(int x[][9]){
// sudoku 행에 대한 중복검사실행 만약 중복이 되었다면 0을 리턴
	int rtest[9] = {1,2,3,4,5,6,7,8,9};
	int i,j, k;
	int mybool =  1;
	for(i = 0; i<9; i ++){
		int save[] = {0,0,0,0,0,0,0,0,0};
		for(j=0; j<9 ; j++){
			for( k =0 ; k < 9 ; k++){
				if(rtest[j] == x[i][k])
					save[j] = save[j] + 1;
				if(save[j] >= 2){
					printf("%d 행에 %d을(를) 중복입력 하였습니다\n", i+1, j+1);
					mybool =0;
				}
			}
		}
	}
	return mybool;
}

int ltest(int x[][9]){
// sudoku 열에 대한 중복검사실행 만약 중복이 되었다면 0을 리턴
	int ltest[9] = {1,2,3,4,5,6,7,8,9};
	int i,j, k;
	int mybool =  1;
	for(i = 0; i<9; i ++){
		int save[] = {0,0,0,0,0,0,0,0,0};
		for(j=0; j<9 ; j++){
	
			for( k =0 ; k < 9 ; k++){
				if(ltest[j] == x[k][i])
					save[j] = save[j] + 1;
				if(save[j] >= 2){
					printf("%d 열에 %d을(를) 중복입력 하였습니다\n", i+1, j+1);
					mybool =0;
				}
			}
		}
	}
	return mybool;
}
// 1~9 소격자 검사 예시
//small_triangle(sdq,1,0,3,0,3)//small_triangle(sdq,2,0,3,3,6)//small_triangle(sdq,3,0,3,6,9)
//small_triangle(sdq,4,3,6,0,3)//small_triangle(sdq,5,3,6,3,6)//small_triangle(sdq,6,3,6,6,9)
//small_triangle(sdq,7,6,9,0,3)//small_triangle(sdq,8,6,9,3,6)//small_triangle(sdq,9,6,9,6,9)
int small_triangle(int x[][9],int num,int rStart,int rEnd,int cStart,int cEnd){
	//소격자에 대한 스도쿠표 중복검사를 진행. 만약 중복이 되었다면 0을 리턴
	//매개변수 설명 : num = 소격자 번호, rStart = 행반복시작값, rEnd = 행반복종료값
	//cStart = 열반복시작값, cEnd = 열반복종료값

	int st1[9] = {1,2,3,4,5,6,7,8,9};
	int i,j, k;
	int mybool =  1;
	int save[] = {0,0,0,0,0,0,0,0,0};
	for(i = 0; i<9; i ++){		
		for(j=rStart; j<rEnd ; j++){
			for( k =cStart ; k < cEnd ; k++){
				if(st1[i] == x[j][k])
					save[i] = save[i] + 1;
				if(save[i] >= 2){
					printf("%d 소격자에 %d을(를) 중복입력 하였습니다\n", num, i+1);
					mybool =0;
				}
			}
		}
	}
	return mybool;
}


int multi_thread2 (int sdq[][9],int x, int row, int col){
//멀티 스레드을 구현한 함수
//스레드 11개를 생성하고 각각의 스레드에서 스도쿠의 행검사, 열검사, 소격자검사를 실행한다.
	int status;
	pid_t pid[TOTALFORK];
	int mybool = 0;
	int run = 0;
	int endvalue = 0;

	while(run < TOTALFORK){
		pid[run] = fork();
		if(pid[run] < 0) {
			exit(1);
			return -1;
		} else if(pid[run] > 0){//부모 프로세스
			printf("[%d]번째 자식생성!! 부모 parent %ld,  id: %ld\n",run+1, (long)getpid(), (long)pid);

			if(mybool == 1){
				sdq[row-1][col-1] = 0;
			}

		} else if (pid[run] == 0) {// 자식스레드 0 일 때
			printf("[%d]번째 자식동작!! parent %ld, child %ld\n",run+1, (long)getppid(), (long)getpid());
			if(run == 0){//행검사하는 자식 스레드
				if(!rtest(sdq)){
					sdq[row-1][col-1] = 0;						
					mybool = 1;		
				}
				if(mybool == 1){
					printf("row2 :: mybool = %d\n", mybool);
				}
			}

			if(run == 1){//열검사하는 자식 스레드
				if(!ltest(sdq)){
					sdq[row-1][col-1] = 0;						
					mybool = 1;		
				}
				if(mybool == 1){
					printf("col2 :: mybool = %d\n", mybool);
				}
			}
			if(run == 2){//1번 소격자를 검사하는 자식 스레드
				if(!small_triangle(sdq,1,0,3,0,3)){
					sdq[row-1][col-1] = 0;						
					mybool = 1;		
				}
			}
			if(run == 3){
				if(!small_triangle(sdq,2,0,3,3,6)){
					sdq[row-1][col-1] = 0;						
					mybool = 1;		
				}
			}
			if(run == 4){
				if(!small_triangle(sdq,3,0,3,6,9)){
					sdq[row-1][col-1] = 0;						
					mybool = 1;		
				}
			}
			if(run == 5){
				if(!small_triangle(sdq,4,3,6,0,3)){
					sdq[row-1][col-1] = 0;						
					mybool = 1;		
				}
			}
			if(run == 6){
				if(!small_triangle(sdq,5,3,6,3,6)){
					sdq[row-1][col-1] = 0;						
					mybool = 1;		
				}
			}
			if(run == 7){
				if(!small_triangle(sdq,6,3,6,6,9)){
					sdq[row-1][col-1] = 0;						
					mybool = 1;		
				}
			}
			if(run == 8){
				if(!small_triangle(sdq,7,6,9,0,3)){
					sdq[row-1][col-1] = 0;						
					mybool = 1;		
				}
			}
			if(run == 9){
				if(!small_triangle(sdq,8,6,9,3,6)){
					sdq[row-1][col-1] = 0;						
					mybool = 1;		
				}
			}
			if(run == 10){
				if(!small_triangle(sdq,9,6,9,6,9)){
					sdq[row-1][col-1] = 0;						
					mybool = 1;		
				}
			}
			if(mybool == 0){
				//printf("검사완료!! 오류 없음\n", mybool);
				endvalue +=1;
			}
			else if(mybool == 1){
				printf("last :: mybool = %d\n", mybool);
			}
			if(endvalue == 10){
				printf("검사완료!! 오류 없음\n");
			}
			sleep(2);//검사를 실시하고 2초 뒤에 스레드 자동 종료
			exit(0);
		}

		run++;
	}
	return mybool;
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


	while(1) {
		myprint(sdq); //sudoku 표 출력
		printf("============================메뉴============================\n");
		printf("1)추가\n");
		printf("2)삭제\n");
		printf("3)출력\n");
		printf("7)종료\n");
		printf(">>실행할 메뉴의 번호를 입력하시오 : ");
		scanf("%d", &num);
		
		switch (num)
		{
		case 1: //sudoku 배열에 데이터 추가
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
			multi_thread2(sdq,data,row,col); // 스레드 11개를 통해 중복검사
			sleep(1);

			break;

		case 2: //sudoku 배열의 데이터 삭제
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


