#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<cstring>
#include<stdio.h>
#include<stdlib.h>
using namespace std;

struct Campo{
	string nombre;
	string tipo;
	int tamano;
};
void split(const string& s,char delimeter, vector<string>& v){
	string::size_type i=0;
	string::size_type j= s.find(delimeter);
	while (j != string::npos){
		v.push_back(s.substr(i, j-i));
		i = ++j;
		j = s.find(delimeter, j);
		if( j == string::npos)
			v.push_back(s.substr(i,s.length()));
	}
}
union charint{
	char raw[sizeof(int)];
	int num;
};
int main(int argc, char* argv[]){
	char*nbin;
	vector<Campo> registros;
	cout<<"Bienvenido"<<endl;
	int opcion;
	cout<<"1.Abrir una Estructura\n2.Crear una Nueva Estructura"<<endl;
	cin>>opcion;
	if(opcion==1){
		string n;
		cout << "Ingrese el nombre de la estructura" << endl;
		cin >> n;
		string x;
		strcpy((char*)x.c_str(),(char*)n.c_str());
		char* nestructura = (char*)n.c_str();
		nbin = (char*)x.c_str();
		strcat(nestructura, ".txt");
		strcat(nbin, ".bin");
		ifstream archivo;
		archivo.open(nestructura);
		if(archivo!=NULL){
		string line;
		while(getline(archivo,line)){
			vector<string> fields;
			split (line,'-',fields);
			Campo c;
			c.nombre = fields[0];
			c.tipo = fields[1];
			c.tamano = atoi(fields[2].c_str());	
			registros.push_back(c);
		}
		archivo.close();
		}else{
			cout<<"La estructura no existe!"<<endl;
			return 0;
		}
	}else{
		string n;
		cout<<"Ingrese el nombre de la estructura"<<endl;
		cin>>n;
		string x;
		strcpy((char*)x.c_str(),(char*)n.c_str());
		char* nestructura = (char*)n.c_str();
		nbin = (char*)x.c_str();
		strcat(nestructura,".txt");//AGREGA LA EXTENSION .TXT
		strcat(nbin,".bin");//AGREGA LA EXTENSION .BIN
		ofstream archivo;
		archivo.open(nestructura);
		char respuesta='s';
		while(respuesta=='s'){
			Campo c;
			cout<<"Ingrese el nombre del Campo"<<endl;
			cin >> c.nombre;
			int type;
			cout << "Seleccione el tipo del campo:\n1.Entero\n2.Texto"<<endl;
			cin >> type;
			if(type==1){
				c.tipo="Entero";
			}else{
				c.tipo="Texto";
				cout << "Ingrese la longitud del texto" << endl;
				cin >> c.tamano;
				if(c.tamano==1)
					c.tamano=2;
			}
			registros.push_back(c);
			archivo << c.nombre<< "-" << c.tipo << "-" << c.tamano << endl;
			archivo.flush();
			cout<<"Desea agregar mas campos?[s/n]"<<endl;
			cin>>respuesta;
		}
		archivo.close();//Se guarda la estructura para ser utilizada en el futuro
	}

	int opcion2=0;
	while(opcion2 != 3){
		cout << "1.Agregar Datos\n2.Listar Datos\n3.Salir" << endl;
		cin >> opcion2;
		if(opcion2==1){
			ofstream out(nbin, ios::out|ios::binary|ios::app);
			for(int i=0; i< registros.size();i++){
				if(registros[i].tipo == "Entero"){
					int value;
					cout << "Ingrese " << registros[i].nombre << endl;
					cin >> value;
					out.write(reinterpret_cast<char*>(&value), sizeof(int));
				}else{
					char texto[registros[i].tamano];
					cout << "Ingrese " << registros[i].nombre << endl;
					cin >> texto;
					out.write(texto,registros[i].tamano-1); 
				}
			}
			out.close();
		}else{
			if(opcion2==2){
				charint ci;
				int i=0;
				ifstream in(nbin,ios::in|ios::binary);
				while(!in.eof()){
					if(i==registros.size())
						i=0;
					if(registros[i].tipo=="Entero"){
						char buffer[sizeof(int)];
						if(!in.read(buffer,sizeof(int)))
							break;
						cout << registros[i].nombre <<"-";
						memcpy(ci.raw,buffer,sizeof(int));
						cout << ci.num << endl;
					}else{
						if(registros[i].tipo=="Texto"){
							char buffer[registros[i].tamano-1];
							if(!in.read(buffer,registros[i].tamano-1))
								break;
							cout << registros[i].nombre << "-";
							buffer[registros[i].tamano-1]='\0';
							cout << buffer << endl;
						}
					}		
					i++;
				}
				in.close();

			}}
	}

	return 0;
}
