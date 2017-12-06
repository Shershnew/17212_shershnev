#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define size_for_qsort 20000

struct file_list{
	char name[12];
	struct file_list * next;
};

void swap(void * arrr, int a, int b){
	int * arr = (int *)arrr;
	int c = arr[a];
	arr[a] = arr[b];
	arr[b] = c;
}

int cmp(void * arrr, int a, int b){
	int *arr = (int *)arrr;
	if(arr[a] > arr[b])
		return 1;
	else if(arr[a] < arr[b])
		return -1;
	else
		return 0;
}

void qsortt(void * arr, int l, int r,
 void (*swap)(void * arrr, int a, int b),
 int (*cmp)(void * arrr, int a, int b)){
	int i = l;
	int j = r;
	if(l < r){
		while(i <= j){
			while(-1 == cmp(arr, i, l)){
				i++;
			}
			while(1 == cmp(arr, j, l)){
				j--;
			}
			if(i <= j){
				swap(arr, i, j);
				i++;
				j--;
			}
		}
	}else return;
	qsortt(arr, l, i-1, swap, cmp);
	qsortt(arr, i, r, swap, cmp);
}

void read(struct file_list ** head, FILE * f, int count){
	*head = (struct file_list *)malloc(sizeof(struct file_list));
	(*head)->next = 0;
	int last = 0;
	int * arr = (int *)malloc(size_for_qsort * sizeof(int));
	int i = 0;
	int stop_read = 0;
	for (i = 0; i < size_for_qsort; ++i){
		if(-1 == fscanf(f, "%d", arr + i)){
			last = i-1;
			stop_read = 1;
			break;
		}
	}
	if(!stop_read){
		last = size_for_qsort - 1;
	}
	qsortt(arr, 0, last, &swap, &cmp);
	for (int j = 0; j < 8; ++j){
		(*head)->name[j] = "sortfile"[j];
	}
	(*head)->name[8] = '0'+ (count/100)%10;
	(*head)->name[9] = '0'+ (count/10)%10;
	(*head)->name[10] = '0'+ (count)%10;
	(*head)->name[11] = '\0';
	FILE * fpr = fopen((*head)->name, "w");
	for (i = 0; i <= last; ++i){
		fprintf(fpr, "%d ", arr[i]);
	}
	fclose(fpr);
	free(arr);
	if(!stop_read){
		read(&((*head)->next), f, count + 1);
	}
}

void merge(struct file_list ** head, int count){
	if(0 == *head){
		return;
	}
	if(0 == (*head)->next){
		return;
	}
	struct file_list * p1 = *head;
	struct file_list * p2 = (*head)->next;
	printf("%d\n", count);
	FILE * f1 = fopen(p1->name, "r");
	FILE * f2 = fopen(p2->name, "r");
	char rezname[12];
	memcpy(rezname, p1->name, 12);
	rezname[2] = '0'+count%10;
	rezname[1] = '0'+(count/10)%10;
	rezname[0] = '0'+(count/100)%10;
	FILE * result = fopen(rezname, "w");
	int a = 0;
	int b = 0;
	int err1 = 0;
	int err2 = 0;
	err1 = fscanf(f1, "%d", &a);
	err2 = fscanf(f2, "%d", &b);
	while(err1 != -1 && err2 != -1){
		if(a <= b){
			fprintf(result, "%d ", a);
			err1 = fscanf(f1, "%d", &a);
		} else{
			fprintf(result, "%d ", b);
			err2 = fscanf(f2, "%d", &b);
		}
	}
	while(-1 != err1){
		fprintf(result, "%d ", a);
		err1 = fscanf(f1, "%d", &a);
	}
	while(-1 != err2){
		fprintf(result, "%d ", b);
		err2 = fscanf(f2, "%d", &b);
	}
	fclose(f1);
	fclose(f2);
	fclose(result);
	remove(p1->name);
	remove(p2->name);
	memcpy(p1->name, rezname, 12);
	p1->next = p2->next;
	free(p2);
	printf("%s %s\n", (*head)->name, (*head)->next->name);
	merge(&((*head)->next), count + 1);
}

void merge_sortt(FILE * f,
 void (*read)(struct file_list ** head, FILE * f, int count)){
	struct file_list * file_base = 0;
	read(&file_base, f, 0);
	int k = 0;
	while(file_base->next != 0){
		merge(&file_base, k);
		k += 100;
	}
}

int main(int argc, char **argv){
	FILE * f = fopen(argv[1], "r");
	if(NULL == f){
		printf("error read\n");
		return -1;
	}
	clock_t start = clock();
	merge_sortt(f, &read);
	clock_t finish = clock();
	printf("Read %f seconds\n", ((float)(finish - start)) / CLOCKS_PER_SEC);
	return 0;
}