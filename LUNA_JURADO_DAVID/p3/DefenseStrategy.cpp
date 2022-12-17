// ###### Config options ################

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"
#include "cronometro.h"
using namespace Asedio;

class Casilla
{
public:
    Casilla(int i = 0, int j = 0, int val = 0) : row(i), col(j), value(val) {}
    int row, col;
    double value;
};
typedef std::pair<Casilla, std::list<Casilla>::iterator> CasillaEnLista;

class Coordenada
{
public:
    Coordenada(int a = 0, int b = 0) : x(a), y(b) {}
    float x, y;
};

Vector3 CasillaToCoordenada(Casilla c, float cellWidth, float cellHeight)
{
    return Vector3((c.col * cellWidth) + cellWidth * 0.5f, (c.row * cellHeight) + cellHeight * 0.5f, 0);
}

Casilla coordenadaToCasilla(const Coordenada &coord, float cellWidth, float cellHeight)
{
    Casilla c;
    c.row = (int)(coord.y * 1.0f) / cellHeight;
    c.col = (int)coord.x * 1.0 / cellWidth;
    return c;
}

// Comprueba si una defensa se ha colocado

bool esFactible(const Casilla casilla, bool **freeCells, double CellWidth, double CellHeight, double mapWidth, double mapHeight, List<Object *> obstacles, List<Defense *> defenses, Defense &defensa, bool *Colocadas)
{
    Vector3 pos = CasillaToCoordenada(casilla, CellWidth, CellHeight);
    int contador = 0;
    Vector3 C(CasillaToCoordenada(casilla, CellWidth, CellHeight));
    if (!freeCells[casilla.row][casilla.col] || C.x + defensa.radio > mapWidth || C.x - defensa.radio < 0 || C.y + defensa.radio > mapHeight || C.y - defensa.radio < 0)
    {
        return false;
    }

    for (Object *currentObstacle : obstacles)
    {

        if (_distance(pos, currentObstacle->position) <= (defensa.radio + currentObstacle->radio))
        {
            return false;
        }
    }

    for (Defense *currentDefense : defenses)
    {

        if (Colocadas[contador] && _distance(pos, currentDefense->position) < (defensa.radio + currentDefense->radio))
        {
            return false;
        }
        contador++;
    }

    return true;
}
float defaultCellValue(int row, int col, bool **freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight, List<Object *> obstacles, List<Defense *> defenses)
{

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;

    Vector3 cellPosition((col * cellWidth) + cellWidth * 0.5f, (row * cellHeight) + cellHeight * 0.5f, 0);

    float val = 0;
    for (List<Object *>::iterator it = obstacles.begin(); it != obstacles.end(); ++it)
    {
        val += _distance(cellPosition, (*it)->position);
    }

    return val;
}

CasillaEnLista seleccion(std::list<Casilla> &Casillas)
{
    Casilla mejorCasilla;
    mejorCasilla = Casillas.front();
    auto bestpos = Casillas.begin();
    // for(Casilla *C : Casillas)
    // {
    //     if(C->value > mejorCasilla->value)
    //     {
    //         mejorCasilla = C;
    //     }
    // }

    for (auto C = Casillas.begin(); C != Casillas.end(); C++)
    {
        if ((*C).value > mejorCasilla.value)
        {
            mejorCasilla = *C;
            bestpos = C;
        }
    }

    return std::make_pair(mejorCasilla, bestpos);
}

void DevoradorSinOrdenar(std::list<Casilla> Casillas, bool **freeCells, double cellWidth, double cellHeight, double mapWidth, double mapHeight, List<Object *> obstacles, List<Defense *> defenses, bool *Colocadas)
{
    int contador = 0;
    Casilla seleccionada;
    CasillaEnLista CasillaSeleccionada;
    for (auto currentDefense = defenses.begin(); currentDefense != defenses.end(); currentDefense++)
    {

        while (!Casillas.empty() && !Colocadas[contador])
        {

            CasillaSeleccionada = seleccion(Casillas);  //* Función de selección
            seleccionada = (CasillaSeleccionada.first); //* Casilla
            Casillas.erase(CasillaSeleccionada.second); //* Eliminamos casilla de la lista

            if (esFactible(seleccionada, freeCells, cellWidth, cellHeight, mapWidth, mapHeight, obstacles, defenses, *(*currentDefense), Colocadas))
            {
                (*currentDefense)->position = CasillaToCoordenada(seleccionada, cellWidth, cellHeight);
                Colocadas[contador] = true;
            }
        }

        contador++;
    }
}

void rellenarVectorConLista(Casilla *vectorCasillas, std::list<Casilla> Casillas)
{
    auto iterador = Casillas.begin();
    for (size_t i = 0; i < Casillas.size(); i++)
    {
        vectorCasillas[i] = *iterador;
        iterador++;
    }
}

void rellenarVectorConLista(std::vector<Casilla> &vectorCasillas, std::list<Casilla> Casillas)
{
    auto iterador = Casillas.begin();
    for (size_t i = 0; i < Casillas.size(); i++)
    {
        vectorCasillas[i] = *iterador;
        iterador++;
    }
}

void ordenacionInsercion(Casilla *v, int i, int j)
{
    Casilla aux;
    for (int l = i + 1; l <= j; l++)
    {
        int m = l;
        bool colocado = 0;
        for (int k = l - 1; k >= 0 && !colocado; k--)
        {
            if (v[m].value > v[k].value)
            {
                aux = v[k];
                v[k] = v[m];
                v[m] = aux;
                m--;
            }
            else
                colocado = true;
        }
    }
}

void fusion(Casilla *v, Casilla *w, int i, int k, int j)
{
    int n = j - i + 1;
    int p = i;
    int q = k + 1;
    for (int l = 0; l < n; l++)
    {
        if (p <= k && (q > j || v[p].value >= v[q].value))
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

void ordenacionFusion(Casilla *vector, Casilla *w, int i, int j)
{
    int n = j - i + 1;
    if (n <= 3)
    {
        ordenacionInsercion(vector, i, j);
    }
    else
    {
        int k = i - 1 + n / 2;
        ordenacionFusion(vector, w, i, k);
        ordenacionFusion(vector, w, k + 1, j);
        fusion(vector, w, i, k, j);
    }
}

void DevoradorOrdenacionFusion(Casilla *vectorCasillas, unsigned tamaño, bool **freeCells, double cellWidth, double cellHeight, double mapWidth, double mapHeight, List<Object *> obstacles, List<Defense *> defenses, bool *Colocadas)
{
    int contador = 0;
    Casilla w[tamaño];
    ordenacionFusion(vectorCasillas, w, 0, tamaño - 1);
    int i = 0;
    Casilla seleccionada;

    for (auto currentDefense = defenses.begin(); currentDefense != defenses.end(); currentDefense++)
    {

        while (i < tamaño && !Colocadas[contador])
        {

            seleccionada = vectorCasillas[i]; //* Función de selección
            i++;

            if (esFactible(seleccionada, freeCells, cellWidth, cellHeight, mapWidth, mapHeight, obstacles, defenses, *(*currentDefense), Colocadas))
            {
                (*currentDefense)->position = CasillaToCoordenada(seleccionada, cellWidth, cellHeight);
                Colocadas[contador] = true;
            }
        }

        contador++;
    }
}

int pivote(Casilla *v, int i, int j)
{
    int p = i;
    Casilla x = v[i];
    Casilla aux;
    for (int k = i + 1; k <= j; k++)
    {
        if (v[k].value >= x.value)
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
void ordenacionRapida(Casilla *v, int i, int j)
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

void DevoradorOrdenacionRapida(Casilla *vectorCasillas, unsigned tamaño, bool **freeCells, double cellWidth, double cellHeight, double mapWidth, double mapHeight, List<Object *> obstacles, List<Defense *> defenses, bool *Colocadas)
{
    int contador = 0;

    ordenacionRapida(vectorCasillas, 0, tamaño - 1);
    int i = 0;
    Casilla seleccionada;

    for (auto currentDefense = defenses.begin(); currentDefense != defenses.end(); currentDefense++)
    {
        while (i < tamaño && !Colocadas[contador])
        {

            seleccionada = vectorCasillas[i]; //* Función de selección
            i++;

            if (esFactible(seleccionada, freeCells, cellWidth, cellHeight, mapWidth, mapHeight, obstacles, defenses, *(*currentDefense), Colocadas))
            {
                (*currentDefense)->position = CasillaToCoordenada(seleccionada, cellWidth, cellHeight);
                Colocadas[contador] = true;
            }
        }

        contador++;
    }
}

void DevoradorMonticulo(std::vector<Casilla> Casillas, unsigned tamaño, bool **freeCells, double cellWidth, double cellHeight, double mapWidth, double mapHeight, List<Object *> obstacles, List<Defense *> &defenses, bool *Colocadas)
{
    int contador = 0;
    std::make_heap(Casillas.begin(), Casillas.end(), [](Casilla a, Casilla b) -> bool
                   { return a.value < b.value; });

    Casilla seleccionada;
    for (auto currentDefense = defenses.begin(); currentDefense != defenses.end(); currentDefense++)
    {

        while (!Casillas.empty() && !Colocadas[contador])
        {

            seleccionada = Casillas.at(0);
            std::pop_heap(Casillas.begin(), Casillas.end(), [](Casilla a, Casilla b)
                          { return a.value < b.value; });
            Casillas.pop_back();
            if (esFactible(seleccionada, freeCells, cellWidth, cellHeight, mapWidth, mapHeight, obstacles, defenses, *(*currentDefense), Colocadas))
            {
                (*currentDefense)->position = CasillaToCoordenada(seleccionada, cellWidth, cellHeight);
                Colocadas[contador] = true;
            }
        }

        contador++;
    }
}
// double CellWidth, double CellHeight, double mapWidth, double mapHeight, List<Object *> obstacles, List<Defense *> defenses, Defense &defensa, bool *Colocadas)
void DEF_LIB_EXPORTED placeDefenses3(bool **freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight, List<Object *> obstacles, List<Defense *> defenses)
{

    cronometro c;
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    int nCasillas = nCellsWidth * nCellsHeight;

    std::list<Casilla> Casillas;
    int n = 0;
    int contador = 0;
    //* Meter todas las casillas en el conjunto de Casillas

    for (size_t i = 0; i < nCellsHeight; i++)
    {
        for (size_t j = 0; j < nCellsWidth; j++)
        {
            Casillas.emplace_back(Casilla(i, j));
        }
    }
    bool Colocadas[Casillas.size()];
    std::fill_n(Colocadas, Casillas.size(), false);
    //*Asignar valor a las celdas para extraer minerales

    for (Casilla &C : Casillas)
    {
        C.value = defaultCellValue(C.row, C.col, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses);
    }

    // TODO Ordenación
    //  Casillas.sort([](Casilla a, Casilla b) -> bool
    //                { return a.value > b.value; });

    std::list<Casilla> CasillasSinOrdenar(Casillas);
    Casilla vectorCasillas[Casillas.size()];
    rellenarVectorConLista(vectorCasillas, Casillas);
    long int r = 0;
    const double e_abs = 0.01;
    double e_rel = 0.001;
    double t;
    // c.activar();
    // //* Devorador
    // do
    // {
    //     DevoradorSinOrdenar(CasillasSinOrdenar, freeCells, cellWidth, cellHeight, mapWidth, mapHeight, obstacles, defenses, Colocadas);
    //     ++r;
    // } while (c.tiempo() < e_abs / e_rel + e_abs);
    // c.parar();
    // double t = c.tiempo() / r;

    // std::cout << t << std::endl;

    // std::fill_n(Colocadas, Casillas.size(), false);

    // rellenarVectorConLista(vectorCasillas, Casillas);

    // c.activar();
    // do
    // {
    //     DevoradorOrdenacionFusion(vectorCasillas, Casillas.size(), freeCells, cellWidth, cellHeight, mapWidth, mapHeight, obstacles, defenses, Colocadas);
    //     ++r;
    // } while (c.tiempo() < e_abs / e_rel + e_abs);
    // c.parar();
    // t = c.tiempo() / r;
    // std::cout << t << std::endl;
    // std::fill_n(Colocadas, Casillas.size(), false);
    // rellenarVectorConLista(vectorCasillas, Casillas);

    // c.activar();
    // do
    // {
    //     DevoradorOrdenacionRapida(vectorCasillas, Casillas.size(), freeCells, cellWidth, cellHeight, mapWidth, mapHeight, obstacles, defenses, Colocadas);
    //     ++r;
    // } while (c.tiempo() < e_abs / e_rel + e_abs);
    // c.parar();
    // t = c.tiempo() / r;

    // std::cout << t << std::endl;

    std::fill_n(Colocadas, Casillas.size(), false);
    std::vector<Casilla> CasillasVector(Casillas.size());

    rellenarVectorConLista(CasillasVector, Casillas);
    c.activar();
    do
    {
        DevoradorMonticulo(CasillasVector, Casillas.size(), freeCells, cellWidth, cellHeight, mapWidth, mapHeight, obstacles, defenses, Colocadas);
        ++r;
    } while (c.tiempo() < e_abs / e_rel + e_abs);
    c.parar();
    t = c.tiempo() / r;

    std::cout << (nCellsWidth * nCellsHeight) << '\t' << c.tiempo() / r << '\t' << c.tiempo() * 2 / r << '\t' << c.tiempo() * 3 / r << '\t' << c.tiempo() * 4 / r << std::endl;

    // float cellWidth = mapWidth / nCellsWidth;
    // float cellHeight = mapHeight / nCellsHeight;

    // cronometro c;
    // long int r = 0;
    // c.activar();
    // do {
    // 	List<Defense*>::iterator currentDefense = defenses.begin();
    // 	while(currentDefense != defenses.end() && maxAttemps > 0) {

    // 		(*currentDefense)->position.x = ((int)(_RAND2(nCellsWidth))) * cellWidth + cellWidth * 0.5f;
    // 		(*currentDefense)->position.y = ((int)(_RAND2(nCellsHeight))) * cellHeight + cellHeight * 0.5f;
    // 		(*currentDefense)->position.z = 0;
    // 		++currentDefense;
    // 	}

    // 	++r;
    // } while(c.tiempo() < 1.0);
    // c.parar();
    // std::cout << (nCellsWidth * nCellsHeight) << '\t' << c.tiempo() / r << '\t' << c.tiempo()*2 / r << '\t' << c.tiempo()*3 / r << '\t' << c.tiempo()*4 / r << std::endl;
}
