
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>

double PI;
typedef double complex cplx;

typedef struct{
    int** musica;
    int linhas;
}Song;

typedef struct{
     int distance;
     int pos;
}valores;

int compare (const void * a, const void * b)
{
  return ( *(double*)b - *(double*)a);
}


int compare_struct (const valores* a, const valores* b)
{
  if(a->distance > b->distance)
    return 1;
  if(a->distance < b->distance)
    return -1;
  return 0;
}



void Insertionsort( valores *values, int N) {
    int i, j;
    valores tmp, tempaux;
    for (j = 1; j < N; j++) {
        tmp = values[j];
        i = j-1;

  while ((i >= 0) && (values[i].distance > tmp.distance)) {
        values[i+1] = values[i];
        i--;
  }
  values[i+1] = tmp;
    }
}


// Esta funcao verificara qual das Musicas eh mais parecida
// com a musica principal.
// Param1 vetor de struct com as musicas
// Param2 matriz com a musica principal
// PAram3 numero de linhas da musica principal
// Param4 numero de musicas teste

int distancia(Song *song, int **musica, int linha, int nr_testes, int *lin){   //FUNCIONA

  int  i, j, k;
  valores *dis;
  dis = (valores*) calloc (nr_testes,sizeof(valores));
  for(i=0; i< nr_testes; i++){
        dis[i].pos = i+1;
  }

  for(i=0; i< nr_testes; i++){
    for(j=0; j<linha; j++){
        if(lin[i]==j) // tratando se a matriz teste eh menor a matriz Principal
            break;
      for(k=0; k<12; k++){
        if(musica[j][k] != song[i].musica[j][k]){
          dis[i].distance++;
        }
      }
    }
  }


  Insertionsort(dis, nr_testes);

  //qsort(dis, nr_testes, sizeof(valores), compare_struct);

  return dis[0].pos;
}

int distancia_precisa(Song *song, int **musica, int linha, int nr_testes){   //FUNCIONA

  int  i, j, k;
  valores *dis;
  dis = (valores*) calloc (nr_testes,sizeof(valores));
  for(i=0; i< nr_testes; i++){
        dis[i].pos = i+1;
  }

  for(i=0; i< nr_testes; i++){
    for(j=0; j<linha; j++){
      for(k=0; k<12; k++){
        if(musica[j][k] != song[i].musica[j][k]){
          dis[i].distance++;
          break;
        }
      }
    }
  }


  //Insertionsort(dis, nr_testes);
    qsort(dis, nr_testes, sizeof(valores), compare_struct);

  return dis[0].pos;
}


void insertion_sort_decrescente(double *magnitude, int tamanho)
 {
    int i, j;
    double temp;

    for (j = 1; j < tamanho; j++){
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


int* intervalo(double *magn, int k){
								                              // FUNCIONA
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


double *magnitude(double complex *coeficientes, int tamanho){

	double *value;
  int i=0;

	value = (double*)malloc(sizeof(double) * tamanho);

    for(i=0;i<tamanho;i++){
        value[i]=sqrt((__real__ coeficientes[i])*(__real__ coeficientes[i]) + (__imag__ coeficientes[i])*(__imag__ coeficientes[i]));
    }

    return value;
}

void _fft(cplx buf[], cplx out[], int n, int step)
{
  int i;
	if (step < n) {
		_fft(out, buf, n, step * 2);
		_fft(out + step, buf + step, n, step * 2);


		for (i = 0; i < n; i += (2 * step)) {
			cplx t = cexp(-I * PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}

void fft(cplx buf[], int n)
{
    int i;
	cplx out[n];
	for ( i = 0; i < n; i++) out[i] = buf[i];

	_fft(buf, out, n, 1);
}

// Esta funcao lera os dados do arquivo de audio
// Param1	nome do arquivo
// Param2	tamanho do arquivo
// Retorno	matriz com dados do arquivo (short int*), e o numero de linhas
//
 int**  leraudio(char *nome, int *nr_linhas){   // FUNCIONA ?!!?!

  unsigned short **dados;
  double **magni, *aux_mag;
  FILE *arq;
  int tam, i , j, *gap;
  cplx *auxi;
  int** musica;

  arq = fopen(nome, "rb+");
  fseek(arq, 1, SEEK_END);
  tam = ftell(arq);

  *nr_linhas = tam /(2.0 * 65536) ; // cada linha representara 2 segundos de musica

  auxi = (cplx*)malloc(sizeof(cplx)*65536);

  dados = (unsigned short**) malloc ( sizeof(unsigned short *) * *nr_linhas);
  for(i=0; i< *nr_linhas ;  i++){
    dados[i] = (unsigned short *)malloc(sizeof(unsigned short) * 65536);
  }

  magni=(double*)malloc(sizeof(double)*65536);

  musica = (int**)malloc(sizeof(int*)*(*nr_linhas));
  for(i=0;i< *nr_linhas;i++){
    musica[i] = (int*)malloc(sizeof(int*)*12);
  }

  rewind(arq);
  for(i=0;i< *nr_linhas ; i++){
        if(fread(dados[i], sizeof(unsigned short ),65536, arq)!=0 ){

          for(j=0; j<65536; j++){
             auxi[j] = dados[i][j];

          }

         fft(auxi, 65536);

         aux_mag = magnitude(auxi, 65536);

         qsort(aux_mag, 65536, sizeof(double),compare);

         gap= intervalo(aux_mag, 30000);

         for(j=0; j<12; j++){
            musica[i][j] = gap[j];
         }

    }

    else return musica;
  }

  fclose(arq);
  free(auxi);
  free(aux_mag);
  for(i=0; i< *nr_linhas; i++){
    free(dados[i]);
  }

  free(dados);

     return musica;
 }




   int main(int argc, char const *argv[])
   {
   	  int  **music, *intervalo, t, dist, line, linhas, i, j,l, *nr_linhas;
      char nome[256];
   	  Song *song;

   	  scanf("%d", &t);

   	  song = (Song*)malloc(sizeof(Song)*t);
   	  nr_linhas = (int*)malloc(sizeof(int)*t);

      for(l=0;l< t; l++){
        scanf("%s", nome);
        song[l].musica = leraudio(nome, &line);
        nr_linhas[l] = line;
      }

      scanf("%s", nome);

      music = leraudio(nome, &line);
      dist = distancia(song, music, line, t, nr_linhas);

      printf("%d\n", dist);

      free(song);
      for(i=0;i< line; i++){
        free(music[i]);
      }
      free(music);
      free(nr_linhas);

   	return 0;
   }
