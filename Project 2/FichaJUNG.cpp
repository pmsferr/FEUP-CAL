#include <cstdio>
#include "graphviewer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <conio.h>
#include <vector>
#include "Graph.h"
#include <algorithm>
#include "Estacao.h"
#include "DadosGraficos.h"

#define DEFAULT_COLOR "black"
#define NOT_DYNAMIC false
#define MAP_TO_METERS 32
#define AVERAGE_SPEED 1/200
#define AVERAGE_WAITING_TIME 20

Graph<Estacao> graph;
GraphViewer *gv;
vector<DadosGraficos> dg;
int edgeId = 0;
int tempId = -2;

void carregarFicheiro(string nome, string cor, int line, const int choice) {

	int lastId = -1;
	ifstream myReadFile;
	myReadFile.open(nome.c_str());
	string output;
	Estacao ultima = Estacao(-1, "", Coordenadas(), line);
	if (myReadFile.is_open()) {
		Estacao tempo;
		int x, y;
		while (!myReadFile.eof()) {
			getline(myReadFile, output);
			int id = atoi(output.c_str());
			getline(myReadFile, output);
			string nome = output;
			getline(myReadFile, output);
			x = 2 * atoi(output.c_str());
			getline(myReadFile, output);
			y = 2 * atoi(output.c_str());
			if (graph.addVertex(Estacao(id, nome, Coordenadas(x, y), line))) {
				gv->addNode(id, x, y);
				gv->setVertexLabel(id, nome);
				gv->setVertexColor(id, cor);
				tempo = Estacao(id, nome, Coordenadas(x, y), line);
			} else {
				gv->setVertexColor(id, "black");
				nome = graph.getVertexbyId(id).getNome();
				graph.addVertex(Estacao(tempId, nome, Coordenadas(x, y), line));
				tempo = graph.getVertexbyId(tempId);
				if (choice == 1) {
					graph.addEdge(graph.getVertexbyId(id), tempo,
							AVERAGE_WAITING_TIME, line);
					graph.addEdge(tempo, graph.getVertexbyId(id),
							AVERAGE_WAITING_TIME, line);
					for (int i = 0;
							i < graph.getVertexbyId(id).getLinked().size();
							i++) {
						int id_temp = graph.getVertexbyId(id).getLinked()[i];
						graph.addEdge(graph.getVertexbyId(id_temp), tempo,
								AVERAGE_WAITING_TIME, line);
						graph.addEdge(tempo, graph.getVertexbyId(id_temp),
								AVERAGE_WAITING_TIME, line);
					}
				} else {
					graph.addEdge(graph.getVertexbyId(id), tempo, 0, line);
					graph.addEdge(tempo, graph.getVertexbyId(id), 0, line);
					for (int i = 0;
							i < graph.getVertexbyId(id).getLinked().size();
							i++) {
						int id_temp = graph.getVertexbyId(id).getLinked()[i];
						graph.addEdge(graph.getVertexbyId(id_temp), tempo, 0,
								line);
						graph.addEdge(tempo, graph.getVertexbyId(id_temp), 0,
								line);
					}
				}
				graph.getVertexbyIdPointer(id)->push_back(tempId);
				tempId--;
			}

			if (lastId == -1) {
				lastId = id;
			} else {
				if (choice == 1) {
					float dist = sqrt(
							((abs(y - ultima.getCoordenadas().gety())) ^ 2)
									+ ((abs(x - ultima.getCoordenadas().getx()))
											^ 2));
					float travelTime = dist * MAP_TO_METERS * AVERAGE_SPEED;
					graph.addEdge(ultima, tempo, travelTime, line);
					graph.addEdge(tempo, ultima, travelTime, line);
					gv->addEdge(edgeId, lastId, id, EdgeType::UNDIRECTED);
					dg.push_back(DadosGraficos(edgeId, lastId, id));
					dg.push_back(DadosGraficos(edgeId, id, lastId));
					lastId = id;
					edgeId++;
				} else {
					float dist = sqrt(
							((abs(y - ultima.getCoordenadas().gety())) ^ 2)
									+ ((abs(x - ultima.getCoordenadas().getx()))
											^ 2));
					float travel = dist * MAP_TO_METERS;
					graph.addEdge(ultima, tempo, travel, line);
					graph.addEdge(tempo, ultima, dist * MAP_TO_METERS, line);
					gv->addEdge(edgeId, lastId, id, EdgeType::UNDIRECTED);
					dg.push_back(DadosGraficos(edgeId, lastId, id));
					dg.push_back(DadosGraficos(edgeId, id, lastId));
					lastId = id;
					edgeId++;
				}
			}

			ultima = tempo;
		}
	} else {
		cout << "File not opened\n";
	}
}

void carregarGraphos(int choice) {

	gv = new GraphViewer(1849, 2376, NOT_DYNAMIC);
	gv->setBackground("background.jpg");
	gv->createWindow(1849, 2376);
	gv->defineVertexColor(DEFAULT_COLOR);
	gv->defineEdgeColor(DEFAULT_COLOR);

	carregarFicheiro("linhaA.txt", "blue", 1, choice);
	carregarFicheiro("linhaB.txt", "red", 2, choice);
	carregarFicheiro("linhaC.txt", "green", 3, choice);
	carregarFicheiro("linhaD.txt", "yellow", 4, choice);
	carregarFicheiro("linhaE.txt", "orange", 5, choice);
	gv->rearrange();
}

int getEdgeId(int a, int b) {
	for (int i = 0; i < dg.size(); i++) {
		if (dg[i].id_origem == a && dg[i].id_destino == b) {
			return dg[i].id_aresta;
		}
	}
	return -1;
}

void interfaceCliente(int choice) {
	system("CLS");
	int origem;
	int destino;
	for (int i = 0; i < graph.getNumVertex(); i++) {
		stringstream ss;
		ss << graph.getVertexSet()[i]->getInfo().getNome() << " (linha "
				<< graph.getVertexSet()[i]->getInfo().getLinha() << ")" << endl;
		string str = ss.str();
		cout << endl << i + 1 << ". " << str;
	}
	cout << endl << "Numero da estacao de origem: ";
	cin >> origem;
	destino = origem;
	while (destino == origem) {
		cout << "Numero da estacao de destino: ";
		cin >> destino;
		if (destino == origem) {
			cout << "O destino nao pode ser igual a origem, escolhe outra vez."
					<< endl;
			return;
		}
		Estacao est_origem = graph.getVertexSet()[origem - 1]->getInfo();
		graph.dijkstraShortestPath(graph.getVertexSet()[origem - 1]->getInfo());
		vector<Estacao> caminho = graph.getPath(
				graph.getVertexSet()[origem - 1]->getInfo(),
				graph.getVertexSet()[destino - 1]->getInfo());
		system("CLS");
		Estacao last;
		cout << "O caminho mais curto: " << endl;
		for (int i = 0; i < caminho.size(); i++) {
			if (i > 0) {
				int id1 = graph.getFirstId(last.getNome());
				int id2 = graph.getFirstId(caminho[i].getNome());
				gv->setEdgeColor(getEdgeId(id1, id2), "green");
				gv->setEdgeThickness(getEdgeId(id1, id2), 8);
			}
			stringstream ss;
			ss << caminho[i].getNome() << " (linha " << caminho[i].getLinha()
					<< ")";
			string str = ss.str();
			cout << str << endl;
			last = caminho[i];
		}
		if (choice == 1)
			cout << "\n\nA distancia percorrida e de cerca de: "
					<< graph.getVertexSet()[destino - 1]->getDist()
					<< " minutos" << endl;
		else if (choice == 2)
			cout << "\n\nO tempo total da viagem e de cerca de: "
					<< graph.getVertexSet()[destino - 1]->getDist() << " metros"
					<< endl;
		gv->rearrange();
		getchar();
	}
}

void pre_kmp(string pattern, vector<int> & prefix) //funcoes implementadas nas aulas
		{
	int m = pattern.length();
	prefix[0] = -1;
	int k = -1;
	for (int q = 1; q < m; q++) {
		while (k > -1 && pattern[k + 1] != pattern[q])
			k = prefix[k];
		if (pattern[k + 1] == pattern[q])
			k = k + 1;
		prefix[q] = k;
	}
}

int kmp(string text, string pattern) //funcoes implementadas nas aulas, retorna o numero de ocorrencias
		{
	int num = 0;
	int m = pattern.length();
	vector<int> prefix(m);
	pre_kmp(pattern, prefix);

	int n = text.length();

	int q = -1;
	for (int i = 0; i < n; i++) {
		while (q > -1 && pattern[q + 1] != text[i])
			q = prefix[q];
		if (pattern[q + 1] == text[i])
			q++;
		if (q == m - 1) {
			num++;
			q = prefix[q];
		}
	}
	return num;
}

void searchName() {
	//int j = 0;
	cout
			<< "\n\n\tPesquisar por:\n\n\t\t1-Pesquisa Exata\n\t\t2-Pesquisa Aproxiamda\n";
	int aux;
	cin >> aux;

	if (aux == 2) {
		cout << "Conceptcao e Analise de Algoritmos - FEUP\n"
				"Ano 2015 - Tema 2 - Pesquisa \n\n"
				"\t\tSearch by Name\n\n\n"
				"Choose a name to search\n\n\n";

		string search;
		cin.ignore();
		getline(cin, search);
		vector<string> words;
		int last_space = 0;
		for (int i = 0; i < search.size(); i++) {
			if (search[i] == ' ' || i + 1 == search.size()) {
				if (last_space != i) {
					int size = i - last_space;
					if (i + 1 == search.size())
						size++;
					words.push_back(search.substr(last_space, size));
				}
				last_space = i + 1;
			}
		}

		cout << "Conceptcao e Analise de Algoritmos - FEUP\n"
				"Ano 2015 - Tema 2 - Pesquisa \n\n"
				"\t\tSearch by Name\n\n\n"
				"Results by order:\n\n\n";
		vector<string> res;
		for (int k = 0; k < graph.getVertexSet().size(); k++) {
			res.push_back(graph.getVertexSet()[k]->getInfo().getNome());
		}

		vector<int> num;
		for (unsigned int a = 0; a < res.size(); a++) {
			int value = 0;
			for (unsigned int i = 0; i < words.size(); i++) {

				//cout << "res[" << a << "]:" << res[a] << " - words[" << i << "]: " << words[i] << "\n";
				value += kmp(res[a], words[i]);
			}
			num.push_back(value);
		}
		int maxValue = -1;
		for (unsigned int j = 0; j < num.size(); j++)
			if (num[j] > maxValue)
				maxValue = num[j];

		vector<string> resprinted;
		for (; maxValue > 0; maxValue--)
			for (unsigned int order = 0; order < res.size(); order++)
				if (num[order] == maxValue) {
					if (resprinted.size() == 0) {
						resprinted.push_back(res[order]);

					} else {
						bool existe = false;
						for (int z = 0; z < resprinted.size(); z++) {
							if (res[order] == resprinted[z])
								existe = true;
						}
						if (!existe) {
							resprinted.push_back(res[order]);

						}
					}

				}
		reverse(resprinted.begin(),resprinted.end());
		for(unsigned int i =0 ; i < resprinted.size();i++)
		{
			cout << resprinted[i] << endl;
		}
		cout << "\n\n";

		graph.setNotVisited();
		system("pause");
	} else if (aux == 1) {
		cout << "Conceptcao e Analise de Algoritmos - FEUP\n"
						"Ano 2015 - Tema 2 - Pesquisa \n\n"
						"\t\tSearch by Exact Name\n\n\n"
						"Choose a name to search\n\n\n";
		string search;
		cin.ignore();
		getline(cin, search);
		vector<string> words;
		int last_space = 0;
		for (int i = 0; i < search.size(); i++) {
			if (search[i] == ' ' || i + 1 == search.size()) {
				if (last_space != i) {
					int size = i - last_space;
					if (i + 1 == search.size())
						size++;
					words.push_back(search.substr(last_space, size));
				}
				last_space = i + 1;
			}
		}
		vector<string> res;
				for (int k = 0; k < graph.getVertexSet().size(); k++) {
					res.push_back(graph.getVertexSet()[k]->getInfo().getNome());
				}

		bool existe = false;
				for(unsigned int z = 0; z < res.size();z++)
		{
			if(res[z] == words[0])
			{
				cout << "Encontrado com sucesso: " << res[z] << endl;

				existe = true;
				break;
			}

		}
				if(!existe)
					cout << "A estacao que procurou nao existe" << endl;

	}
}

int chooseTypeOfWeight() {
	int choice = -1;
	while (choice != 1 && choice != 2 && choice != 3) {
		cout
				<< "Qual o criterio para calcular percurso:\nEscolha o criterio pelo qual deseja que a rota seja calculada\n\n\n\t1.\tO tempo total de viagem\n\t2.\tA distancia da viagem\n\t3.\tPesquisa de nomes\n\n";
		cin >> choice;
	}
	return choice;
}

int main() {

	int choice = chooseTypeOfWeight();
	cout << "LOADING...\n";

	if (choice == 3) {
		carregarGraphos(1);
		searchName();
	}

	else {
		carregarGraphos(choice);
		interfaceCliente(choice);
		getchar();
	}

	return 0;
}
