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
void ILBorrar(const char* , vector<Campo>,map<string,int>&);
void ILBuscar(const char* ,vector<Campo>, map<string,int>);
int Menu(bool&,const char*,const char*, vector<Campo>&, map<string,int>&);
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
void Buscar(const char*, vector<Campo>);
void guardarindices(const char*, map<string,int>&,Campo);
void ILModificar(const char*,vector<Campo>,map<string,int>&);
union charint{
	char raw[sizeof(int)];
	int num;
};
union charlongint{
	char raw[sizeof(long int)];
	long int num;
};
int main(int argc, char* argv[]){
	char*nbin;
	char*nind;
	string n;
	vector<Campo> registros;
	map<string,int> indices;
	cout<<"Bienvenido"<<endl;
	int opcion;
	cout<<"1.Abrir una Estructura\n2.Crear una Nueva Estructura"<<endl;
	cin>>opcion;
	if(opcion==1){
		cout << "Ingrese el nombre de la estructura" << endl;
		cin >> n;
		string s;
		strcpy((char*)s.c_str(),(const char*)n.c_str());
		nbin = (char*)s.c_str();
		nind = (char*)n.c_str();
		strcat(nind, ".ind");
		strcat(nbin, ".bin");
		if(getStructure(nbin, registros)==0)
			return 0;

	}else{
		cout<<"Ingrese el nombre de la estructura"<<endl;
		cin>>n;
		string s;			
		strcpy((char*)s.c_str(),(const char*)n.c_str());		
		nind = (char*)s.c_str();
		nbin = (char*)n.c_str();
		strcat(nind,".ind");
		strcat(nbin,".bin");//AGREGA LA EXTENSION .BIN
		Header(nbin,registros);
		headerindices(nind);

	}
	bool flag=false;
	int opcion2=0;
	while(Menu(flag,nbin,nind,registros,indices) != 9);
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
int Menu(bool& flag,const char* nbin,const char* nind, vector<Campo>&registros, map<string,int>&indices){
	if(!flag){
		cout << "1.Agregar Datos\n2.Listar Datos\n3.Buscar Registro\n4.Borrar\n5.Compactar"
			<< "\n6.Modificar\n7.Reindexar\n8.Activar Indice Lineal" 
			<<"\n9.Salir"<< endl;
	}else{
		cout << "1.Agregar Datos\n2.Listar Datos\n3.Buscar Registro\n4.Borrar"
			<<"\n5.Compactar\n6.Modificar\n7.Reindexar\n8.Desactivar Indice Lineal\n9.Salir"<< endl;
	}
	int opcion;
	cin >> opcion;
	if(opcion==1)
		Agregar(nbin,registros,indices,flag);
	if(opcion==2){
		if(flag){
			ILListar(nbin, indices, registros);
		}else
			Listar(nbin, registros);
	}
	if(opcion==3)
		if(flag)
			ILBuscar(nbin,registros,indices);
		else 
			Buscar(nbin,registros);
	if(opcion==4)
		if(flag)
			ILBorrar(nbin,registros,indices);
		else
			Borrar(nbin,registros);
	if(opcion==5)
		Compactar(nbin,registros);
	if(opcion==6){
		if(flag){
			ILModificar(nbin,registros,indices);
		}else
			Modificar(nbin,registros);
	}
	if(opcion==7){
		indices.clear();
		Reindexar(nbin,indices,registros);

	}
	if(opcion==8){
		if(flag)
			flag=false;
		else{
			leerindices(nind,indices,registros[1]);
			flag = true;
		}
	}
	if(opcion==9)
		return 9;
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
	char resp = 's';
	int cbytes = 2*sizeof(int) + sizeof(long int);
	int numcampos, tipo;
	long int records = 0;
	int availlist= 0;
	cout << "Ingrese el numero de campos " << endl;
	cin >> numcampos;
	numcampos++;
	Campo cam;
	cam.tamano = 2;
	string s = "CHAR";
	strcpy(cam.nombre,s.c_str());
	string ss = "Texto";
	strcpy(cam.tipo,ss.c_str());
	registros.push_back(cam);
	cbytes = cbytes + sizeof(Campo)*numcampos;
	out.write(reinterpret_cast<char*>(&cbytes), sizeof(int));
	out.write(reinterpret_cast<char*>(&availlist),sizeof(int));
	out.write(reinterpret_cast<char*>(&records),sizeof(long int));
	out.write(reinterpret_cast<char*>(&cam),sizeof(Campo));
	for(int i=0; i  < numcampos-1; i++){
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
			if(c.tamano==1)
				c.tamano=2;
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
		cout << "No se pudo abrir el archivo" << endl;
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
void Buscar(const char* nbin, vector<Campo> registros){
	string data;
	cout << "Ingrese el dato a buscar" << endl;
	cin >> data;
	charint cbyte;
	charint ci;
	//	cout << registros.size() << endl;
	int i=0;
	ifstream in(nbin,ios::in|ios::binary);
	char b[sizeof(int)];
	in.read(b, sizeof(int));
	memcpy(cbyte.raw,b,sizeof(int));
	in.seekg(cbyte.num,ios::beg);
	//cout << cbyte.num << endl;
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
	if(flag)
		cout << registro.str() << endl;
	else
		cout << "No se encontro el registro" << endl;	
	in.close();	

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

