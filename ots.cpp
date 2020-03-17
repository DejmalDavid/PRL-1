/*
David Dejmal PRL 2020

Odd-even transposition sort

*/
#include <mpi.h>
#include <fstream>

#define TAG 0
#define BUFSIZE 256    //maximalni mnozstvi hodnot

using namespace std;

int main(int argc, char *argv[])
{
    int moje_cislo=-1;
    int vedlejsi_cislo=-1;
    int pocet_procesu;
    int id;
    MPI_Status stat; 
    int pole_hodnot[BUFSIZE];
    memset(pole_hodnot, -1, BUFSIZE);    //inicializace pole
    int count = 0; //pocitadlo procesu

    //Paralelizace
    MPI_Init(&argc,&argv);                          
    MPI_Comm_size(MPI_COMM_WORLD, &pocet_procesu);       
    MPI_Comm_rank(MPI_COMM_WORLD, &id);           

    if(id == 0)    //hodnoty nacte master
    {
        //cteni souboru
        int number;  
        fstream myReadFile;
        myReadFile.open("numbers"); //soubor numbers se generuje pri spousteni -> pevny nazev souboru
        if (myReadFile.is_open()) 
        {
            while (!myReadFile.eof()) 
            {
                number = myReadFile.get(); //cteni cisla
                if(number==-1)  //EOF -> break
                {
                    break;
                }
                //cout<<number<<endl;
                pole_hodnot[count]=number;
                count++;
            }
        }
        myReadFile.close();
     
        //vypis bufferu + odeslani ostatnim
        for(int i = 0; i < BUFSIZE ; i++)
        {
            if(pole_hodnot[i]<0)    //zarazka je nastavena na -1
            {
                break;
            }
            if(i==0)    //prvni nevypisuje mezeru
            {
                cout<<pole_hodnot[i];
            }
            else    //ostatni pisou pred sebou mezeru
            {
                cout<<" "<<pole_hodnot[i];
            }
            //vypis done
            //odeslani ostatnim
            //buffer,velikost,typ,rank prijemce,tag,komunikacni skupina
            MPI_Send(&pole_hodnot[i], 1, MPI_INT, i, TAG, MPI_COMM_WORLD); 
        }  
        cout<<endl;
    }

    //vsichni cekaji na prijeti
    //buffer,velikost,typ,rank odesilatele,tag, skupina, stat
    MPI_Recv(&moje_cislo, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat); 

    //urcovani lichych/sudych
    bool flag=1;
    
    for(int i=0; i<pocet_procesu; i++){
        if((id%2==flag)&&(id!=(pocet_procesu-1))) //prvni pulka
        {
            MPI_Send(&moje_cislo, 1, MPI_INT, id+1, TAG, MPI_COMM_WORLD);          //poslu sousedovi svoje cislo
            MPI_Recv(&moje_cislo, 1, MPI_INT, id+1, TAG, MPI_COMM_WORLD, &stat);   //a cekam na nizsi
        }
        if((id%2!=flag)&&(id!=0)) //druha pulka
        {
            MPI_Recv(&vedlejsi_cislo, 1, MPI_INT, id-1, TAG, MPI_COMM_WORLD, &stat);   //cekam na souseda

            if(moje_cislo<vedlejsi_cislo)
            {
                MPI_Send(&moje_cislo, 1, MPI_INT, id-1, TAG, MPI_COMM_WORLD);   //swap
                moje_cislo=vedlejsi_cislo;  //swap
            }
            else
            {
                MPI_Send(&vedlejsi_cislo, 1, MPI_INT, id-1, TAG, MPI_COMM_WORLD); //vracim to same    
            }       
        }
        flag=!flag; //preklopeni na z lichych na sude a obracene
    }

    MPI_Send(&moje_cislo, 1, MPI_INT, 0, TAG,  MPI_COMM_WORLD); //poslani sveho cilsa sousedovi
    
    if(id == 0)    //hodnoty nacte master
    {
        for(int i=0; i<pocet_procesu; i++) //nacteni hodnot zpet do pole
        {
            MPI_Recv(&pole_hodnot[i], 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &stat);
        }
        for(int i=0; i<pocet_procesu; i++) //vypis pole
        {
            cout<<pole_hodnot[i]<<endl;
        }
    }
    //hotovo
    MPI_Finalize(); 
    return 0;
}

