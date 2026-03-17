#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define s_t uint32_t
#define d_t uint64_t

typedef struct num_t {
	s_t *d;
	uint32_t n;
	uint32_t t;
} num_t;

const d_t b = ((d_t)(1) << (sizeof(s_t) << 3));

s_t min(s_t x,s_t y) {
	if(x<y) return x;
	else return y;
}

s_t max(s_t x,s_t y) {
	if(x>y) return x;
	else return y;
}

void zerar(num_t *x) {
	memset(x->d,0,x->t);
	x->n=0;
}

num_t *criar() {
	num_t *novo=malloc(sizeof(num_t));
	novo->d=malloc(600*sizeof(s_t));
	novo->t=600;
	zerar(novo);
	return novo;
}

void atribuir(num_t *x, num_t *y) {
	zerar(x);
	for(int i=0; i<y->n; i++) {
		x->d[i]=y->d[i];
	}
	x->n=y->n;
}

void ajustar_n(num_t *x) {
	while(x->n!=0 && x->d[x->n-1]==0) x->n--;
}

void destruir(num_t **x) {
	free((*x)->d);
	free(*x);
}

s_t dividir_digito(num_t *q,num_t *u,s_t v) {
	num_t *x=criar();
	x->n=u->n;
	d_t carry=0;
	for(int i=u->n-1; i>=0; i--) {
		d_t temp=carry*b+u->d[i];
		x->d[i]=temp/v;
		carry=temp%v;
	}
	ajustar_n(x);
	atribuir(q,x);
	destruir(&x);
	return carry;
}

void multiplicar_digito(num_t *x,num_t *y,s_t v) {
	num_t *w=criar();
	w->n=min(y->n+1,w->t);
	s_t carry=0;
	d_t temp=0;
	int i;
	for(i=0; i<y->n; i++) {
		temp=(d_t)y->d[i]*v+carry;
		w->d[i]=temp;
		carry=temp/b;
	}
	if(i<w->t) w->d[i]=carry;

	ajustar_n(w);
	atribuir(x,w);
	destruir(&w);
}

void checar_estimativa(d_t* qc, d_t* rc, num_t* x, num_t* y, int32_t i) {
	// qc = b or qc * y[n - 2] > rc * b + x[n - 2 + i]
	if((*qc) == b || (*qc) * y->d[y->n - 2] > (*rc) * b + x->d[y->n - 2 + i]) {
		// Ajustando quociente e resto
		(*qc) = (*qc) - 1;
		(*rc) = (*rc) + y->d[y->n - 1];
	}
}

d_t es(s_t c) {
	if (c>0) return -1;
	else return 0;
}

void subtrair(num_t* w, num_t* u, num_t* v) {
	// InicializaC'C#o de variC!veis
	s_t c = 0;
	d_t wi = 0;
	w->n = max(u->n, v->n);
	// wi = ui - vi + c
	for(uint32_t i = 0; i < w->n; i++) {
		wi = (d_t)(u->d[i]) - (d_t)(v->d[i]) + es(c);
		w->d[i] = wi;
		c = wi / b;
	}
	// Ajuste do carry (extensC#o de sinal) e dC-gitos
	while(c != 0 && w->n < w->t) w->d[w->n++] = c;
	ajustar_n(w);
}

void adicionar(num_t* w, num_t* u, num_t* v) {
	// InicializaC'C#o de variC!veis
	s_t c = 0;
	d_t wi = 0;
	w->n = max(u->n, v->n);
	// wi = ui + vi + c
	for(uint32_t i = 0; i < w->n; i++) {
		wi = (d_t)(u->d[i]) + (d_t)(v->d[i]) + (d_t)(c);
		w->d[i] = wi;
		c = wi / b;
	}
	// Ajuste do carry e dC-gitos
	if(w->n < w->t) w->d[w->n++] = c;
	ajustar_n(w);
}


void deslocar_esquerda(num_t *x,s_t i) {
	for(int j=min(x->n+i,x->t); j>=(int)i; j--) {
		x->d[j]=x->d[j-i];
	}
	for(int j=0; j<i; j++) {
		x->d[j]=0;
	}
	x->n=min(x->n+i,x->t);
	ajustar_n(x);
}

void checar_overflow(num_t* q, num_t* x, num_t* y, int32_t i) {
	// Checagem de overflow da subtraC'C#o
	if(x->d[y->n + 1 + i] != 0) {
		// Reduzir quociente
		q->d[i] = q->d[i] - 1;
		// Adicionar divisor no dividendo
		y->d[y->n] = 0;
		deslocar_esquerda(y, i);
		adicionar(x, x, y);
	}
}

void ajustar_qc_rc(d_t* qc, d_t* rc, num_t* x, num_t* y, int32_t i) {
	// Checando estimativa
	checar_estimativa(qc, rc, x, y, i);
	if((*rc) < b) checar_estimativa(qc, rc, x, y, i);
}


void dividir_numero(num_t* q, num_t* r, num_t* u, num_t* v) {
	// Criando nC:meros auxiliares
	num_t* x = criar();
	num_t* y = criar();
	// NormalizaC'C#o dos nC:meros
	s_t f = b / ((d_t)(v->d[v->n - 1]) + 1);
	multiplicar_digito(x, u, f);
	multiplicar_digito(y, v, f);
	// Inicializando q
	zerar(q);
	// Quantidade de dC-gitos de q
	q->n = u->n - v->n + 1;
	for(int32_t i = q->n - 1; i >= 0; i--) {
		// Calculando e ajustando as estimativas
		d_t xx = (d_t)(x->d[v->n + i]) * b +  (d_t)(x->d[v->n - 1 + i]);
		d_t qc = xx / y->d[v->n - 1], rc = xx % y->d[v->n - 1];
		ajustar_qc_rc(&qc, &rc, x, y, i);
		// Realizando a divisC#o
		multiplicar_digito(r, y, qc);
		deslocar_esquerda(r, i);
		subtrair(x, x, r);
		q->d[i] = qc;
		checar_overflow(q, x, y, i);
	}
	// Calculando o resto
	dividir_digito(r, x, f);
	// Ajuste na quantidade de dC-gitos de q
	ajustar_n(q);
	// Desalocando nC:meros auxiliares
	destruir(&x);
	destruir(&y);
}

void multiplicar(num_t* w, num_t* u, num_t* v) {
	// InicializaC'C#o de variC!veis
	s_t c = 0;
	d_t wij = 0;
	num_t* x = criar();
	x->n = min(u->n + v->n, x->t);
	// wi+j = wi+j + ui * vj + c
	for(uint32_t i = 0, j; i < u->n; c = 0, i++) {
		for(j = 0; j < v->n; j++) {
			wij = (d_t)(x->d[i + j]) + (d_t)(u->d[i]) *(d_t)(v->d[j]) + (d_t)(c);
			x->d[i + j] = wij;
			c = wij / b;
		}
		if(i + j < x->t) x->d[i + j] = c;
	}
	ajustar_n(x);
	atribuir(w, x);
	destruir(&x);
}


void dividir(num_t *q,num_t *r,num_t *u,num_t *v) {
	if(v->n==1) {
		zerar(r);
		r->d[0]=dividir_digito(q,u,v->d[0]);
		r->n=(r->d[0]!=0);

	}
	else if (v->n>u->n) {
		zerar(q);
		atribuir(r,u);
	}
	else dividir_numero(q,r,u,v);
}

void exponenciacao_modular(num_t* v, num_t* u, num_t* k, num_t *mod) {
	// x = u, y = k, v = 1
	num_t* x = criar();
	num_t* y = criar();
	num_t *q=criar();
	num_t *w=criar();
	atribuir(x,u);
	atribuir(y, k);
	zerar(v);
	v->n=1;
	v->d[0]=1;
	// Repete enquanto y > 0
	while(y->n > 0) {
		if(dividir_digito(y, y, 2) == 1) {
			multiplicar(v, v, x);
			atribuir(w,v);
			dividir(q,v,w,mod);
		}
		multiplicar(x, x, x);
		atribuir(w,x);
		dividir(q,x,w,mod);
	}
	destruir(&x);
	destruir(&y);
	destruir(&q);
}

int table1[256];
char table2[16]= {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

const char *int_to_str(num_t *num) {
	static char a[8*8+1];
	char *helper=a;
	for(int i=num->n-1; i>=0; i--) {
		for(int j=7; j>=0; j--) {
			char b=table2[(num->d[i]>>(j*4))&0xF];
			*(helper++)=b;
		}
	}
	*helper='\0';
	return a;
}

void str_to_int(num_t *num,const char * str) {
	zerar(num);
	int tamanho=strlen(str);
	num->n=(tamanho+7)>>3;
	for(int i=num->n-1; i>=0; i--) {
		for(int j=((tamanho-1)%8); j>=0; j--,tamanho--) {
			int k=table1[*(str++)];
			num->d[i]|=k<<((j)*4);
		}
	}
}
//break

uint8_t sbox[256];
uint8_t sbox_inversa[256];
const uint8_t rcon[11] = { 0x86, 0x01, 0x02, 0x04,
                           0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
                         };
typedef struct aes_t {
	uint8_t* c0;
	uint8_t* k;
	uint8_t* ke;
	size_t Nk;
} aes_t;

typedef uint8_t tipo_estado[4][4];

#define ROTL8(x,shift)((uint8_t)((x)<<(shift))|((x)>>(8-(shift))))

void sbox_inicializar(uint8_t sbox[256], uint8_t sbox_inversa[256]) {
	uint8_t p = 1, q = 1;
	do {
		p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);
		q ^= q << 1;
		q ^= q << 2;
		q ^= q << 4;
		q ^= q & 0x80 ? 0x09 : 0;
		uint8_t xformed = q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);
		sbox[p] = xformed^0x63;
	} while (p != 1);
	sbox[0] = 0x63;

	for(int i=0; i<256; i++) {
		sbox_inversa[sbox[i]]=i;
	}
}

uint8_t xtime(uint8_t x)
{
	return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}

void mix_column(uint8_t r[4]) {
    uint8_t a[4];
    uint8_t b[4];
    uint8_t c;
    uint8_t h;

    for (c = 0; c < 4; c++) {
        a[c] = r[c];
        h = r[c] >> 7;
        b[c] = r[c] << 1; 
        b[c] ^= h * 0x1B;
    }
    r[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1];
    r[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2];
    r[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3];
    r[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0];
}

void MixColumns(tipo_estado* state)
{
	uint8_t i;
	for (i = 0; i < 4; i++)
	{
		mix_column((*state)[i]);
	}
}

void AddRoundKey(tipo_estado* state, const uint8_t* RoundKey, uint8_t round)
{
	for (uint8_t i = 0; i < 4; i++) {
		for (uint8_t j = 0; j < 4; j++) {
			(*state)[i][j] ^=RoundKey[(round *16) + (i * 4) + j];
		}
	}
}

void SubBytes(tipo_estado* state)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		for (uint8_t j = 0; j < 4; j++)
		{
			(*state)[j][i] = sbox[((*state)[j][i])];
		}
	}
}

void RotWord(uint8_t* data) {
	// [d1, d2, d3, d0] <- [d0, d1, d2, d3]
	uint8_t rotation[4] = { data[1], data[2], data[3], data[0] };
	memcpy(data, rotation, 4);
}

void SubWord(uint8_t* data) {
	// [sbox[d0], sbox[d1], sbox[d2], sbox[d3]] <- [d0, d1, d2, d3]
	data[0] = sbox[data[0]];
	data[1] = sbox[data[1]];
	data[2] = sbox[data[2]];
	data[3] = sbox[data[3]];
}

void KeyExpansion(uint8_t* RoundKey, const uint8_t* Key, uint8_t Nk)
{
	const uint8_t Nr=Nk+6;
	unsigned i, j, k;
	uint8_t temp[4];

	for (i = 0; i < Nk; ++i) {
		memcpy(RoundKey+i*4,Key+i*4,4);
	}

	for (i = Nk; i < 4 * (Nr + 1); i++)
	{
		memcpy(temp,RoundKey+(i-1)*4,4);
		if (i%Nk== 0) {
			RotWord(temp);
			SubWord(temp);
			temp[0] = temp[0] ^ rcon[i/Nk];
		}
		else if(Nk>6 && i % Nk == 4) {
			SubWord(temp);
		}

		k=(i-Nk)*4;
		for(int l=0; l<4; l++) {
			RoundKey[i*4+l]=RoundKey[k+l] ^temp[l];
		}
	}
}
void ShiftRows(tipo_estado* state) {

	tipo_estado newState;
	memcpy(newState,*state,16);
	for(int i=1; i<4; i++) {
		for(int j=0; j<4; j++) {
			(*state)[j][i]=newState[(j+i)%4][i];
		}
	}
}



void Cipher(tipo_estado* state, const uint8_t* k, size_t Nk)
{
	const uint8_t Nr=Nk+6;

	AddRoundKey(state, k, 0);

	for(uint8_t i = 1; i < Nr; i++) {
		SubBytes(state);
		ShiftRows(state);
		MixColumns(state);
		AddRoundKey(state, k, i);
	}
	SubBytes(state);
	ShiftRows(state);
	AddRoundKey(state, k, Nr);
}

void InvShiftRows(tipo_estado* state)
{
	uint8_t temp;
	tipo_estado newState;
	memcpy(newState,*state,16);
	for(int i=1; i<4; i++) {
		for(int j=0; j<4; j++) {
			(*state)[j][i]=newState[(j-i)&3][i];
		}
	}
}

static void InvSubBytes(tipo_estado* state)
{
	uint8_t i, j;
	for (i = 0; i < 4; ++i)
	{
		for (j = 0; j < 4; ++j)
		{
			(*state)[j][i] = sbox_inversa[(*state)[j][i]];
		}
	}
}

uint8_t Multiply(uint8_t a, uint8_t b) {
	// c(x) = 0, m(x) = x^4 + x^3 + x + 1
	uint8_t c = 0, m = 0x1B;
	// Enquanto b for maior que 0
	while(b > 0) {
		// b C) impar (b[0] = 1) -> c(x) = c(x) + a(x)
		c = c ^ ((b & 1) * a);
		// Multiplica a(x) por 2
		// Overflow (a[7] = 1) -> a(x) mod m(x)
		a = (a << 1) ^ ((a >> 7) * m);
		// Divide b por 2
		b = b >> 1;
	}
	// c(x) = (a(x) + b(x)) mod m(x)
	return c;
}


void InvMixColumns(tipo_estado* state)
{
	int i;
	uint8_t a, b, c, d;
	for (i = 0; i < 4; ++i)
	{
		a = (*state)[i][0];
		b = (*state)[i][1];
		c = (*state)[i][2];
		d = (*state)[i][3];

		(*state)[i][0] = Multiply(14,a) ^ Multiply(11,b) ^ Multiply(13,c) ^ Multiply(9,d);
		(*state)[i][1] = Multiply(9,a) ^ Multiply(14,b) ^ Multiply(11,c) ^ Multiply(13,d);
		(*state)[i][2] = Multiply(13,a) ^ Multiply(9,b) ^ Multiply(14,c) ^ Multiply(11,d);
		(*state)[i][3] = Multiply(11,a) ^ Multiply(13,b) ^ Multiply(9,c) ^ Multiply(14,d);
	}
}

void Decipher(tipo_estado* state, const uint8_t* k,size_t Nk)
{
	const uint8_t Nr=Nk+6;

	AddRoundKey(state, k, Nr);

	for(int8_t i = Nr - 1; i >= 1; i--) {
		InvShiftRows(state);
		InvSubBytes(state);
		AddRoundKey(state, k, i);
		InvMixColumns(state);
	}
	InvShiftRows(state);
	InvSubBytes(state);
	AddRoundKey(state, k, 0);

}


int main(int argc, char *argv[])
{
	for(int i=0; i<16; i++) {
		table1["0123456789ABCDEF"[i]]=i;
	}
	sbox_inicializar(sbox,sbox_inversa);

	aes_t blah;
	blah.k=malloc(50);
	blah.ke=malloc(500);

	FILE *entrada=fopen(argv[1],"r");
	FILE *saida=fopen(argv[2],"w");

	num_t *g=criar();
	num_t *a=criar();
	num_t *b=criar();
	num_t *p=criar();
	num_t *chave=criar();


	int quantasOps;
	fscanf(entrada, "%d ",&quantasOps);
	char *linha=NULL;
	size_t tamanholinha=0;
	char *txt;
	char *textode;
	for(int i=0; i<quantasOps; i++) {
		getline(&linha,&tamanholinha,entrada);
		char *qual=strtok(linha," ");
		if(!strcmp(qual,"dh")) {
			char *pedaco=strtok(NULL," ");
			str_to_int(a,pedaco);
			blah.Nk=a->n;
			pedaco=strtok(NULL," ");
			str_to_int(b,pedaco);
			pedaco=strtok(NULL," ");
			str_to_int(g,pedaco);
			pedaco=strtok(NULL,"\n");
			int test=strlen(pedaco);
			if(!strchr("0123456789ABCDEF",pedaco[test-1])) pedaco[test-1]='\0';
			str_to_int(p,pedaco);
			ajustar_n(p);
			ajustar_n(b);
			ajustar_n(g);
			ajustar_n(a);
			multiplicar(b,b,a);
			exponenciacao_modular(chave,g,b,p);
			chave->n=blah.Nk;
			fprintf(saida,"s=%s\n",int_to_str(chave));

			uint8_t *contador=blah.k;
			for(int j=blah.Nk-1; j>=0; j--) {
				for(int k=3; k>=0; k--) {
					*(contador++) = (chave->d[j]>>(k<<3))&0xFF;
				}
			}
			KeyExpansion(blah.ke,blah.k,blah.Nk);
		}
		else if(!strcmp(qual,"e")) {
			textode=strtok(NULL,"\n");
			int test=strlen(textode);
			if(!strchr("0123456789ABCDEF",textode[test-1])) {
				textode[test-1]='\0';
				test--;
			}
			uint8_t bloco[16];
			char *final=malloc((((test+31)/32)*32+1)*sizeof(char));
			final[((test+31)/32)*32]='\0';
			char criptarBloco[32];

			for(int i=0; i<(test+31)/32; i++) {
				memset(bloco,0,16);
				int j=0;
				if(i==0&& test%32!=0) {
					int k=test%2;
					j=((32-test%32)+1)/2-1;
					if(k) {
						bloco[j]|=table1[*(textode++)];
					}
					j++;
				}
				for(; j<16; j++) {
					bloco[j]|=table1[*(textode++)]<<4;
					bloco[j]|=table1[*(textode++)];
				}
				tipo_estado *state=(tipo_estado *) &bloco;
				Cipher(state,blah.ke,blah.Nk);
				char *auxCriptar=criptarBloco;
				for(int i=0; i<16; i++) {
					*(auxCriptar++)=table2[bloco[i]>>4];
					*(auxCriptar++)=table2[bloco[i]%16];
				}
				memcpy(final+i*32,criptarBloco,32);
			}
			fprintf(saida,"c=%s\n",final);
			free(final);
		}
		else {
			textode=strtok(NULL,"\n");
			int test=strlen(textode);
			if(!strchr("0123456789ABCDEF",textode[test-1])) {
				textode[test-1]='\0';
				test--;
			}
			uint8_t bloco[16];
			char *final=malloc((((test+31)/32)*32+1)*sizeof(char));
			final[((test+31)/32)*32]='\0';
			char decriptarBloco[32];

			for(int i=0; i<(test+31)/32; i++) {
				memset(bloco,0,16);
				int j=0;
				if(i==0 && test%32!=0) {
					int k=test%2;
					j=((32-test%32)+1)/2-1;
					if(k) {
						bloco[j]|=table1[*(textode++)];
					}
					j++;
				}
				for(; j<16; j++) {
					bloco[j]|=table1[*(textode++)]<<4;
					bloco[j]|=table1[*(textode++)];
				}
				tipo_estado *state=(tipo_estado *) &bloco;
				Decipher(state,blah.ke,blah.Nk);
				char *auxDecriptar=decriptarBloco;
				for(int i=0; i<16; i++) {
					*(auxDecriptar++)=table2[bloco[i]>>4];
					*(auxDecriptar++)=table2[bloco[i]%16];
				}
				memcpy(final+i*32,decriptarBloco,32);
			}
			fprintf(saida,"m=%s\n",final);
			free(final);
		}
	}


	destruir(&a);
	destruir(&g);
	destruir(&b);
	destruir(&p);
	destruir(&chave);
	free(blah.k);
	free(blah.ke);
	fclose(entrada);
	fclose(saida);
	return 0;
}