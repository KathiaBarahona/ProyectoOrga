#ifndef BTREE_H
#define BTREE_H
#define MAXKEYS 5
#define MINKEYS MAXKEYS/2
#define NULO (-1)
#include<string>
#include "Campo.h"
#include "BPAGE.h"
#include "Indice.h"
#include <vector>
using std::string;
class BTree{
        public:
        BTree(); 
        int sizepage(Campo);
        bool opentree(const char*);//abre el archivo para almacenar el arbol
        int RRNRoot(const char*);//Toma los primeros 4 bytes del archivo que contienen el RRN del arbol
        int createtree(const char*,Indice,Campo);//Crea el arbol y el nodo raiz
        int createroot(const char*,Campo,Indice,int,int);
        void initpage(BPAGE&,vector<Indice>&,vector<int>&,Campo);//Inicializa los valores de la pagina
        string insert(const char*,Campo,int,Indice,int&,Indice&);//Inserta elementos
        void insertinpage(const char*,Campo,Indice,int,BPAGE&,vector<Indice>&,vector<int>&);//Inserta en la pagina
        void closetree(const char*);//cierra el archivo
        void readpage(const char*,int,BPAGE&,vector<Indice>&,vector<int>&,Campo);//lee las paginas
        bool search(const char*,string,BPAGE&,int&);//Busca elementos
        void split(Campo,const char*,Indice,int,BPAGE&,Indice&,int&,BPAGE&);
        void writepage(const char*,Campo,int,BPAGE);//Escribe las paginas
        void putroot(const char*,int);//Coloca el rrn de la raiz en los primeros 4 bytes
        int takepage(const char*);//Toma el siguiente bloque disponible para una pagina
        void ListarB(const char*,int,Campo);

};
#endif
