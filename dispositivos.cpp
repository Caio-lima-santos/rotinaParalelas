#ifndef CAIO_DISPOSITIVOS
#define CAIO_DISPOSITIVOS

#ifndef CAIO_DISPOSITIVOS_DEBUG
#define  CAIO_DISPOSITIVOS_DEBUG
#include "server2.cpp"

std::vector<std::string>mensagensP;
std::vector<std::string>mensagensC;

inline void dbgPMODE(const char* x) {
#ifdef DEBUG_PARALELA
    mensagensP.push_back(std::string(x));
#endif
}

#define   debugParalela(x) dbgPMODE(x);
#define   debugConexao(x) mensagensC.push_back(std::string(x));
#endif // !CAIO_DISPOSITIVOS_DEBUG

#ifndef  APENAS_SERVER
#define  APENAS_SERVER

void rot_padrao(void(*f)(void* pdata, int* statu), void* p);


class Buffer :public std::vector<std::vector<char>> {


    size_t linhas = 0;

    Buffer() {}

    Buffer(const char* data) {
        this->resize(strlen(data));
        std::vector<char>temp;
        for (int i = 0; data[i] != NULL; i++)
            temp.push_back(data[i]);

        this->push_back(temp);
        linhas++;
    }

    void adicionar(const char* data) {
        std::vector<char>temp;
        for (int i = 0; data[i] != NULL; i++)
            temp.push_back(data[i]);

        this->push_back(temp);
        linhas++;
    }

};

class Paralela {

public:
    std::string nome;
    std::thread* rotina;
    int status;
    void* pData;
    std::vector<Paralela*>::iterator iterador;


    Paralela() {
        memset(this, 0, sizeof(*this));
    }
    Paralela(static void(*f)(void* pdata, int* statu), const char* n, void* pd) {
        if (pd == nullptr)return;
        this->pData = pd;
        nome = std::string(n);
        status = 0;
        rotina = new std::thread(rot_padrao, f, this);

    }

    ~Paralela() {
        status = -1000;
        if (rotina == nullptr) return;
        rotina->join();
        delete rotina;
        debugParalela("paralela finalizada");
    }

};

static void rot_padrao(void(*f)(void* pdata, int* statu), void* p) {
    Paralela* pP = (Paralela*)p;
    while (pP->status != -1000) {

        if (pP->status != 0) {
            debugParalela(std::string(pP->nome + std::to_string(pP->status)).c_str())
        }
        f(pP->pData, &pP->status);
    }
    std::this_thread::yield();
    return;
}

class Controlador_de_Threads {

public:
    std::vector<Paralela*>paralelas;
    std::vector<Paralela*>paralelas_paradas;
    int status = 0;
    bool limpando = false;


    Controlador_de_Threads() {
        memset(this, 0, sizeof(*this));
    }

    size_t adc(Paralela* para) {
        if (para != nullptr) {
            paralelas.push_back(para);
            para->iterador = paralelas.end();
            debugParalela(std::string("numero de paralelas" + std::to_string(paralelas.size())).c_str());
            return paralelas.size();
        }
        else {
            status = -1;
            return 12345678;
        }
    }

    bool excluir(size_t pos) {
        Paralela* p = paralelas[pos];
        if (p != nullptr) {

            if (limpando) return false;

            p->status = -1000;
            paralelas.erase(p->iterador);
            paralelas_paradas.push_back(p);
            p->iterador = paralelas_paradas.end();
            return true;
        }
        else {
            status = -1;
            return false;
        }
    }

    void limpar_paradas() {
        limpando = true;
        size_t tm = paralelas_paradas.size();
        for (size_t i = 0; i < tm; i++)
        {
            delete paralelas_paradas[i];
        }
        paralelas_paradas.clear();
        limpando = false;

    }

    ~Controlador_de_Threads() {
        size_t tm = paralelas.size();
        for (size_t i = 0; i < tm; i++)
        {
            this->excluir(i);
        }
        limpar_paradas();
    }

};

Controlador_de_Threads Con_Thr = Controlador_de_Threads();

#define   CTad(x) Con_Thr.adc(x);
#define   CTex(x) Con_Thr.excluir(x);
#define   CTlmp() Con_Thr.limpar_paradas();
#define   CTsts Con_Thr.status

/*
void rotina(void* f(),int tempo) {
    std::thread rotina(f);




   // std::this_thread::yield();

}*/
#endif // !





#ifndef APENAS_PARALELA
#define APENAS_PARALELA



 void rotinaServer(int * status,server *serve) {

     *status = 1;
    serve->conectar("dispositivo");
   *status = 0;
   std::this_thread::yield();

}

 void rotina(const char* executavel, int * status) {

     
     *status = 1;
     *status=system(executavel);
     *status = 0;
   
    std::this_thread::yield();
   
 }

 class conexão{


    public:
   
    server server{};
    std::thread threadDispositivo;
    std::thread threadServer;
    int statusDispositivo;
    int statusServer;
    bool carregado;

    conexão(const char * dispositivo,const char * executavel,const char * porta) {

        memset(this, NULL, sizeof(this));
        carregado = true;
        strcpy(server.porta,porta);
        debugServer(server.porta)
        threadServer = std::thread(rotinaServer, &statusServer, &server);
        threadDispositivo =  std::thread(rotina, executavel, &statusDispositivo);
       
     
    }
    ~conexão() {

        carregado = false;
    }

   void fechar() {
     
      
    }

    bool enviar(const char* data)
    {    

        server.filaEO.inserirE((char*)data);
     //ADICIONAR O THREAD PAUSE
        return true;
    }
    aiString receber_ultima_mensagem() {
        std::vector<aiString>* temF = &server.filaEO.filaDeRecebimento;

        return (*temF)[temF->size() - 1];
    }
   
   
};

class Impresora :public conexão
{
public:
    Impresora() : conexão("impresora", "node app.js", "27016")
    {

    }

    ~Impresora()
    {
       

    }


};

class banco_de_dados :public conexão
{
   
public:
    banco_de_dados() : conexão("banco de dados", "node db.js","2733")
    {


    }

 

};

class IA_Praec : public conexão{

public:
    IA_Praec() : conexão("PraecAi","node PraecAi.js","27017") {
    
    
    }

    ~IA_Praec() {
    
    
    }

};




#endif
#endif