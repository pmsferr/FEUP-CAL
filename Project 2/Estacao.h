#ifndef ESTACAO_H_
#define ESTACAO_H_
#include "Coordenadas.h"

class Estacao {
	int id;
	Coordenadas coord;
	string nome;
	vector<int> linked;
	int linha;
public:
	Estacao(){};
	Estacao(int i,string nome, Coordenadas coord, int linha){
		this->id = i;
		this->coord = coord;
		this->nome = nome;
		this->linha = linha;
	}
	int getId();
	const int getIdConst();
	Coordenadas getCoordenadas();
	string getNome();
	void push_back(int id){
		linked.push_back(id);
	}
	vector<int> getLinked(){
		return linked;
	}
	string getLinha(){
		char aaa[1];
			itoa(linha,aaa,10);
				aaa[0] += 16;

		return aaa;
	}
};

Coordenadas Estacao::getCoordenadas()
{
	return coord;
}

bool operator==(Estacao&a, const Estacao &rhs) {
	Estacao estacao = rhs;
	if(a.getId() == estacao.getId()) return true;
	else return false;
}

bool operator!=(Estacao&a, const Estacao &rhs) {
	Estacao estacao = rhs;
	if(a.getId() != estacao.getId()) return true;
	else return false;
}

int Estacao::getId(){
	return id;
}

const int Estacao::getIdConst(){
	return id;
}

string Estacao::getNome(){
	return nome;
}
#endif /* ESTACAO_H_ */
