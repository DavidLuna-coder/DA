// ###### Config options ################

// #define PRINT_DEFENSE_STRATEGY 1    // generate map images

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

#ifdef PRINT_DEFENSE_STRATEGY
#include "ppm.h"
#endif

#ifdef CUSTOM_RAND_GENERATOR
RAND_TYPE SimpleRandomGenerator::a;
#endif

using namespace Asedio;

class Casilla
{
public:
    Casilla(int i = 0, int j = 0, int val = 0) : row(i), col(j), value(val) {}
    int row, col;
    double value;
};
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

// Asigna un valor a una casilla segun el extractor
double cellValueExtractor(Casilla *C, bool **freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight, List<Object *> obstacles, List<Defense *> defenses)
{
    int centroX = nCellsWidth / 2;
    int centroY = nCellsHeight / 2;

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    Casilla Centro(centroY, centroX);
    Vector3 posCentro = CasillaToCoordenada(Centro, cellWidth, cellHeight);
    Vector3 posCasilla = CasillaToCoordenada(*C, cellWidth, cellHeight);
    double distancia = _distance(posCentro, posCasilla);


    // for (Object *obstacle : obstacles)
    // {
    //     distObs += _distance(obstacle->position, posCasilla);
    // }

    // double distObj = 0;

    // for (Object *obstacle : obstacles)
    // {
    //     distObj += _distance(obstacle->position, posCasilla);
    // }
    if (distancia == 0)
    {
        return 2;
    }
    return 1 / distancia; // implemente aqui la función que asigna valores a las celdas
}

double cellValue(Casilla *C, bool **freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight, List<Object *> obstacles, List<Defense *> defenses)
{
    int centroX = nCellsWidth / 2;
    int centroY = nCellsHeight / 2;
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    double puntuacion = 0;
    Casilla Centro(centroY, centroX);
    Defense *Extractor = defenses.front();
    Vector3 posicionExtractor = Extractor->position;
    Casilla casillaExtractor = coordenadaToCasilla(Coordenada(posicionExtractor.x,posicionExtractor.y), cellWidth,cellHeight);
    double distancia = _distance(posicionExtractor, CasillaToCoordenada(*C,cellWidth,cellHeight));

    if ((std::abs(casillaExtractor.row - C->row) + std::abs(casillaExtractor.col - C->col) == 2))
    {
        return 2;
    }
    
    return 1/distancia; // implemente aqui la función que asigna valores a las celdas
}

void DEF_LIB_EXPORTED placeDefenses(bool **freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight, std::list<Object *> obstacles, std::list<Defense *> defenses)
{

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    int nCasillas = nCellsWidth * nCellsHeight;
    std::list<Casilla> Casillas;
    int n = 0;
    Casilla seleccionada;
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
        C.value = cellValueExtractor(&C, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses);
    }
    //* Ordenar las casillas por valor
    Casillas.sort([](Casilla a, Casilla b) -> bool
                  { return a.value > b.value; });

    //* Tomamos el centro de estracción
    Defense *centroDeExtraccion = defenses.front();
    std::list<Casilla> CasillasExtractor(Casillas);

    while (!CasillasExtractor.empty() && !Colocadas[contador])
    {
        seleccionada = CasillasExtractor.front();

        CasillasExtractor.pop_front();

        if (esFactible(seleccionada, freeCells, cellWidth, cellHeight, mapWidth, mapHeight, obstacles, defenses, *centroDeExtraccion, Colocadas))
        {
            centroDeExtraccion->position = CasillaToCoordenada(seleccionada, cellWidth, cellHeight);
            freeCells[seleccionada.row][seleccionada.col] = false;
            Colocadas[0] = true;
        }
    }

    for (Casilla &C : Casillas)
    {
        C.value = cellValue(&C, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses);
    }

    Casillas.sort([](Casilla a, Casilla b) -> bool
                  { return a.value > b.value; });
    contador = 0;

    for (auto currentDefense = defenses.begin(); currentDefense != defenses.end(); currentDefense++)
    {

        while (!Casillas.empty() && !Colocadas[contador])
        {

            seleccionada = Casillas.front();
            Casillas.pop_front();
            if (esFactible(seleccionada, freeCells, cellWidth, cellHeight, mapWidth, mapHeight, obstacles, defenses, *(*currentDefense), Colocadas))
            {
                (*currentDefense)->position = CasillaToCoordenada(seleccionada, cellWidth, cellHeight);
                Colocadas[contador] = true;
                freeCells[seleccionada.row][seleccionada.col] = false;
            }
        }

        contador++;
    }

    // int maxAttemps = 1000;
    // List<Defense *>::iterator currentDefense = defenses.begin();
    // while (currentDefense != defenses.end() && maxAttemps > 0)
    // {

    //     (*currentDefense)->position.x = ((int)(_RAND2(nCellsWidth))) * cellWidth + cellWidth * 0.5f;
    //     (*currentDefense)->position.y = ((int)(_RAND2(nCellsHeight))) * cellHeight + cellHeight * 0.5f;
    //     (*currentDefense)->position.z = 0;
    //     ++currentDefense;
    // }

#ifdef PRINT_DEFENSE_STRATEGY

    float **cellValues = new float *[nCellsHeight];
    for (int i = 0; i < nCellsHeight; ++i)
    {
        cellValues[i] = new float[nCellsWidth];
        for (int j = 0; j < nCellsWidth; ++j)
        {
            cellValues[i][j] = ((int)(cellValue(i, j))) % 256;
        }
    }
    dPrintMap("strategy.ppm", nCellsHeight, nCellsWidth, cellHeight, cellWidth, freeCells, cellValues, std::list<Defense *>(), true);

    for (int i = 0; i < nCellsHeight; ++i)
        delete[] cellValues[i];
    delete[] cellValues;
    cellValues = NULL;

#endif
}
