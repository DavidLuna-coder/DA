// ###### Config options ################

//#define PRINT_DEFENSE_STRATEGY 1    // generate map images

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
    Casilla(int i = 0, int j = 0,int valE = 0, int val=0): row(i), col(j), valueExtractor(valE),value(val){}
    int row,col,valueExtractor,value;
};
class Coordenada
{
    public:
    Coordenada(int a = 0, int b = 0): x(a),y(b){}
    float x,y;
};

Coordenada CasillaToCoordenada(Casilla c, float cellWidth, float cellHeight)
{
    Coordenada coord;

    coord.x = (float)(c.row * cellWidth + cellWidth / 2);
    coord.y = (float)(c.col * cellWidth + cellHeight / 2);

    return coord;
}

Casilla coordenadaToCasilla(const Coordenada& coord, float cellWidth, float cellHeight)
{
    Casilla c;
    c.row = (int)coord.x / cellWidth;
    c.col = (int)coord.x / cellWidth;
    return c;
}

float distance(float x1, float y1, float x2, float y2)
{
    float x = std::abs(x1 - x2);
    float y = std::abs(y1 - y2);

    return (std::sqrt(x * x + y * y));
}

//Comprueba si una defensa se ha colocado
bool isPlaced(Defense& d)
{
    return (d.position.x != 0 || d.position.y != 0);
}

bool isPlaced(Defense* d)
{
    return (d->position.x != 0 || d->position.y != 0);
}

bool esFactible(const Casilla& casilla, bool **freeCells, double CellWidth, double CellHeight, double mapWidth, double mapHeight, List<Object *> obstacles, List<Defense *> defenses, const Defense &defensa)
{

    
    Coordenada C = CasillaToCoordenada(casilla,CellWidth,CellHeight);
    if (!freeCells[casilla.row][casilla.col] || C.x + defensa.radio > mapWidth || C.x - defensa.radio < 0 || C.y + defensa.radio > mapHeight || C.y - defensa.radio < 0)
    {
        return false;
    }

    for (Object *currentObstacle : obstacles)
    {
        if (distance(defensa.position.x, defensa.position.y, currentObstacle->position.x, currentObstacle->position.y) <= defensa.radio + currentObstacle->radio)
        {
            return false;
        }
    }

    for (Defense *currentDefense : defenses)
    {
        if (isPlaced(currentDefense) && distance(defensa.position.x, defensa.position.y, currentDefense->position.x, currentDefense->position.y) <= defensa.radio + currentDefense->radio)
        {
            return false;
        }
    }

    return true;
}


//Asigna un valor a una casilla
int cellValueExtractor(Casilla& C, bool **freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight, List<Object *> obstacles, List<Defense *> defenses)
{
    int puntuacion = 0;
    int centroX = nCellsWidth / 2;
    int centroY = nCellsHeight / 2;

    if ((centroX - C.row <= nCellsWidth / 3 && centroX - C.row >= 0))
    {
        puntuacion += 1;
    }

    if ((centroY - C.col <= nCellsHeight / 3 && centroY - C.col >= 0))
    {
        puntuacion+= 1;
    }

    return puntuacion; // implemente aqui la función que asigna valores a las celdas
}

int cellValue(Casilla& C, bool **freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight, List<Object *> obstacles, List<Defense *> defenses)
{
    int puntuacion = 0;
    int centroX = nCellsWidth / 2;
    int centroY = nCellsHeight / 2;

    if ((centroX - C.row <= nCellsWidth / 3 && centroX - C.row >= 0))
    {
        puntuacion += 1;
    }

    if ((centroY - C.col <= nCellsHeight / 3 && centroY - C.col >= 0))
    {
        puntuacion+= 1;
    }

    return puntuacion; // implemente aqui la función que asigna valores a las celdas
}

void DEF_LIB_EXPORTED placeDefenses(bool **freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight, std::list<Object *> obstacles, std::list<Defense *> defenses)
{

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    int nCasillas = nCellsWidth * nCellsHeight;
    std::list<Casilla&> Casillas;
    int n = 0;
    //* Meter todas las casillas en el conjunto de Casillas 
    for (size_t i = 0; i < nCellsWidth; i++)
    {
        for (size_t j = 0; j < nCellsHeight; j++)
        {
            Casillas.emplace_back(Casilla(i,j));
        }
    }
    
    //Asignar valor a las celdas para extraer minerales
    for(Casilla C:Casillas)
    {
        C.valueExtractor = cellValueExtractor(C,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses);
        C.value = cellValue(C,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses);
    }

    Casillas.sort([] (Casilla& a, Casilla& b) -> bool {return a.valueExtractor > b.valueExtractor;} );
    

    Defense* centroDeExtraccion = *(defenses.begin());
    std::list<Casilla&> CasillasExtractor = Casillas;
    Casilla seleccionada;
    while (!isPlaced(centroDeExtraccion) && !CasillasExtractor.empty())
    {
        Casilla& seleccionada = *(CasillasExtractor.begin());
        if (esFactible(seleccionada,freeCells,cellWidth,cellHeight,mapWidth,mapHeight,obstacles,defenses,*centroDeExtraccion))
        {
            Coordenada c = CasillaToCoordenada(seleccionada,cellWidth,cellHeight);
            centroDeExtraccion->position.x = c.x;
            centroDeExtraccion->position.y = c.y; 
            CasillasExtractor.erase(CasillasExtractor.begin());
        }
        
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
