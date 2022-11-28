// ###### Config options ################

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

using namespace Asedio;
float defenseValue(Defense *d)
{
    float valor = 0;

    float dps = d->damage / d->attacksPerSecond;
    valor = d->health * 0.4 + dps * 0.4 + d->range * 0.15 + d->dispersion * 0.05;
    return valor;
}
void DEF_LIB_EXPORTED selectDefenses(std::list<Defense *> defenses, unsigned int ases, std::list<int> &selectedIDs, float mapWidth, float mapHeight, std::list<Object *> obstacles)
{

    // unsigned int cost = 0;
    // std::list<Defense*>::iterator it = defenses.begin();
    // while(it != defenses.end()) {
    //     if(cost + (*it)->cost <= ases) {
    //         selectedIDs.push_back((*it)->id);
    //         cost += (*it)->cost;
    //     }
    //     ++it;
    // }...................................................+0
    int coste = 0;
    int tam = defenses.size();
    Defense *defensas[tam];
    float valor[tam];
    int i = 0;

    if (ases >= defenses.front()->cost)
    {
        ases -= defenses.front()->cost;

        for (Defense *d : defenses)
        {
            if (d != defenses.front())
            {
                defensas[i] = d;
                valor[i] = defenseValue(d);
                i++;
            }
        }

        float tabla[tam][ases + 1];
        for (size_t i = 0; i < tam; i++)
        {
            for (size_t j = 0; j <= ases; j++)
            {
                if (i == 0)
                {

                    if (j < defensas[i]->cost)
                    {
                        tabla[i][j] = 0;
                    }

                    else
                    {
                        tabla[i][j] = valor[i];
                    }
                }
                else // i > 0
                {
                    if (j < defensas[i]->cost)
                        tabla[i][j] = tabla[i - 1][j];
                    else // j >= costes[i]
                    {
                        tabla[i][j] = std::max(tabla[i - 1][j], tabla[i - 1][j - defensas[i]->cost] + valor[i]);
                    }
                }
            }
        }

        int valorOptimo = tabla[tam - 1][ases];

        // Reconstruir camino
        int i = tam - 1;
        int j = ases;
        while (i >= 0 && j > 0)
        {
            if (i > 0)
            {
                if (tabla[i][j] != tabla[i - 1][j])
                {
                    selectedIDs.push_front(defensas[i]->id);
                    coste += defensas[i]->cost;

                    j -= defensas[i]->cost;
                }
            }
            if (i == 0 && tabla[i][j] > 0)
            {
                selectedIDs.push_front(defensas[i]->id);
                coste += defensas[i]->cost;
            }
            i--;
        }
        selectedIDs.push_front(defenses.front()->id);
        coste += defenses.front()->cost;
        // for (size_t i = 0; i < tam; i++)
        // {
        //     std::cout<< "[ ";

        //     for (size_t j = 0; j <= ases; j++)
        //     {
        //         std::cout << tabla[i][j] << " ";
        //     }

        //     std::cout<< "]\n";

        // }
    }
}
