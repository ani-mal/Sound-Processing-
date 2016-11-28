#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <stdlib.h>


typedef int** mus;

typedef struct{
    mus musica;
    int linhas;
}Song;



double complex* FastFourier(short *val, int n) {

	int i, j, k;
	k=n;
	double complex *coefficients = (double complex*)calloc(n, sizeof(double complex));
	double complex aux;
	for(i=0; i< k; i++){
		for(j=0; j<n ; j++){

			double complex expon = -2.0 * M_PI * ( i / (n *1.0 )) * j * _Complex_I;
			 aux +=  val[j] * cexp(expon);
		}

		aux = (aux / ( k *1.0));
		coefficients[i] = (double complex) ((int) __real__ aux);
	}

	return coefficients;
}

double *magnitude(double complex *coeficientes, int tamanho)
{
	double *value;
	value = (double*)malloc(sizeof(double) * tamanho);
    int i=0;

    for(i=0;i<tamanho;i++)
    {
        value[i]=sqrt((__real__ coeficientes[i])*(__real__ coeficientes[i]) + (__imag__ coeficientes[i])*(__imag__ coeficientes[i]));
    }

    return value;
}




// Esta funcao lera os dados do arquivo de audio
// Param1	nome do arquivo
// Param2	tamanho do arquivo
// Retorno	matriz com dados do arquivo (short*), e o numero de linhas
//
 short**  leraudio(char *nome, int *nr_linhas){   // FUNCIONA ?!!?!

 	short **dados;
 	FILE *arq;
 	int tam, i;

 	arq = fopen(nome, "rb+");
 	fseek(arq, 1, SEEK_END);
 	tam = ftell(arq);

 	*nr_linhas = tam / 32000; // cada linha representara 2 segundos de musica


 	dados = (short **) malloc (sizeof(short *) * (*nr_linhas));
 	for(i=0; i< (*nr_linhas);  i++){
 		dados[i] = (short *)malloc(sizeof(short) * 32000);
 	}

 	i=0;
 	rewind(arq);
 	while(fread(dados[i], sizeof(short ),32000, arq)==1 ){
 		i++;
 	}

 	fclose(arq);

 	return dados;
   }


// Alloca uma matriz de inteiros
// Param1	numero de linhas a serem alocadas
// Retorno	Matriz com n linhas e 12 colunas

mus create(int nr_linhas){													// FUNCIONAAA
	mus m; 	// ponteiro para a msuica m sendo criada
	int i;

	m = (mus)malloc(sizeof(int*)*nr_linhas);	// alocando n linhas de notas
	for(i=0;i<nr_linhas; i++){
		m[i]=(int*)malloc(sizeof(int)*12);	// alocando 12 notas numa linha
	}

	return m;
}

// Calcula o intervalo ( nota ) a partir da frequencia fornecida
// Param1	vetor com as magnitude ordenadas
// Param2	k primeiras magnitude a serem consideradas e calculadas
// Retorno	vetor de inteiros que contem os intervalos (1...12)
int* intervalo(double *magn, int k){								// FUNCIONA
	int i, full, *inter, *aux_inter;
	double aux;
	inter=(int*) malloc(sizeof(int)*k);
	aux_inter=(int*)calloc(12,sizeof(int));

	for(i=0;i<k;i++){
		aux= ((12*log(magn[i]/440))) / log(2);
		full=  round(aux);
		inter[i] = full % 12;
	}

                                                // inter : 0 1  2 0 8 0 0 4 0 3 0 1 0 8 0 7  9 12 11
	for(i=0; i<k; i++){
		aux_inter[inter[i]] = aux_inter[inter[i]] + 1; //  aux_inter : 2 1 1 1 0 0 1 2 1 0 1 1
	}

    return aux_inter;
}


void Insertionsort(int *values, int *aux, int N) {
    int i, j, tmp, tempaux;
    for (j = 1; j < N; j++) {
	tmp = values[j];
	tempaux = aux[j];
	i = j-1;
	while ((i >= 0) && (values[i] < tmp)) {
	      values[i+1] = values[i];
	      aux[i+1] = aux[i];
	      i--;
	}
	values[i+1] = tmp;
	aux[i+1] = tempaux;
    }
}


// Esta funcao verificara qual das Musicas eh mais parecida
// com a musica principal.
// Param1 vetor de struct com as musicas
// Param2 matriz com a musica principal
// PAram3 numero de linhas da musica principal
// Param4 numero de musicas teste

int distancia(Song *song, int **musica, int linha, int nr_testes){   //FUNCIONA

	int *dis, *aux, i, j, k;
	dis = (int*) calloc (nr_testes,sizeof(int));
	aux = (int*) malloc(sizeof(int)*nr_testes);
	for(i=0; i< nr_testes; i++){
        aux[i] = i+1;
	}
	for(j=0;j<nr_testes;j++){
		aux[j] =j;
	}

	for(i=0; i< nr_testes; i++){
		for(j=0; j<linha; j++){
			for(k=0; k<12; k++){
				if(musica[j][k] != song[i].musica[j][k]){
					dis[i]++;
				}
			}
		}
	}

	for(i=0; i< nr_testes; i ++){

		printf(" \n %d", dis[i]);
	}

	Insertionsort(dis, aux, nr_testes);

	return aux[0];
}



void insertion_sort_decrescente(double *magnitude, int tamanho)
 {
    int i, j;
    double temp;

    for (j = 1; j < tamanho; j++)
    {
        temp= (magnitude)[j];
        i = j-1;

        while ((i >= 0) && ((magnitude)[i] < temp))
            {
               (magnitude)[i+1] = (magnitude)[i];
                i--;
            }

        (magnitude)[i+1] = temp;
    }
}


// Esta funcao retornara uma matriz com os valores dos intervalos (quantidade)
// de notas musicais
// Retorno	mus* musica ( matriz)
mus obternotas(int *linhas){
    mus musica;
    char nome[40];
    int tam, i, k, nr_linhas;
    short **dados;
    double complex *coeficiente;
    double *mag;

    scanf("%s", nome);
    k=24; // DESCOBRIR QUAL MELHOR K

    dados = leraudio(nome, &nr_linhas);
    musica = create(nr_linhas);

    for(i=0; i<nr_linhas; i++){

    	  coeficiente = FastFourier(dados[i], 32000); // fast fourier transformate
          mag = magnitude( coeficiente, 32000);
          insertion_sort_decrescente(mag, 32000);
          musica[i] = intervalo(mag, k);

    }
  	*linhas = nr_linhas;
    return musica;
}





int main(int argc, char const *argv[])
{
	int nr, i, linhas, resultado;
	Song *som;
	mus compara;


	scanf("%d", &nr);
	 som = (Song*)malloc(sizeof(Song)*nr);

	for (i = 0; i< nr; i++){

		som[i].musica = obternotas(&linhas); // obtendo os intervalos das musicas
		som[i].linhas = linhas;
	}

	compara = obternotas(&linhas); // obtendo as notas da musica principal

	resultado = distancia(som, compara, linhas, nr);

	printf("%d", resultado);

	//free_em_todo_mundo();

	return 0;
}


