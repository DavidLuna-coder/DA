// ###### Config options ################

#define PRINT_PATHS 1

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

#ifdef PRINT_PATHS
#include "ppm.h"
#endif

using namespace Asedio;

Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight)
{
    return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0);
}

void DEF_LIB_EXPORTED calculateAdditionalCost(float **additionalCost, int cellsWidth, int cellsHeight, float mapWidth, float mapHeight, List<Object *> obstacles, List<Defense *> defenses)
{

    float cellWidth = mapWidth / cellsWidth;
    float cellHeight = mapHeight / cellsHeight;

    for (int i = 0; i < cellsHeight; ++i)
    {
        for (int j = 0; j < cellsWidth; ++j)
        {
            Vector3 cellPosition = cellCenterToPosition(i, j, cellWidth, cellHeight);
            float cost = 0;
            if ((i + j) % 2 == 0)
            {
                cost = cellWidth * 100;
            }

            additionalCost[i][j] = cost;
        }
    }
}

bool NodeCompare(AStarNode *A, AStarNode *B) { return A->F < B->F; }
std::list<AStarNode *>::const_iterator find(const std::list<AStarNode *> &Nodes, const AStarNode *value)
{
    std::list<AStarNode *>::const_iterator it = Nodes.begin();
    for (it = Nodes.begin(); it != Nodes.end(); it++)
    {
        if (value == *it)
        {
            return it;
        }
    }
    return it;
}

const std::list<Vector3> recuperaCamino(AStarNode *origen, AStarNode *destino)
{
    AStarNode *currentNode = destino;
    std::list<Vector3> path;
    while (currentNode != origen)
    {
        path.push_front(currentNode->position);
        currentNode = currentNode->parent;
    }
    path.push_front(origen->position);

    return path;
}

void printPath(std::list<Vector3> &path)
{
    for (auto posicion : path)
    {
        std::cout << posicion.x << "\t" << posicion.y << "\t" << posicion.z << "\n";
    }
}

void DEF_LIB_EXPORTED calculatePath(AStarNode *originNode, AStarNode *targetNode, int cellsWidth, int cellsHeight, float mapWidth, float mapHeight, float **additionalCost, std::list<Vector3> &path)
{
    std::list<AStarNode *> Abierta;
    int maxIter = 100;
    originNode->G = 0;
    originNode->H = _sdistance(originNode->position, targetNode->position);
    originNode->F = originNode->G + originNode->H;
    Abierta.push_back(originNode);
    std::list<AStarNode *> Cerrada;
    bool fin = false;
    while (!fin && !Abierta.empty() && maxIter > 0)
    {
        Abierta.sort(NodeCompare);
        AStarNode *Actual = Abierta.front();
        Abierta.pop_front();
        Cerrada.push_back(Actual);
        if (Actual == targetNode)
        {
            path = recuperaCamino(originNode, targetNode);
            fin = true;
        }
        else
        {
            for (auto it = Actual->adjacents.begin(); it != Actual->adjacents.end(); it++)
            {
                float coste = Actual->G + _sdistance(Actual->position, (*it)->position);
                auto posAbierta = find(Abierta, *it);
                auto posCerrada = find(Cerrada, *it);
                if (coste < (*it)->G)
                {
                    if (posAbierta != Abierta.end())
                    {
                        Abierta.erase(posAbierta);
                    }
                    if (posCerrada != Cerrada.end())
                    {
                        Cerrada.erase(posCerrada);
                    }
                }

                if (posAbierta == Abierta.end() && posCerrada == Cerrada.end())
                {
                    (*it)->G = coste;
                    (*it)->H = _sdistance((*it)->position, targetNode->position);
                    (*it)->F = (*it)->H + (*it)->G;
                    (*it)->parent = Actual;

                    Abierta.push_back(*it);
                }
            }
        }
        --maxIter;
    }
}

// void DEF_LIB_EXPORTED calculatePath(AStarNode *originNode, AStarNode *targetNode, int cellsWidth, int cellsHeight, float mapWidth, float mapHeight, float **additionalCost, std::list<Vector3> &path)
// {
//     std::list<AStarNode *> Abierta;
//     std::list<AStarNode *> Cerrada;
//     originNode->G = 0;
//     originNode->H = _distance(originNode->position, targetNode->position);
//     originNode->F = originNode->G + originNode->H;
//     Abierta.push_back(originNode);
//     while (path.empty() && !Abierta.empty())
//     {

//         Abierta.sort(NodeCompare);
//         AStarNode *current = Abierta.front();
//         Abierta.pop_front();
//         Cerrada.push_back(current);
//         if (current == targetNode)
//         {
//             std::cout << "ACABA";
//             path = recuperaCamino(originNode, targetNode);
//         }
//         else
//         {
//             std::list<AStarNode*> sucesores = current->adjacents;
//             for (auto sucesor : sucesores)
//             {

//                 int G = current->G + _distance(current->position, sucesor->position);
//                 const std::list<AStarNode *>::iterator &posAbierta = find(Abierta, sucesor);
//                 const std::list<AStarNode *>::iterator &posCerrada = find(Cerrada, sucesor);
//                 if (G < sucesor->G)
//                 {
//                     if (posAbierta != Abierta.end())
//                     {

//                         Abierta.erase(posAbierta);
//                     }

//                     if (posCerrada != Cerrada.end())
//                     {
//                         Cerrada.erase(posCerrada);
//                     }
//                 }

//                 if (posAbierta == Abierta.end() && posCerrada == Cerrada.end())
//                 {
//                     sucesor->G = G;
//                     sucesor->H = _distance(sucesor->position, targetNode->position);
//                     sucesor->F = sucesor->G + sucesor->H;
//                     Abierta.push_back(sucesor);
//                     sucesor->parent = current;
//                 }
//             }
//         }
//     }
// }

/*

    int maxIter = 100;
    AStarNode* current = originNode;
    while(current != targetNode && maxIter > 0) { // @todo ensure current and target are connected
        float min = INF_F;
        AStarNode* o = NULL;
        for (List<AStarNode*>::iterator it=current->adjacents.begin(); it != current->adjacents.end(); ++it) {
            float dist = _sdistance((*it)->position, targetNode->position);
            if(additionalCost != NULL) {
                dist += additionalCost[(int)((*it)->position.y / cellsHeight)][(int)((*it)->position.x / cellsWidth)];
            }
            //std::cout << (*it)->position.y << ", " << (*it)->position.x << std::endl;
            if(dist < min) {
                min = dist;
                o = (*it);
            }
        }

        current = o;

        if(current == NULL) {
            break;
        }

        path.push_back(current->position);
        --maxIter;
    }
*/