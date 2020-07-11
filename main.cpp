#include<iostream>
#include<fstream>
#include<stdlib.h>
using namespace std;

union dado {
    struct {
        int quant, first, last, free;
    } cabecalho;
    struct {
        int chave, next, prev;
    } registro;
};

void inicializar(fstream &arq, int &numreg) {
    dado d;
    arq.seekp (0,arq.beg);
    d.cabecalho.quant=0;
    d.cabecalho.first=-1;
    d.cabecalho.last=-1;
    d.cabecalho.free=1;
    arq.write((char*)&d, sizeof(d));
    for (int i = 1; i <= numreg; i++) {
        if(i==numreg)
            d.registro.next=-1;
        else
            d.registro.next=i+1;

        arq.write((char*)&d, sizeof(d));
    }
}

void imprimir(fstream &arq, int &numreg) {
    dado d;
    arq.seekg (0,arq.beg);
    arq.read((char*)&d, sizeof(d));
    cout << " Cabecalho" << endl;
    cout << " Quant: " << d.cabecalho.quant <<endl;
    cout << " First: " << d.cabecalho.first <<endl;
    cout << " Last: " << d.cabecalho.last <<endl;
    cout << " Free: " << d.cabecalho.free <<endl;
    cout << " Registros" << endl;
    for (int i = 1; i <= numreg; i++) {
        arq.read((char*)&d, sizeof(d));
        cout << " " << i << "- Chave: " << d.registro.chave << endl;
        cout << " " << i << "- Next: " << d.registro.next << endl;
        cout << " " << i << "- Prev: " << d.registro.prev << endl << endl;
    }
}

void imprimirSeq(fstream &arq) {
    dado cab,reg;
    arq.seekg (0,arq.beg);
    arq.read((char*)&cab, sizeof(cab));
    cout<<" Cabecalho" << endl;
    cout<<" Quant: "<<cab.cabecalho.quant<<endl;
    cout<<" First: "<<cab.cabecalho.first<<endl;
    cout<<" Last: "<<cab.cabecalho.last<<endl;
    cout<<" Free: "<<cab.cabecalho.free<<endl;
    cout<<" Registros " << endl;
    if(cab.cabecalho.first==-1) {
        cout<<" Nao existe registros";
        return;
    }
    arq.seekg(cab.cabecalho.first*sizeof(cab),arq.beg);
    do {
        arq.read((char*)&reg, sizeof(reg));
        cout<<" Chave: "<<reg.registro.chave<<endl;
        cout<<" Next: "<<reg.registro.next<<endl;
        cout<<" Prev: "<<reg.registro.prev<<endl<<endl;
        if(reg.registro.next!=-1)
            arq.seekg(reg.registro.next*sizeof(reg),arq.beg);

    }
    while(reg.registro.next!=-1);
}

void ImprimeLivres(fstream &arq, int &numreg) {
    dado cab;
    arq.seekg(0,arq.beg);
    arq.read((char*)&cab, sizeof(cab));
    if(cab.cabecalho.quant < numreg) {
        arq.seekg(sizeof(cab)*cab.cabecalho.free,arq.beg);
        arq.read((char*)&cab, sizeof(cab));
        cout << "Registro(s) livres" << endl;
        do {
            cout<<"- Chave: "<<cab.registro.chave<<endl;
            cout<<"- Next: "<<cab.registro.next<<endl;
            cout<<"- Prev: "<<cab.registro.prev<<endl<<endl;
            arq.seekg(sizeof(cab)*cab.registro.next,arq.beg);
            arq.read((char*)&cab, sizeof(cab));
            if(cab.registro.next == -1) {
                cout<<"- Chave: "<<cab.registro.chave<<endl;
                cout<<"- Next: "<<cab.registro.next<<endl;
                cout<<"- Prev: "<<cab.registro.prev<<endl<<endl;
            }
        } while(cab.registro.next != -1);
    } else {
        cout << " Desculpe, nao temos registros livres no momento" << endl;
    }
}

bool PesquisadoInsere(fstream &arq, int &j) {
    dado cab, aux;
    arq.seekg(0,arq.beg);
    arq.read((char*)&cab, sizeof(cab));
    if(cab.cabecalho.quant != 0) {
        arq.seekg(sizeof(aux)*cab.cabecalho.first,arq.beg);
        arq.read((char*)&aux, sizeof(aux));
        while(aux.registro.chave != j) {
            if(aux.registro.next != -1) {
                arq.seekg(sizeof(aux)*aux.registro.next,arq.beg);
                arq.read((char*)&aux, sizeof(aux));
            } else {
                break;
            }
        }
        if(aux.registro.chave != j) {
            return true;
        } else {
            return false;
        }

    } else {
        return true;
    }
}

void insere(fstream &arq, int &j, int &numreg) {
    dado cab, aux, aux2;
    arq.seekg(0,arq.beg);
    arq.read((char*)&cab, sizeof(cab));
    if(cab.cabecalho.quant < numreg && PesquisadoInsere(arq, j) == true) { // não deixa inserir quando tem 8, nem uma chave repetida
        if(cab.cabecalho.quant == 0) { // inserindo quando não tem nada
            arq.seekg(sizeof(aux)*cab.cabecalho.free,arq.beg);
            arq.read((char*)&aux, sizeof(aux));
            arq.seekp (sizeof(aux)*cab.cabecalho.free,arq.beg);
            aux.registro.chave = j;
            cab.cabecalho.first = cab.cabecalho.free;
            cab.cabecalho.last = cab.cabecalho.first;
            cab.cabecalho.free = aux.registro.next;
            aux.registro.next = -1;
            cab.cabecalho.quant++;
            arq.write((char*)&aux, sizeof(aux));
            arq.seekp (0,arq.beg);
            arq.write((char*)&cab, sizeof(cab));
        } else { // quando tem mais de 1
            arq.seekg(sizeof(aux)*cab.cabecalho.first,arq.beg);
            arq.read((char*)&aux, sizeof(aux));
            arq.seekg(sizeof(aux2)*cab.cabecalho.free,arq.beg);
            arq.read((char*)&aux2, sizeof(aux2));
            if(aux.registro.chave > j) { // inserindo no primeiro
                arq.seekp (sizeof(aux)*cab.cabecalho.first,arq.beg);
                aux.registro.prev = cab.cabecalho.free;
                arq.write((char*)&aux, sizeof(aux));
                cab.cabecalho.free = aux2.registro.next;
                arq.seekp (sizeof(aux2)*aux.registro.prev,arq.beg);
                aux2.registro.chave = j;
                aux2.registro.next = cab.cabecalho.first;
                arq.write((char*)&aux2, sizeof(aux2));
                arq.seekp (0,arq.beg);
                cab.cabecalho.first = aux.registro.prev;
                cab.cabecalho.quant++;
                arq.write((char*)&cab, sizeof(cab));
            } else {
                arq.seekg(sizeof(aux)*cab.cabecalho.last,arq.beg);
                arq.read((char*)&aux, sizeof(aux));
                if(aux.registro.chave < j) { // inserindo no fim
                    arq.seekp (sizeof(aux)*cab.cabecalho.last,arq.beg);
                    aux.registro.next = cab.cabecalho.free;
                    arq.write((char*)&aux, sizeof(aux));
                    cab.cabecalho.free = aux2.registro.next;
                    arq.seekp (sizeof(aux2)*aux.registro.next,arq.beg);
                    aux2.registro.chave = j;
                    aux2.registro.next = -1;
                    aux2.registro.prev = cab.cabecalho.last;
                    arq.write((char*)&aux2, sizeof(aux2));
                    cab.cabecalho.quant++;
                    cab.cabecalho.last = aux.registro.next;
                    arq.seekp (0,arq.beg);
                    arq.write((char*)&cab, sizeof(cab));
                } else { // inserindo no meio
                    int comp, posicao;
                    do {
                        posicao = aux.registro.prev;
                        comp = aux.registro.chave;
                        arq.seekg(sizeof(aux)*aux.registro.prev,arq.beg);
                        arq.read((char*)&aux, sizeof(aux));
                    } while(comp > j && aux.registro.chave > j);
                    comp = aux.registro.next;
                    aux.registro.next = cab.cabecalho.free;
                    arq.seekp (sizeof(aux)*posicao,arq.beg);
                    arq.write((char*)&aux, sizeof(aux));
                    arq.seekg(sizeof(aux)*comp,arq.beg);
                    arq.read((char*)&aux, sizeof(aux));
                    aux.registro.prev = cab.cabecalho.free;
                    arq.seekp (sizeof(aux)*comp,arq.beg);
                    arq.write((char*)&aux, sizeof(aux));
                    cab.cabecalho.free = aux2.registro.next;
                    aux2.registro.chave = j;
                    aux2.registro.next = comp;
                    aux2.registro.prev = posicao;
                    arq.seekp (sizeof(aux2)*aux.registro.prev,arq.beg);
                    arq.write((char*)&aux2, sizeof(aux2));
                    cab.cabecalho.quant++;
                    arq.seekp (0,arq.beg);
                    arq.write((char*)&cab, sizeof(cab));
                }
            }
        }
    } else {
        cout << " Desculpe, nao podemos inserir" << endl;
    }
}

void Pesquisa(fstream &arq, int &j) {
    dado cab, aux;
    arq.seekg(0,arq.beg);
    arq.read((char*)&cab, sizeof(cab));
    if(cab.cabecalho.quant != 0) {
        arq.seekg(sizeof(aux)*cab.cabecalho.first,arq.beg);
        arq.read((char*)&aux, sizeof(aux));
        while(aux.registro.chave != j) {
            if(aux.registro.next != -1) {
                arq.seekg(sizeof(aux)*aux.registro.next,arq.beg);
                arq.read((char*)&aux, sizeof(aux));
            } else {
                break;
            }
        }
        if(aux.registro.chave != j) {
            cout << "Não possui registros";
        } else {
            cout << "\nO registro foi encontrado\n";
            cout << "Seu prev: " << aux.registro.prev << endl;
            cout << "Seu next: " << aux.registro.next << endl;
        }
    } else {
        cout << "Não possui registros";
    }
}

int PesquisadoRemove(fstream &arq, int &j) {
    dado cab;
    arq.seekg(0,arq.beg);
    arq.read((char*)&cab, sizeof(cab));
    int quant = cab.cabecalho.quant;
    if(cab.cabecalho.quant != 0) {
        arq.seekg(sizeof(cab)*cab.cabecalho.first,arq.beg);
        arq.read((char*)&cab, sizeof(cab));
        int i = 1;
        if(quant == 1) {
            if(cab.registro.chave == j) {
                return i;
            } else {
                return 0;
            }
        } else {
            do {
                if(cab.registro.chave == j) {
                    return i;
                } else if(cab.registro.next == -1) {
                    return 0;
                } else {
                    arq.seekg(sizeof(cab)*cab.registro.next,arq.beg);
                    arq.read((char*)&cab, sizeof(cab));
                    i++;
                }
            } while(i<=quant);
        }
    } else {
        return 0;
    }
}

void Remocao(fstream &arq, int &j) {
    int quant = PesquisadoRemove(arq, j); // retorna o numero de vezes q ele passou por um registo, porém quant != posicao
    if(quant != 0) { // se retornar 0 sei que o registro com a chave não existe, logo impossivel remover
        dado cab,aux, aux2;
        int posicao,pos; // posicao para aux e pos para aux2
        arq.seekg(0,arq.beg);
        arq.read((char*)&cab, sizeof(cab));
        arq.seekg(sizeof(cab)*cab.cabecalho.first,arq.beg);
        arq.read((char*)&aux, sizeof(aux)); // colocando o aux no first
        posicao = cab.cabecalho.first;
        for(int i = 1; i <= quant; i++) { // começa em 1,pois caso quant=1 o registro a ser excluído é o first
            if(aux.registro.chave != j) {
                posicao = aux.registro.next; // posicao vai guardar o next do aux atual, pois caso o proximo seja o esperado temos a posicao q queremos excluir em posicao
                arq.seekg(sizeof(aux)*aux.registro.next,arq.beg);
                arq.read((char*)&aux, sizeof(aux));
            } else {
                if(cab.cabecalho.quant == 1) {
                    break;
                } else if (aux.registro.prev != -1) { // caso em que o aux não é o first
                    pos = aux.registro.prev;
                    arq.seekg(sizeof(aux)*pos,arq.beg);
                    arq.read((char*)&aux2, sizeof(aux2));
                } else { // caso o aux seja o first
                    pos = aux.registro.next;
                    arq.seekg(sizeof(aux2)*pos,arq.beg);
                    arq.read((char*)&aux2, sizeof(aux2));
                }
            }
        }
        if(cab.cabecalho.quant == 1) { // quando so tem um registro
            aux.registro.chave = 0;
            aux.registro.next = cab.cabecalho.free;
            arq.seekp (sizeof(aux)*posicao,arq.beg);
            arq.write((char*)&aux, sizeof(aux));
            cab.cabecalho.quant=0;
            cab.cabecalho.first=-1;
            cab.cabecalho.last=-1;
            cab.cabecalho.free=1;
            arq.seekp (0,arq.beg);
            arq.write((char*)&cab, sizeof(cab));
        } else if(posicao != cab.cabecalho.first) { // se posição não for a first
            if(quant == cab.cabecalho.quant) { // excluindo ultimo
                aux2.registro.next = -1;
                arq.seekp (sizeof(aux2)*pos,arq.beg);
                arq.write((char*)&aux2, sizeof(aux2));
                aux.registro.chave = 0;
                aux.registro.next = cab.cabecalho.free;
                aux.registro.prev = -1;
                arq.seekp (sizeof(aux)*posicao,arq.beg);
                arq.write((char*)&aux, sizeof(aux));
                cab.cabecalho.last = pos;
                cab.cabecalho.free = posicao;
                cab.cabecalho.quant--;
                arq.seekp (0,arq.beg);
                arq.write((char*)&cab, sizeof(cab));
            } else { // não é a ultima posicao
                aux2.registro.next = aux.registro.next;
                arq.seekp (sizeof(aux2)*pos,arq.beg);
                arq.write((char*)&aux2, sizeof(aux2));
                arq.seekg(sizeof(aux2)*aux.registro.next,arq.beg);
                arq.read((char*)&aux2, sizeof(aux2));
                aux2.registro.prev = pos;
                arq.seekp (sizeof(aux2)*aux.registro.next,arq.beg);
                arq.write((char*)&aux2, sizeof(aux2));
                aux.registro.chave = 0;
                aux.registro.next = cab.cabecalho.free;
                aux.registro.prev = -1;
                arq.seekp (sizeof(aux)*posicao,arq.beg);
                arq.write((char*)&aux, sizeof(aux));
                cab.cabecalho.quant--;
                cab.cabecalho.free = posicao;
                arq.seekp (0,arq.beg);
                arq.write((char*)&cab, sizeof(cab));
            }
        } else { // Excluir o primeiro
            aux2.registro.prev = -1;
            arq.seekp (sizeof(aux2)*pos,arq.beg);
            arq.write((char*)&aux2, sizeof(aux2));
            aux.registro.chave = 0;
            aux.registro.next = cab.cabecalho.free;
            arq.seekp (sizeof(aux)*posicao,arq.beg);
            arq.write((char*)&aux, sizeof(aux));
            cab.cabecalho.first = pos;
            cab.cabecalho.free = posicao;
            cab.cabecalho.quant--;
            arq.seekp (0,arq.beg);
            arq.write((char*)&cab, sizeof(cab));
        }
    } else {
        cout << " Desculpe, o registro nao existe!" << endl;
    }
}

void Tamanho(fstream &arq, int &numreg) {
    streampos begin,end;
    begin = arq.tellg();
    arq.seekg (0, ios::end);
    end = arq.tellg();
    numreg = ((end-begin)/sizeof(dado))-1;
}

void leNomeArquivo(char nomeArquivo[]) {
    cout << " Nome do arquivo: ";
    fflush(stdin);
    gets(nomeArquivo);
}

int menu1() {
    int escolha;
    cout << endl;
    cout << " ----------MENU----------" << endl;
    cout << " 1: Deseja ler um arquivo " << endl;
    cout << " 2: Deseja criar um arquivo " << endl;
    cout << " 0: Sair " << endl;
    cout << " Escolha: ";
    cin >> escolha;
    cout << endl;
    system("cls");
    return escolha;
}

int menu2() {
    int escolha;
    cout << endl;
    cout << " ----------MENU----------" << endl;
    cout << " 1: Inserir " << endl;
    cout << " 2: Remover " << endl;
    cout << " 3: Pesquisar " << endl;
    cout << " 4: Imprimir os registros inseridos " << endl;
    cout << " 5: Imprimir registros livres " << endl;
    cout << " 6: Imprimir estrutura " << endl;
    cout << " 0: Sair" << endl;
    cout << " Escolha: ";
    cin >> escolha;
    cout << endl;
    system("cls");
    return escolha;
}

int main() {
    fstream arq;
    int j, escolha, escolha1, numreg;
    char nomeArquivo[81];
    do {
        escolha1 = menu1();
        if(escolha1 != 0 && escolha1 != 1 && escolha1 != 2) {
            cout << " Nao existe essa opcao. Tente novamente " << endl;
        } else if(escolha1 == 1 || escolha1 == 2) {
            if(escolha1 == 1) {
                leNomeArquivo(nomeArquivo);
                arq.open(nomeArquivo,ios::binary| fstream::in | fstream::out);
                Tamanho(arq, numreg);
            } else {
                cout << " Vamos criar o arquivo! " << endl;
                cout << " Nos informe a quantidade de Registros que deseja: ";
                cin >> numreg;
                leNomeArquivo(nomeArquivo);
                arq.open(nomeArquivo,ios::binary| fstream::in | fstream::out | fstream::trunc);
                inicializar(arq, numreg);
            }
            if(arq.is_open()) {
                do {
                    escolha = menu2();
                    if(escolha == 1) {
                        cout<<" Inserindo novo registro: " << endl << " Digite a chave: ";
                        cin>>j;
                        insere(arq,j, numreg);
                    } else if(escolha == 2) {
                        cout << " Insira a chave para excluir: ";
                        cin >> j;
                        Remocao(arq, j);
                    } else if(escolha == 3) {
                        cout << " Insira um registro para pesquisa: ";
                        cin >> j;
                        Pesquisa(arq, j);
                    } else if(escolha == 4) {
                        imprimirSeq(arq);
                    } else if(escolha == 5) {
                        ImprimeLivres(arq, numreg);
                    } else if(escolha == 6) {
                        imprimir(arq, numreg);
                    }
                } while(escolha != 0);
            } else {
                cout << " Ocorreu um problema. Arquivo nao abriu. " << endl;
            }
        }
        arq.close();
    } while(escolha1 != 0);

    cout << " ----------OBRIGADO----------" << endl;

    return 0;

}
