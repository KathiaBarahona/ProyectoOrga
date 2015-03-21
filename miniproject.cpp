#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<cstring>
#include<stdio.h>
#include<stdlib.h>
#include<map>
#include<iomanip>
using namespace std;
struct Campo{
        char nombre[15];
        char tipo[7];
        int tamano;
};
struct Indice{
        string Key;
        int offset;
};
struct bpage{
        int keycount;
        Indice indices[15];
        int RRNHijos[16];
};
struct bpage2{
        int keycount;
        Indice indices[16];
        int RRNHijos[17];
};
union charint{
        char raw[sizeof(int)];
        int num;
};
union charlongint{
        char raw[sizeof(long int)];
        long int num;
};
void Cruzar();
void ILCruzar();
void ILBorrar(const char* , vector<Campo>,map<string,int>&);
void ILBuscar(const char* ,vector<Campo>, map<string,int>);
int Menu(bool&,char*&,char*&,char*&, vector<Campo>&, map<string,int>&,string&,string&);
int updateKey(map<string,int>&, int offset);
int offset(int,vector<Campo>);
void headerindices(const char*);
void Reindexar(const char*,map<string,int>&, vector<Campo>);
void leerindices(const char*, map<string,int>&, Campo);
int getStructure(const char*, vector<Campo>&);
void Header(const char*, vector<Campo>&);
void Agregar(const char*,vector<Campo>,map<string,int>&,bool);
void Listar(const char*, vector<Campo>);
void ILListar(const char*, map<string,int>, vector<Campo>);
void Borrar(const char*, vector<Campo>);
void Modificar(const char*,vector<Campo>);
void Compactar(const char*,vector<Campo>);
int Buscar(const char*, vector<Campo>,string,bool);
void guardarindices(const char*, map<string,int>&,Campo);
void ILModificar(const char*,vector<Campo>,map<string,int>&);
//Funciones ArbolB
void ListarB(const char*,const char*,int,Campo,vector<Campo>);
Indice BusquedaB(const char*,string, bpage, int,Campo);
void split(const char*,Indice,int,bpage, Indice&,int&,bpage&);
void readpage(const char*,bpage&,int,Campo);
void writepage(const char*,bpage,int,Campo);
string insert(const char*,Campo,int&,Indice&,int&,Indice&);
void ListarB(const char* arbol,const char* nbin,int RRN,Campo c,vector<Campo>campos){
        if(RRN == -1)
                return;
        else{
                bpage page;
                readpage(arbol,page,RRN,c);
                for(int i=0; i < page.keycount+1;i++){
                        ListarB(arbol,nbin,page.RRNHijos[i],c,campos);
                        if(i<page.keycount){
                                ifstream in (nbin,ios::in|ios::out);
                                in.seekg(page.indices[i].offset);
                                for(int j=0;j<campos.size();j++){
                                        string tipo(campos[j].tipo);
                                        if(j == 0){
                                                char p;
                                                in.read(&p,sizeof(char));
                                        }else{
                                                if(tipo.compare("Entero")==0){
                                                        charint ci;
                                                        char buffer[sizeof(int)];
                                                        in.read(buffer,sizeof(int));
                                                        memcpy(ci.raw,buffer,sizeof(int));
                                                        cout << ci.num << endl;
                                                }else{
                                                        char buffer[campos[j].tamano-1];
                                                        in.read(buffer,campos[j].tamano-1);
                                                        buffer[campos[j].tamano-1] = '\0';
                                                        cout << buffer << endl;
                                                }
                                        }
                                }
                        }
                }
        }
}
void writepage(const char* arbol, bpage page,int RRN, Campo c){
        ofstream out(arbol,ios::out|ios::binary);
        out.seekp(RRN);
        out.write(reinterpret_cast<char*>(&page.keycount),sizeof(int));
        for(int i=0;i<page.keycount;i++){
                string tipo(c.tipo);
                if(tipo.compare("Entero")==0){
                        int value = atoi(page.indices[i].Key.c_str());
                        out.write(reinterpret_cast<char*>(&value),sizeof(int));
                }else{
                        out.write(page.indices[i].Key.c_str(),c.tamano-1);
                        
                }
                out.write(reinterpret_cast<char*>(&page.indices[i].offset),sizeof(int));

        }
        for(int i = page.keycount;i<15;i++){
                string tipo(c.tipo);
                if(tipo.compare("Entero")==0){
                        int value = 0;
                        out.write(reinterpret_cast<char*>(&value),sizeof(int));
                }else{  
                        string st = "";
                        out.write(st.c_str(),c.tamano-1);
                        
                }
                int n = -1;
                out.write(reinterpret_cast<char*>(&n),sizeof(int));                
        }
        for(int i = 0; i < page.keycount+1; i++){
                out.write(reinterpret_cast<char*>(&page.RRNHijos[i]),sizeof(int));
        }
        for(int i = page.keycount+1;i < 16;i++){
                int n =-1;
                out.write(reinterpret_cast<char*>(&n),sizeof(int));                                
        }

}
void readpage(const char* arbol, bpage& page,int RRN, Campo c){
        ifstream in(arbol,ios::in|ios::binary);
        in.seekg(RRN);
        charint ci;
        char buffer[sizeof(int)];
        in.read(buffer,sizeof(int));
        memcpy(ci.raw,buffer,sizeof(int));
        page.keycount = ci.num;
        for(int i = 0; i < 15 ; i++){
                Indice ind;
                string tipo(c.tipo);
                if(tipo.compare("Entero")==0){
                        char b[sizeof(int)];
                        charint c1;
                        in.read(b,sizeof(int));
                        memcpy(c1.raw,b,sizeof(int));
                        stringstream ss;
                        ss << c1.num;
                        strcpy((char*)ind.Key.c_str(),(const char*)ss.str().c_str());
                }else{
                        char b[c.tamano-1];
                        in.read(b,c.tamano-1);
                         b[c.tamano-1] ='\0';
                        strcpy((char*)ind.Key.c_str(),b);
                }
                charint c2;
                char b3[sizeof(int)];
                in.read(b3,sizeof(int));
                memcpy(c2.raw,b3,sizeof(int));
                ind.offset = c2.num;
                page.indices[i] = ind;
        }
        for(int i = 0; i < 16; i++){
                charint c3;
                char buff[sizeof(int)];
                in.read(buff,sizeof(int));
                memcpy(c3.raw,buff,sizeof(int));
                page.RRNHijos[i] = c3.num;
        }

}
void split(const char* arbol,Indice nuevo,int newRRN,bpage page, Indice& Promo_Key, int& Promo_RRN,bpage& newpage){
            bpage2 workingpage;
            map<string,int> temporal;
            temporal.insert(pair<string,int>(nuevo.Key,nuevo.offset));
            for(int i=0;i<15;i++){
                       temporal.insert(pair<string,int>(page.indices[i].Key,page.indices[i].offset));
            }
            int p = 0;
            int position;
            for(map<string,int>::iterator it = temporal.begin(); it != temporal.end();++it){
                    if(nuevo.Key.compare(it->first)==0)
                            position = p;
                    strcpy((char*)workingpage.indices[p].Key.c_str(),(const char*)(it->first).c_str());
                    workingpage.indices[p].offset = it->second;
                    workingpage.keycount++;
                    p++;
            }//organiza los indices
            workingpage.RRNHijos[position] = newRRN;//agrega el RRN 
            p = 0 ;
            for(int i=0;i<16;i++){
                    if(p == position)
                            p++;
                    workingpage.RRNHijos[p] = page.RRNHijos[i];
            }//Organiza los RRN
            strcpy((char*)Promo_Key.Key.c_str(),(const char*)workingpage.indices[7].Key.c_str());
            Promo_Key.offset = workingpage.indices[7].offset;
            ifstream in(arbol,ios::in|ios::binary);
            in.seekg(0,ios::end);
            Promo_RRN = in.tellg();
            in.close();
            p = 0;
            while(workingpage.indices[p].Key.compare(workingpage.indices[7].Key)!=0){
                     strcpy((char*)page.indices[p].Key.c_str(),(const char*)workingpage.indices[p].Key.c_str());
                     page.indices[p].offset = workingpage.indices[p].offset;
                     p++;
            }//agrega los indices a la pagina
            page.keycount = p - 1;
            for(p; p < 15;p++){
                    Indice ind;
                    ind.Key = "";
                    ind.offset = -1;
                    page.indices[p]=ind;
            }

            p = page.keycount + 2;
            int i = 0;
            for(p;p<16;p++,i++){
                     strcpy((char*)newpage.indices[i].Key.c_str(),(const char*)workingpage.indices[p].Key.c_str());
                     newpage.indices[i].offset = workingpage.indices[p].offset;                  
                     strcpy((char*)newpage.indices[p].Key.c_str(),(const char*)workingpage.indices[p].Key.c_str());
                     newpage.indices[p].offset = workingpage.indices[p].offset;
                     p++;
            }//agrega los indices a la pagina nueva
            for(int k = 0; k < 8; k++){
                    page.RRNHijos[k] = workingpage.RRNHijos[k];
            }
            i = 0;
            for(int k = 8 ; k < 17;k++,i++)
                    newpage.RRNHijos[i] = workingpage.RRNHijos[k];
           
}
Indice BusquedaB(const char* arbol, string data, bpage page, int RRN, Campo c){
        int RRNant = RRN;
        for(int i = 0; i < 15 ; i++){
                if(page.indices[i].Key.compare("") == 0)
                        break;
                if(page.indices[i].Key.compare(data)==0)
                        return page.indices[i];
                if(page.indices[i].Key.compare(data) > 0)
                        RRN = page.RRNHijos[i];
                else
                        RRN = page.RRNHijos[i+1];

        }
        if ( RRN == 0 ){
                Indice ind;
                ind.Key = "RRN Page";
                ind.offset = RRNant;
                return ind;
        }
        readpage(arbol,page,RRN,c);        
        return BusquedaB(arbol,data,page,RRN,c);
}
string insert(const char* arbol,Campo c,int& RRN, Indice& ind, int& Promo_RRN, Indice& Promo_Key){
        Indice P_B_Key;
        int P_B_RRN;
        if(RRN == -1){
             strcpy((char*)Promo_Key.Key.c_str(),(const char*)ind.Key.c_str());
             Promo_Key.offset = ind.offset;
             Promo_RRN = -1;
             return "Promoted";
        }else{
                bpage page;
                readpage(arbol,page,RRN, c);
                Indice pos = BusquedaB(arbol,ind.Key,page,RRN,c);
                if(pos.Key.compare("RRN Page")!=0)
                        return "Error";
                string return_value = insert(arbol,c,pos.offset,ind,P_B_RRN,P_B_Key);
                if(return_value.compare("No Promotion") == 0 || return_value.compare("Error")==0){
                        return return_value;
                }else if (page.keycount != 15){
                        map<string,int>temporal;
                        temporal.insert(pair<string,int>(P_B_Key.Key,P_B_Key.offset));
                        for(int i=0;i<page.keycount;i++){
                                temporal.insert(pair<string,int>(page.indices[i].Key,page.indices[i].offset));
                        }
                        int p = 0;
                        int position;
                        for(map<string,int>::iterator it = temporal.begin(); it != temporal.end();++it,p++){
                                if(P_B_Key.Key.compare(it->first)==0)
                                        position = p;                                
                                strcpy((char*)page.indices[p].Key.c_str(),(const char*)(it->first).c_str());
                                page.indices[p].offset = it->second;
                        }
                        p=0;
                        for(int i = page.keycount+1; i >=0;i--){
                                if(i == position){
                                        page.RRNHijos[i] = P_B_RRN;
                                        break;
                                }
                                page.RRNHijos[i] = page.RRNHijos[i-1];
                                
                        }
                        page.keycount++;
                        writepage(arbol,page,RRN,c);
                        return "No Promotion";
                }else{
                        bpage newpage;
                        split(arbol,P_B_Key,P_B_RRN,page, Promo_Key,Promo_RRN,newpage);
                        writepage(arbol,page,RRN,c);
                        writepage(arbol,newpage,Promo_RRN,c);
                        return "Promoted";

                }

        }
}
void ILCruzar(){
        bool bandera = true;
        char*archivo1;
        char*archivo2;
        char*bin1;
        char*bin2;
        vector<Campo>campos1;
        vector<Campo>campos2;
        map<string,int> indices1;
        map<string,int> indices2;
        string name1;
        string nb1;
        string nb2;
        string name2;
        int f1=0;
        int f2=0;
        cout << "Ingrese el nombre del archivo 1" << endl;
        cin >> name1;
        strcpy((char*)nb1.c_str(),(const char*)name1.c_str());
        cout << "Ingrese el nombre del  archivo 2" << endl;
        cin >> name2;
        strcpy((char*)nb2.c_str(),(const char*)name2.c_str());        
        archivo1 = (char*)name1.c_str();
        bin1 = (char*)nb1.c_str();
        strcat(bin1, ".ind");
        strcat(archivo1, ".bin");
        archivo2 = (char*)name2.c_str();
        strcat(archivo2, ".bin");
        bin2 = (char*)nb2.c_str();
        strcat(bin2, ".ind");        
        if(getStructure(archivo1,campos1)==0){
                cout << name1 << " no existe" << endl;
                bandera = false;
        }
        if(getStructure(archivo2,campos2)==0){
                cout << name2 << " no existe" << endl;
                bandera = false;
        }
        leerindices(bin1, indices1, campos1[1]);   
        leerindices(bin2, indices2, campos2[1]);
        if(bandera){
                Campo c1,c2;
                vector<Campo>campos3;
                map<string,int> indices3;
                Campo cam;
                cam.tamano = 2;
                string s = "CHAR";
                strcpy(cam.nombre,s.c_str());
                string ss = "Texto";
                strcpy(cam.tipo,ss.c_str());
                campos3.push_back(cam);
                int posicion;
                for(int i=1;i<campos1.size();i++){
                        cout << i << campos1[i].nombre << endl; 
                }
                cout << "Ingrese el primer campo a relacionar " << endl;
                cin >> posicion;
                c1 = campos1[posicion];
                for(int i=1;i<campos2.size();i++){
                        cout << i << campos2[i].nombre << endl; 
                }
                cout << "Ingrese el segundo  campo a relacionar " << endl;
                cin >> posicion; 
                c2 = campos2[posicion];
                int i;
                char* archivo3;
                char* bin3;
                string nb3;
                string name3;
                cout << "Ingrese el nombre del nuevo archivo " << endl;
                cin >> name3;
                strcpy((char*)nb3.c_str(),(const char*)name3.c_str());
                archivo3 = (char*)name3.c_str();
                bin3 = (char*)nb3.c_str();
                strcat(bin3, ".ind");
                strcat(archivo3,".bin");
                headerindices(bin3);                 
                cout << "Seleccione los campos del primer archivo que desea en " << name3 << endl;
                int resp=0;
                while(resp==0){
                        for(int i=1;i<campos1.size();i++){
                                cout << i << campos1[i].nombre << endl; 
                        }
                        cin >> posicion;
                        campos3.push_back(campos1[posicion]);
                        f1++;
                        cout << "Desea seleccionar otro? [0(si)/1(no)]?" << endl;
                        cin >> resp;
                }//fin while
                resp = 0;
                cout << "Seleccione los campos del segundo archivo que desea en " << name3 << endl;
                while(resp==0){
                        for(int i=1;i<campos2.size();i++){
                                cout << i << campos2[i].nombre << endl; 
                        }
                        cin >> posicion;
                        campos3.push_back(campos2[posicion]);
                        cout << "Desea seleccionar otro? [0(si)/1(no)]?" << endl;
                        cin >> resp;

                }//fin while
                             
                Header(archivo3, campos3);
                i = 0;
                ifstream in1 (archivo1,ios::in|ios::binary);
                charint cbyte;
                char a[sizeof(int)];
                in1.read(a, sizeof(int));
                memcpy(cbyte.raw,a,sizeof(int));
                in1.seekg(cbyte.num,ios::beg);                
                ifstream in2(archivo2,ios::in|ios::binary);
                ofstream out(archivo3,ios::out|ios::binary|ios::app);
                out.seekp(0,ios::end);
                int of;
                while(true){
                        if(i == campos1.size())
                                i=0;
                        if(i == 0){
                                of = in1.tellg();
                                //cout << of << endl;
                                char p;
                                if(!in1.read(&p,sizeof(char)))
                                        break;
                                if(p == '*'){
                                        in1.seekg((int)in1.tellg()-1);
                                        int size = 0;
                                        for(int k=0;k<campos1.size();k++){
                                                string str(campos1[k].tipo);
                                                if(str.compare("Entero")==0)
                                                        size += sizeof(int);
                                                else
                                                        size = size + campos1[k].tamano -1;

                                        }
                                        size = size + in1.tellg();
                                        in1.seekg(size);
                                        i=0;
                                }else{
                                        i++;
                                }
                        }else{
                                stringstream sstream;
                                charint ci;
                                string tipo(campos1[i].tipo);
                                if(tipo.compare("Entero")==0){
                                        char buf[sizeof(int)];
                                        if(!in1.read(buf,sizeof(int)))
                                                break;
                                        memcpy(ci.raw,buf,sizeof(int));
                                        sstream << ci.num;
                                        i++;
                                }else{
                                        char buf[campos1[i].tamano-1];                                       
                                        if(!in1.read(buf,campos1[i].tamano-1))
                                                break;
                                        buf[campos1[i].tamano-1]='\0';
                                        sstream << buf;
                                        i++;
                                }//fin else1
                                cout << sstream.str() << endl;
                                string camp1(c1.nombre);
                                string name1(campos1[i-1].nombre);
                                int offset2 = Buscar(archivo2,campos2,sstream.str(),false); 
                                if(offset2!=-1 && camp1.compare(name1)==0){
                                         for(int k = 0; k <= f1 ; k++){
                                                 cout << campos3[k].nombre << endl;
                                                if(k == 0){
                                                        char p = '0';
                                                        out.write(reinterpret_cast<char*>(&p),sizeof(char));
                                                }else{
                                                        in1.seekg(of);
                                                        for(int j = 0; j < campos1.size();j++){
                                                                if(j==0){
                                                                        char p;
                                                                        if(!in1.read(&p,sizeof(char)))
                                                                                break;
                                                                }else{
                                                                        string str(campos1[j].tipo);
                                                                        if(str.compare("Entero")==0){
                                                                                charint ci2;
                                                                                char b[sizeof(int)];
                                                                                if(!in1.read(b,sizeof(int)))
                                                                                        break;
                                                                                memcpy(ci2.raw,b,sizeof(int));
                                                                                stringstream s;
                                                                                s << ci2.num;
                                                                                string str1(campos1[j].nombre);
                                                                                string str2(campos3[k].nombre);
                                                                                if(str1.compare(str2)==0)
                                                                                        out.write(reinterpret_cast<char*>
                                                                                        (&ci2.num),sizeof(int));
                                                                        } else{
                                                                                char b[campos1[j].tamano-1];
                                                                                if(!in1.read(b,campos1[j].tamano-1))
                                                                                        break;
                                                                                b[campos1[j].tamano-1]='\0'; 
                                                                                string str1(campos1[j].nombre);
                                                                                string str2(campos3[k].nombre);
                                                                                if(str1.compare(str2)==0)
                                                                                        out.write(reinterpret_cast<char*>
                                                                                        (&b),campos1[j].tamano-1);


                                                                        }

                                                                }
                                                        }
                                                }

                                        }
                                        int sz = 0;
                                        for(int k=0;k<campos1.size();k++){
                                                string str(campos1[k].tipo);
                                                if(str.compare("Entero")==0)
                                                        sz += sizeof(int);
                                                else
                                                        sz = sz + campos1[k].tamano -1;

                                        }
                                        sz = sz + of;
                                        in1.seekg(sz);   
                                        cout << in1.tellg() << endl;
                                        for(int k = f1+1; k < campos3.size() ; k++){
                                                if(k == 0){
                                                        char p = '0';
                                                        out.write(reinterpret_cast<char*>(&p),sizeof(char));
                                                }else{
                                                        in2.seekg(offset2);
                                                        for(int j = 0; j < campos2.size();j++){
                                                                if(j==0){
                                                                        char p;
                                                                        if(!in2.read(&p,sizeof(char)))
                                                                                break;
                                                                }else{
                                                                        string str(campos2[j].tipo);
                                                                        if(str.compare("Entero")==0){
                                                                                charint ci2;
                                                                                char b[sizeof(int)];
                                                                                if(!in2.read(b,sizeof(int)))
                                                                                        break;
                                                                                memcpy(ci2.raw,b,sizeof(int));
                                                                                stringstream s;
                                                                                s << ci2.num;
                                                                                string str1(campos2[j].nombre);
                                                                                string str2(campos3[k].nombre);
                                                                                if(str1.compare(str2)==0)
                                                                                        out.write(reinterpret_cast<char*>
                                                                                        (&ci2.num),sizeof(int));
                                                                        } else{
                                                                                char b[campos2[j].tamano-1];
                                                                                if(!in2.read(b,campos2[j].tamano-1))
                                                                                        break;
                                                                                b[campos2[j].tamano-1]='\0'; 
                                                                                string str1(campos2[j].nombre);
                                                                                string str2(campos3[k].nombre);
                                                                                if(str1.compare(str2)==0)
                                                                                        out.write(reinterpret_cast<char*>
                                                                                        (&b),campos2[j].tamano-1);


                                                                        }

                                                                }
                                                        }
                                                }

                                        }
                                        i =  0;
                                    }
                                }//fin else
                }//fin escritura
            in1.close();
            in2.close();
            out.close();
            Reindexar(archivo3,indices3, campos3);
            guardarindices(bin3, indices3,campos3[1]);            

        }//fin bandera


}
void Cruzar(){
        bool bandera = true;
        char*archivo1;
        char*archivo2;
        vector<Campo>campos1;
        vector<Campo>campos2;
        string name1;
        string name2;
        int f1=0;
        int f2=0;
        cout << "Ingrese el nombre del archivo 1" << endl;
        cin >> name1;
        cout << "Ingrese el nombre del  archivo 2" << endl;
        cin >> name2;
        archivo1 = (char*)name1.c_str();
        strcat(archivo1, ".bin");
        archivo2 = (char*)name2.c_str();
        strcat(archivo2, ".bin");
        if(getStructure(archivo1,campos1)==0){
                cout << name1 << " no existe" << endl;
                bandera = false;
        }
        if(getStructure(archivo2,campos2)==0){
                cout << name2 << " no existe" << endl;
                bandera = false;
        }
        if(bandera){
                Campo c1,c2;
                vector<Campo>campos3;
                Campo cam;
                cam.tamano = 2;
                string s = "CHAR";
                strcpy(cam.nombre,s.c_str());
                string ss = "Texto";
                strcpy(cam.tipo,ss.c_str());
                campos3.push_back(cam);
                int posicion;
                for(int i=1;i<campos1.size();i++){
                        cout << i << campos1[i].nombre << endl; 
                }
                cout << "Ingrese el primer campo a relacionar " << endl;
                cin >> posicion;
                c1 = campos1[posicion];
                for(int i=1;i<campos2.size();i++){
                        cout << i << campos2[i].nombre << endl; 
                }
                cout << "Ingrese el segundo  campo a relacionar " << endl;
                cin >> posicion; 
                c2 = campos2[posicion];
                int i;
                char* archivo3;
                string name3;
                cout << "Ingrese el nombre del nuevo archivo " << endl;
                cin >> name3;
                archivo3 = (char*)name3.c_str();
                strcat(archivo3,".bin");
                cout << "Seleccione los campos del primer archivo que desea en " << name3 << endl;
                int resp=0;
                while(resp==0){
                        for(int i=1;i<campos1.size();i++){
                                cout << i << campos1[i].nombre << endl; 
                        }
                        cin >> posicion;
                        campos3.push_back(campos1[posicion]);
                        f1++;
                        cout << "Desea seleccionar otro? [0(si)/1(no)]?" << endl;
                        cin >> resp;
                }//fin while
                resp = 0;
                cout << "Seleccione los campos del segundo archivo que desea en " << name3 << endl;
                while(resp==0){
                        for(int i=1;i<campos2.size();i++){
                                cout << i << campos2[i].nombre << endl; 
                        }
                        cin >> posicion;
                        campos3.push_back(campos2[posicion]);
                        cout << "Desea seleccionar otro? [0(si)/1(no)]?" << endl;
                        cin >> resp;

                }//fin while
                             
                Header(archivo3, campos3);
                i = 0;
                ifstream in1 (archivo1,ios::in|ios::binary);
                charint cbyte;
                char a[sizeof(int)];
                in1.read(a, sizeof(int));
                memcpy(cbyte.raw,a,sizeof(int));
                in1.seekg(cbyte.num,ios::beg);                
                ifstream in2(archivo2,ios::in|ios::binary);
                ofstream out(archivo3,ios::out|ios::binary|ios::app);
                out.seekp(0,ios::end);
                int of;
                while(true){
                        if(i == campos1.size())
                                i=0;
                        if(i == 0){
                                of = in1.tellg();
                                //cout << of << endl;
                                char p;
                                if(!in1.read(&p,sizeof(char)))
                                        break;
                                if(p == '*'){
                                        in1.seekg((int)in1.tellg()-1);
                                        int size = 0;
                                        for(int k=0;k<campos1.size();k++){
                                                string str(campos1[k].tipo);
                                                if(str.compare("Entero")==0)
                                                        size += sizeof(int);
                                                else
                                                        size = size + campos1[k].tamano -1;

                                        }
                                        size = size + in1.tellg();
                                        in1.seekg(size);
                                        i=0;
                                }else{
                                        i++;
                                }
                        }else{
                                stringstream sstream;
                                charint ci;
                                string tipo(campos1[i].tipo);
                                if(tipo.compare("Entero")==0){
                                        char buf[sizeof(int)];
                                        if(!in1.read(buf,sizeof(int)))
                                                break;
                                        memcpy(ci.raw,buf,sizeof(int));
                                        sstream << ci.num;
                                        i++;
                                }else{
                                        char buf[campos1[i].tamano-1];                                       
                                        if(!in1.read(buf,campos1[i].tamano-1))
                                                break;
                                        buf[campos1[i].tamano-1]='\0';
                                        sstream << buf;
                                        i++;
                                }//fin else1
                                cout << sstream.str() << endl;
                                string camp1(c1.nombre);
                                string name1(campos1[i-1].nombre);
                                int offset2 = Buscar(archivo2,campos2,sstream.str(),false); 
                                if(offset2!=-1 && camp1.compare(name1)==0){
                                         for(int k = 0; k <= f1 ; k++){
                                                 cout << campos3[k].nombre << endl;
                                                if(k == 0){
                                                        char p = '0';
                                                        out.write(reinterpret_cast<char*>(&p),sizeof(char));
                                                }else{
                                                        in1.seekg(of);
                                                        for(int j = 0; j < campos1.size();j++){
                                                                if(j==0){
                                                                        char p;
                                                                        if(!in1.read(&p,sizeof(char)))
                                                                                break;
                                                                }else{
                                                                        string str(campos1[j].tipo);
                                                                        if(str.compare("Entero")==0){
                                                                                charint ci2;
                                                                                char b[sizeof(int)];
                                                                                if(!in1.read(b,sizeof(int)))
                                                                                        break;
                                                                                memcpy(ci2.raw,b,sizeof(int));
                                                                                stringstream s;
                                                                                s << ci2.num;
                                                                                string str1(campos1[j].nombre);
                                                                                string str2(campos3[k].nombre);
                                                                                if(str1.compare(str2)==0)
                                                                                        out.write(reinterpret_cast<char*>
                                                                                        (&ci2.num),sizeof(int));
                                                                        } else{
                                                                                char b[campos1[j].tamano-1];
                                                                                if(!in1.read(b,campos1[j].tamano-1))
                                                                                        break;
                                                                                b[campos1[j].tamano-1]='\0'; 
                                                                                string str1(campos1[j].nombre);
                                                                                string str2(campos3[k].nombre);
                                                                                if(str1.compare(str2)==0)
                                                                                        out.write(reinterpret_cast<char*>
                                                                                        (&b),campos1[j].tamano-1);


                                                                        }

                                                                }
                                                        }
                                                }

                                        }
                                        int sz = 0;
                                        for(int k=0;k<campos1.size();k++){
                                                string str(campos1[k].tipo);
                                                if(str.compare("Entero")==0)
                                                        sz += sizeof(int);
                                                else
                                                        sz = sz + campos1[k].tamano -1;

                                        }
                                        sz = sz + of;
                                        in1.seekg(sz);   
                                        cout << in1.tellg() << endl;
                                        for(int k = f1+1; k < campos3.size() ; k++){
                                                if(k == 0){
                                                        char p = '0';
                                                        out.write(reinterpret_cast<char*>(&p),sizeof(char));
                                                }else{
                                                        in2.seekg(offset2);
                                                        for(int j = 0; j < campos2.size();j++){
                                                                if(j==0){
                                                                        char p;
                                                                        if(!in2.read(&p,sizeof(char)))
                                                                                break;
                                                                }else{
                                                                        string str(campos2[j].tipo);
                                                                        if(str.compare("Entero")==0){
                                                                                charint ci2;
                                                                                char b[sizeof(int)];
                                                                                if(!in2.read(b,sizeof(int)))
                                                                                        break;
                                                                                memcpy(ci2.raw,b,sizeof(int));
                                                                                stringstream s;
                                                                                s << ci2.num;
                                                                                string str1(campos2[j].nombre);
                                                                                string str2(campos3[k].nombre);
                                                                                if(str1.compare(str2)==0)
                                                                                        out.write(reinterpret_cast<char*>
                                                                                        (&ci2.num),sizeof(int));
                                                                        } else{
                                                                                char b[campos2[j].tamano-1];
                                                                                if(!in2.read(b,campos2[j].tamano-1))
                                                                                        break;
                                                                                b[campos2[j].tamano-1]='\0'; 
                                                                                string str1(campos2[j].nombre);
                                                                                string str2(campos3[k].nombre);
                                                                                if(str1.compare(str2)==0)
                                                                                        out.write(reinterpret_cast<char*>
                                                                                        (&b),campos2[j].tamano-1);


                                                                        }

                                                                }
                                                        }
                                                }

                                        }
                                        i =  0;
                                    }
                                }//fin else
                }//fin escritura
            in1.close();
            in2.close();
            out.close();
        }//fin bandera
}//fin Cruzar



int main(int argc, char* argv[]){
        char*nbin;
        char*nind;
        char*btree;
        string name;
        string trein;
        vector<Campo> registros;
        map<string,int> indices;
        cout<<"Bienvenido"<<endl;
        int opcion;
        bool flag=false;
        int opcion2=0;
        while(Menu(flag,nbin,nind,btree,registros,indices,name,trein) != 9);
        Reindexar(nbin,indices,registros);
        guardarindices(nind,indices,registros[1]);
        return 0;
}
void Reindexar(const char* nbin,map<string,int>&indices, vector<Campo> registros){
        indices.clear();
        charint cbyte;
        charint ci;
        int i=0;
        ifstream in(nbin,ios::in|ios::binary);
        char b[sizeof(int)];
        in.read(b, sizeof(int));
        memcpy(cbyte.raw,b,sizeof(int));
        in.seekg(cbyte.num,ios::beg);
        int rrn = 0;
        while(true){
                Indice i;
                int os = offset(rrn, registros)+cbyte.num;
                in.seekg(os);
                char p;
                if(!in.read(&p,sizeof(char)))
                        break;
                if(p != '*'){
                        in.seekg(os+sizeof(char));
                        string tipo(registros[1].tipo);
                        if(tipo.compare("Entero")==0){
                                char buffer[sizeof(int)];
                                if(!in.read(buffer,sizeof(int)))
                                        break;
                                charint ci;
                                memcpy(ci.raw,buffer,sizeof(int));
                                stringstream ss ;
                                ss << ci.num;
                                i.Key = ss.str();
                        }else{
                                char buffer[registros[1].tamano-1];
                                if(!in.read(buffer, registros[1].tamano-1))
                                        break;
                                buffer[registros[1].tamano-1]='\0';
                                string k(buffer);
                                //cout << buffer << endl;
                                i.Key = k;
                        }
                        i.offset = os;
                        indices.insert(pair<string,int>(i.Key,i.offset));

                }
                rrn++;
        }
        in.close();	
}
void headerindices(const char* nind){
        int seguro=0;
        ofstream out (nind,ios::out|ios::binary);
        out.write(reinterpret_cast<char*>(&seguro),sizeof(int));
        out.close();
}
void guardarindices(const char*nind, map<string,int>&indices,Campo c){
        ofstream out(nind, ios::out|ios::binary);
        out.seekp(4);
        for(map<string,int>::iterator it = indices.begin(); it != indices.end(); ++it){     
                string p(c.tipo);
                if(p.compare("Entero")==0){
                        int value = atoi((it->first).c_str());
                        out.write(reinterpret_cast<char*>(&value),sizeof(int));
                }else{
                        string s (it->first);
                        out.write(s.c_str(),c.tamano-1); 
                }
                int off = it->second;		
                out.write(reinterpret_cast<char*>(&off),sizeof(int));
        }
        int seguro=1;
        out.seekp(0,ios::beg);
        out.write(reinterpret_cast<char*>(&seguro),sizeof(int));
        out.close();
}
void leerindices(const char*nind, map<string,int>&indices,Campo c){
        indices.clear();
        ifstream in(nind, ios::in|ios::binary);
        in.seekg(sizeof(int),ios::beg);
        Indice i;
        while(true){
                string tipo(c.tipo);
                if(tipo.compare("Entero")==0){
                        charint value;
                        char buffer[sizeof(int)];
                        if(!in.read(buffer,sizeof(int)))
                                break;
                        memcpy(value.raw,buffer,sizeof(int));
                        stringstream ss;
                        ss << value.num;
                        i.Key = ss.str();
                }else{
                        char buffer[c.tamano-1];
                        if(!in.read(buffer,c.tamano-1))
                                break;
                        buffer[c.tamano-1] = '\0';
                        string s(buffer);
                        i.Key = s;
                }
                charint off;
                char b[sizeof(int)];
                if(!in.read(b,sizeof(int)))
                        break;
                memcpy(off.raw,b,sizeof(int));
                i.offset = off.num;
                indices.insert(pair<string,int>(i.Key,i.offset));
        }

      
        in.close();
}
int Menu(bool& flag, char*& nbin,char*& nind,char*& btree, vector<Campo>&registros, map<string,int>&indices,string&name
                ,string&trein){
        int opcion;
        string name1;
        string name2; 
        stringstream strstream;
        cout << "1.Abrir Archivo\n2.Insertar Datos\n3.Listar\n4.Borrar\n5.Modificar\n6.Buscar"<< endl;
        cout << "7.Compactar\n8.Cruzar Archivos\n9.Salir" << endl;
        cin >> opcion; 
       
        if(opcion == 1){
                trein = "Normal";
                registros.clear();
                cout << "Ingrese el nombre del archivo" << endl;
                cin >> name;
                strstream << name;
                strcpy((char*)name1.c_str(),(const char*)name.c_str());
                strcpy((char*)name2.c_str(),(const char*)name1.c_str());
                nbin = (char*)name1.c_str();
                nind = (char*)strstream.str().c_str();
                cout << nind << endl;
                btree = (char*)name.c_str();
                strcat(nbin,".bin");
                if(getStructure(nbin,registros)==0){
                        int numcampos;
                        cout << "Ingrese el numero de campos " << endl;
                        cin >> numcampos;
                        numcampos;
                        Campo cam;
                        cam.tamano = 2;
                        cout << nind << endl;
                        string s2 = "CHAR";
                        cout << nind << endl;
                        strcpy(cam.nombre,s2.c_str());
                        string ss = "Texto";
                        strcpy(cam.tipo,ss.c_str());
                        registros.push_back(cam);
                        cout << nind << endl;
                        for(int i=0; i  < numcampos; i++){
                                Campo c;
                                int tipo;
                                cout << "Ingrese el nombre del campo" << endl;
                                cin >> c.nombre;
                                cout << "Seleccione el tipo: " << endl << "1.Entero" << endl << "2.Texto" << endl;
                                cin >> tipo;
                                if(tipo == 1){
                                        string s3 = "Entero";
                                        strcpy(c.tipo,s3.c_str());
                                }else{
                                        string s3 = "Texto";
                                        strcpy(c.tipo,s3.c_str());
                                        cout << "Ingrese la longitud " << endl;
                                        cin >> c.tamano;
                                        c.tamano++;
                                }
                                registros.push_back(c);
                        }
                        cout << nind << endl;
                        Header(nbin,registros);                 
                        int choice;
                        cout << "Desea utilizar indices?[0(si)/1(no)]" << endl;
                        cin >> choice;
                        if(choice == 0){
                                cout << "1.Indice Lineal\n2.Arbol B" << endl;
                                cin >> choice; 
                                if(choice == 1){
                                        trein = "Indice";
                                        nind = (char*)strstream.str().c_str();
                                        strcat(nind,".ind");
                                        cout << nind << endl;
                                        headerindices(nind);
                                }else{
                                        trein = "Arbol";
                                        strcat(btree,".btree");
                                }
                        }else{
                                trein  = "Normal";
                        }

                }else{
                        strcat(nind,".ind");
                        strcat(btree,".btree");
                        ifstream in1(nind,ios::in|ios::binary);
                        ifstream in2(btree,ios::in|ios::binary);
                        if(in1){
                                trein = "Indice"; 
                                leerindices(nind,indices,registros[1]);
                        }else{
                                if(in2){
                                        trein = "Arbol";                              
                                }
                        }

                }
        }
        if(opcion == 2){
                if(name.compare("")==0)
                        cout << "Debe de abrir un archivo para poder insertar" << endl;
                else{
                        if(trein.compare("Normal")==0){
                                Agregar(nbin,registros,indices,false);         
                        }else{
                                if(trein.compare("Indice")==0){
                                        Agregar(nbin,registros,indices,true);
                                        Reindexar(nbin,indices,registros);
                                }else{
                                }
                        }
                }

        }
        if(opcion == 3){
                if(name.compare("")==0)
                        cout << "Debe de abrir un archivo para poder listar" << endl;
                else{
                        if(trein.compare("Normal")==0){
                                Listar(nbin,registros);

                        }else{
                                if(trein.compare("Indice")==0){
                                        ILListar(nbin,indices,registros);
                                }else{
                                }
                        }
                }

        }
        if(opcion == 4){
                if(name.compare("")==0)
                        cout << "Debe de abrir un archivo para poder borrar" << endl;
                else{
                        if(trein.compare("Normal")==0){
                                Borrar(nbin,registros);

                        }else{
                                if(trein.compare("Indice")==0){
                                        ILBorrar(nbin,registros,indices);
                                        Reindexar(nbin,indices,registros);

                                }else{
                                }
                        }
                }

        }
        if(opcion == 5){
                if(name.compare("")==0)
                        cout << "Debe de abrir un archivo para poder modificar" << endl;
                else{
                        if(trein.compare("Normal")==0){
                                Modificar(nbin,registros);

                        }else{
                                if(trein.compare("Indice")==0){
                                        ILModificar(nbin,registros,indices);
                                        Reindexar(nbin,indices,registros);
                                }else{
                                }
                        }
                }

        }
        if(opcion == 6){
                if(name.compare("")==0)
                        cout << "Debe de abrir un archivo para poder buscar" << endl;
                else{
                        string dato;
                        cout << "Ingrese el dato a buscar" << endl;
                        cin >> dato;
                        if(trein.compare("Normal")==0){
                                Buscar(nbin, registros,dato,true);
                        }else{
                                if(trein.compare("Indice")==0){
                                        ILBuscar(nbin,registros, indices);
                                }else{
                                }
                        }
                }

        }
        if(opcion == 7){
                if(name.compare("")==0)
                        cout << "Debe de abrir un archivo para poder compactar" << endl;
                else{
                        if(trein.compare("Normal")==0){
                                Compactar(nbin,registros);

                        }else{
                                if(trein.compare("Indice")==0){
                                        Reindexar(nbin,indices, registros);
                                        guardarindices(nind, indices,registros[1]);                              
                                }else{
                                }
                        }
                }

        }
        if(opcion == 8){
                if(trein.compare("Normal")==0){
                            Cruzar();
                }else{
                       if(trein.compare("Indice")==0){
                                  ILCruzar();
                       }
                }
        }
            
        if(opcion ==9){
                if(trein.compare("Indice")==0)
                        guardarindices(nind, indices,registros[1]);
                return 9;
        }
        return 0;

}
void ILBuscar(const char* nbin,vector<Campo>registros, map<string,int>Indices){
        string dato;
        cout << "Ingrese la llave del registro" << endl;
        cin >> dato;
        map<string,int>::iterator it;
        it = Indices.find(dato);
        if (it != Indices.end()){
                ifstream in(nbin,ios::in|ios::binary);
                in.seekg(it->second);
                for(int i=0;i<registros.size();i++){
                        string p(registros[i].tipo);
                        if(p.compare("Entero")==0){
                                charint value;
                                char buffer[sizeof(int)];
                                in.read(buffer,sizeof(int));
                                memcpy(value.raw,buffer,sizeof(int));
                                if(i!=0)
                                        cout << value.num << ",";
                        }else{
                                char buffer[registros[i].tamano-1];
                                in.read(buffer,registros[i].tamano-1);
                                buffer[registros[i].tamano-1] = '\0';
                                if(i!=0)
                                        cout << buffer << ",";
                        }
                }
                cout << endl;
        }else{
                cout << "El registro no fue encontrado" << endl;
        }
}
void Header(const char* nbin, vector<Campo>& registros){
        ofstream out(nbin, ios::out|ios::binary);
        int cbytes = 2*sizeof(int) + sizeof(long int);
        int numcampos, tipo;
        long int records = 0;
        int availlist= 0;
        cbytes = cbytes + sizeof(Campo)*(registros.size());
        out.write(reinterpret_cast<char*>(&cbytes), sizeof(int));
        out.write(reinterpret_cast<char*>(&availlist),sizeof(int));
        out.write(reinterpret_cast<char*>(&records),sizeof(long int));
        for(int i=0; i  < registros.size(); i++){
                out.write(reinterpret_cast<char*>(&registros[i]), sizeof(Campo));
        }	
        out.close();
}//Es en donde el usuario define la estructura
int getStructure(const char*nbin, vector<Campo>& registros){
        ifstream in(nbin, ios::in|ios::binary);
        charint ci;
        charint availlist;
        charlongint records;
        if(in){
                char buffer[sizeof(int)];
                in.read(buffer,sizeof(int));
                memcpy(ci.raw,buffer,sizeof(int));
                in.read(buffer,sizeof(int));
                memcpy(availlist.raw,buffer,sizeof(int));
                char b[sizeof(long int)];
                in.read(b,sizeof(long int));
                memcpy(records.raw,b,sizeof(long int));
                Campo c;
                int bytes=0;
                ci.num=ci.num-2*sizeof(int)-sizeof(long int);
                while(bytes < ci.num){
                        if(!in.read(reinterpret_cast<char*>(&c),sizeof(Campo)))
                                break;
                        registros.push_back(c);
                        bytes+=sizeof(Campo);
                }
                in.close();
        }else{
                return 0;
        }

}//Donde se obtiene una estructura guardada previamente
void Agregar(const char* nbin, vector<Campo> registros, map<string,int>& indices, bool flag){
        fstream out (nbin, ios::in|ios::binary|ios::out);
        charint availist;
        charlongint records;
        out.seekg(4);
        char b[sizeof(int)];
        out.read(b, sizeof(int));
        memcpy(availist.raw,b,sizeof(int));
        char m[sizeof(long int)];
        out.read(m, sizeof(long int));
        memcpy(records.raw, m, sizeof(long int));
        if(availist.num != 0){
                out.seekp(availist.num+1);
                char c[sizeof(int)];
                out.read(c,sizeof(int));
                charint av;
                memcpy(av.raw,c,sizeof(int));
                int x = av.num;
                out.seekp(4,ios::beg);
                out.write(reinterpret_cast<char*>(&x),sizeof(int));
                out.seekp(availist.num,ios::beg);
        }else{
                out.seekp(0,ios::end);
        }
        Indice ind;
        ind.offset = out.tellg();
        for(int i=0; i< registros.size();i++){
                if(i==0){
                        char p = '0';
                        out.write(&p,sizeof(char));
                }else{
                        string s(registros[i].tipo);
                        if(s.compare("Entero")==0){
                                int value;
                                cout << "Ingrese " << registros[i].nombre << endl;
                                cin >> value;
                                if(i==0){
                                        stringstream ss;
                                        ss << value;
                                        ind.Key = ss.str();
                                }
                                out.write(reinterpret_cast<char*>(&value), sizeof(int));
                        }else{
                                char texto[registros[i].tamano];
                                cout << "Ingrese " << registros[i].nombre << endl;
                                cin >> texto;
                                if(i==0)
                                        ind.Key = texto;
                                out.write(texto,registros[i].tamano-1); 
                        }
                }	
        }
        if(flag){
                indices.insert(pair<string,int>(ind.Key,ind.offset));
        }
        records.num +=1;
        out.seekp(8,ios::beg);
        out.write(reinterpret_cast<char*>(&records.num),sizeof(long int));
        out.close();

}//Agrega los registros
void ILListar(const char*nbin, map<string,int>indices, vector<Campo>campos){
        ifstream in(nbin, ios::in|ios::binary);
        int i = 1;
        string s(campos[1].tipo);
        stringstream border;
        stringstream header;
        header <<setfill(' ')<<setw(1)<<"|"<<setw(5)<<left<<"RRN"<<setw(1)<<"|";
        for(int k=1;k<campos.size();k++){
                if(k==1)
                        border << setfill('-')<<setw(1)<<"+"<<setw(5)<<"-"<<setw(1)<<"+";
                string tipo(campos[k].tipo);
                if(tipo.compare("Entero")==0){
                        header<<setw(10)<<left<<campos[k].nombre<<setw(1)<<"|";
                        border << setw(10) << "-" << setw(1)<<"+";
                }else{
                        header<<setw(campos[k].tamano+5)<<left << campos[k].nombre << setw(1) << "|";
                        border << setw(campos[k].tamano+5) << "-" << setw(1) << "+";
                }
        }
        border << endl;
        cout << border.str();
        cout <<header.str();
        cout <<endl<< border.str();
        int cant =0;
        if(s.compare("Entero")==0){
                map<int,int> temporal;
                for(map<string,int>::iterator it = indices.begin(); it != indices.end(); ++it){
                        string Key = it->first;
                        int offset = it->second;
                        temporal.insert(pair<int,int>(atoi(Key.c_str()),offset));
                }
                for(map<int,int>::iterator it = temporal.begin(); it != temporal.end();++it){

                        in.seekg(it->second,ios::beg);
                        cout <<setfill(' ')<<setw(1)<<"|"<<setw(5)<<right<< i << setw(1)<<"|";			
                        for(int j=0;j<campos.size(); j++){
                                string tipo(campos[j].tipo);
                                if(tipo.compare("Entero")==0){
                                        charint ci;
                                        char buffer[sizeof(int)];
                                        if(!in.read(buffer,sizeof(int)))
                                                break;
                                        memcpy(ci.raw,buffer,sizeof(int));
                                        if(j!=0)
                                                cout<<setw(10)<<left<<ci.num<<setw(1)<<"|";
                                }else{
                                        char buffer[campos[j].tamano-1];
                                        if(!in.read(buffer,campos[j].tamano-1))
                                                break;
                                        buffer[campos[j].tamano-1]='\0';
                                        if(j!=0)
                                                cout<<setw(campos[j].tamano+5)<<left<<buffer<<setw(1)<<"|";            					
                                }
                        }
                        cout << endl << border.str();
                        i++;
                        cant++;
                        if(cant==18){
                                int option;
                                cant=0;
                                cout <<"Desea ver mas registros?[0/1]"<<endl;
                                cin >>option;
                                if(option!=0){
                                        break;
                                }else{
                                        cout << endl << border.str();
                                        cout << header.str() << endl;
                                        cout << border.str();

                                }	
                        }	
                }
        }else{
                int cant=0;
                for(map<string,int>::iterator it = indices.begin(); it != indices.end(); it++){
                        in.seekg(it->second,ios::beg);
                        cout <<setfill(' ')<<setw(1)<<"|"<<setw(5)<<left<< i << setw(1)<<"|";						
                        for(int j=0;j<campos.size(); j++){
                                string tipo(campos[j].tipo);
                                if(tipo.compare("Entero")==0){
                                        charint ci;
                                        char buf[sizeof(int)];
                                        if(!in.read(buf,sizeof(int)))
                                                break;
                                        memcpy(ci.raw,buf,sizeof(int));
                                        if(j!=0){
                                                cout<<setw(10)<<left<<ci.num<<setw(1)<<"|";	
                                        }
                                }else{
                                        char buf2[campos[j].tamano-1];
                                        if(!in.read(buf2,campos[j].tamano-1))
                                                break;
                                        buf2[campos[j].tamano-1]='\0';
                                        if(j!=0)
                                                cout<<setw(campos[j].tamano+5)<<left<<buf2<<setw(1)<<"|";            										
                                }
                        }
                        cout << endl<<border.str();
                        i++;
                        cant++;
                        if(cant==18){
                                int option;
                                cant=0;
                                cout <<"Desea ver mas registros?[0/1]"<<endl;
                                cin >>option;
                                if(option!=0){
                                        break;
                                }else{
                                        cout << endl << border.str();
                                        cout << header.str() << endl;
                                        cout << border.str();

                                }								
                        }
                }
        }
}


void Listar(const char* nbin, vector<Campo> registros){
        charint cbyte;
        charint ci;
        int i=0;
        ifstream in(nbin,ios::in|ios::binary);
        char b[sizeof(int)];
        in.read(b, sizeof(int));
        memcpy(cbyte.raw,b,sizeof(int));
        in.seekg(cbyte.num,ios::beg);
        stringstream border;
        stringstream header;
        header <<setfill(' ')<<setw(1)<<"|"<<setw(5)<<left<<"RRN"<<setw(1)<<"|";
        for(int k=1;k<registros.size();k++){
                if(k==1)
                        border << setfill('-')<<setw(1)<<"+"<<setw(5)<<"-"<<setw(1)<<"+";
                string tipo(registros[k].tipo);
                if(tipo.compare("Entero")==0){
                        border << setw(10) << "-" << setw(1)<<"+";
                        header << setw(10) << left << registros[k].nombre << setw(1) << "|";
                }else{
                        header<<setw(registros[k].tamano+5)<<left << registros[k].nombre << setw(1) << "|";
                        border << setw(registros[k].tamano+5) << "-" << setw(1) << "+";
                }
        }
        border << endl;
        cout << border.str();
        cout << header.str();
        int cant=0;
        int pos = 1;
        bool flag = true;
        cout  << endl << border.str();
        while(flag){
                if(i==registros.size()){	
                        cout << endl << border.str();
                        i=0;
                        pos++;
                        cant++;
                }
                if(i==0){
                        char p;
                        if(!in.read(&p,sizeof(char)))
                                break;
                        if(p == '*'){
                                in.seekg((int)in.tellg()-1);
                                int size = 0;
                                for(int k = 0 ;k<registros.size() ;k++){
                                        string str(registros[k].tipo);
                                        if(str.compare("Entero")==0)
                                                size += sizeof(int);
                                        else
                                                size = size + registros[k].tamano -1;
                                }
                                size = size + in.tellg();
                                in.seekg(size);
                                pos++;
                                i=0;
                        }else{
                                i++;
                        }
                }else{
                        if(i==1){
                                cout <<setfill(' ')<<setw(1)<<"|"<<setw(5)<<left<< pos <<setw(1)<<"|"; 
                        }             
                        string tipo(registros[i].tipo);
                        if(tipo.compare("Entero")==0){
                                char buffer[sizeof(int)];
                                if(!in.read(buffer,sizeof(int)))
                                        break;
                                memcpy(ci.raw,buffer,sizeof(int));
                                cout <<setw(10)<< left<< ci.num << setw(1)<<"|";
                                i++;	
                        }else{
                                char buffer[registros[i].tamano-1];
                                if(!in.read(buffer,registros[i].tamano-1))
                                        break;
                                buffer[registros[i].tamano-1]='\0';
                                cout <<setw(registros[i].tamano+5)<<left<<buffer<<setw(1)<<"|";            
                                i++;
                        }
                }
                if(cant == 18){
                        cant=0;
                        int option;
                        cout << "Desea ver mas registros? [0/1]" << endl;
                        cin >> option;
                        if(option == 1)
                                flag = false;
                        else{
                                cout << border.str();
                                cout << header.str()<<endl;
                                cout << border.str();
                        }
                }
        }	
        in.close();	

}//Lista los registros

void ILBorrar(const char* nbin, vector<Campo> registros,map<string,int>& indices){
        fstream in(nbin, ios::out|ios::binary|ios::in);
        charint cbyte,availlist;
        charlongint records;
        char m[sizeof(long int)];//agregado
        char b[sizeof(int)];
        in.read(b, sizeof(int));
        memcpy(cbyte.raw,b,sizeof(int));
        in.read(b, sizeof(int));
        memcpy(availlist.raw,b, sizeof(int));
        in.read(m, sizeof(long int));//agregado
        memcpy(records.raw,m, sizeof(long int));//agregado
        records.num-=1;//agregado	
        ILListar(nbin,indices,registros);
        int posicion;
        cout << "Ingrese el numero del registro" << endl;
        cin >> posicion;
        posicion--;
        int i=0;
        int os ;
        for(map<string,int>::iterator it = indices.begin(); it != indices.end(); it++){
                if(i==posicion){
                        os = it -> second;
                        indices.erase(it->first);
                        break;
                }
                i++;
        }
        in.seekp(os);
        char p = '*';
        in.write(reinterpret_cast<char*>(&p),sizeof(char));
        in.write(reinterpret_cast<char*>(&availlist.num),sizeof(int));
        in.seekp(4);
        in.write(reinterpret_cast<char*>(&os),sizeof(int));
        in.seekp(8);
        in.write(reinterpret_cast<char*>(&records.num),sizeof(long int));//agregado
        in.close();

}
void Borrar(const char* nbin, vector<Campo> registros){
        Listar(nbin, registros);
        int rrn;
        cout << "Ingrese el numero del registro a eliminar" << endl;
        cin >> rrn;
        rrn=rrn-1;
        int os = offset(rrn, registros);
        charint cbyte;
        charint availlist;
        charlongint records;
        fstream in(nbin,ios::in|ios::binary|ios::out);
        char m[sizeof(long int)];//agregado
        char b[sizeof(int)];
        in.read(b, sizeof(int));
        memcpy(cbyte.raw,b,sizeof(int));
        os=os+cbyte.num;
        in.read(b, sizeof(int));
        memcpy(availlist.raw,b, sizeof(int));
        in.read(m, sizeof(long int));//agregado
        memcpy(records.raw,m, sizeof(long int));//agregado
        records.num-=1;//agregado
        in.seekp(os);
        char p = '*';
        in.write(reinterpret_cast<char*>(&p),sizeof(char));
        in.write(reinterpret_cast<char*>(&availlist.num),sizeof(int));
        in.seekp(4);
        in.write(reinterpret_cast<char*>(&os),sizeof(int));
        in.seekp(8);
        in.write(reinterpret_cast<char*>(&records.num),sizeof(long int));//agregado
        in.close();
}//Borra al agregar un * en el primer byte del registro y un caracter representando el offset del siguiente espacio disponible
int offset(int rrn,vector<Campo> registros){
        int size = 0;
        for(int i=0; i < registros.size() ; i++){
                string p(registros[i].tipo);
                if(p.compare("Entero")==0)
                        size+=sizeof(int);
                else
                        size = size + registros[i].tamano -1;
        }
        return rrn*size;
}
void ILModificar(const char*nbin,vector<Campo> registros,map<string,int>& indices){
        ILListar(nbin,indices,registros);
        fstream out(nbin, ios::out|ios::binary|ios::in);
        int posicion;
        cout << "Ingrese el numero del registro" << endl;
        cin >> posicion;
        posicion--;
        int i=0;
        for(map<string,int>::iterator it = indices.begin(); it != indices.end(); it++){
                if(i==posicion){
                        out.seekp(it->second);
                        indices.erase(it->first);
                        break;
                }
                i++;

        }
        Indice ind;
        ind.offset = out.tellp();
        for(int i=0;i<registros.size();i++){
                string tipo(registros[i].tipo);
                if(i==0){
                        char p = '0';
                        out.write(&p,sizeof(char));
                }else{
                        if(tipo.compare("Entero")==0){
                                int value;
                                cout << "Ingrese "<< registros[i].nombre << endl;
                                cin >> value;
                                if(i==1){
                                        stringstream ss;
                                        ss << value;
                                        ind.Key = ss.str();
                                }
                                out.write(reinterpret_cast<char*>(&value),sizeof(int));
                        }else{
                                char dato[registros[i].tamano];
                                cout << "Ingrese " << registros[i].nombre << endl;
                                cin >> dato;
                                if(i==1){
                                        string s(dato);
                                        ind.Key = s;
                                }
                                out.write(dato,registros[i].tamano-1);
                        }
                }
        }
        indices.insert(pair<string,int>(ind.Key,ind.offset));
        out.close();	
}
void Modificar(const char* nbin, vector<Campo> registros){
        Listar(nbin, registros);
        int rrn;
        cout << "Ingrese el numero del registro " << endl;
        cin >> rrn;
        rrn=rrn-1;
        int os = offset(rrn, registros);
        charint cbyte;
        fstream in(nbin,ios::in|ios::binary|ios::out);
        char b[sizeof(int)];
        in.read(b,sizeof(int));
        memcpy(cbyte.raw,b,sizeof(int));
        os+=cbyte.num;
        in.seekp(os);
        for(int i=0; i< registros.size();i++){
                if(i == 0){
                        char p = '0';
                        in.write(reinterpret_cast<char*>(&p),sizeof(char));
                }else{
                        string p(registros[i].tipo);
                        if(p.compare("Entero")==0){
                                int value;
                                cout << "Ingrese " << registros[i].nombre << endl;
                                cin >> value;
                                if(i==0)
                                        in.seekp(os);
                                in.write(reinterpret_cast<char*>(&value), sizeof(int));

                        }else{
                                char texto[registros[i].tamano];
                                cout << "Ingrese " << registros[i].nombre << endl;
                                cin >> texto;
                                in.write(texto,registros[i].tamano-1);//si lo modifica 
                        }
                }
        }
        in.close();
}
int Buscar(const char* nbin, vector<Campo> registros,string data,bool bandera){
        charint cbyte;
        charint ci;
        int offset;
        int i=0;
        ifstream in(nbin,ios::in|ios::binary);
        char b[sizeof(int)];
        in.read(b, sizeof(int));
        memcpy(cbyte.raw,b,sizeof(int));
        in.seekg(cbyte.num,ios::beg);
        bool flag = false;
        stringstream registro;
        while(true){
                if(i==registros.size()){
                        if(flag)
                                break;
                        registro.str("");
                        i=0;
                }
                if(i==0){
                        offset = in.tellg();
                        char p;
                        if(!in.read(&p,sizeof(char)))
                                break;
                        if(p=='*'){
                                int size = (int)in.tellg() - sizeof(char);
                                for(int k=1; k<registros.size(); k++){
                                        string str(registros[k].tipo);
                                        if(str.compare("Entero"))
                                                size+=sizeof(int);
                                        else{
                                                size = size + registros[k].tamano-1;
                                        }
                                }
                                in.seekg(size);
                                i=0;
                        }else{
                                i++;
                        }
                }else{
                        string p(registros[i].tipo);
                        if(p.compare("Entero")==0){
                                char buffer[sizeof(int)];
                                if(!in.read(buffer,sizeof(int)))
                                        break;

                                memcpy(ci.raw,buffer,sizeof(int));
                                stringstream s;
                                s << ci.num;
                                if(s.str().compare(data)==0){
                                        flag=true;
                                }
                                registro << s.str() << ",";
                                i++;	
                        }else{
                                char buffer[registros[i].tamano-1];
                                if(!in.read(buffer,registros[i].tamano-1))
                                        break;
                                buffer[registros[i].tamano-1]='\0';
                                string s(buffer);
                                if(s.compare(data)==0){
                                        flag=true;
                                }
                                registro << s << ",";
                                i++;
                        }
                }
        }
        in.close();	
        if(bandera){
                if(flag)
                        cout << registro.str() << endl;
                else
                        cout << "No se encontro el registro" << endl;
        }
        if(!flag)
                return -1;
        return offset;
}
void Compactar(const char* nbin,vector<Campo> registros){
        fstream in(nbin, ios::in|ios::binary);
        ofstream out("temporal.bin", ios::out|ios::binary);
        charint cbyte;
        charint ci;
        charint availlist;
        charlongint records;
        int i=0;
        char b[sizeof(int)];
        in.read(b, sizeof(int));
        memcpy(cbyte.raw,b,sizeof(int));
        in.read(b, sizeof(int));
        memcpy(availlist.raw,b,sizeof(int));	 
        char b2[sizeof(long int)];
        in.read(b2, sizeof(long int));
        memcpy(records.raw, b2, sizeof(long int));
        int al=0;  
        Campo c;
        int bytes=0;
        out.write(reinterpret_cast<char*>(&cbyte.num),sizeof(int));
        out.write(reinterpret_cast<char*>(&al),sizeof(int));
        out.write(reinterpret_cast<char*>(&records.num),sizeof(long int));
        while(bytes < cbyte.num-2*sizeof(int)-sizeof(long int)){
                if(!in.read(reinterpret_cast<char*>(&c),sizeof(Campo)))
                        break;
                out.write(reinterpret_cast<char*>(&c),sizeof(Campo));
                bytes+=sizeof(Campo);
        }
        in.seekg(cbyte.num);
        while(true){
                if(i==registros.size()){
                        i=0;
                }
                char m;
                if(!in.read(reinterpret_cast<char*>(&m),sizeof(char)))
                        break;
                if(m == '*'){
                        in.seekg((int)in.tellg()-sizeof(char));
                        int size = 0;
                        for(int i=0;i < registros.size() ;i++){
                                string str(registros[i].tipo);
                                if(str.compare("Entero")==0)
                                        size += sizeof(int);
                                else
                                        size = size + registros[i].tamano-1;
                        }
                        size = size + in.tellg();
                        in.seekg(size);
                }else{
                        in.seekg((int)in.tellg()-sizeof(char));
                }		
                string g(registros[i].tipo);
                if(g.compare("Entero")==0){
                        char buffer[sizeof(int)];
                        if(!in.read(buffer,sizeof(int)))
                                break;
                        i++;
                        charint value;
                        memcpy(value.raw,buffer,sizeof(int));
                        out.write(reinterpret_cast<char*>(&value.num),sizeof(int));
                }else{
                        char buffer[registros[i].tamano-1];
                        if(!in.read(buffer,registros[i].tamano-1))
                                break;			
                        out.write(buffer, registros[i].tamano-1);	
                        i++;
                }
        }	

        out.close();	
        in.close();
        remove(nbin);
        rename("temporal.bin",nbin);
}

