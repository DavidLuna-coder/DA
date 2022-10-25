#include "mochilaDiscreta.hpp"
#include <iostream>
int main()
{
    std::vector<Objeto> Objetos;
    Objetos.push_back(Objeto(1,1));
    Objetos.push_back(Objeto(2,6));
    Objetos.push_back(Objeto(5,18));
    Objetos.push_back(Objeto(6,22));
    Objetos.push_back(Objeto(7,28));

    std::vector<Objeto> Resultado = MochilaDiscreta(Objetos,10);
    for (Objeto o : Resultado)
    {
        std::cout << o.peso << "\t" << o.valor << std::endl;
    }





    return 0;
}