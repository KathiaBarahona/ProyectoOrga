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
struct Indice{
	string Key;
	int offset;
};
int updateKey(vector<Indice>indices, int offset);
void deletekey(vector<Indice>&,string, Campo);
int offset(int,vector<Campo>);
void headerindices(const char*);
void BinaryInsertion(vector<Indice>&,Campo);
void Reindexar(const char*,vector<Indice>&, vector<Campo>);
void leerindices(const char*, vector<Indice>&, Campo);
int getStructure(const char*, vector<Campo>&);
int Menu(bool&,const char*, vector<Campo>&,vector<Indice>&);
void Header(const char*, vector<Campo>&);
void Agregar(const char*,vector<Campo>,vector<Indice>&,bool);
void Listar(const char*, vector<Campo>);
void ILListar(const char*, vector<Indice>, vector<Campo>);
void Borrar(const char*, vector<Campo>,bool,vector<Indice>&);
void Modificar(const char*,vector<Campo>);
void Compactar(const char*,vector<Campo>);
void Buscar(const char*, vector<Campo>);
void guardarindices(const char*, vector<Indice>&,Campo);
void ILModificar(const char*,vector<Campo>,vector<Indice>&);
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
	vector<Indice> indices;
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
		//leerindices(nind, indices,registros[0]);

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
	while(Menu(flag,nbin,registros,indices) != 8);
	guardarindices(nind,indices,registros[0]);
	return 0;
}
void Reindexar(const char* nbin,vector<Indice>&indices, vector<Campo> registros){
	charint cbyte;
	charint ci;
	//	cout << registros.size() << endl;
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
			in.seekg(os);
			string tipo(registros[0].tipo);
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
				char buffer[registros[0].tamano-1];
				if(!in.read(buffer, registros[0].tamano-1))
					break;
				buffer[registros[0].tamano-1]='\0';
				string k(buffer);
				//cout << buffer << endl;
				i.Key =k;
			}
		}else{
			i.Key = "";
		}
		i.offset = os;
		rrn++;
		indices.push_back(i);
		BinaryInsertion(indices,registros[0]);
	}
	in.close();	
}
void BinaryInsertion(vector<Indice>&indices,Campo c){
	string p(c.tipo);
	if(p.compare("Entero")==0){
		for(int i = 1; i < indices.size() ; i++){
			Indice auxiliar = indices[i];
			int first = 0;
			int last = i-1;
			while(first <= last){
				int middle = (int) ((first + last)/2);
				if(atoi(auxiliar.Key.c_str()) <= atoi(indices[middle].Key.c_str()))
					last = middle-1;
				else
					first = middle+1;
			}
			for(int k = i-1; k>=first; k--){
				indices[k+1]=indices[k];
			}//Se corren los demas elementos a la derecha
			indices[first] = auxiliar;
		}
	}else{
	}//cuando el tipo es texto (duda)

}
int updateKey(vector<Indice>indices, int offset){
		int first = 1;
		int last = indices.size();
		while(first <= last){
			int midpoint = (int)((first+last)/2);
			if(indices[midpoint].offset == offset){
				return midpoint;
			}else if(indices[midpoint].offset < offset){
				first = first + 1;
			}else
				last = last -1;			
		}	
		return -1;
}
void headerindices(const char* nind){
	int seguro=0;
	ofstream out (nind,ios::out|ios::binary);
	out.write(reinterpret_cast<char*>(&seguro),sizeof(int));
	out.close();
}
void guardarindices(const char*nind, vector<Indice>&indices,Campo c){
	ofstream out(nind, ios::in|ios::binary);
	out.seekp(4,ios::beg);
	for(int i=0;i<indices.size();i++){
		string p(c.tipo);
		if(p.compare("Entero")==0){
			int value = atoi(indices[i].Key.c_str());
			out.write(reinterpret_cast<char*>(&value),sizeof(int));
		}
		else{
			out.write(indices[i].Key.c_str(),c.tamano-1);
		}
		out.write(reinterpret_cast<char*>(&indices[i].offset),sizeof(int));
	}
	int seguro=1;
	out.seekp(0,ios::beg);
	out.write(reinterpret_cast<char*>(&seguro),sizeof(int));
	out.flush();
	out.close();
}
void leerindices(const char*nind, vector<Indice>&indices,Campo c){
	ifstream in(nind, ios::in|ios::binary);
	charint seguro;
	charint value;
	char buffer[sizeof(int)];
	in.read(buffer,sizeof(int));
	memcpy(seguro.raw,buffer,sizeof(int));
	while(true){
		Indice i;
		string e(c.tipo);
		if(e.compare("Entero")==0){
			if(!in.read(buffer,sizeof(int)))
				break;
			memcpy(value.raw,buffer,sizeof(int));
			stringstream ss;
			ss << value.num;
			i.Key=ss.str();
		}else{
			char b[c.tamano-1];
			if(!in.read(b,c.tamano-1))
				break;
			b[c.tamano-1]='\0';
			string s(b);
			i.Key=s;
		}
		if(!in.read(buffer,sizeof(int)))
			break;
		memcpy(value.raw,buffer,sizeof(int));
		i.offset = value.num;
		indices.push_back(i);
	}
	in.close();
}

void deletekey(vector<Indice>&indices,int offset){
		int first = 1;
		int last = indices.size();
		while(first <= last){
			int midpoint = (int)((first+last)/2);
			if(indices[midpoint].offset == offset){
				indices[midpoint].Key = "";
				break;
			}else if(indices[midpoint].offset < offset){
				first = first + 1;
			}else
				last = last -1;			
		}	
}
int Menu(bool& flag,const char* nbin, vector<Campo>&registros, vector<Indice>&indices){
	if(!flag){
		cout << "1.Agregar Datos\n2.Listar Datos\n3.Buscar Registro\n4.Borrar\n5.Compactar\n6.Modificar\n7.Activar Indice Lineal" 
			<<"\n8.Salir"<< endl;
	}else{
		cout << "1.Agregar Datos\n2.Listar Datos\n3.Buscar Registro\n4.Borrar"
			<<"\n5.Compactar\n6.Modificar\n7.Desactivar Indice Lineal\n8.Salir"<< endl;
	}
	int opcion;
	cin >> opcion;
	if(opcion==1)
		Agregar(nbin,registros,indices,flag);
	if(opcion==2){
		if(flag)
			ILListar(nbin, indices, registros);
		else
			Listar(nbin, registros);
	}
	if(opcion==3)
		Buscar(nbin,registros);
	if(opcion==4)
		Borrar(nbin,registros,flag,indices);
	if(opcion==5)
		Compactar(nbin,registros);
	if(opcion==6){
		if(flag)
			ILModificar(nbin,registros,indices);
		else
			Modificar(nbin,registros);
	}
	if(opcion==7){
		if(flag)
			flag=false;
		else{
			Reindexar(nbin,indices,registros);
			flag = true;
		}
	}
	if(opcion==8)
		return 8;
	return 0;

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
	cbytes = cbytes + sizeof(Campo)*numcampos;
	out.write(reinterpret_cast<char*>(&cbytes), sizeof(int));
	out.write(reinterpret_cast<char*>(&availlist),sizeof(int));
	out.write(reinterpret_cast<char*>(&records),sizeof(long int));
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
void Agregar(const char* nbin, vector<Campo> registros, vector<Indice>& indices, bool flag){
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
		char c[sizeof(char)];
		out.read(c,sizeof(char));
		int x = (int)c[0];
		x-=48;
		out.seekp(4,ios::beg);
		out.write(reinterpret_cast<char*>(&x),sizeof(int));
		out.seekp(availist.num,ios::beg);
	}else{
		out.seekp(0,ios::end);
	}
	Indice ind;
	ind.offset = out.tellg();
	for(int i=0; i< registros.size();i++){
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
	if(flag){
		int posicion = updateKey(indices, ind.offset);
		if(posicion == -1)
			indices.push_back(ind);
		else
			indices[posicion].Key = ind.Key;
		BinaryInsertion(indices,registros[0]);
	}
	records.num +=1;
	out.seekp(8,ios::beg);
	out.write(reinterpret_cast<char*>(&records.num),sizeof(long int));
	out.close();

}//Agrega los registros
void ILListar(const char*nbin, vector<Indice>indices, vector<Campo>campos){
	ifstream in(nbin, ios::in|ios::binary);
	for(int i=0; i< indices.size(); i++){
		if(indices[i].Key != ""){
			cout <<i+1<<".";
			in.seekg(indices[i].offset,ios::beg);
			for(int j=0;j<campos.size(); j++){
				string tipo(campos[j].tipo);
				if(tipo.compare("Entero")==0){
					charint ci;
					char buffer[sizeof(int)];
			    	in.read(buffer,sizeof(int));
					memcpy(ci.raw,buffer,sizeof(int));
					cout << ci.num << ",";
				}else{
					char buffer[sizeof(campos[j].tamano-1)];
					in.read(buffer,campos[j].tamano-1);
					buffer[campos[j].tamano-1]='\0';
					cout << buffer <<",";
				}
			}
     		cout << endl;
		}
	}
}

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
	//cout << cbyte.num << endl;
	int pos=1;
	while(true){
		if(i==registros.size()){
			cout << endl;
			i=0;
			pos++;
		}
		string tipo(registros[i].tipo);
		if(tipo.compare("Entero")==0){
			char buffer[sizeof(int)];
			if(!in.read(buffer,sizeof(int)))
				break;
			if(buffer[0] == '*'){
				int size = -1*sizeof(int);
				for(int i=0;i < registros.size() ;i++){
					string str(registros[i].tipo);
					if(str.compare("Entero")==0)
						size += sizeof(int);
					else
						size = size + registros[i].tamano-1;
				}
				size = size + in.tellg();
				in.seekg(size);
				pos++;
			}else{
				if(i==0)
					cout << pos << ". ";
				cout << registros[i].nombre <<"-";
				memcpy(ci.raw,buffer,sizeof(int));
				if(i != registros.size()-1){
					cout << ci.num << ",";
				}else{
					cout << ci.num;
				}
				i++;	
			}
		}else{
			char buffer[registros[i].tamano-1];
			if(!in.read(buffer,registros[i].tamano-1))
				break;
			if(buffer[0] == '*'){
				int size = -1*(registros[i].tamano-1);
				for(int i=0;i < registros.size() ;i++){
					string str(registros[i].tipo);
					if(str.compare("Entero")==0)
						size += sizeof(int);
					else
						size = size + registros[i].tamano-1;
				}				
				size = size + in.tellg();
				in.seekg(size);
				pos++;

			}else{
				if(i==0)
					cout << pos << ". ";
				cout << registros[i].nombre << "-";
				buffer[registros[i].tamano-1]='\0';
				if(i != registros.size()-1){
					cout << buffer << ",";
				}else{
					cout << buffer;
				}
				i++;
			}
		}
	}	
	in.close();	

}//Lista los registros


void Borrar(const char* nbin, vector<Campo> registros, bool flag, vector<Indice>& indices){
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
	if (flag)
		deletekey(indices,os);
	char p = '*';
	in.write(reinterpret_cast<char*>(&p),sizeof(char));
	char av = (char)(availlist.num + 48);
	in.put(av);
	in.seekp(4);
	in.write(reinterpret_cast<char*>(&os),sizeof(int));
	in.seekp(8);
	in.write(reinterpret_cast<char*>(&records.num),sizeof(long int));//agregado
	in.close();
}//Borra al agregar un * en el primer byte del registro y un caracter representando el offset del siguiente espacio disponible
int offset(int rrn,vector<Campo> registros){
	int size=0;
	for(int i=0; i < registros.size() ; i++){
		string p(registros[i].tipo);
		if(p.compare("Entero")==0)
			size+=sizeof(int);
		else
			size = size + registros[i].tamano -1;
	}
	return rrn*size;
}
void ILModificar(const char*nbin,vector<Campo> registros,vector<Indice>& indices){
	ILListar(nbin,indices,registros);
	fstream out(nbin, ios::out|ios::binary|ios::in);
	int posicion;
	cout << "Ingrese el numero del registro" << endl;
	cin >> posicion;
	posicion--;
	out.seekp(indices[posicion].offset,ios::beg);
	for(int i=0;i<registros.size();i++){
		string tipo(registros[i].tipo);
		if(tipo.compare("Entero")==0){
			int value;
			cout << "Ingrese "<< registros[i].nombre << endl;
			cin >> value;
			if(i==0){
				stringstream ss;
				ss << value;
				indices[posicion].Key = ss.str();
			}
			out.write(reinterpret_cast<char*>(&value),sizeof(int));
		}else{
			char dato[registros[i].tamano];
			cout << "Ingrese " << registros[i].nombre << endl;
			cin >> dato;
			if(i==0){
				string s(dato);
				indices[posicion].Key = s;
			}
			out.write(dato,registros[i].tamano-1);
		}
	}
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
		string p(registros[i].tipo);
		if(p.compare("Entero")==0){
			char buffer[sizeof(int)];
			if(!in.read(buffer,sizeof(int)))
				break;
			if(buffer[0] == '*'){
				int size = -1*sizeof(int);
				for(int i=0;i < registros.size() ;i++){
					string g(registros[i].tipo);
					if(g.compare("Entero")==0)
						size += sizeof(int);
					else
						size = size + registros[i].tamano-1;
				}
				size = size + in.tellg();
				in.seekg(size);
			}else{
				memcpy(ci.raw,buffer,sizeof(int));
				stringstream s;
				s << ci.num;
				if(s.str().compare(data)==0){
					flag=true;
				}
				registro << s.str() << ",";
				i++;	
			}
		}else{
			char buffer[registros[i].tamano-1];
			if(!in.read(buffer,registros[i].tamano-1))
				break;
			if(buffer[0] == '*'){
				int size = -1*(registros[i].tamano-1);
				for(int i=0;i < registros.size() ;i++){
					string q(registros[i].tipo);
					if(q.compare("Entero")==0)
						size += sizeof(int);
					else
						size = size + registros[i].tamano-1;
				}				
				size = size + in.tellg();
				in.seekg(size);

			}else{
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

