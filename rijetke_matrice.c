#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>

#define MAX 500

int A[250000], IA[250000], JA[250000];
int A1[250000], IA1[250000], JA1[250000];
int A2[250000], IA2[250000], JA2[250000];

// funkcija generisea matrice s nasumicnim vrijednostima, s obzirom na minimalni i maksimalni postotak nula

void create(int m, int n, int mat[m][n], int proc_min, int proc_max)
{
    int range = proc_max-proc_min;
    int i,j,k,l;

    int x = rand() % range + -1;
    int indicator = rand() % 10 + 0;
    if (indicator%2==0)
    {
        x = 0;
    }

    int brojac=0;
    int novi_brojac=0;
    for (i=0; i<m; i++)
    {
        for(j=0; j<n; j++)
        {
            int t;
            if (brojac<=proc_min + x)
            {
                while(1)
                {
                    t = rand() % 2 + 0;
                    if (t==0)
                    {
                        brojac++;
                        mat[i][j] = t;
                        break;
                    }
                }
            }
            if (brojac==proc_min + x)
            {
                novi_brojac = brojac;
            }
            int indicator=0;
            if (novi_brojac>=proc_min + x)
            {
                if (novi_brojac<proc_max)
                {
                    indicator = rand() % 2 + 0;
                    novi_brojac++;
                }
            }
            if (novi_brojac>(proc_min + x) && indicator==1)
            {
                mat[i][j] = 0;
            }
            if (novi_brojac>(proc_min + x) && indicator==0)
            {
                mat[i][j] = 1;
            }

        }
    }

    for (i=0; i<m; i++)
    {
        for(j=0; j<n; j++)
        {
            int k = rand() % (m * n);
            int l = rand() % (m * n);
            int temp = mat[i][j];
            mat[i][j] = mat[k/n][k%n];
            mat[k/n][k%n] = temp;
        }
    }





}
/*funkcija pretvara matricu iz standardnog formata u CSR format
  prolazi kroz elemente matrice i smjesta nenulte vrijednosti u niz A,
 odgovarajucee indekse redova u niz IA i indekse kolona u niz JA. */

void transformToCSR(int m, int n,int l, int mat[m][n], int A[], int IA[], int JA[])
{

    int i,j;
    int k=0;
    for (i=0; i<m; i++)
    {
        for (j=0; j<n; j++)
        {
            if (mat[i][j]!=0)
            {
                A[k] = mat[i][j];
                k++;
            }
        }
    }



    k=0;
    for (i=0; i<m; i++)
    {
        for (j=0; j<n; j++)
        {
            if (mat[i][j] != 0)
            {
                IA[k] = i;
                k++;
            }
        }
    }

    k=0;
    for (i=0; i<m; i++)
    {
        for (j=0; j<n; j++)
        {
            if (mat[i][j] != 0)
            {
                JA[k] = j;
                k++;
            }
        }
    }


}
/*funkcija vrsi sabiranje dvije matrice i rezultat smjesta u matricu C
mjere se vremena izvodjenja operacije sabiranja*/
void sum(int m, int n, int A[m][n], int B[m][n], int C[m][n])
{

    int i,j;

    LARGE_INTEGER tu_freq, tu_start, tu_end;
    __int64 t_ns;
    QueryPerformanceFrequency(&tu_freq);
    QueryPerformanceCounter(&tu_start);

    for(i=0; i<m; i++)
    {
        for(j=0; j<n; j++)
        {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
    QueryPerformanceCounter(&tu_end);
    t_ns = 1000000000ULL * (tu_end.QuadPart - tu_start.QuadPart) / tu_freq.QuadPart;

    printf("Vrijeme sabiranja:%llu ms \n",t_ns/1000000);




}
/*funkcija vrsi mnozenje dvije matrice i rezultat sprema u matricu C
mjere se vremena izvodjenja operacije mnozenja.*/

void product(int m, int n, int p, int A[m][n], int B[n][p], int C[m][p])
{
    int i,j,k;

    LARGE_INTEGER tu_freq, tu_start, tu_end;
    __int64 t_ns;
    QueryPerformanceFrequency(&tu_freq);
    QueryPerformanceCounter(&tu_start);

    for (i=0; i<m; i++)
    {
        for (j=0; j<n; j++)
        {
            C[i][j] = 0;
            for (k=0; k<p; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    QueryPerformanceCounter(&tu_end);
    t_ns = 1000000000ULL * (tu_end.QuadPart - tu_start.QuadPart) / tu_freq.QuadPart;

    printf("Vrijeme trajanja mnozenja:%llu ms\n", t_ns/1000000);
}
/*Ova funkcija vrši zbrajanje dvije matrice u CSR formatu
 rezultat pohranjuje u nizove A, IA i JA
 mjere se vremena izvodjenja operacije sabiranja u CSR formatu.*/
void sum1(int u,int l, int A1[], int IA1[], int JA1[],int o, int A2[], int IA2[], int JA2[],int A[], int IA[], int JA[])
{
    //svaki niz se sabira sa odg
    int i, j, k;
    int nnz1 = l, nnz2 = o; //nnz

    i = j = k = 0;


    LARGE_INTEGER tu_freq, tu_start, tu_end;
    __int64 t_ns;
    QueryPerformanceFrequency(&tu_freq);
    QueryPerformanceCounter(&tu_start);

    while(i < nnz1 && j < nnz2)
    {
        int vrsta1 = IA1[i];
        int vrsta2 = IA2[j];
        if(vrsta1 == vrsta2)
        {
            int kolona1 = JA1[i];
            int kolona2 = JA2[j];
            if(kolona1 == kolona2)
            {
                A[k] = A1[i] + A2[j];
                IA[k] = vrsta1;
                JA[k] = kolona1;
                k++;
                i++;
                j++;
            }
            else if(kolona1 < kolona2)
            {
                A[k] = A1[i];
                IA[k] = vrsta1;
                JA[k] = kolona1;
                k++;
                i++;
            }
            else
            {
                A[k] = A2[j];
                IA[k] = vrsta2;
                JA[k] = kolona2;
                k++;
                j++;
            }
        }
        else if(vrsta1 < vrsta2)
        {
            A[k] = A1[i];
            IA[k] = vrsta1;
            JA[k] = JA1[i];
            k++;
            i++;
        }
        else
        {
            A[k] = A2[j];
            IA[k] = vrsta2;
            JA[k] = JA2[j];
            k++;
            j++;
        }
    }


    while(i < nnz1)
    {
        A[k] = A1[i];
        IA[k] = IA1[i];
        JA[k] = JA1[i];
        i++;
        k++;

    }

    while(j < nnz2)
    {
        A[k] = A2[j];
        IA[k] = IA2[j];
        JA[k] = JA2[j];
        j++;
        k++;

    }
    QueryPerformanceCounter(&tu_end);
    t_ns = 1000000000ULL * (tu_end.QuadPart - tu_start.QuadPart) / tu_freq.QuadPart;

    printf("Vrijeme sabiranja CSR :%llu ms", t_ns/1000000);

}
//ova funkcija broji koliko ima nenultih elemenata u matrici i vraca taj broj

int jedinice(int m,int n, int mat[m][n])
{

    int jedinice=0,i,j;
    for (i=0; i<m; i++)
    {
        for(j=0; j<n; j++)
        {
            if (mat[i][j] != 0)
            {
                jedinice++;
            }


        }
    }

    return jedinice;
}

int main()
{
    srand(time(0));

    //mijanjenje u svrhu testiranja razlicitih vrijednosti
    int m=500,n=500,p=500; //p=n
    int *Amat = (int *)malloc(MAX * MAX * sizeof(int));
    int *Bmat = (int *)malloc(MAX * MAX * sizeof(int));
    int *C = (int *)malloc(MAX * MAX * sizeof(int));


    printf("Matrica[%d][%d]\n", m,n);

    //mijenjenje %nula u svrhu testiranja
    int min=90, maks=98;
    int proc_min = ((m*n) * min) / 100;
    int proc_max = ((m*n) * maks)/ 100;
    printf("%d do %d posto nula.\n" ,min, maks);


    create(m,n,Amat,proc_min,proc_max);
    create(m,n,Bmat,proc_min,proc_max);



    sum(m,n,Amat,Bmat,C);
    int u = jedinice(m,n,C);



        product(m,n,p,Amat,Bmat,C);


    int jed = jedinice(m,n,Amat);
    int nul = jedinice(m,n,Bmat);

    transformToCSR(m,n,jed,Amat,A1,IA1,JA1);
    transformToCSR(m,n,nul,Bmat,A2,IA2,JA2);


    sum1(u,jed,A1,IA1,JA1,nul,A2,IA2,JA2,A,IA,JA);


    free(Amat);
    free(Bmat);
    free(C);

    return 0;

}
