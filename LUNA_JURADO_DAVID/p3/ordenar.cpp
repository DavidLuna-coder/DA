#include <iostream>
#include <algorithm>
#include <cstring>
void ordenacionInsercion(int *v, int i, int j)
{
    int aux = 0;
    int k;
    for (int l = i + 1; l <= j; l++)
    {
        bool colocado = false;
        int m = l;
        for (k = l - 1; k >= i && !colocado; k--)
        {
            if (v[m] > v[k])
            {
                aux = v[k];
                v[k] = v[m];
                v[m] = aux;
                m--;
            }
        }
    }
}

void fusion(int *v, int *w, int i, int k, int j)
{
    int n = j - i + 1;
    int p = i;
    int q = k + 1;
    for (int l = 0; l < n; l++)
    {
        if (p <= k && (q > j || v[p] > v[q]))
        {
            w[l] = v[p];
            p = p + 1;
        }
        else
        {
            w[l] = v[q];
            q = q + 1;
        }
    }

    for (int l = 0; l < n; l++)
    {
        v[i + l] = w[l];
    }
}
void ordenacionFusion(int *v, int *w, int i, int j)
{
    int n = j - i + 1;
    if (n <= 3)
    {
        ordenacionInsercion(v, i, j);
    }
    else
    {
        int k = i - 1 + n / 2;
        ordenacionFusion(v, w, i, k);
        ordenacionFusion(v, w, k + 1, j);
        fusion(v, w, i, k, j);
    }
}

int pivote(int *v, int i, int j)
{
    int p = i;
    int x = v[i];
    int aux;
    for (int k = i + 1; k <= j; k++)
    {
        if (v[k] >= x)
        {
            p = p + 1;
            aux = v[p];
            v[p] = v[k];
            v[k] = aux;
        }
    }
    v[i] = v[p];
    v[p] = x;
    return p;
}
void ordenacionRapida(int *v, int i, int j)
{
    int n = j - i + 1;
    if (n <= 3)
    {
        ordenacionInsercion(v, i, j);
    }
    else
    {
        int p = pivote(v, i, j);
        ordenacionRapida(v, i, p - 1);
        ordenacionRapida(v, p + 1, j);
    }
}

void display(int *v, int n)
{
    std::cout << "[ ";
    for (size_t i = 0; i < n; i++)
    {
        std::cout << v[i] << " ";
    }
    std::cout << "]" << std::endl;
}

void cajaNegra(int *v, int n, void(*ordenacion)(int*,int,int))
{
    int copia[n];
    int permutaciones[n];
    memcpy(permutaciones, v, n * sizeof(int));
    do
    {
        memcpy(copia, permutaciones, n * sizeof(int));
        ordenacion(copia, 0, n - 1);
        std::cout << "Vector Original\n";
        display(permutaciones, n);
        std::cout << "Ordenado\n";
        display(copia, n);
    } while (std::next_permutation(
        permutaciones, permutaciones + n));
}


// La función prueba todas las permutaciones posibles que existen a partir de un vector ordenada ascendentemente, y las ordena de una en una para comprobar que funciona la función de ordenacion pasada por parámetro
void cajaNegra(int *v, int n, void(*ordenacion)(int*,int*,int,int))
{
    int copia[n];
    int w[n];
    int permutaciones[n];
    memcpy(permutaciones, v, n * sizeof(int));
    do
    {
        memcpy(copia, permutaciones, n * sizeof(int));
        ordenacion(copia, w,0, n - 1);
        std::cout << "Vector Original\n";
        display(permutaciones, n); //La función display muestra un vector por pantalla
        std::cout << "Ordenado\n";
        display(copia, n);
    } while (std::next_permutation(
        permutaciones, permutaciones + n));
}

int main()
{
    int n = 5;
    int numeros[n] = {1, 2, 3, 4, 5};

    cajaNegra(numeros,n,ordenacionRapida);    
    cajaNegra(numeros,n,ordenacionFusion);    

    std::cout << std::endl;

    return 0;
}