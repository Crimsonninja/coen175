//#include <stdlib.h>
//#include <stdio.h>

int search(int arr[], int lo, int hi, int n) {
	int index = -1;
	int i;
	for (i = lo; i < hi; ++ i) {
		if (arr[i] == n) {
			index = i;
		}
	}
	return index;
}

int main(int argc, char *argv[]) {
	int pageSize = atoi(argv[1]);
	int pageRequests = 0;
	int pageFaults = 0;

	int physMem[pageSize];
	int num;
	int numOfElts=0;
	int pointer=0;
	int i;
	while (scanf("%d",&num)==1) {
		
		if (numOfElts > pageSize)
			numOfElts = pageSize;
		pointer = pointer%pageSize;
		if (search(physMem,0,numOfElts,num)==-1) {
			printf("Page being replaced is: %d\n",physMem[pointer]);
			physMem[pointer] = num;
			++pointer;
			++numOfElts;
			++pageFaults;
			
		}
		++pageRequests;
	}
	//printf("The array is:\n");
	//for (i = 0; i < pageSize; ++i) {
	//	printf("%d\n",physMem[i]);
	//}
	printf("Number of page requests: %d\nNumber of page faults: %d\n",pageRequests, pageFaults);

	
}
