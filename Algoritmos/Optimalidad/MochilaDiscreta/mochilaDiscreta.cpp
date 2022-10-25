#include "mochilaDiscreta.hpp"

const std::vector<Objeto> MochilaDiscreta(const std::vector<Objeto> &Objetos, int capacidad)
{
    int MatrizBeneficio[Objetos.size()][capacidad + 1];
    std::vector<Objeto> Resultado;
    for (int j = 0; j <= capacidad; j++)
    {
        if (Objetos[0].peso > j)
            MatrizBeneficio[0][j] = 0;
        else
            MatrizBeneficio[0][j] = Objetos[0].valor;
    }

    // Construccion de la matriz de beneficios
    for (int i = 1; i < Objetos.size(); i++)
    {
        for (int j = 0; j <= capacidad; j++)
        {
            if (Objetos[i].peso >= capacidad)
            {
                MatrizBeneficio[i][j] = MatrizBeneficio[i - 1][j];
            }
            else
            {
                MatrizBeneficio[i][j] = std::max(MatrizBeneficio[i - 1][j], MatrizBeneficio[i - 1][j - Objetos[i].peso] + Objetos[i].valor);

            }
        }
    }

    int j = capacidad;
    for (int i = Objetos.size() - 1; i >= 0 && j >= 0; i--)
    {
        if(i == 0 && MatrizBeneficio[i][j] == 1)
        {
            Resultado.push_back(Objetos[i]);
        }
        else if(MatrizBeneficio[i][j] != MatrizBeneficio[i-1][j])
        {
            Resultado.push_back(Objetos[i]);
            j -= Objetos[i].peso;
        }

    }

    return Resultado;
}
