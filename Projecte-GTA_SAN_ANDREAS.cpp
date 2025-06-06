#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define ATTACK_KEY VK_SPACE

// Configuraci� del joc
struct Config
{
    int amplada;
    int altura;
    int peatonsLosSantos;
    int peatgeLosSantos;
    int dinersMaxLosSantos;
    int peatonsSanFierro;
    int peatgeSanFierro;
    int dinersMaxSanFierro;
};

//Per guardar posicions
struct Posicio
{
    int x;
    int y;
};

//  Pels peatons
struct Peatons
{
    Posicio posicio;
    char direccio;
    bool viu;
};

// Estructura per al jugador
struct Jugador
{
    Posicio posicio;
    Posicio posAnterior;
    char Direccio;
    int diners;
};

// Estructura pels diners
struct Diners
{
    Posicio posicio;
    int cantitat;
    Diners* diners;
};

// Struct principal del joc
struct GTASANANDREAS
{
    Config config;
    char** mapa;
    Jugador jugador;
    int ampladaIsla;
    Peatons* peatons;
    int cantitatPeatons;
    int ampladaVista;
    int alturaVista;
    int FPS = 10;
    Diners* diners = nullptr;

    // Constructor
    GTASANANDREAS()
    {
        srand((time(NULL)));

        cargarConfiguracio();


        ampladaVista = 40;
        alturaVista = 20;


        inicialitzarMapa();
        inicialitzarJugador();
        inicialitzarPeatons();
        mostrarVista();

        // GameLopp
        while (true)
        {
            procesarInput();
            actualizar();
            Sleep(1000 / FPS);
        }
    }

    // Destructor per allibera la memoria
    ~GTASANANDREAS()
    {
        // Libera diners
        Diners* actual = diners;
        while (actual != nullptr)
        {
            Diners* siguiente = actual->diners;
            delete actual;
            actual = siguiente;
        }

        // Libera mapa correctamente
        if (mapa != nullptr)
        {
            for (int i = 0; i < config.altura; i++)
            {
                delete[] mapa[i];
            }
            delete[] mapa;
        }

        delete[] peatons;
    }

    // Carrega la configuraci�
    void cargarConfiguracio()
    {
        std::ifstream archivo("config.txt");
        if (archivo.is_open())
        {
            char separador;
            // Llegeix les dimensions del mapa
            archivo >> config.amplada >> separador >> config.altura >> separador;
            // Llegeix la configuraci� de Los Santos
            archivo >> config.peatonsLosSantos >> separador >> config.peatgeLosSantos >> separador >> config.dinersMaxLosSantos >> separador;
            // Llegeix la configuraci� de San Fierro
            archivo >> config.peatonsSanFierro >> separador >> config.peatgeSanFierro >> separador >> config.dinersMaxSanFierro;
            archivo.close();
        }
        else
        {
            std::cout << "No es pot obrir el fitxer";
        }
        std::cout << config.amplada;
    }

    // Inicialitza el mapa del joc
    void inicialitzarMapa()
    {

        mapa = new char* [config.altura];


        for (int i = 0; i < config.altura; i++)
        {
            mapa[i] = new char[config.amplada];
            for (int j = 0; j < config.amplada; j++)
            {
                mapa[i][j] = ' ';
            }
        }


        ampladaIsla = config.amplada / 3;
        for (int i = 0; i < config.altura; i++)
        {
            mapa[i][ampladaIsla] = 'X';
            mapa[i][ampladaIsla * 2] = 'X';


            if (i >= config.altura / 2 - 1 && i <= config.altura / 2 + 1)
            {
                mapa[i][ampladaIsla] = ' ';
                mapa[i][ampladaIsla * 2] = ' ';
            }
        }


        for (int j = 0; j < config.amplada; j++)
        {
            mapa[0][j] = 'X';
            mapa[config.altura - 1][j] = 'X';
        }
    }

    // Inicialitza el jugador
    void inicialitzarJugador()
    {

        jugador.posicio.x = config.amplada / 2;
        jugador.posicio.y = config.altura / 2;
        jugador.posAnterior = jugador.posicio;
        jugador.Direccio = '^';
        jugador.diners = 0;
    }

    // Inicialitza els peatons
    void inicialitzarPeatons()
    {
        // Calcula el total de peatons
        cantitatPeatons = config.peatonsLosSantos + config.peatonsSanFierro;
        peatons = new Peatons[cantitatPeatons];

        // Peatons de Los Santos
        for (int i = 0; i < config.peatonsLosSantos; i++)
        {
            peatons[i].posicio.x = 5 + rand() % (ampladaIsla - 10);  // Posici� aleat�ria dins Los Santos
            peatons[i].posicio.y = 5 + rand() % (config.altura - 10);
            peatons[i].direccio = (rand() % 2 == 0) ? 'H' : 'V';     // Direcci� aleat�ria
            peatons[i].viu = true;                                   // Comencen vius
            mapa[peatons[i].posicio.y][peatons[i].posicio.x] = 'P';  // Marca al mapa
        }

        // Peatons de San Fierro
        for (int i = 0; i < config.peatonsSanFierro; i++)
        {
            int index = config.peatonsLosSantos + i;
            peatons[index].posicio.x = ampladaIsla + 5 + rand() % (ampladaIsla - 10);  // Posici� aleat�ria dins San Fierro
            peatons[index].posicio.y = 5 + rand() % (config.altura - 10);
            peatons[index].direccio = (rand() % 2 == 0) ? 'H' : 'V';  // Direcci� aleat�ria
            peatons[index].viu = true;                                // Comencen vius
            mapa[peatons[index].posicio.y][peatons[index].posicio.x] = 'P';  // Marca al mapa
        }
    }


    void regenerarPeaton(int indice)
    {

        if (peatons[indice].posicio.x < ampladaIsla)
        {

            peatons[indice].posicio.x = 5 + rand() % (ampladaIsla - 10);
        }
        else
        {

            peatons[indice].posicio.x = ampladaIsla + 5 + rand() % (ampladaIsla - 10);
        }
        peatons[indice].posicio.y = 5 + rand() % (config.altura - 10);
        peatons[indice].viu = true;
        mapa[peatons[indice].posicio.y][peatons[indice].posicio.x] = 'P';
    }


    bool estaCostatJugador(const Posicio& posPeaton)
    {

        return (abs(jugador.posicio.x - posPeaton.x) + abs(jugador.posicio.y - posPeaton.y)) <= 3;
    }


    void mourePeatons()
    {
        for (int i = 0; i < cantitatPeatons; i++)
        {
            if (!peatons[i].viu || estaCostatJugador(peatons[i].posicio))
            {
                continue;  // Salta els peatons morts o a prop del jugador ja que no cal
            }

            // Esborra la posicio anterior del peat� amb ' '
            mapa[peatons[i].posicio.y][peatons[i].posicio.x] = ' ';

            if (peatons[i].direccio == 'H')//si es 'H' el peato va en horizontal
            {

                int PeatoX = peatons[i].posicio.x + (rand() % 3 - 1); // -1, 0 o 1,esquerra o d
                if (PeatoX > 0 && PeatoX < config.amplada - 1 && mapa[peatons[i].posicio.y][PeatoX] == ' ')
                {
                    peatons[i].posicio.x = PeatoX;
                }
            }
            else
            {
                // Moviment vertical simple
                int PeatoY = peatons[i].posicio.y + (rand() % 3 - 1); // -1, 0 o 1
                if (PeatoY > 0 && PeatoY < config.altura - 1 && mapa[PeatoY][peatons[i].posicio.x] == ' ')
                {
                    peatons[i].posicio.y = PeatoY;
                }
            }

            // Actualitza la nova posicio al mapa amb 'P'
            mapa[peatons[i].posicio.y][peatons[i].posicio.x] = 'P';
        }
    }
    // Afegeix diners
    void agregarDiners(Posicio pos, int cantidad)
    {
        Diners* nuevo = new Diners;
        nuevo->posicio = pos;
        nuevo->cantitat = cantidad;
        nuevo->diners = diners;
        diners = nuevo;
    }

    void AtacJugador()
    {
        if (GetAsyncKeyState(ATTACK_KEY))
        {
            for (int i = 0; i < cantitatPeatons; i++)
            {
                if (!peatons[i].viu)
                {
                    continue;  //si el peato ja esta mort, no cal i es pot saltar
                }


                if (abs(jugador.posicio.x - peatons[i].posicio.x) <= 1 && abs(jugador.posicio.y - peatons[i].posicio.y) <= 1)
                {
                    //matar al peato
                    peatons[i].viu = false;
                    mapa[peatons[i].posicio.y][peatons[i].posicio.x] = ' ';

                    //Diners aleatori
                    int dinero = (peatons[i].posicio.x < ampladaIsla) ? rand() % config.dinersMaxLosSantos + 1 : rand() % config.dinersMaxSanFierro + 1;

                    // agregar diners quan el peato mor
                    agregarDiners(peatons[i].posicio, dinero);

                    // Regenera el peat� que ha sigut eliminat
                    regenerarPeaton(i);
                }
            }
        }
    }



    // Recull diners quan el jugador passa per sobre
    void recollirDiners()
    {
        Diners* actual = diners;
        Diners* anterior = nullptr;

        while (actual != nullptr)
        {
            if (jugador.posicio.x == actual->posicio.x && jugador.posicio.y == actual->posicio.y)
            {
                // El jugador est� sobre els diners
                jugador.diners += actual->cantitat;


                if (anterior == nullptr)
                {
                    diners = actual->diners;
                    delete actual;
                    actual = diners;
                }
                else
                {
                    anterior->diners = actual->diners;
                    delete actual;
                    actual = anterior->diners;
                }
            }
            else
            {
                anterior = actual;
                actual = actual->diners;
            }
        }
    }


    void procesarInput()
    {

        jugador.posAnterior = jugador.posicio;

        // Moviment cap adalt
        if (GetAsyncKeyState('W') && jugador.posicio.y > 1)
        {
            if (mapa[jugador.posicio.y - 1][jugador.posicio.x] != 'X')
            {
                jugador.posicio.y--;
                jugador.Direccio = '^';
            }
        }
        // Moviment cap avall
        else if (GetAsyncKeyState('S') && jugador.posicio.y < config.altura - 2)
        {
            if (mapa[jugador.posicio.y + 1][jugador.posicio.x] != 'X')
            {
                jugador.posicio.y++;
                jugador.Direccio = 'v';
            }
        }
        // Moviment cap a l'esquerra
        else if (GetAsyncKeyState('A') && jugador.posicio.x > 1)
        {
            if (mapa[jugador.posicio.y][jugador.posicio.x - 1] != 'X')
            {
                jugador.posicio.x--;
                jugador.Direccio = '<';
            }
        }
        // Moviment cap a la dreta
        else if (GetAsyncKeyState('D') && jugador.posicio.x < config.amplada - 2)
        {
            if (mapa[jugador.posicio.y][jugador.posicio.x + 1] != 'X')
            {
                jugador.posicio.x++;
                jugador.Direccio = '>';
            }
        }
    }


    void mostrarVista()
    {
        system("cls");
        std::cout << "                 GTA SAN ANDREAS" << std::endl;

        // Posici� anterior del jugador amb ' ' per borrar-ho
        mapa[jugador.posAnterior.y][jugador.posAnterior.x] = ' ';

        // Guarda jugador
        char jugadorOriginal = mapa[jugador.posicio.y][jugador.posicio.x];

        // Usa una variable temporal para diners
        Diners* temp = diners;
        while (temp != nullptr)
        {
            mapa[temp->posicio.y][temp->posicio.x] = '$';
            temp = temp->diners;
        }

        // Afegir el jugador al mapa
        mapa[jugador.posicio.y][jugador.posicio.x] = jugador.Direccio;

        // La vista del jugador
        int VistaX = jugador.posicio.x - ampladaVista / 2;
        int VistaY = jugador.posicio.y - alturaVista / 2;

        // Limits del mapa
        if (VistaX < 0)
        {
            VistaX = 0;
        }
        if (VistaY < 0)
        {
            VistaY = 0;
        }
        if (VistaX + ampladaVista > config.amplada)
        {
            VistaX = config.amplada - ampladaVista;
        }
        if (VistaY + alturaVista > config.altura)
        {
            VistaY = config.altura - alturaVista;
        }

        // Mostrar nom�s la vista del jugador
        for (int i = VistaY; i < VistaY + alturaVista && i < config.altura; i++)
        {
            for (int j = VistaX; j < VistaX + ampladaVista && j < config.amplada; j++)
            {
                std::cout << mapa[i][j];
            }
            std::cout << '\n';
        }

        // Restaura jugador
        mapa[jugador.posicio.y][jugador.posicio.x] = jugadorOriginal;

        // Mostrar els diners abaix per saber quan t� tot el moment
        std::cout << "Diners: " << jugador.diners << std::endl;
    }

    //actualitzar el joc per el GameLoop
    void actualizar()
    {
        AtacJugador();
        recollirDiners();
        mourePeatons();
        mostrarVista();
    }
};


int main()
{
    GTASANANDREAS juego;
}