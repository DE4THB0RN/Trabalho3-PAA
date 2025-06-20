#include <iostream>
#include <cstring>
#include <ctime>
using namespace std;

typedef struct No {
    char valor;
    No *filho;
    No *proximoIrmao;
} No;

No* criaNo(char valor);
void liberaArvore(No *raiz);
int contadorDeNos(No *raiz);
const char* transformaEmArvore(const char* str, No** node);
int distanciaFlorestas(No *floresta1, No *floresta2);
int distanciaDeEdicaoRecursiva(No *raiz1, No *raiz2);
int distanciaStringEuler(const char *str1, const char *str2);

int retornaMenor(int a, int b, int c) {
    if (a <= b && a <= c) return a;
    if (b <= a && b <= c) return b;
    return c;
}

No* criaNo(char valor) {
    No *novoNo = new No;
    novoNo->valor = valor;
    novoNo->filho = nullptr;
    novoNo->proximoIrmao = nullptr;
    return novoNo;
}

void liberaArvore(No *raiz) {
    if (raiz == nullptr) return;
    liberaArvore(raiz->filho);
    liberaArvore(raiz->proximoIrmao);
    delete raiz;
}

int contadorDeNos(No *raiz) {
    if (raiz == nullptr) return 0;
    int count = 1;
    No *filho = raiz->filho;
    while (filho != nullptr) {
        count += contadorDeNos(filho);
        filho = filho->proximoIrmao;
    }
    return count;
}

const char* transformaEmArvore(const char* str, No** node) {
    if (*str == '\0' || *str == ')') {
        *node = nullptr;
        return str;
    }

    *node = criaNo(*str);
    const char *ponteiro = str + 1;

    if (*ponteiro == '(') {
        ponteiro++;
        No **filho_ptr = &((*node)->filho);
        while (*ponteiro != ')' && *ponteiro != '\0') {
            ponteiro = transformaEmArvore(ponteiro, filho_ptr);
            filho_ptr = &((*filho_ptr)->proximoIrmao);
        }
        if (*ponteiro == ')') {
            ponteiro++;
        }
    }
    return ponteiro;
}

int distanciaFlorestas(No *floresta1, No *floresta2) {
    int n1 = 0, n2 = 0;
    for (No *atual = floresta1; atual != nullptr; atual = atual->proximoIrmao) n1++;
    for (No *atual = floresta2; atual != nullptr; atual = atual->proximoIrmao) n2++;

    No **f1Array = new No*[n1];
    No **f2Array = new No*[n2];

    int i = 0;
    for (No *atual = floresta1; atual != nullptr; atual = atual->proximoIrmao) f1Array[i++] = atual;
    i = 0;
    for (No *atual = floresta2; atual != nullptr; atual = atual->proximoIrmao) f2Array[i++] = atual;

    int **tabelaProgDinamica = new int*[n1 + 1];
    for (i = 0; i <= n1; i++) {
        tabelaProgDinamica[i] = new int[n2 + 1];
    }

    tabelaProgDinamica[0][0] = 0;
    for (i = 1; i <= n1; i++) {
        tabelaProgDinamica[i][0] = tabelaProgDinamica[i-1][0] + contadorDeNos(f1Array[i-1]);
    }
    for (int j = 1; j <= n2; j++) {
        tabelaProgDinamica[0][j] = tabelaProgDinamica[0][j-1] + contadorDeNos(f2Array[j-1]);
    }

    for (i = 1; i <= n1; i++) {
        for (int j = 1; j <= n2; j++) {
            int custoApagar = tabelaProgDinamica[i-1][j] + contadorDeNos(f1Array[i-1]);
            int custoInserir = tabelaProgDinamica[i][j-1] + contadorDeNos(f2Array[j-1]);
            int custoSub = tabelaProgDinamica[i-1][j-1] + distanciaDeEdicaoRecursiva(f1Array[i-1], f2Array[j-1]);
            tabelaProgDinamica[i][j] = retornaMenor(custoApagar, custoInserir, custoSub);
        }
    }

    int result = tabelaProgDinamica[n1][n2];

    for (i = 0; i <= n1; i++) delete[] tabelaProgDinamica[i];
    delete[] tabelaProgDinamica;
    delete[] f1Array;
    delete[] f2Array;
    return result;
}

int distanciaDeEdicaoRecursiva(No *raiz1, No *raiz2) {
    if (raiz1 == nullptr && raiz2 == nullptr) return 0;
    if (raiz1 == nullptr) return contadorDeNos(raiz2);
    if (raiz2 == nullptr) return contadorDeNos(raiz1);

    int custoRenomear = (raiz1->valor == raiz2->valor) ? 0 : 1;
    int custoMatch = custoRenomear + distanciaFlorestas(raiz1->filho, raiz2->filho);
    int custoApagar = 1 + distanciaFlorestas(raiz1->filho, raiz2);
    int custoInserir = 1 + distanciaFlorestas(raiz1, raiz2->filho);

    return retornaMenor(custoMatch, custoApagar, custoInserir);
}

int distanciaStringEuler(const char *str1, const char *str2) {
    No *raiz1 = nullptr;
    No *raiz2 = nullptr;

    transformaEmArvore(str1, &raiz1);
    transformaEmArvore(str2, &raiz2);

    int distancia = distanciaDeEdicaoRecursiva(raiz1, raiz2);

    liberaArvore(raiz1);
    liberaArvore(raiz2);

    return distancia;
}

int main() {
    char casoBase[] = "A(B(E(K()L())F(M()))C(G()H())D(I()J(P())))";
    char arvore_fina_1[] = "A(B(C(D(E(F(G(H(I(J(K(L(M(N(O(P()))))))))))))))))";
    char arvore_fina_2[] = "Z(Y(X(W(V(U(T(S(R(Q(P(O(N(M(L(K(J(I())))))))))))))))))))";
    char arvore_larga_1[] = "A(B()C()D()E()F()G()H()I()J()K()L()M()N()O()P()Q())";
    char arvore_larga_2[] = "A(B(E()F()G()H()I())C(J()K()L()M()N())D(O()P()Q()R()S()))";
    char arvore_media_1[] = "R(S(W(A()B())X())T(Y(C()D(E())))U(Z())V(F(G()H(I()))))";
    char teste1[] = "d(b(a()c())f(e()g()))";
    char teste2[] = "f(e(x())g())";

    clock_t start, end;
    double cpu_time_used;
    int dist;

    cout << "Iniciando calculos de distancia a partir do caso base:\n'" << casoBase << "'\n\n";

    start = clock();
    dist = distanciaStringEuler(teste1, teste2);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout << "--- Teste da sala ---\n";
    cout << "Distancia: " << dist << "\n";
    cout << "Tempo de execucao: " << cpu_time_used << " segundos\n\n";

    start = clock();
    dist = distanciaStringEuler(casoBase, arvore_fina_1);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout << "--- Arvore Fina 1 ---\n";
    cout << "Distancia: " << dist << "\n";
    cout << "Tempo de execucao: " << cpu_time_used << " segundos\n\n";

    start = clock();
    dist = distanciaStringEuler(casoBase, arvore_fina_2);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout << "--- Arvore Fina 2 ---\n";
    cout << "Distancia: " << dist << "\n";
    cout << "Tempo de execucao: " << cpu_time_used << " segundos\n\n";

    start = clock();
    dist = distanciaStringEuler(casoBase, arvore_larga_1);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout << "--- Arvore Larga 1 ---\n";
    cout << "Distancia: " << dist << "\n";
    cout << "Tempo de execucao: " << cpu_time_used << " segundos\n\n";

    start = clock();
    dist = distanciaStringEuler(casoBase, arvore_larga_2);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout << "--- Arvore Larga 2 ---\n";
    cout << "Distancia: " << dist << "\n";
    cout << "Tempo de execucao: " << cpu_time_used << " segundos\n\n";

    start = clock();
    dist = distanciaStringEuler(casoBase, arvore_media_1);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout << "--- Arvore Media 1 ---\n";
    cout << "Distancia: " << dist << "\n";
    cout << "Tempo de execucao: " << cpu_time_used << " segundos\n\n";

    return 0;
}
