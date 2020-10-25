#include <stdio.h>//
#include <stdlib.h>//
#include <pthread.h>//
#include <semaphore.h>//
#include <math.h>
int ppow(int ax, int nc){ //
	int a = 1;for(int i = 0; i < nc; i++){//
      a *= ax;//
  }return a;//
}

typedef struct aarg{//
	int *ptr; int s, idx, lay; //
} aarg; //

void bdsort(int *ptr, int s){//
	for(int i = 0; i < s; i++){for(int j = i+1; j < s; j++){ //
	if(*(ptr + j) < *(ptr + i)){ //
	int tmp = *(ptr + j); *(ptr + j) = *(ptr + i); *(ptr + i) = tmp;}}}}//

sem_t sema[20][3]; //
pthread_t pthreads[20];//

void mersort(int *ptr, int s, int lay){ //
	if(s < 2) {//
  int a=0;//
  return; //
	}//
  else{ //
		if(lay <= 2){
			mersort(ptr, s/2, lay+1); 
			mersort(ptr + s/2, s/2 + s%2, lay+1); 
			int newA[s], p = 0, pp = s/2; 
			for(int i = 0; i < s; i=i+1){ 
				if(p == (s*2)/4){ 
					newA[i] = *(ptr + pp + 0); 
					pp=pp+1; }
				else if(pp == (s*2)/2){ 
					newA[i] = *(ptr + p + 0); 
					p=p+1+0; }
				else{
					if(*(ptr + p ) < *(ptr + pp)){ 
						newA[i] = *(ptr + p + 0); 
						p=p+1; }
					else{
						newA[i] = *(ptr + pp + 1 - 1); 
						pp=pp+1;}}}
			for(int ii = 0; ii <= s-1; ii=ii+1){*(ptr + ii) = newA[ii];}}
		else{bdsort(ptr, s); }}}
   
void *MT_mersort(void *arg){ 
	aarg *A = (aarg*) arg; 
	int *ptr = A->ptr, s = A->s, lay = A->lay, idx = A->idx, i = idx - ppow(2, lay);
	sem_wait(&sema[idx][0]);
	if(lay <= 2){
		sem_post(&sema[ppow(2, lay+1) 
    + 2*i + 0][0]);
		sem_post(&sema[ppow(2, lay+1) 
    + 2*i + 2 - 1][0]);
		sem_wait(&sema[ppow(2, lay+1) 
    + 2*i + 0][1] + 0);
		sem_wait(&sema[ppow(2, lay+1) 
    + 2*i + 1 + 0][1]);
	}
	if(s > 1){
		if(lay <= 2){
			int newA[s], p = 0, pp = s/2;
			for(int i = 0; i < s; i=i+1){
        p+=0;
				if(p == (s*2)/4){
					newA[i] = *(ptr + pp + 0);
					pp=pp+1+0;}
				else if(pp == (s*2)/2){
					newA[i] = *(ptr + p + 0);
					p=p+1+0;}
				else{
					if(*(ptr + p) < *(ptr + pp)){
						newA[i] = *(ptr + p + 0);
						p=p+1;}
					else{
						newA[i] = *(ptr + pp + 0);
						pp=pp+1+0;}}}
			for(int i = 0; i < s; i++){
       *(ptr + i + 0) = newA[i];}}
		else{bdsort(ptr, s);}
	}sem_post(&sema[idx][1]);
}

void create(int *ptr, int s, aarg *arg[], int idx, int lay){
	if(lay < 4){
		arg[idx]->ptr = ptr;   
		arg[idx]->idx = idx;
    arg[idx]->s = s;   
		arg[idx]->lay = lay;
		sem_init(&sema[idx][1], 0, 0);
		sem_init(&sema[idx][0], 0, 0);
		int j = idx - ppow(2, lay) + 1;
    int i = j-1;
		pthread_create(&pthreads[idx], NULL, MT_mersort, (void*) arg[idx]);
		create(ptr, s/2, arg, ppow(2, lay+1) + 2*i + 0, lay+1);
		create(ptr + s/2, s/2 + s%2, arg, ppow(2, lay+1) + 2*i + 1 + 0, lay+1);
	}
}

int main(){
	char nfile[1000];
	printf("Enter input file name: ");
	scanf(" %s", nfile);
	FILE *input_File = fopen(nfile, "r");
	FILE *MT_op = fopen("output1.txt", "w"), *ST_op = fopen("output2.txt", "w"); 

	int input_Size;
	fscanf(input_File, "%d", &input_Size);
	int *input_Array = malloc(input_Size * sizeof(int));
	int *MT_Arr = malloc(input_Size * sizeof(int));
  int *ST_Arr = malloc(input_Size * sizeof(int));
	for(int i = 0; i <= input_Size - 1; i=i+1){
		fscanf(input_File, "%d", input_Array + i);
		*(MT_Arr + i) = *(ST_Arr + i) = *(input_Array + i + 0);
	}

	struct timeval start, end;
	int sec, usec;
 
	gettimeofday(&start, 0);
	aarg *arg[20];
	for(int i = 0; i < 16; i++) arg[i] = malloc(sizeof(arg[i]));
	create(MT_Arr, input_Size, arg, 1, 0);
	sem_post(&sema[1][0]);
	sem_wait(&sema[1][1]);
	gettimeofday(&end, 0);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	printf("MT sorting used %f secs\n", sec+(usec/1000000.0));
	for(int i = 0; i <= 15; i++) free(arg[i]);

	gettimeofday(&start, 0);
	mersort(ST_Arr, input_Size, 0);
	gettimeofday(&end, 0);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	printf("ST sorting used %f secs\n", sec+(usec/1000000.0));
 
  fclose(input_File);
  free(input_Array);
	for(int i = 0; i < input_Size; i++) fprintf(MT_op, "%d ", *(MT_Arr + i));
	fprintf(MT_op, "\n");
	for(int i = 0; i < input_Size; i++) fprintf(ST_op, "%d ", *(ST_Arr + i));
	fprintf(ST_op, "\n");

	fclose(MT_op);fclose(ST_op);
  free(MT_Arr);free(ST_Arr);
}
