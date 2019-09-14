// gcc sleepingTA.c -lpthread -o sleepingTA
// ./sleepingTA

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_STUDENT_NUM 10 //可使用椅子的最大數量

//- global param defination ------------------//
int studentPerTime[100];				   // 每個單位時間點是否有學生來的情況 0：沒有學生來 1:有學生來
int numOfChairs = 0;		   // 被佔據椅子初始值
int waitTime = 0, waitingStudents = 0; // 總等待時間，等待的學生數

//設定旗標semaphore ={可不可以坐椅子（椅子鎖）， 學生的sem，TA的sem}
sem_t chairs_mutex,time_mutex, student_sem, TA_sem;

// 時間的flag
int timeFlag = 0;
int allStudent = 0;

//- function prototype -----------------------//
void *TA(void *temp);
void *Student(void *temp);

int main(void)
{
	//- thread defination --------------------//
	// 定義2個thread
	pthread_t TA_id;
	pthread_t student_ids[100]; //可能有100個student來

	//- read input.txt -----------------------//
	FILE *f;
	//FILE * fopen ( const char * filename, const char * mode )
	//"r" : 開啟檔案，供程式讀取文字內容
	f = fopen("input.txt", "r");

	for (int i = 0; i < 100; i++)
	{
		int temp;
		// &temp為返回值
		// %d 代表 十進位數
		fscanf(f, "%d\n", &temp);
		studentPerTime[i] = temp;
	}
	fclose(f);

	//- initialize semaphores ----------------//
	// 初始化旗標semaphore
	// 第二個參數是指定是否要讓其他的行程（process）共用旗標
	// 第三個參數則是設定旗標的初始值。
    // sem > 0
	sem_init(&chairs_mutex, 0, 1); // 鎖，範圍：0-1
	sem_init(&time_mutex, 0, 1);   // 時間鎖，最多1
	sem_init(&student_sem, 0, 0);   //student數量 0-n
	sem_init(&TA_sem, 0,  0);   //TA sem 0-1

	//- create TA thread -----------------//
	pthread_create(&TA_id, NULL, TA, NULL);

	//- create Student thread -----------------//
	//建立Student thread
	for (int i = 0; i < 100; i++)
	{
		if (studentPerTime[i] != 0)
		{
			allStudent++;
			// (id's pointer, thread之屬性, 處理函數之pointer, 帶入函數之參數)
			pthread_create(&student_ids[i], NULL, Student, NULL);
		}
		//近來一次為一個單位時間
		sleep(1);
	}

	printf("Total waiting time = %d\n\n", waitTime);
	double avgWaitTime;
	//計算平均等待時間
	avgWaitTime = (double)waitTime / waitingStudents;
	printf("Avg waiting Time %lf\n", avgWaitTime);
	printf("came studets nuber %d\n", allStudent);

	return 0;
}

void *TA(void *temp)
{
	while (1)
	{
		// wait => sem --
		sem_wait(&student_sem);    //等待學生 -1
		sem_wait(&chairs_mutex); //鎖住座椅

		//在要執行CS的時候要鎖住
		numOfChairs--; //CS
 
		//signal => sem ++
		sem_post(&chairs_mutex); // unlock mutex
		sem_post(&TA_sem);   // free barber

		//TA教學一次要2個單位時間 
		sleep(2);

		//總時間加2
		waitTime += 2;
	}
}

void *Student(void *temp)
{

	//確保只有一個customer嘗試坐椅子
	sem_wait(&chairs_mutex); // lock mutex 等帶其他thread numOfChairs運算完成

	// 不能坐超過10張椅子
	if (numOfChairs < MAX_STUDENT_NUM)
	{
		numOfChairs++;
		waitingStudents++;
		sem_post(&chairs_mutex); // unlock mutex
		sem_post(&student_sem);   // free student
		sem_wait(&TA_sem);   // wait for barber available
 
		sem_wait(&time_mutex); //鎖住座椅
		//每一秒前先確認外面等待學生的總量，也代表他們要等待多久
		waitTime += numOfChairs; //增加這個人總等待的時間
		sem_post(&time_mutex);
		sleep(1);
		sem_wait(&time_mutex);
		waitTime += numOfChairs;
		sem_post(&time_mutex);
		sleep(1);
	}
	else
	{
		sem_post(&chairs_mutex); // unlock mutex
	}
}
