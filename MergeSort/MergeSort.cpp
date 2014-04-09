
#include "stdafx.h"
#include <time.h>
#include <windows.h>


void printArray(int array[], int size)
{
    int i;
    for (i=0; i < size; i++)
        printf("%d ", array[i]);
    printf("\n");
}


void arrayCopy(int a[], int *b, int arraySize)
{
	_asm {

		mov eax, [a] // i = ecx 
		mov ebx, [b] // j = edx
		mov ecx, 0 // i
		mov edi, [arraySize]

	label:	
		//b[j] = a[i]
		mov esi, [eax]
		mov [ebx], esi

		inc ecx

		add eax, 4
		add ebx, 4
			
		cmp ecx, edi
		jl label
	}
}


void mergeASM(int a[], int b[], int lowest, int arrayPivot, int highest)
{

// eax = h | ebx = i = eax | ecx = j = pivot + 1 
	_asm {

		mov eax, [lowest]
		mov ebx, [highest] // sets to low also
		mov ecx, [arrayPivot]

		// load the stack with the input vars so we dont have to call on memory
		push eax // low
		push ebx // high
		push ecx // pivot

		// now set ebx to its proper value
		mov ebx, eax // sets to low 
		inc ecx

		// Set up Arrays
		mov edx, [b] // set to array b[i]
		lea edx, [ebx * 4 + edx]

		mov esi, [a] // set to array a[h]
		mov edi, esi
		lea esi, [eax * 4 + esi]	

		lea edi, [ecx * 4 + edi]

	label_while:		// (h<=pivot)&&(j<=high)

		// if / else (a[h]<=a[j])
		push ebx 
		mov ebx, [esi]

		cmp ebx, [edi] 
		jge label_whileElse // jle large to small // jge small to large

		//b[i] = a[h]  
			mov ebx, [esi]
			mov [edx], ebx

			inc eax // h++
			add esi, 4 // increments a[h]
		
		jmp label_whileEndif // jump when finished with the if section

	label_whileElse:

		//b[i]=a[j]
			mov ebx, [edi]
			mov [edx], ebx

			inc ecx // j++
			add edi, 4 // increments a[j]

	label_whileEndif:

		pop ebx
		inc ebx // i++
		add edx, 4 // increment b[i]

		// While loop comparisons
		cmp eax, [arrayPivot] // if (h<=pivot) continue and skip jump
		jg label_exitWhile

		cmp ecx, [highest] // if (j<=high) keep looping
		jle label_while 
	label_exitWhile:

		pop ebx // pivot

		// if / else (h>pivot)
		cmp eax, ebx
		jg label_outerElse

	//b[i] = a[h]
	label_ifWhile:

			mov ecx, [esi]
			mov [edx], ecx

			inc eax // h++
			add esi, 4 // increments a[h]
			add edx, 4

		cmp eax, ebx
		jle label_ifWhile

		pop ebx // pop high in prep for the next loop

		jmp label_outerEndif // skips the while statement
	label_outerElse:

	//b[i]=a[j]
		pop ebx
	label_elseWhile:

			mov eax, [edi]
			mov [edx], eax

			inc ecx // j++
			add edi, 4 // increments a[h]
			add edx, 4

		cmp ecx, ebx
		jle label_elseWhile
	label_outerEndif:

		pop eax // k = low
		mov edx, [a] // set to array a[k]
		lea edx, [eax * 4 + edx]

		mov ecx, [b] // set to array b[k]
		lea ecx, [eax * 4 + ecx]

	label_While2:

	//a[k] = b[k]		
		mov esi, [ecx]
		mov [edx], esi 

		inc eax // k++
		add edx, 4 // increments a[k]
		add ecx, 4

		cmp eax, ebx
		jle label_While2
	}
}


void mergesortASM(int *a, int*b, int lowIndex, int highIndex)
{
    int pivot;
    if(lowIndex<highIndex)
    {
        pivot=(lowIndex+highIndex)/2;

        mergesortASM(a,b,lowIndex,pivot);
        mergesortASM(a,b,pivot+1,highIndex);
        mergeASM(a,b,lowIndex,pivot,highIndex);
    }
}


int _tmain(int argc, _TCHAR* argv[]) {

	srand((unsigned) time(NULL));

	const int size = 100; // Do not set to over 100,000 overloads the stack and heap
	int a[size]; // load with random values
	int b[size];

	for(int i=0;i<size;i++){
		a[i]=rand()%1000;
	}

	// Code for timer
	unsigned __int64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	double timerFrequency = (1.0/freq);
	unsigned __int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER *)&startTime);

    mergesortASM(a,b,0,size-1);

	unsigned __int64 endTime;
	QueryPerformanceCounter((LARGE_INTEGER *)&endTime);
	double timeDifferenceInMilliseconds = ((endTime-startTime) * timerFrequency);

	if(size<=200){
		printArray(a, size); // Dont print if theres over 200 entries in the array
	}

	printf ("It took %lf microseconds.\n",timeDifferenceInMilliseconds*1000);

return 0;
}


