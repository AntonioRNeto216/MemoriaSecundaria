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

void inicializar(fstream &arq) {
    dado d;
    arq.seekp (0,arq.beg);
    d.cabecalho.quant=0;
    d.cabecalho.first=-1;
    d.cabecalho.last=-1;
    d.cabecalho.free=1;
    arq.write((char*)&d, sizeof(d));
    for (int i = 1; i <= 8; i++) {
        if(i==8)
            d.registro.next=-1;
        else
            d.registro.next=i+1;

        arq.write((char*)&d, sizeof(d));
    }
}

void imprimir(fstream &arq) {
    dado d;
    arq.seekg (0,arq.beg);
    arq.read((char*)&d, sizeof(d));
    cout << " Cabecalho" << endl;
    cout << " Quant: " << d.cabecalho.quant <<endl;
    cout << " First: " << d.cabecalho.first <<endl;
    cout << " Last: " << d.cabecalho.last <<endl;
    cout << " Free: " << d.cabecalho.free <<endl;
    cout << " Registros" << endl;
    for (int i = 1; i <= 8; i++) {
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

void ImprimeLivres(fstream &arq) {
    dado cab;
    arq.seekg(0,arq.beg);
    arq.read((char*)&cab, sizeof(cab));
    if(cab.cabecalho.quant < 8) {
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

// OK
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

//OK
void insere(fstream &arq, int &j) { 
    dado cab, aux, aux2; 
    arq.seekg(0,arq.beg);
    arq.read((char*)&cab, sizeof(cab));
    if(cab.cabecalho.quant < 8 && PesquisadoInsere(arq, j) == true) { // não deixa inserir quando tem 8, nem uma chave repetida
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
                } else { // ta no meio 
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

// OK
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
        if(aux.registro.chave == j) {
            cout << " O registro foi encontrado!!" << endl;
            cout << " Sua chave: " << cab.registro.chave << endl;
            cout << " Seu prev: " << cab.registro.prev << endl;
            cout << " Seu next: " << cab.registro.next << endl;
        } else {
            cout << " Desculpe, o registro não foi encontrado" << endl;
        }
    } else {
        cout << " Nao possui registros" << endl;
    }
}

// OK
int PesquisadoRemove(fstream &arq, int &j) { 
    dado cab;
    arq.seekg(0,arq.beg);
    arq.read((char*)&cab, sizeof(cab));
    int quant = cab.cabecalho.quant;
    if(cab.cabecalho.quant != 0) {
        arq.seekg(sizeof(cab)*cab.cabecalho.first,arq.beg);
        arq.read((char*)&cab, sizeof(cab));
        int i = 1;
        do {
            if(cab.registro.chave == j) {
                return i;
            } else {
                arq.seekg(sizeof(cab)*cab.registro.next,arq.beg);
                arq.read((char*)&cab, sizeof(cab));
                i++;
            }
        } while(i<=quant);
    } else {
        return 0;
    }
    return 0;
}

// OK
void Remocao(fstream &arq, int j) { 
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

int menu() {
    int escolha;
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
    //system("cls");
    return escolha;
}

int main() {
    fstream arq;
    int j, escolha;

    arq.open("pagina.dat",ios::binary| fstream::in | fstream::out );
    if(!arq.is_open()) {
       arq.open("pagina.dat",ios::binary| fstream::in | fstream::out|fstream::trunc );
       if(!arq.is_open()) {
            cout<<"Erro ao abrir o arquivo!!";
            return 0;
        }
    }

    inicializar(arq);

    do {
        escolha = menu();
        if(escolha == 1) { 
            cout<<"Inserindo novo registro:\nDigite a chave: ";
            cin>>j;
            insere(arq,j);
        } else if(escolha == 2) {
            cout << "\n Insira a chave para excluir: ";
            cin >> j;
            Remocao(arq, j);
        } else if(escolha == 3) {
            cout << "\n Insira um registro para pesquisa: ";
            cin >> j;
            Pesquisa(arq, j);
        } else if(escolha == 4) {
            imprimirSeq(arq);
        } else if(escolha == 5) {
            ImprimeLivres(arq);
        } else if(escolha == 6) {
            imprimir(arq);
        }
    } while(escolha != 0);
    //system("cls");
    cout << " ----------OBRIGADO----------" << endl;
    arq.close();
    return 0;
}


