#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<cstring>
#include<stdio.h>
#include<stdlib.h>
using namespace std;
struct Campo{
	char nombre[15];
	char tipo[7];
	int tamano;
};
int getStructure(const char*, vector<Campo>&);
void Header(const char*, vector<Campo>&);
void Agregar(const char*,vector<Campo>);
void Listar(const char*, vector<Campo>);
void Borrar();
void Modificar();
void Compactar();
void Buscar();
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
		nbin = (char*)n.c_str();
		strcat(nbin, ".bin");
		getStructure(nbin, registros);
	
	}else{
		string n;
		cout<<"Ingrese el nombre de la estructura"<<endl;
		cin>>n;
		nbin = (char*)n.c_str();
		strcat(nbin,".bin");//AGREGA LA EXTENSION .BIN
		Header(nbin,registros);
	}
	int opcion2=0;
	while(opcion2 != 3){
		cout << "1.Agregar Datos\n2.Listar Datos\n3.Salir" << endl;
		cin >> opcion2;
		if(opcion2==1){
			cout << registros.size() << endl;
			Agregar(nbin, registros);
		}else{
			if(opcion2==2){
				Listar(nbin, registros);
			}
		}
	}

	return 0;
}
void Header(const char* nbin, vector<Campo>& registros){
	ofstream out(nbin, ios::out|ios::binary);
	char delimitador = ';';
	char resp = 's';
	int cbytes=8;
	int numcampos, tipo;
	int availlist= 0;
	cout << "Ingrese el numero de campos " << endl;
	cin >> numcampos;
	cbytes = cbytes + sizeof(Campo)*numcampos;
	out.write(reinterpret_cast<char*>(&cbytes), sizeof(int));
	out.write(reinterpret_cast<char*>(&availlist),sizeof(int));
    for(int i=0; i  < numcampos ; i++){
		Campo c;
		cout << "Ingrese el nombre del campo" << endl;
		cin >> c.nombre;
		cout << "Seleccione el tipo: " << endl << "1.Entero" << endl << "2.Texto" << endl;
		cin >> tipo;
		if(tipo == 1){
			string s = "Entero";
			strcpy(c.tipo,s.c_str());
		}else{
			string s = "Texto";
			strcpy(c.tipo,s.c_str());
			cout << "Ingrese la longitud " << endl;
			cin >> c.tamano;
		}
		out.write(reinterpret_cast<char*>(&c), sizeof(Campo));
		registros.push_back(c);
	}	
	out.close();
}//Es en donde el usuario define la estructura
int getStructure(const char*nbin, vector<Campo>& registros){
	ifstream in(nbin, ios::in|ios::binary);
	charint ci;
	charint availlist;
	if(in){
		char buffer[sizeof(int)];
		in.read(buffer,sizeof(int));
		memcpy(ci.raw,buffer,sizeof(int));
		in.read(buffer,sizeof(int));
	    memcpy(availlist.raw,buffer,sizeof(int));
		Campo c;
		int bytes=0;
		ci.num=ci.num-8;
		while(bytes < ci.num){
			if(!in.read(reinterpret_cast<char*>(&c),sizeof(Campo)))
				break;
			registros.push_back(c);
			bytes+=sizeof(Campo);
		}
		in.close();
	}else{
		cout << "No se pudo abrir el archivo" << endl;
		return 0;
	}

}//Donde se obtiene una estructura guardada previamente
void Agregar(const char* nbin, vector<Campo> registros){
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

}//Agrega los registros
void Listar(const char* nbin, vector<Campo> registros){
	charint cbyte;
	charint ci;
//	cout << registros.size() << endl;
	int i=0;
	ifstream in(nbin,ios::in|ios::binary);
	char b[sizeof(int)];
	in.read(b, sizeof(int));
	memcpy(cbyte.raw,b,sizeof(int));
	in.seekg(cbyte.num,ios::beg);
	while(!in.eof()){
		if(i==registros.size())
			i=0;
		if(registros[i].tipo=="Entero"){
			cout << i;
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

}
void Borrar(const char* nbin, vector<Campo> registros){
	Listar(nbin, registros);
	int offset;
	cout << "Ingrese el offset del registro a eliminar" << endl;
	cin >> offset;
	ofstream out(nbin,ios::out|ios::binary|ios::app);
}
void Modificar(){
}
void Buscar(){
}
void Compactar(){
}

