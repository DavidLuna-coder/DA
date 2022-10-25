#pragma once
#include <vector>

class Objeto
{
    public:
    Objeto(int p,int v): valor(v),peso(p){}
    int valor;
    int peso;
};

const std::vector<Objeto> MochilaDiscreta(const std::vector<Objeto>& Objetos, int capacidad);
