

/* This function serves to search for a specific page
 * given an array.
 */
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
	auto int i;
  const double woot = 4.521;
  float _som3_var3_ = 1.9e-6;

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

	printf("Number of page requests: %d\nNumber of page faults: %d\n",pageRequests, pageFaults);

  printf('Test characters\t\n');	
}
