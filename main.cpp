#include<iostream>
#include<fstream>
#include<stdlib.h>
using namespace std;

union dado { // definindo dado
    struct { // cabeçalho 
        int quant, first, last, free;
    } cabecalho;
    struct {
        int chave, next, prev;
    } registro; // registro
};

void inicializar(fstream &arq, int &numreg) { // inicialização. Recebe arquivo e numero de registros
    dado d; 
    arq.seekp (0,arq.beg); // posiciona ponteiro de escrita
    d.cabecalho.quant=0;
    d.cabecalho.first=-1;
    d.cabecalho.last=-1;
    d.cabecalho.free=1; 
    arq.write((char*)&d, sizeof(d)); // depois das modificações no cabeçalho ele escreve
    for (int i = 1; i <= numreg; i++) { // do primeiro registro até o último
        if(i==numreg)
            d.registro.next=-1; // next do ultimo é -1
        else
            d.registro.next=i+1; // se não for o último é i+1

        arq.write((char*)&d, sizeof(d)); // escreve a modifição no arquivo
    }
}

void imprimir(fstream &arq, int &numreg) { // imprimir toda a estrutura. Recebe arquivo e número de registros
    dado d;
    arq.seekg (0,arq.beg); // coloca o ponteiro de leitura no inicio
    arq.read((char*)&d, sizeof(d));
    cout << " Cabecalho" << endl;
    cout << " Quant: " << d.cabecalho.quant <<endl;
    cout << " First: " << d.cabecalho.first <<endl;
    cout << " Last: " << d.cabecalho.last <<endl;
    cout << " Free: " << d.cabecalho.free <<endl; // mostra todos os elementos do cabeçalho
    cout << " Registros" << endl;
    for (int i = 1; i <= numreg; i++) { // do primeiro registro até o ultimo
        arq.read((char*)&d, sizeof(d));
        cout << " " << i << "- Chave: " << d.registro.chave << endl;
        cout << " " << i << "- Next: " << d.registro.next << endl;
        cout << " " << i << "- Prev: " << d.registro.prev << endl << endl; // mostra todas as informações do registro
    }
}

void imprimirSeq(fstream &arq) { // imprime na sequencia. Recebe so arquivo
    dado cab,reg;
    arq.seekg (0,arq.beg);
    arq.read((char*)&cab, sizeof(cab)); // posiciona cab no inicio do arquivo
    cout<<" Cabecalho" << endl;
    cout<<" Quant: "<<cab.cabecalho.quant<<endl;
    cout<<" First: "<<cab.cabecalho.first<<endl;
    cout<<" Last: "<<cab.cabecalho.last<<endl;
    cout<<" Free: "<<cab.cabecalho.free<<endl; // imprime todas as infos do cabeçalho
    cout<<" Registros " << endl;
    if(cab.cabecalho.first==-1) { // condição caso não tenha registros
        cout<<" Nao existe registros";
        return;
    }
    arq.seekg(cab.cabecalho.first*sizeof(cab),arq.beg); // caso tenha reg vai para o first
    do {
        arq.read((char*)&reg, sizeof(reg));
        cout<<" Chave: "<<reg.registro.chave<<endl;
        cout<<" Next: "<<reg.registro.next<<endl;
        cout<<" Prev: "<<reg.registro.prev<<endl<<endl; // imprime infos do registro atual 
        if(reg.registro.next!=-1) // se não for o último registro
            arq.seekg(reg.registro.next*sizeof(reg),arq.beg);

    }
    while(reg.registro.next!=-1);
}

void ImprimeLivres(fstream &arq, int &numreg) { // imprime livres. Recebe arquivo e numero de registros do arquivo
    dado cab;
    arq.seekg(0,arq.beg);
    arq.read((char*)&cab, sizeof(cab)); // coloca cab no inicio do arquivo
    if(cab.cabecalho.quant < numreg) { // se for = a numreg não existe registros livres no momento
        arq.seekg(sizeof(cab)*cab.cabecalho.free,arq.beg);
        arq.read((char*)&cab, sizeof(cab)); // coloca cab no free
        cout << "Registro(s) livres" << endl;
        do {
            cout<<"- Chave: "<<cab.registro.chave<<endl;
            cout<<"- Next: "<<cab.registro.next<<endl;
            cout<<"- Prev: "<<cab.registro.prev<<endl<<endl; // imprime infos
            arq.seekg(sizeof(cab)*cab.registro.next,arq.beg);
            arq.read((char*)&cab, sizeof(cab)); // vai para o próximo do atual
            if(cab.registro.next == -1) { // se for o último
                cout<<"- Chave: "<<cab.registro.chave<<endl;
                cout<<"- Next: "<<cab.registro.next<<endl;
                cout<<"- Prev: "<<cab.registro.prev<<endl<<endl; // infos do último
            }
        } while(cab.registro.next != -1); // enquanto não é o último
    } else { // informa condição ao usuário
        cout << " Desculpe, nao temos registros livres no momento" << endl;
    }
}

bool PesquisadoInsere(fstream &arq, int &j) { // Pesquisa do Insere. Recebe arquivo e a chave 
    dado cab, aux;
    arq.seekg(0,arq.beg);
    arq.read((char*)&cab, sizeof(cab)); // coloca cab no inicio do arquivo
    if(cab.cabecalho.quant != 0) { // se a quantidade não for 0
        arq.seekg(sizeof(aux)*cab.cabecalho.first,arq.beg);
        arq.read((char*)&aux, sizeof(aux)); // aux vai para o first
        while(aux.registro.chave != j) { // enquanto a chave é diferente
            if(aux.registro.next != -1) { // se o next é diferente de -1
                arq.seekg(sizeof(aux)*aux.registro.next,arq.beg);
                arq.read((char*)&aux, sizeof(aux)); // próximo do atual
            } else {
                break; // se for igual quebra a repetição 
            }
        }
        if(aux.registro.chave != j) { // se não achou uma chave igual
            return true; 
        } else { // caso achou uma chave igual
            return false;
        }

    } else { // já que não tem nada não precisa preocupar com qual chave o usuário inseriu
        return true;
    }
}

void insere(fstream &arq, int &j, int &numreg) { // insere. Recebe arquivo, chave e número de registros
    dado cab, aux, aux2;
    arq.seekg(0,arq.beg);
    arq.read((char*)&cab, sizeof(cab)); // coloca cab no inicio do arquivo
    if(cab.cabecalho.quant < numreg && PesquisadoInsere(arq, j) == true) { // não deixa inserir quando tem número de registros, nem uma chave repetida
        if(cab.cabecalho.quant == 0) { // inserindo quando não tem nada
            arq.seekg(sizeof(aux)*cab.cabecalho.free,arq.beg);
            arq.read((char*)&aux, sizeof(aux)); // posiciona aux no free
            arq.seekp (sizeof(aux)*cab.cabecalho.free,arq.beg); // posiciona o ponteiro de escrita do aux
            aux.registro.chave = j; // muda chave de aux
            cab.cabecalho.first = cab.cabecalho.free;
            cab.cabecalho.last = cab.cabecalho.first;
            cab.cabecalho.free = aux.registro.next; // muda infos do cab
            aux.registro.next = -1; // muda next do aux
            cab.cabecalho.quant++; // muda quant do cab
            arq.write((char*)&aux, sizeof(aux)); // escreve infos aux
            arq.seekp (0,arq.beg);
            arq.write((char*)&cab, sizeof(cab)); // escreve infos cab
        } else { // quando tem mais de 1
            arq.seekg(sizeof(aux)*cab.cabecalho.first,arq.beg); // posiciona aux no first
            arq.read((char*)&aux, sizeof(aux));
            arq.seekg(sizeof(aux2)*cab.cabecalho.free,arq.beg); // posiciona aux2 no free
            arq.read((char*)&aux2, sizeof(aux2));
            if(aux.registro.chave > j) { // inserindo no primeiro
                arq.seekp (sizeof(aux)*cab.cabecalho.first,arq.beg); // posiciona o ponteiro de escrita do aux
                aux.registro.prev = cab.cabecalho.free; // muda info aux
                arq.write((char*)&aux, sizeof(aux)); // escreve info aux
                cab.cabecalho.free = aux2.registro.next; // muda info cab
                arq.seekp (sizeof(aux2)*aux.registro.prev,arq.beg); // posiciona ponteiro de escrita de aux2
                aux2.registro.chave = j;
                aux2.registro.next = cab.cabecalho.first; // muda infos de aux2
                arq.write((char*)&aux2, sizeof(aux2)); // escreve infos do aux2
                arq.seekp (0,arq.beg); // posiciona ponteiro de escrita no começo do cabeçalho
                cab.cabecalho.first = aux.registro.prev;
                cab.cabecalho.quant++; // muda infos do cab
                arq.write((char*)&cab, sizeof(cab)); // escreve
            } else { // se não for o primeiro
                arq.seekg(sizeof(aux)*cab.cabecalho.last,arq.beg); // coloca o aux no last
                arq.read((char*)&aux, sizeof(aux));
                if(aux.registro.chave < j) { // inserindo no fim
                    arq.seekp (sizeof(aux)*cab.cabecalho.last,arq.beg); // posiciona o ponteiro de escrita do aux
                    aux.registro.next = cab.cabecalho.free; // muda info aux
                    arq.write((char*)&aux, sizeof(aux)); // escreve infos 
                    cab.cabecalho.free = aux2.registro.next; // muda info cab
                    arq.seekp (sizeof(aux2)*aux.registro.next,arq.beg); // posiciona o ponteiro de escrita do aux2
                    aux2.registro.chave = j;
                    aux2.registro.next = -1;
                    aux2.registro.prev = cab.cabecalho.last; // muda infos do aux2
                    arq.write((char*)&aux2, sizeof(aux2)); // escreve infos de aux2
                    cab.cabecalho.quant++;
                    cab.cabecalho.last = aux.registro.next; // muda infos cab
                    arq.seekp (0,arq.beg); // posiciona o ponteiro de escrita do cab
                    arq.write((char*)&cab, sizeof(cab)); // escreve cab
                } else { // inserindo no meio
                    int comp, posicao; // declara variáveis necessárias
                    do {
                        posicao = aux.registro.prev;
                        comp = aux.registro.chave;
                        arq.seekg(sizeof(aux)*aux.registro.prev,arq.beg);
                        arq.read((char*)&aux, sizeof(aux));
                    } while(comp > j && aux.registro.chave > j); // percorre os registros 
                    comp = aux.registro.next; // iguala comp ao next do aux
                    aux.registro.next = cab.cabecalho.free; // muda info do aux
                    arq.seekp (sizeof(aux)*posicao,arq.beg); // posiciona o ponteiro de escrita do aux
                    arq.write((char*)&aux, sizeof(aux)); // escreve nova info
                    arq.seekg(sizeof(aux)*comp,arq.beg); // leva o aux para posição de número comp
                    arq.read((char*)&aux, sizeof(aux));
                    aux.registro.prev = cab.cabecalho.free; // muda info 
                    arq.seekp (sizeof(aux)*comp,arq.beg); // posiciona o ponteiro de escrita do aux
                    arq.write((char*)&aux, sizeof(aux)); // escreve info
                    cab.cabecalho.free = aux2.registro.next; // muda info cab
                    aux2.registro.chave = j;
                    aux2.registro.next = comp;
                    aux2.registro.prev = posicao; // muda infos de aux2
                    arq.seekp (sizeof(aux2)*aux.registro.prev,arq.beg); // posiciona o ponteiro de escrita do aux2
                    arq.write((char*)&aux2, sizeof(aux2)); // escreve infos aux2
                    cab.cabecalho.quant++; // muda infos cab
                    arq.seekp (0,arq.beg); // posiciona o ponteiro de escrita do cab
                    arq.write((char*)&cab, sizeof(cab)); // escreve cab
                }
            }
        }
    } else { // se as condições não forem atendidas a seguinte mensagem aparece
        cout << " Desculpe, nao podemos inserir" << endl;
    }
}

void Pesquisa(fstream &arq, int &j) { // pesquisa. Recebe o arquivo e a chave
    dado cab, aux;
    arq.seekg(0,arq.beg); 
    arq.read((char*)&cab, sizeof(cab)); // posiciona a leitura de cab no inicio 
    if(cab.cabecalho.quant != 0) { // se quantidade não for 0
        arq.seekg(sizeof(aux)*cab.cabecalho.first,arq.beg); // aux vai para o first
        arq.read((char*)&aux, sizeof(aux));
        while(aux.registro.chave != j) { // enquanto não for a chave esperada
            if(aux.registro.next != -1) { // se o next não for -1
                arq.seekg(sizeof(aux)*aux.registro.next,arq.beg);
                arq.read((char*)&aux, sizeof(aux)); // vai para o next do aux
            } else {
                break; // se for -1 quebra a repetição
            }
        }
        if(aux.registro.chave != j) { // se a chave de aux for diferente 
            cout << "Não possui registros";
        } else { // se for a chave esperada
            cout << "\nO registro foi encontrado\n";
            cout << "Seu prev: " << aux.registro.prev << endl;
            cout << "Seu next: " << aux.registro.next << endl;
        }
    } else { // caso não tenha registros
        cout << "Não possui registros";
    }
}

int PesquisadoRemove(fstream &arq, int &j) { // pesquisa do remove. Recebe arquivo e chave 
    dado cab;
    arq.seekg(0,arq.beg);
    arq.read((char*)&cab, sizeof(cab)); // posiciona cab no inicio do arquivo
    int quant = cab.cabecalho.quant; // declara quant = quantidade de registros
    if(cab.cabecalho.quant != 0) { // se quantidade diferente de 0
        arq.seekg(sizeof(cab)*cab.cabecalho.first,arq.beg);
        arq.read((char*)&cab, sizeof(cab)); // cabeçalho vai pro first
        int i = 1; // declara i
        if(quant == 1) { // se quantidade for 1
            if(cab.registro.chave == j) { // se chave igual a j
                return i; // retorna i
            } else {
                return 0; // se não retorna 0
            }
        } else { // tiver mais que 1
            do {
                if(cab.registro.chave == j) { // se a chave for igual a j
                    return i; // retorna i
                } else if(cab.registro.next == -1) { // se o next for -1
                    return 0; // retorna 0
                } else {
                    arq.seekg(sizeof(cab)*cab.registro.next,arq.beg);
                    arq.read((char*)&cab, sizeof(cab)); // cab vai para o next
                    i++; // +1 em i
                }
            } while(i<=quant); // até i menor ou igual a quant
        }
    } else {
        return 0; // se quantidade for 0 retorna 0
    }
}

void Remocao(fstream &arq, int &j) { // remoção. Recebe arquivo e chave
    int quant = PesquisadoRemove(arq, j); // retorna o numero de vezes q ele passou por um registo, porém quant != posicao
    if(quant != 0) { // se retornar 0 sei que o registro com a chave não existe, logo impossivel remover
        dado cab,aux, aux2;
        int posicao,pos; // posicao para aux e pos para aux2
        arq.seekg(0,arq.beg);
        arq.read((char*)&cab, sizeof(cab)); // colocando cab no inicio do arquivo
        arq.seekg(sizeof(cab)*cab.cabecalho.first,arq.beg);
        arq.read((char*)&aux, sizeof(aux)); // colocando o aux no first
        posicao = cab.cabecalho.first; // posicao é igualada ao first
        for(int i = 1; i <= quant; i++) { // começa em 1,pois caso quant=1 o registro a ser excluído é o first
            if(aux.registro.chave != j) {
                posicao = aux.registro.next; // posicao vai guardar o next do aux atual, pois caso o proximo seja o esperado temos a posicao q queremos excluir em posicao
                arq.seekg(sizeof(aux)*aux.registro.next,arq.beg);
                arq.read((char*)&aux, sizeof(aux)); // passa o aux para o next dele 
            } else { // senão for diferente
                if(cab.cabecalho.quant == 1) { // igual a 1 quebra
                    break;
                } else if (aux.registro.prev != -1) { // caso em que o aux não é o first
                    pos = aux.registro.prev;
                    arq.seekg(sizeof(aux)*pos,arq.beg);
                    arq.read((char*)&aux2, sizeof(aux2)); // aux2 vai para pos
                } else { // caso o aux seja o first
                    pos = aux.registro.next;
                    arq.seekg(sizeof(aux2)*pos,arq.beg); // aux2 vai para pos
                    arq.read((char*)&aux2, sizeof(aux2));
                }
            }
        }
        if(cab.cabecalho.quant == 1) { // quando so tem um registro
            aux.registro.chave = 0;
            aux.registro.next = cab.cabecalho.free; // muda infos aux
            arq.seekp (sizeof(aux)*posicao,arq.beg); // posiciona o ponteiro de escrita do aux
            arq.write((char*)&aux, sizeof(aux)); // escreve infos 
            cab.cabecalho.quant=0;
            cab.cabecalho.first=-1;
            cab.cabecalho.last=-1;
            cab.cabecalho.free=1; // muda infos cab
            arq.seekp (0,arq.beg); // posiciona o ponteiro de escrita do cab
            arq.write((char*)&cab, sizeof(cab)); // escreve cab
        } else if(posicao != cab.cabecalho.first) { // se posição não for a first
            if(quant == cab.cabecalho.quant) { // excluindo ultimo
                aux2.registro.next = -1; // muda next do aux2
                arq.seekp (sizeof(aux2)*pos,arq.beg); // posiciona o ponteiro de escrita do aux2
                arq.write((char*)&aux2, sizeof(aux2)); // escreve info aux2
                aux.registro.chave = 0;
                aux.registro.next = cab.cabecalho.free;
                aux.registro.prev = -1; // muda infos de aux
                arq.seekp (sizeof(aux)*posicao,arq.beg); // posiciona o ponteiro de escrita do aux
                arq.write((char*)&aux, sizeof(aux)); // escreve aux
                cab.cabecalho.last = pos;
                cab.cabecalho.free = posicao;
                cab.cabecalho.quant--; // muda infos cab
                arq.seekp (0,arq.beg); // posiciona o ponteiro de escrita do cab
                arq.write((char*)&cab, sizeof(cab)); // escreve cab
            } else { // não é a ultima posicao
                aux2.registro.next = aux.registro.next; // muda aux2
                arq.seekp (sizeof(aux2)*pos,arq.beg); // posiciona o ponteiro de escrita do aux2
                arq.write((char*)&aux2, sizeof(aux2)); // escreve aux2
                arq.seekg(sizeof(aux2)*aux.registro.next,arq.beg); // muda aux2 para o next do aux
                arq.read((char*)&aux2, sizeof(aux2));
                aux2.registro.prev = pos; // muda a info de aux2
                arq.seekp (sizeof(aux2)*aux.registro.next,arq.beg); // posiciona o ponteiro de escrita do aux2
                arq.write((char*)&aux2, sizeof(aux2)); // escreve infos
                aux.registro.chave = 0;
                aux.registro.next = cab.cabecalho.free;
                aux.registro.prev = -1; // muda infos de aux
                arq.seekp (sizeof(aux)*posicao,arq.beg); // posiciona o ponteiro de escrita do aux
                arq.write((char*)&aux, sizeof(aux)); // escreve infos 
                cab.cabecalho.quant--;
                cab.cabecalho.free = posicao;// muda infos de cab
                arq.seekp (0,arq.beg); // posiciona o ponteiro de escrita do cab
                arq.write((char*)&cab, sizeof(cab)); // muda infos cab
            }
        } else { // Excluir o primeiro
            aux2.registro.prev = -1; // muda o prev de aux2
            arq.seekp (sizeof(aux2)*pos,arq.beg); // posiciona o ponteiro de escrita do aux2
            arq.write((char*)&aux2, sizeof(aux2)); // escreve aux2 
            aux.registro.chave = 0;
            aux.registro.next = cab.cabecalho.free; // muda infos aux
            arq.seekp (sizeof(aux)*posicao,arq.beg); // posiciona o ponteiro de escrita do aux
            arq.write((char*)&aux, sizeof(aux)); // escreve infos aux
            cab.cabecalho.first = pos;
            cab.cabecalho.free = posicao;
            cab.cabecalho.quant--; // muda infos cab
            arq.seekp (0,arq.beg); // posiciona o ponteiro de escrita do cab
            arq.write((char*)&cab, sizeof(cab)); // escreve infos de cab
        }
    } else { // caso o registro não exista
        cout << " Desculpe, o registro nao existe!" << endl;
    }
}

void Tamanho(fstream &arq, int &numreg) { // calcula o número de registros presentes no arquivo
    streampos begin,end;
    begin = arq.tellg();
    arq.seekg (0, ios::end);
    end = arq.tellg();
    numreg = ((end-begin)/sizeof(dado))-1; // pega o tamanho do arquivo. Divide pelo tamanho de dado. Tira 1 porque o cabeçalho não conta
}

void leNomeArquivo(char nomeArquivo[]) { // lê o nome do arquivo
    cout << " Nome do arquivo: ";
    fflush(stdin);
    gets(nomeArquivo); // recebe nome arquivo
}

int menu1() { // menu 1 
    int escolha;
    cout << endl;
    cout << " ----------MENU----------" << endl;
    cout << " 1: Deseja ler um arquivo " << endl;
    cout << " 2: Deseja criar um arquivo " << endl;
    cout << " 0: Sair " << endl;
    cout << " Escolha: ";
    cin >> escolha;
    cout << endl;
    system("cls"); // limpa cmd
    return escolha; // retorna escolha
}

int menu2() { // menu 2
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
    system("cls"); // limpa cmd
    return escolha; // retorna escolha
}

int main() {
    fstream arq; // declaração do arquivo
    int j, escolha, escolha1, numreg; // variáveis necessárias
    char nomeArquivo[81]; // nome arquivo
    do {
        escolha1 = menu1(); // retorno do menu1 é escolha1
        if(escolha1 != 0 && escolha1 != 1 && escolha1 != 2) { // se não for 0,1,2 ele chama novamente o menu 
            cout << " Nao existe essa opcao. Tente novamente " << endl; // informa que não existe a opção 
        } else if(escolha1 == 1 || escolha1 == 2) {
            if(escolha1 == 1) { // se a escolha é ler um arquivo 
                leNomeArquivo(nomeArquivo); // pego o nome na função
                arq.open(nomeArquivo,ios::binary| fstream::in | fstream::out); // abro ele 
                Tamanho(arq, numreg); // e calculo o tamanho
            } else { // se quer criar um arquivo
                cout << " Vamos criar o arquivo! " << endl;
                cout << " Nos informe a quantidade de Registros que deseja: ";
                cin >> numreg; // recebe o número de registros diretamente do usuário
                leNomeArquivo(nomeArquivo); // le o nome do arquivo
                arq.open(nomeArquivo,ios::binary| fstream::in | fstream::out | fstream::trunc); // cria ele 
                inicializar(arq, numreg); // inicializa as variáveis 
            }
            if(arq.is_open()) { // se estive aberto
                do {
                    escolha = menu2(); // chama menu2
                    if(escolha == 1) { // caso queira inserir
                        cout<<" Inserindo novo registro: " << endl << " Digite a chave: ";
                        cin>>j;
                        insere(arq,j, numreg); 
                    } else if(escolha == 2) { // caso queira remover
                        cout << " Insira a chave para excluir: ";
                        cin >> j;
                        Remocao(arq, j);
                    } else if(escolha == 3) { // caso queira pesquisar 
                        cout << " Insira um registro para pesquisa: ";
                        cin >> j;
                        Pesquisa(arq, j);
                    } else if(escolha == 4) { // caso queira imprimir a sequencia dos registros
                        imprimirSeq(arq);
                    } else if(escolha == 5) { // caso queira imprimir livres
                        ImprimeLivres(arq, numreg);
                    } else if(escolha == 6) { // caso queira imprimir toda a estrutura
                        imprimir(arq, numreg);
                    }
                } while(escolha != 0); // enquanto o usuário não queira sair
            } else { // se o arquivo não existe ou ele não abriu
                cout << " Ocorreu um problema. Arquivo nao abriu. " << endl;
            }
        }
        arq.close(); // fecho arquivo
    } while(escolha1 != 0); // enquanto o usuário não queira sair

    cout << " ----------OBRIGADO----------" << endl; // mensagem ao usuário

    return 0;

}
