#include<fstream>
#include<iostream>
#include "btree.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<sstream>
using namespace std;

union charint{
        char raw[sizeof(int)];
        int num;
};
BTree:: BTree(){}
void BTree::ListarB(const char*bt,int rrn,Campo c){
        if(rrn == NULO)
                return;
        else{
                BPAGE page;
                readpage(bt,rrn,page,page.indices,page.children,c);
                for(int i=0; i < page.keycount+1;i++){
                        ListarB(bt,page.children[i],c);
                        if(i<page.keycount){
                                cout << page.indices[i].Key << endl;
                       }
                }
         
        }
        
       
}

int BTree::sizepage(Campo c){
        int size = 0;
        string tipo (c.tipo);
        size+=sizeof(int);
        if(tipo.compare("Entero")==0)
                size = size + MAXKEYS*sizeof(int);
        else
                size = size + MAXKEYS*(c.tamano-1);
        size = size + (MAXKEYS + 1)*sizeof(int);
        return size;

}
bool BTree::opentree(const char* bt){
        ifstream in(bt,ios::in|ios::binary);
        if(in)
                return true;
        return false;
}//abre el archivo para almacenar el arbol  

int BTree::RRNRoot(const char* bt){
        charint ci;
        char buffer[sizeof(int)];
        ifstream in(bt,ios::in|ios::binary);
        in.read(buffer,sizeof(int));
        memcpy(ci.raw,buffer,sizeof(int));
        return ci.num;
}//Toma los primeros 4 bytes del archivo que contienen el RRN del arbol
int BTree::createtree(const char* bt,Indice indice,Campo c){
        ofstream out(bt,ios::out|ios::binary);
        int value = -1;
        out.write(reinterpret_cast<char*>(&value),sizeof(int));
        out.close();
        return createroot(bt,c,indice,NULO,NULO);
}//Crea el arbol y el nodo raiz
int BTree::createroot(const char* bt,Campo c,Indice indice,int left,int right){
        BPAGE page;
        int rrn;
        rrn = takepage(bt);
        initpage(page,page.indices,page.children,c);
        page.indices[0].Key=indice.Key;
        page.indices[0].offset = indice.offset;
        page.children[0] = left;
        page.children[1] = right;
        page.keycount = 1;
        writepage(bt,c,rrn,page);
        putroot(bt,rrn); 
        return rrn;
}
void BTree::initpage(BPAGE&page,vector<Indice>&indices,vector<int>&children,Campo c){
        Indice ind;
        string tipo(c.tipo);
        if(tipo.compare("Entero")==0)
            ind.Key = "-1";
        else
            ind.Key = "NULL";
        ind.offset = NULO;
        for(int i = 0; i < MAXKEYS;i++){
                indices.push_back(ind);
                children.push_back(NULO);
        }
        children.push_back(NULO);
}//Inicializa los valores de la pagina
string BTree::insert(const char* bt,Campo c,int rrn,Indice indice,int& promo_child,Indice& promo_ind){
        BPAGE page,newpage;
        bool found;
        int pos,p_b_rrn;
        Indice p_b_ind;
        if(rrn == NULO){
                promo_ind.Key = indice.Key;
                promo_ind.offset = indice.offset;
                promo_child = NULO;
                return "Promoted";
        }
        readpage(bt,rrn,page,page.indices,page.children,c);
        found = search(bt,indice.Key,page,pos);
        if(found){
                return "Error";
        }

        string promotion = insert(bt,c,page.children[pos],indice,p_b_rrn,p_b_ind);
        if(promotion.compare("Not Promoted")==0 || promotion.compare("Error")==0)
                return promotion;
        if(page.keycount < MAXKEYS){
                insertinpage(bt,c,p_b_ind,p_b_rrn,page,page.indices,page.children);
                writepage(bt,c,rrn,page);
                return "Not Promoted";
        }else{
                split(c,bt,p_b_ind,p_b_rrn,page,promo_ind,promo_child,newpage);
                writepage(bt,c,rrn,page);
                writepage(bt,c,promo_child,newpage);
                cout << promo_ind.Key << endl;
                return "Promoted";
        }
}//Inserta elementos
void BTree::insertinpage(const char*bt,Campo c,Indice ind,int
        rrn,BPAGE&page,vector<Indice>&indices,vector<int>&children){
        int i;
        string tipo(c.tipo);
        if(tipo.compare("Entero")==0){
                for(i = page.keycount;atoi(ind.Key.c_str())<atoi(indices[i-1].Key.c_str()) && i > 0;i--){
                        indices[i].Key = indices[i-1].Key;
                        indices[i].offset = indices[i-1].offset;
                        children[i+1] = children[i];

                }
        }else{
                for(i = page.keycount; i > 0 && ind.Key.compare(page.indices[i-1].Key) < 0; i--){
                        indices[i].Key = indices[i-1].Key;
                        indices[i].offset = indices[i-1].offset;
                        children[i+1] = children[i];
                }
                
        }
        page.keycount++;
        indices[i].Key = ind.Key;
        indices[i].offset = ind.offset;
        children[i+1] = rrn;

}//Inserta en la pagina
void BTree::closetree(const char*bt){
        ifstream in(bt,ios::in|ios::binary);
        in.close();
}
void BTree::readpage(const char*bt,int rrn,BPAGE& page,vector<Indice>&indices,vector<int>&children,Campo c){
        ifstream in(bt,ios::in|ios::binary);
        in.seekg(rrn,ios::beg);
        charint kc;
        indices.clear();
        children.clear();
        char buffer[sizeof(int)];
        in.read(buffer,sizeof(int));
        memcpy(kc.raw,buffer,sizeof(int));
        page.keycount = kc.num;
        string tipo(c.tipo);
        for(int i = 0; i < MAXKEYS; i++){
                Indice ind;
                if(tipo.compare("Entero")==0){
                        charint c1;
                        in.read(buffer,sizeof(int));
                        memcpy(c1.raw,buffer,sizeof(int));
                        stringstream ss;
                        ss << c1.num;
                        ind.Key = ss.str();
                }else{
                        char b[c.tamano-1];
                        in.read(b,c.tamano-1);
                        b[c.tamano-1] = '\0';
                        string key(b);
                        ind.Key = key;
                        
                }
                in.read(buffer,sizeof(int));
                memcpy(kc.raw,buffer,sizeof(int));
                ind.offset = kc.num;
                indices.push_back(ind);
        }
        for(int i=0;i <= MAXKEYS; i++){
                in.read(buffer,sizeof(int));
                memcpy(kc.raw,buffer,sizeof(int));
                children.push_back(kc.num);
                
        }
        in.close();
}//lee las paginas
bool BTree:: search(const char* bt,string data,BPAGE& page,int& pos){
        int i;
        for(i = 0; i < page.keycount && data.compare(page.indices[i].Key) > 0; i++);
        pos = i;
        if(pos < page.keycount && data.compare(page.indices[pos].Key)==0)
                 return true;
         else
                 return false;
        

}//Busca elementos
void BTree::split(Campo c,const char* bt,Indice indice,int rrn,BPAGE& page_ant,Indice& promo_ind,
                int& promo_child,BPAGE& newpage_ant){
        int i;
        int middle;
        Indice ind;
        string tipo(c.tipo);
        vector<Indice> indice_aux;
        vector<int>children_aux;
        if(tipo.compare("Entero")==0)
            ind.Key = "-1";
        else
            ind.Key = "NULL";
        ind.offset = NULO;
        for(int i=0;i<=MAXKEYS+1;i++){
                if(i < MAXKEYS + 1)
                    indice_aux.push_back(ind);
                children_aux.push_back(NULO);
        }
        for(i = 0; i < MAXKEYS; i++){
                indice_aux[i].Key = page_ant.indices[i].Key;
                indice_aux[i].offset = page_ant.indices[i].offset;
                children_aux[i]=page_ant.children[i];

        }
        children_aux[i] = page_ant.children[i];
               if(tipo.compare("Entero")==0){
                for(i = MAXKEYS ;atoi(indice.Key.c_str())<atoi(indice_aux[i-1].Key.c_str()) && i > 0;i--){
                        indice_aux[i] = indice_aux[i-1];
                        children_aux[i+1] = children_aux[i];

                }
        }else{
                for(i = MAXKEYS ; indice.Key.compare(indice_aux[i-1].Key) < 0 && i > 0; i--){
                        indice_aux[i]  = indice_aux[i-1];
                        children_aux[i+1] = children_aux[i];
                }
        }
        indice_aux[i] = indice;
        children_aux[i+1] = rrn;
        promo_child = takepage(bt);
        initpage(newpage_ant,newpage_ant.indices,newpage_ant.children,c);
        int p = 0;
        for ( i = 0; i < MAXKEYS; i++){
                if(p < MINKEYS){
                    page_ant.indices[i].Key = indice_aux[p].Key;
                    page_ant.indices[i].offset = indice_aux[p].offset;
                    page_ant.children[i] = children_aux[p];
                    p++;
                }else{
                    if(tipo.compare("Entero")==0){
                        page_ant.indices[i].Key = "-1";
                    }else{
                        page_ant.indices[i].Key = "NULL";
                    }
                        page_ant.indices[i].offset = NULO;
                        page_ant.children[i] = NULO;
                }

        }
        page_ant.children[MINKEYS] = children_aux[MINKEYS];
        p = MINKEYS + 1;
        for(i = 0; i < MAXKEYS; i++){
            if(p <= MAXKEYS){ 
                newpage_ant.indices[i].Key = indice_aux[p].Key;
                newpage_ant.indices[i].offset = indice_aux[p].offset;
                newpage_ant.children[i] = children_aux[p];
                p++;
            }
            else{
                    if(tipo.compare("Entero")==0){
                        newpage_ant.indices[i].Key = "-1";
                    }else{
                        newpage_ant.indices[i].Key = "NULL";
                    }
                        newpage_ant.indices[i].offset = NULO;
                        if( p == MAXKEYS +1){
                            newpage_ant.children[i] = children_aux[p];
                            p++;
                        }else{
                            newpage_ant.children[i] = NULO;
                        }
                }
        }

        newpage_ant.keycount = MAXKEYS - MINKEYS;
        page_ant.keycount = MINKEYS;
        promo_ind.Key = indice_aux[MINKEYS].Key;
        promo_ind.offset = indice_aux[MINKEYS].offset;
        


}
void BTree:: writepage(const char* bt,Campo c,int rrn,BPAGE page){
        fstream out(bt,ios::in|ios::out|ios::binary);
        out.seekp(rrn,ios::beg);
        out.write(reinterpret_cast<char*>(&page.keycount),sizeof(int));
        string tipo(c.tipo);
        for(int i=0;i < MAXKEYS;i++){
                if(tipo.compare("Entero")==0){
                        int value = atoi(page.indices[i].Key.c_str());
                        out.write(reinterpret_cast<char*>(&value),sizeof(int));
                }else{
                        out.write(page.indices[i].Key.c_str(),c.tamano-1);
                
                }
                out.write(reinterpret_cast<char*>(&page.indices[i].offset),sizeof(int));
                
        }
        for(int i=0;i <= MAXKEYS;i++){
                out.write(reinterpret_cast<char*>(&page.children[i]),sizeof(int));

        }
        out.close();
}//Escribe las paginas
void BTree::putroot(const char*bt,int rrn){
        fstream out(bt,ios::in|ios::out|ios::binary);
        out.seekp(ios::beg);
        out.write(reinterpret_cast<char*>(&rrn),sizeof(int));
        //out.close();
}//Coloca el rrn de la raiz en los primeros 4 bytes
int BTree::takepage(const char* bt){
        ifstream in(bt,ios::in|ios::binary);
        in.seekg(0,ios::end);
        return in.tellg();

}//Toma el siguiente bloque disponible para una pagina


