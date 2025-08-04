#ifndef SIMULACAO_TRAFEGO_AEREO_HPP
#define SIMULACAO_TRAFEGO_AEREO_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// --- Mova a definição de No<T> para AQUI ---
template <typename T>
struct No {
    T dado;
    No* proximo;
    No(T dado) : dado(dado), proximo(nullptr) {}
};
// ------------------------------------------

struct Aeronave {
    int id;
    int combustivel;
    int tempo_espera;

    Aeronave() : id(-1), combustivel(-1), tempo_espera(-1) {}

    Aeronave(int id, int combustivel = -1) : id(id), combustivel(combustivel), tempo_espera(0) {}
};

template <typename T>
class ListaEncadeada {
private:
    No<T>* cabeca; // Agora o compilador sabe o que é No<T>
    No<T>* cauda;  // Agora o compilador sabe o que é No<T>
    int tamanho;

public:
    ListaEncadeada();
    ~ListaEncadeada();

    void adicionar(T dado);
    T remover_frente();
    T remover(int id);
    bool esta_vazia();
    int get_tamanho();
    No<T>* get_cabeca(); // Agora o compilador sabe o que é No<T>

    void decrementar_combustivel();
    Aeronave* encontrar_emergencia();
    Aeronave* encontrar_maior_espera();

    void imprimir_fila(const std::string& nome_fila, bool mostrar_combustivel = false);
};

// As implementações dos métodos da ListaEncadeada podem permanecer aqui
template <typename T>
ListaEncadeada<T>::ListaEncadeada() : cabeca(nullptr), cauda(nullptr), tamanho(0) {}

template <typename T>
ListaEncadeada<T>::~ListaEncadeada() {
    No<T>* atual = cabeca;
    while (atual != nullptr) {
        No<T>* proximo = atual->proximo;
        delete atual;
        atual = proximo;
    }
}

template <typename T>
void ListaEncadeada<T>::adicionar(T dado) {
    No<T>* novo_no = new No<T>(dado);
    if (esta_vazia()) {
        cabeca = cauda = novo_no;
    } else {
        cauda->proximo = novo_no;
        cauda = novo_no;
    }
    tamanho++;
}

template <typename T>
T ListaEncadeada<T>::remover_frente() {
    if (esta_vazia()) {
        throw std::out_of_range("A fila esta vazia.");
    }
    No<T>* temp = cabeca;
    T dado = temp->dado;
    cabeca = cabeca->proximo;
    if (cabeca == nullptr) {
        cauda = nullptr;
    }
    delete temp;
    tamanho--;
    return dado;
}

template <typename T>
T ListaEncadeada<T>::remover(int id) {
    if (esta_vazia()) {
        throw std::out_of_range("A fila esta vazia.");
    }
    No<T>* atual = cabeca;
    No<T>* anterior = nullptr;
    while (atual != nullptr && atual->dado.id != id) {
        anterior = atual;
        atual = atual->proximo;
    }
    if (atual == nullptr) {
        throw std::runtime_error("Aeronave nao encontrada na fila.");
    }
    T dado = atual->dado;
    if (anterior == nullptr) {
        cabeca = atual->proximo;
        if (cabeca == nullptr) {
            cauda = nullptr;
        }
    } else {
        anterior->proximo = atual->proximo;
        if (atual->proximo == nullptr) {
            cauda = anterior;
        }
    }
    delete atual;
    tamanho--;
    return dado;
}

template <typename T>
bool ListaEncadeada<T>::esta_vazia() {
    return cabeca == nullptr;
}

template <typename T>
int ListaEncadeada<T>::get_tamanho() {
    return tamanho;
}

template <typename T>
No<T>* ListaEncadeada<T>::get_cabeca() {
    return cabeca;
}

template <typename T>
void ListaEncadeada<T>::decrementar_combustivel() {
    No<T>* atual = cabeca;
    while (atual != nullptr) {
        atual->dado.combustivel--;
        atual = atual->proximo;
    }
}

template <typename T>
Aeronave* ListaEncadeada<T>::encontrar_emergencia() {
    No<T>* atual = cabeca;
    while (atual != nullptr) {
        if (atual->dado.combustivel <= 0) {
            return &atual->dado;
        }
        atual = atual->proximo;
    }
    return nullptr;
}

template <typename T>
Aeronave* ListaEncadeada<T>::encontrar_maior_espera() {
    if (esta_vazia()) {
        return nullptr;
    }
    Aeronave* maior_espera = &cabeca->dado;
    No<T>* atual = cabeca->proximo;
    while (atual != nullptr) {
        if (atual->dado.tempo_espera > maior_espera->tempo_espera) {
            maior_espera = &atual->dado;
        }
        atual = atual->proximo;
    }
    return maior_espera;
}

template <typename T>
void ListaEncadeada<T>::imprimir_fila(const std::string& nome_fila, bool mostrar_combustivel) {
    std::cout << "  Fila " << nome_fila << " (" << tamanho << " aeronaves): ";
    No<T>* atual = cabeca;
    while (atual != nullptr) {
        std::cout << "[ID: " << atual->dado.id;
        if (mostrar_combustivel) {
            std::cout << ", Comb: " << atual->dado.combustivel;
        }
        std::cout << "]";
        if (atual->proximo != nullptr) {
            std::cout << " -> ";
        }
        atual = atual->proximo;
    }
    std::cout << std::endl;
}

#endif