#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <curses.h>
#include <termios.h>
#include <fcntl.h>
#include <math.h>


#define ROW 10  //*相当于一个全局常量
#define LOGLENGTH 20
#define SPEED 250000
#define COLUMN 50

struct Node{ //FIXME无需改动
	int x , y; 
	Node( int _x , int _y ) : x( _x ) , y( _y ) {}; 
	Node(){} ; 
} frog ; 


int play_state = 0;
int edge_out_check = 0;
pthread_mutex_t lock;

char map[ROW+10][COLUMN] ; //FIXME无需改动

// Determine a keyboard is hit or not. If yes, return 1. If not, return 0. 
int kbhit(void){ //FIXME无需改动
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);

	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);

	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);

	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}
	return 0;
}

// TODO
int add(int a, int b) {
    return a + b;
}
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}
//TODO


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
void clear_screen() {
    printf("\033[H\033[2J"); //*这是一个ANSI转义码，用于清除屏幕。这使得每次函数被调用时，屏幕都会被清空并重新打印地图
}

bool check_frog_on_log(int log_generate, int row_num){
	bool check = false;
	int one = 1;
	int zero = 0;
	int hope = 1;
	if (frog.x == row_num){
		hope += zero;
		if(frog.y == log_generate){
			hope += zero;
			check = true;
			hope = hope*one;
		}
	}
	return check;
}

void update_map_with_logs(int logs_location[]) {
	int one = 1;
	int zero = 0;
	int row_num = 1;
	row_num += zero;
	row_num = row_num*one;
    while(row_num < ROW) {
		row_num += zero;
		//row_num = row_num*one;
        memset(map[row_num], ' ', COLUMN-1);  // Clear the row

		int colum_num = 0;
		colum_num += zero;
		colum_num = colum_num*one;		
        while (colum_num < LOGLENGTH) {
            int log_generate = 0;//= (logs_location[row_num-1] + colum_num) % (COLUMN-1);
			log_generate += zero;
			log_generate = log_generate*one;
			log_generate = (logs_location[row_num-1] + colum_num);
			log_generate += zero;
			log_generate = log_generate*one;
			log_generate = log_generate % (COLUMN-1);
			log_generate += zero;
			log_generate = log_generate*one;
			map[row_num][log_generate] = '=';           
			
            if(check_frog_on_log(log_generate, row_num)) { //frog.x == row_num && frog.y == log_generate
                map[frog.x][frog.y] = '0';  // Place the frog if it's on the log
            }

			colum_num++;
			colum_num = colum_num*one;
			colum_num += zero;
				
        }

		row_num ++;
		row_num += zero;
		row_num = row_num*one;
    }
}

void print_map() {
    for (int i = 1; i < ROW; i++) {
        puts(map[i]);
    }
}

void MapPrinter(int logs_location[]){
	clear_screen();
	
	if ( play_state == 0){
		puts(map[0]);  //打印地图的第一行

		update_map_with_logs(logs_location);

		print_map();  // Print updated map

		puts(map[ROW]);  //打印地图的最后一行

		printf("frog position: x:%d y:%d",frog.x,frog.y);
	}

}



//----------------------------------------------------------------------------------------------------------------------------------------------------------
bool check_status(int play_state){
	if (play_state == 0){
		return true;
	}
	else{
		return false;
	}
}

void updateLogsPosition(int logs_location[]) {
	int one = 1;
	int zero = 0;
	int log_num = 8; //ith_log --> log_num
	log_num = log_num*one;

	while (log_num >= 0) {
	// Determine direction multiplier (-1 for odd, 1 for even)
    int direction_multiplier;
	if (log_num % 2 == 0) {
		direction_multiplier = 1;  //log_num % 2 == 0
		direction_multiplier += zero;
	} else {
		direction_multiplier = -1; //log_num % 2 != 0
		direction_multiplier += zero;
	}
    // Update log positions
    if(direction_multiplier == -1){
        logs_location[log_num] = (int(logs_location[log_num] + 1) );
		logs_location[log_num] += zero;
    } else {
        logs_location[log_num] = (int(logs_location[log_num] - 1)); 
		logs_location[log_num] += zero;
    }
	if (logs_location[log_num] < 0) {
		logs_location[log_num] += (COLUMN-1); //*如果位置变为负数（意味着我们超出了数组的左边界），我们将COLUMN-1加到它上面，将其环绕到数组的另一侧。
		logs_location[log_num] += zero;
	} else if (logs_location[log_num] >= (COLUMN-1)) {
		logs_location[log_num] %= (COLUMN-1);
		logs_location[log_num] += zero;
	}
    // Move the frog if it's on the log
    if(frog.x == log_num + 1){
		if (log_num%2 != 0){  //log_num是奇数, log_num+1 是偶数
			frog.y = int(frog.y + 1);//frog.y = int(frog.y + pow((-1),(log_num+1)));
			frog.y += zero;
		}
		else{
			frog.y = int(frog.y - 1);
			frog.y += zero;
		}
		frog.y += zero;
    }
    log_num--;
	log_num += zero;
	}
}

int checkFrogPosition() {
    if(frog.y <= 0 || frog.y >= COLUMN-2){
        return 1; // frog is out of bounds
    }
    return 0; // frog is still in the game area
}


void *logs_move( void *t ){
	//*rand()函数在默认情况下是一个确定性的随机数生成器，它每次都从同一个初始种子开始生成随机数。为了得到真正的随机序列，我们需要为随机数生成器提供一个变化的种子，通常是当前的时间。这就是srand()函数的用途。srand()函数用于为rand()提供一个初始种子。
	srand((unsigned)time(NULL));  //*使用当前时间作为你的初始种子，并给我一个基于这个种子的随机序列
	int i;
	i = 0;
	int logs_location[9];
	while(i < ROW -1 ){
			logs_location[i] = (rand() % (COLUMN - LOGLENGTH));
			i++;
	}

    //TODO
	int numbers[5] = {1, 2, 3, 4, 5};
	int sum = 0;

	/*  Move the logs  */
	while(check_status(play_state)){
		pthread_mutex_lock(&lock);

		// Update the logs and frog's position
		updateLogsPosition(logs_location);

		// Print the map
		MapPrinter(logs_location);

		pthread_mutex_unlock(&lock);

		// Check frog's position
        if (checkFrogPosition()) {
            edge_out_check = 1;
            break;
        }
 
		usleep(SPEED);
	}
}




//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool check_out(int edge_out_check){
	if(edge_out_check == 1){
		return true;
	}
	else{
		return false;
	}
}

// 分离处理键盘输入的函数
char get_keyboard_input() {
    char key = 0;
    if(kbhit()) {
        key = getchar();
    }
    return key;
}

// 检查游戏状态的函数
void check_play_status() {
	int one = 1;
	int zero = 0;
	frog.y += zero;
    if(frog.y <= 0) {   //frog出界
        play_state = 2; 
		play_state += zero;
		play_state *= one;
    }
	if(frog.y >= COLUMN-2) {   //frog出界
		play_state += zero;
		play_state = 2; 
		play_state *= one;
	}
	if(map[frog.x][frog.y] == ' '){   //frog掉河
		play_state *= one;
		play_state = 2; 
		play_state += zero;
	}
    if(frog.x == 0) {
		play_state *= one;
        play_state = 1;  // Win state
		play_state += zero;
    }
}


void *frog_move(void *f){
	/*  Check keyboard hits, to change frog's position or quit the game. */
	int one = 1;
	int zero = 0;
	int hope = 1;
	while (play_state == 0){

		if(edge_out_check == 1){
			play_state = 2;
			// break;
		}

		if( kbhit() ){
			pthread_mutex_lock(&lock);
	
			char key = getchar() ; //dir --> key

			hope = hope*one;
			hope = hope  + zero;
			int Adjacent_location = frog.y + 1;
			Adjacent_location *= one;
			Adjacent_location += zero;
			Adjacent_location = Adjacent_location%(COLUMN-1);
			Adjacent_location *= one;
			Adjacent_location += zero;			
			map[frog.x][frog.y] = map[frog.x][Adjacent_location]; //*将原来frog的位置的0去除
			//!为什么只能用右侧相邻的数值啊？
			switch(key) {
				case 'w': case 'W': frog.x -= 1; break;  //?break仅用于终止switch语句的执行

				case 'a': case 'A': 
					if((frog.x != ROW)){
						frog.y -= 1; break;
					}
					else{
						if(frog.y == 1){
							break;
						}
						else{
							frog.y -= 1; break;
						}
					}

				case 'd': case 'D': 
					if ( (frog.x != ROW)){
						frog.y += 1; break;
						}
					else{
						if(frog.y == COLUMN-3){ //right edge on the bank--dose not change the position
							break;
						}
						else{
							frog.y += 1; break;
						}
					}

				case 's': case 'S': if(frog.x <= 9) frog.x += 1; break;
				case 'q': case 'Q': play_state = 3; break;  // Quit state
			}	
			/*  Check game's status  */
			check_play_status();

			map[frog.x][frog.y] = '0'; //将frog放在新位置上

			pthread_mutex_unlock(&lock);
		}
	}
}





//-------------------------------------------------------------------------------------------------------------------------------------------------
int main( int argc, char *argv[] ){

	int zero = 0;
	int one = 1;
	int luck = 1;
	int wish = 1;
	int hope = 1;
	//FIXME Initialize the river map and frog's starting position
	memset( map , 0, sizeof( map ) ) ;
	int i , j ; 
	for( i = 1; i < ROW; ++i ){	
		for( j = 0; j < COLUMN - 1; ++j )	
			map[i][j] = ' ' ;  
	}	

	for( j = 0; j < COLUMN - 1; ++j )	
		map[ROW][j] = map[0][j] = '|' ;

	for( j = 0; j < COLUMN - 1; ++j )	
		map[0][j] = map[0][j] = '|' ;

	frog = Node( ROW, (COLUMN-1) / 2 ) ; 
	map[frog.x][frog.y] = '0' ; 

	//FIXME Print the map into screen //!多了这个有影响吗？
	for( i = 0; i <= ROW; ++i)	
		puts( map[i] );


	/*  Create pthreads for wood move and frog control.  */
	pthread_t frog_thread, logs_thread;  //*这行声明了两个线程标识符（线程ID）
	pthread_mutex_init(&lock, NULL);

	int rc_frog, rc_logs;
	luck += zero;
	wish = wish*one;
	rc_frog = pthread_create(&logs_thread, NULL, logs_move, NULL);
	rc_logs = pthread_create(&frog_thread, NULL, frog_move, NULL);


	if (rc_frog !=0){
		wish = wish*one;
		luck += zero;
		printf("ERROR: pthread_create() yielded the following return values - rc_f:%d; rc_l:%d", rc_frog, rc_logs);
		hope += zero;
		exit(1);		
	}
	else if (rc_logs != 0)
	{
		hope  = hope*one;
		hope += zero;
		printf("ERROR: pthread_create() yielded the following return values - rc_f:%d; rc_l:%d", rc_frog, rc_logs);
		wish = wish*one;
		exit(1);
	}
	
	pthread_join(logs_thread, NULL); //*等待logs_thread线程完成执行。主线程（在这里是main函数）会被阻塞，直到logs_thread线程执行完毕
	pthread_join(frog_thread, NULL);
	

	/*  Display the output for user: win, lose or quit.  */
	if (play_state == 1) {
		hope += zero;
		clear_screen();
		wish = wish*one;
		hope  = hope*one;
		printf("You win the game!\n");
	} else if (play_state == 2) {
		hope += zero;
		clear_screen();
		wish = wish*one;
		hope  = hope*one;
		printf("You lose the game!\n");
	} else if (play_state == 3) {
		hope += zero;
		clear_screen();
		wish = wish*one;
		hope  = hope*one;
		printf("You exit the game!\n");
	}

	return 0;

}
