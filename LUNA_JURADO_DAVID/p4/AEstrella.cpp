#include <list>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
using std::cout;
class Casilla
{
public:
    int i, j, valor;
    Casilla(int x = 0, int y = 0, int v = 0) : i(x), j(y), valor(v) {}
};

class Nodo
{
public:
    Nodo(Casilla &D, float c, float h) : C(D), coste(c), heurista(h) {}
    Casilla &C;
    float coste;
    float heurista;
};
Nodo &extraeMejor(std::list<Nodo &>);
bool objetivo(Nodo &c);
std::list<Nodo &> getSucesores(Nodo &C);
void actualiza();
int heuristica(Nodo &c);
int coste(Nodo &k, Nodo &j);

void print_list(std::list<int*> v)
{
    cout << "[ ";
    for (auto num : v)
    {
        cout << *num << " ";
    }
    cout << "]\n";
}

bool comp(int* a, int* b)
{
    return *a > *b;
}



std::list<int *>::iterator find(std::list<int *> &Nodes, const int *value)
{
    for (auto it = Nodes.begin(); it != Nodes.end(); it++)
    {
        if (value == *it)
        {
            return it;
        }
    }
    return Nodes.end();
}
int main()
{
    std::list<int*> L;
    int x = 3;
    int y = 4;
    int z = 3;
    int *p = &x;
    int *q = &y;
    int *r = &z;

    L.push_back(p);
    L.push_back(q);
    L.push_back(r);
    L.sort(comp);
    if(find(L,p) != L.end())
    {
        std::cout << "Encontrado :D\n";
    }
    print_list(L);

    L.remove(p);
    if(find(L,p) == L.end())
    {
        std::cout << "No Encontrado :D\n";
    }
    print_list(L);
}
