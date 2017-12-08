#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define size_for_qsort 100000

struct file_list{
	char name[12];
	struct file_list * next;
};

void swap(void * arrr, int a, int b){
	double * arr = (double *)arrr;
	double c = arr[a];
	arr[a] = arr[b];
	arr[b] = c;
}

int cmp(void * arrr, int a, int b){
	double *arr = (double *)arrr;
	if(arr[a] > arr[b])
		return 1;
	else if(arr[a] < arr[b])
		return -1;
	else
		return 0;
}

int cmp2(void * aa, void * bb){
	double * a = (double *)aa;
	double * b = (double *)bb;
	if(*a > *b)
		return 1;
	else if(*a < *b)
		return -1;
	else
		return 0;
}

int getff(FILE * f, void * a){
	int err = 0;
	err = fscanf(f, "%lf", (double *)a);
	if(-1 != err)
		return 0;
	else
		return 1;
}

void putff(FILE * f, void * a){
	double * r = (double *)a;
	fprintf(f, "%lf ", *r);
}

void qsortt(void * arr, int l, int r,
 void (*swap)(void * arrr, int a, int b),
 int (*cmp)(void * arrr, int a, int b),
 int typesort){
	int i = l;
	int j = r;
	if(l < r){
		while(i <= j){
			while(-1 * typesort == cmp(arr, i, l)){
				i++;
			}
			while(1 * typesort == cmp(arr, j, l)){
				j--;
			}
			if(i <= j){
				swap(arr, i, j);
				i++;
				j--;
			}
		}
	}else return;
	qsortt(arr, l, i-1, swap, cmp, typesort);
	qsortt(arr, i, r, swap, cmp, typesort);
}

void read(struct file_list ** head, FILE * f, int count, int typesort){
	*head = (struct file_list *)malloc(sizeof(struct file_list));
	(*head)->next = 0;
	int last = 0;
	double * arr = (double *)malloc(size_for_qsort * sizeof(double));
	int i = 0;
	int stop_read = 0;
	for (i = 0; i < size_for_qsort; ++i){
		if(-1 == fscanf(f, "%lf", arr + i)){
			last = i-1;
			stop_read = 1;
			break;
		}
	}
	if(!stop_read){
		last = size_for_qsort - 1;
	}
	qsortt(arr, 0, last, &swap, &cmp, typesort);
	for (int j = 0; j < 8; ++j){
		(*head)->name[j] = "sortfile"[j];
	}
	(*head)->name[6] = '0'+ (count/10000)%10;
	(*head)->name[7] = '0'+ (count/1000)%10;
	(*head)->name[8] = '0'+ (count/100)%10;
	(*head)->name[9] = '0'+ (count/10)%10;
	(*head)->name[10] = '0'+ (count)%10;
	(*head)->name[11] = '\0';
	FILE * fpr = fopen((*head)->name, "w");
	for (i = 0; i <= last; ++i){
		fprintf(fpr, "%lf ", arr[i]);
	}
	fclose(fpr);
	free(arr);
	if(!stop_read){
		read(&((*head)->next), f, count + 1, typesort);
	}
}

void merge(struct file_list ** head, int count,
	int (*cmp2)(void * aa, void * bb),
	int (*getff)(FILE * f, void * a),
	void (*putff)(FILE * f, void * a),
	int typesort){

	if(0 == *head){
		return;
	}
	if(0 == (*head)->next){
		return;
	}
	struct file_list * p1 = *head;
	struct file_list * p2 = (*head)->next;
	FILE * f1 = fopen(p1->name, "r");
	FILE * f2 = fopen(p2->name, "r");
	char rezname[12];
	memcpy(rezname, p1->name, 12);
	rezname[4] = '0'+count%10;
	rezname[3] = '0'+(count/10)%10;
	rezname[2] = '0'+(count/100)%10;
	rezname[1] = '0'+(count/1000)%10;
	rezname[0] = '0'+(count/10000)%10;
	FILE * result = fopen(rezname, "w");
	void * a = malloc(sizeof(double));
	void * b = malloc(sizeof(double));
	int err1 = 0;
	int err2 = 0;
	err1 = getff(f1, a);
	err2 = getff(f2, b);
	while(err1 == 0 && err2 == 0){
		if(-1 * typesort == cmp2(a, b)){
			putff(result, a);
			err1 = getff(f1, a);
		} else{
			putff(result, b);
			err2 = getff(f2, b);
		}
	}
	while(0 == err1){
		putff(result, a);
		err1 = getff(f1, a);
	}
	while(0 == err2){
		putff(result, b);
		err2 = getff(f2, b);
	}
	fclose(f1);
	fclose(f2);
	fclose(result);
	remove(p1->name);
	remove(p2->name);
	memcpy(p1->name, rezname, 12);
	p1->next = p2->next;
	free(p2);
	merge(&((*head)->next), count + 1, cmp2, getff, putff, typesort);
}

void merge_sortt(FILE * f, int typesort){
	struct file_list * file_base = 0;
	read(&file_base, f, 0, typesort);
	int k = 0;
	while(file_base->next != 0){
		merge(&file_base, k, &cmp2, &getff, &putff, typesort);
		k += 1000;
	}
}

int main(int argc, char **argv){
	FILE * f = fopen(argv[1], "r");
	if(NULL == f){
		printf("error read\n");
		return -1;
	}
	int typesort = 1;
	if(argv[2][0] == '>'){
		typesort = -1;
	}
	clock_t start = clock();
	merge_sortt(f, typesort);
	clock_t finish = clock();
	printf("Read %f seconds\n", ((float)(finish - start)) / CLOCKS_PER_SEC);
	return 0;
}