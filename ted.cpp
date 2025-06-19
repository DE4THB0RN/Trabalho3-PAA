using namespace std;


#include <iostream>
#include <vector>
#include <stack>
#include <sstream>

class Node
{
public:
    int index;
    string nome;
    vector<Node*> children;
    Node* pai;
    Node* mais_esq;

    Node(string nome)
    {
        this->nome = nome;
        this->pai = nullptr;
        this->index = -1;
        this->mais_esq = nullptr;
    }
};

class Tree
{
private:
    Node* root;

    void posOrdemRec(Node* no, int* i)
    {
        if (no != nullptr)
        {
            for (int c = 0; c < no->children.size(); c++)
            {
                posOrdemRec(no->children.at(c), i);
            }
            no->index = *i;
            *i = *i + 1;
        }
    }


    void criar_com_string(string s)
    {
        stringstream stream(s);
        string temp;

        stack<Node*> pilha;

        while (stream >> temp)
        {
            if (temp == ")")
            {
                if (!pilha.empty())
                {
                    pilha.pop();
                }
            }
            else
            {
                Node* novo = new Node(temp);

                if (root == nullptr)
                {
                    root = novo;
                }
                else if (!pilha.empty())
                {
                    Node* pai = pilha.top();
                    novo->pai = pai;
                    pai->children.push_back(novo);
                }

                pilha.push(novo);
            }
        }
    }

    void ordena_nome_rec(Node* no)
    {
        if (no != nullptr)
        {
            for (int i = 0; i < no->children.size(); i++)
            {
                ordena_nome_rec(no->children.at(i));
            }
            nomes.push_back(no->nome);
        }
    }

    void mais_esq_rec(Node* no)
    {
        if (no == nullptr)
            return;
        for (int i = 0; i < no->children.size(); i++)
        {
            mais_esq_rec(no->children.at(i));
        }

        if (no->children.size() == 0)
            no->mais_esq = no;
        else
            no->mais_esq = no->children.at(0)->mais_esq;
    }

    void mais_esq()
    {
        mais_esq_rec(root);
    }

    void l_rec(Node* no)
    {
        for (int i = 0; i < no->children.size(); i++)
        {
            l_rec(no->children[i]);
        }
        l.push_back(no->mais_esq->index);
    }

public:
    Tree() : root(nullptr) {}

    vector<int> l;
    vector<int> lr_keyroots;
    vector<string> nomes;

    void indexar()
    {
        int categoria = 1;
        int* i = &categoria;

        posOrdemRec(root, i);
    }

    void ordena_nomes()
    {
        ordena_nome_rec(root);
    }

    void pegar_l()
    {
        mais_esq();
        l_rec(root);
    }

    void pegar_keyroots()
    {
        for (int i = 0; i < l.size(); i++)
        {
            int flag = 0;
            for (int j = i + 1; j < l.size(); j++)
            {
                if (l.at(i) == l.at(j))
                {
                    flag = 1;
                }
            }
            if (flag == 0)
            {
                lr_keyroots.push_back(i + 1);
            }
        }
    }

    Tree(string s)
    {
        criar_com_string(s);
    }
};

vector<vector<int>> TD;

int treedist(vector<int> l1, vector<int> l2, const int i, const int j, Tree* t1, Tree* t2)
{
    vector<vector<int>> forestdist;

    forestdist.resize(i + 1, vector<int>(j + 1));

    int del = 1, insert = 1, rename = 1;

    forestdist[0][0] = 0;

    for (int i1 = l1[i - 1]; i1 <= i; i1++)
    {
        forestdist[i1][0] = forestdist[i1 - 1][0] + del;
    }
    for (int j1 = l2[j - 1]; j1 <= j; j1++)
    {
        forestdist[0][j1] = forestdist[0][j1 - 1] + insert;
    }

    for (int i1 = l1[i - 1]; i1 <= i; i1++)
    {
        for (int j1 = l2[j - 1]; j1 <= j; j1++)
        {

            int i_temp = (l1[i - 1] > i1 - 1) ? 0 : i1 - 1;
            int j_temp = (l2[j - 1] > j1 - 1) ? 0 : j1 - 1;

            if ((l1[i1 - 1] == l1[i - 1]) && (l2[j1 - 1] == l2[j - 1]))
            {

                int custo = (t1->nomes[i1 - 1] == t2->nomes[j1 - 1]) ? 0 : rename;

                forestdist[i1][j1] = min(
                    min(forestdist[i_temp][j1] + del, forestdist[i1][j_temp] + insert),
                    forestdist[i_temp][j_temp] + custo);
                TD[i1][j1] = forestdist[i1][j1];

            }
            else
            {
                int i1_temp = l1[i1 - 1] - 1;
                int j1_temp = l2[j1 - 1] - 1;

                int i_temp2 = (l1[i - 1] > i1_temp) ? 0 : i1_temp;
                int j_temp2 = (l2[j - 1] > j1_temp) ? 0 : j1_temp;

                forestdist[i1][j1] = min(
                    min(forestdist[i_temp][j1] + del, forestdist[i1][j_temp] + insert),
                    forestdist[i_temp2][j_temp2] + TD[i1][j1]);
            }
        }
    }


    return forestdist[i][j];
}

int TED(Tree* t1, Tree* t2)
{
    t1->indexar();
    t1->pegar_l();
    t1->pegar_keyroots();
    t1->ordena_nomes();

    t2->indexar();
    t2->pegar_l();
    t2->pegar_keyroots();
    t2->ordena_nomes();

    vector<int> l1 = t1->l;
    vector<int> lrkr1 = t1->lr_keyroots;

    vector<int> l2 = t2->l;
    vector<int> lrkr2 = t2->lr_keyroots;



    TD.resize(l1.size() + 1, vector<int>(l2.size() + 1));


    for (int i : lrkr1)
    {
        for (int j : lrkr2)
        {
            TD[i][j] = treedist(l1, l2, i, j, t1, t2);
            cout << i << " " << j << " " << TD[i][j] << endl;
        }
    }


    return TD[l1.size()][l2.size()];
}

int main()
{
    Tree* t1 = new Tree("A B ) C ) D E ) ) F ) )");
    Tree* t2 = new Tree("C A ) D E ) ) F B ) ) )");

    int dist = TED(t1, t2);
    cout << "Distância: " << dist;
}