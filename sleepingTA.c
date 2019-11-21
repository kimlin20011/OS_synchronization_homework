// gcc sleepingTA.c -pthread -o sleepingTA
// ./sleepingTA
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_CHAIRS_NUM 10 //實驗室外可坐椅子的最大數量
#define TOTAL_TIME 100	//定義100個單位時間

//- global param defination ------------------//
int studentPerTime[TOTAL_TIME]; // 每個單位時間點是否有學生來的情況 0：沒有學生來 1:有學生來
int numOfOccupiedChairs = 0;	// 被佔據椅子的計數器
double waitingTime = 0;			// 所有學生總等待時間計數器
double avgWaitingTime;

//mutuex
sem_t chairMutex; //調用numOfOccupiedChairs變數時，使用mutex。範圍 0-1，初始值1
sem_t timeMutex;  //調用waitingTime時，使用此mutex。範圍 0-1，初始值1
//semaphore
sem_t availableStudentsSem; //代表等待學生之semaphore。範圍 0-n，初始值0
sem_t TA_sem;				//代表TA之semaphore。範圍 0-1，初始值0

int timeFlag = 0;
int allStudents = 0;	 //100個單位時間內，來的學生的計數器
int waitingStudents = 0; //有進實驗室的學生人數

//- function prototype -----------------------//
void *TA(void *temp);
void *Student(void *temp);

int main(void)
{
	//- thread defination --------------------//
	pthread_t TA_id;
	pthread_t student_ids[TOTAL_TIME]; //可能有100個student來

	//- read input.txt -----------------------//
	FILE *f;
	//FILE * fopen ( const char * filename, const char * mode )
	//"r" : 開啟檔案，供程式讀取文字內容
	f = fopen("input.txt", "r");

	for (int i = 0; i < TOTAL_TIME; i++)
	{
		int temp;
		// &temp為返回值
		// %d 代表 十進位數
		fscanf(f, "%d\n", &temp);
		studentPerTime[i] = temp;
	}
	fclose(f);

	//- initialize semaphores ----------------//
	// 初始化semaphore
	// 第二個參數是指定是否要讓其他的process共用semaphore
	// 第三個參數則是設定semaphore的初始值。
	// sem > 0
	sem_init(&chairMutex, 0, 1);
	sem_init(&timeMutex, 0, 1);
	sem_init(&availableStudentsSem, 0, 0);
	sem_init(&TA_sem, 0, 0);

	//- create TA thread -----------------//
	pthread_create(&TA_id, NULL, TA, NULL);

	//- create Student thread -----------------//
	for (int i = 0; i < TOTAL_TIME; i++)
	{
		timeFlag++;
		printf("time %d \n", timeFlag);

		if (studentPerTime[i] != 0)
		{
			allStudents++;
			printf("Student %d comes.\n", allStudents);
			// pthread_create(id's pointer, thread之屬性, 處理函數之pointer, 帶入函數之參數)
			pthread_create(&student_ids[i], NULL, Student, NULL);
		}
		else
		{
			printf("No student come.\n");
		}
		//將每次學生來的時間設為1毫秒
		usleep(1000);
	}

	//確認每個執行緒執行完成
	for (int i = 0; i < 100; i++)
	{
		if (studentPerTime[i] != 0)
		{
			pthread_join(student_ids[i], NULL);
			printf("student_ids[%d] thread complete\n", i);
		}
	}

	printf("Total waiting time = %lf\n\n", waitingTime);
	//計算平均等待時間
	avgWaitingTime = waitingTime / waitingStudents + 2;
	printf("Number of came studets  %d\n", allStudents);
	printf("Number of waiting studets  %d\n", waitingStudents);
	printf("Avg waiting Time %lf\n", avgWaitingTime);

	return 0;
}

void *TA(void *temp)
{
	while (1)
	{
		// wait => sem --
		sem_wait(&availableStudentsSem); //等待學生 -1
		sem_wait(&chairMutex);			 //鎖住座椅

		//在要執行CS的時候要鎖住
		numOfOccupiedChairs--; //CS

		//signal => sem ++
		sem_post(&TA_sem);	 // free TA
		sem_post(&chairMutex); // unlock mutex

		printf("%s\n", "TA starts teaching.");
		//TA教學一次要2個單位時間
		usleep(2000);
	}
}

void *Student(void *temp)
{

	//確保只有一個customer嘗試坐椅子
	sem_wait(&chairMutex); // lock mutex 等帶其他thread numOfOccupiedChairs運算完成

	if (numOfOccupiedChairs < MAX_CHAIRS_NUM) //檢查實驗室外是否還有多的椅子可以坐
	{
		printf("%s\n", "Student come and occupy a chair.");
		numOfOccupiedChairs++;
		waitingStudents++;  
		sem_post(&availableStudentsSem); // free student
		sem_post(&chairMutex);			 // unlock mutex
		sem_wait(&TA_sem);				 // wait for barber available
		printf("%s\n", "Student come in LAB.");
		sem_wait(&timeMutex); //lock waitingTime
		//每一秒前先確認外面等待學生的總量，也代表他們要等待多久
		waitingTime += numOfOccupiedChairs; //增加這個人總等待的時間
		sem_post(&timeMutex);
		usleep(1000);
		sem_wait(&timeMutex);
		waitingTime += numOfOccupiedChairs;
		sem_post(&timeMutex);
		usleep(1000);
		printf("%s\n", "Finish TA teaching, Student leave.");
	}
	else
	{
		sem_post(&chairMutex); // unlock mutex
		printf("%s\n", "No avaliable chair,Student leave.");
	}
}