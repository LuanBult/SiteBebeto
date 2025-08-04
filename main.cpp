#include "simulacao_trafego_aereo.hpp"
#include <random>
#include <chrono>
#include <thread>
#include <iostream>


int proximo_id_impar = 1;
int proximo_id_par = 2;
int aterrissagens_emergencia = 0;
long long tempo_total_espera_aterrissagem = 0;
int count_aterrissagens = 0;
long long tempo_total_espera_decolagem = 0;
int count_decolagens = 0;

ListaEncadeada<Aeronave> prateleira1_pista1;
ListaEncadeada<Aeronave> prateleira2_pista1;
ListaEncadeada<Aeronave> prateleira1_pista2;
ListaEncadeada<Aeronave> prateleira2_pista2;

ListaEncadeada<Aeronave> fila_decolagem_pista1;
ListaEncadeada<Aeronave> fila_decolagem_pista2;
ListaEncadeada<Aeronave> fila_decolagem_pista3;

void adicionar_aeronave_aterrissagem(int combustivel) {
    Aeronave nova_aeronave(proximo_id_impar, combustivel);
    proximo_id_impar += 2;
    int tamanho_p1_p1 = prateleira1_pista1.get_tamanho();
    int tamanho_p2_p1 = prateleira2_pista1.get_tamanho();
    int tamanho_p1_p2 = prateleira1_pista2.get_tamanho();
    int tamanho_p2_p2 = prateleira2_pista2.get_tamanho();

    if (tamanho_p1_p1 <= tamanho_p2_p1 && tamanho_p1_p1 <= tamanho_p1_p2 && tamanho_p1_p1 <= tamanho_p2_p2) {
        prateleira1_pista1.adicionar(nova_aeronave);
    } else if (tamanho_p2_p1 <= tamanho_p1_p1 && tamanho_p2_p1 <= tamanho_p1_p2 && tamanho_p2_p1 <= tamanho_p2_p2) {
        prateleira2_pista1.adicionar(nova_aeronave);
    } else if (tamanho_p1_p2 <= tamanho_p1_p1 && tamanho_p1_p2 <= tamanho_p2_p1 && tamanho_p1_p2 <= tamanho_p2_p2) {
        prateleira1_pista2.adicionar(nova_aeronave);
    } else {
        prateleira2_pista2.adicionar(nova_aeronave);
    }
}

void adicionar_aeronave_decolagem() {
    Aeronave nova_aeronave(proximo_id_par);
    proximo_id_par += 2;
    int tamanho_p1 = fila_decolagem_pista1.get_tamanho();
    int tamanho_p2 = fila_decolagem_pista2.get_tamanho();
    int tamanho_p3 = fila_decolagem_pista3.get_tamanho();

    if (tamanho_p1 <= tamanho_p2 && tamanho_p1 <= tamanho_p3) {
        fila_decolagem_pista1.adicionar(nova_aeronave);
    } else if (tamanho_p2 <= tamanho_p1 && tamanho_p2 <= tamanho_p3) {
        fila_decolagem_pista2.adicionar(nova_aeronave);
    } else {
        fila_decolagem_pista3.adicionar(nova_aeronave);
    }
}

void processar_aterrissagem(Aeronave& aeronave, int pista) {
    std::cout << "  > Aeronave ID " << aeronave.id << " pousou na Pista " << pista << ". (Tempo de espera: " << aeronave.tempo_espera << "s)" << std::endl;
    tempo_total_espera_aterrissagem += aeronave.tempo_espera;
    count_aterrissagens++;
}

void processar_decolagem(Aeronave& aeronave, int pista) {
    std::cout << "  > Aeronave ID " << aeronave.id << " decolou na Pista " << pista << ". (Tempo de espera: " << aeronave.tempo_espera << "s)" << std::endl;
    tempo_total_espera_decolagem += aeronave.tempo_espera;
    count_decolagens++;
}

Aeronave* encontrar_aeronave_emergencia() {
    Aeronave* emerg;
    if ((emerg = prateleira1_pista1.encontrar_emergencia()) != nullptr) return emerg;
    if ((emerg = prateleira2_pista1.encontrar_emergencia()) != nullptr) return emerg;
    if ((emerg = prateleira1_pista2.encontrar_emergencia()) != nullptr) return emerg;
    if ((emerg = prateleira2_pista2.encontrar_emergencia()) != nullptr) return emerg;
    return nullptr;
}

Aeronave* encontrar_aterrissagem_regular() {
    Aeronave* maior_espera_p1_p1 = prateleira1_pista1.encontrar_maior_espera();
    Aeronave* maior_espera_p2_p1 = prateleira2_pista1.encontrar_maior_espera();
    Aeronave* maior_espera_p1_p2 = prateleira1_pista2.encontrar_maior_espera();
    Aeronave* maior_espera_p2_p2 = prateleira2_pista2.encontrar_maior_espera();

    Aeronave* aeronave_escolhida = nullptr;

    if (maior_espera_p1_p1 != nullptr) {
        aeronave_escolhida = maior_espera_p1_p1;
    }
    if (maior_espera_p2_p1 != nullptr && (aeronave_escolhida == nullptr || maior_espera_p2_p1->tempo_espera > aeronave_escolhida->tempo_espera)) {
        aeronave_escolhida = maior_espera_p2_p1;
    }
    if (maior_espera_p1_p2 != nullptr && (aeronave_escolhida == nullptr || maior_espera_p1_p2->tempo_espera > aeronave_escolhida->tempo_espera)) {
        aeronave_escolhida = maior_espera_p1_p2;
    }
    if (maior_espera_p2_p2 != nullptr && (aeronave_escolhida == nullptr || maior_espera_p2_p2->tempo_espera > aeronave_escolhida->tempo_espera)) {
        aeronave_escolhida = maior_espera_p2_p2;
    }

    return aeronave_escolhida;
}

Aeronave* encontrar_decolagem_regular() {
    Aeronave* maior_espera_p1 = fila_decolagem_pista1.encontrar_maior_espera();
    Aeronave* maior_espera_p2 = fila_decolagem_pista2.encontrar_maior_espera();
    Aeronave* maior_espera_p3 = fila_decolagem_pista3.encontrar_maior_espera();

    Aeronave* aeronave_escolhida = nullptr;

    if (maior_espera_p1 != nullptr) {
        aeronave_escolhida = maior_espera_p1;
    }
    if (maior_espera_p2 != nullptr && (aeronave_escolhida == nullptr || maior_espera_p2->tempo_espera > aeronave_escolhida->tempo_espera)) {
        aeronave_escolhida = maior_espera_p2;
    }
    if (maior_espera_p3 != nullptr && (aeronave_escolhida == nullptr || maior_espera_p3->tempo_espera > aeronave_escolhida->tempo_espera)) {
        aeronave_escolhida = maior_espera_p3;
    }

    return aeronave_escolhida;
}

int main() {
	using namespace std::this_thread;
    const int TEMPO_SIMULACAO = 60;

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist_chegada(0, 3);
    std::uniform_int_distribution<int> dist_combustivel(1, 20);

    std::cout << "### Simulacao de Trafego Aereo ###" << std::endl;
    std::cout << "Simulacao rodando por " << TEMPO_SIMULACAO << " unidades de tempo." << std::endl << std::endl;

    for (int tempo = 1; tempo <= TEMPO_SIMULACAO; ++tempo) {
        std::cout << "--- Unidade de Tempo: " << tempo << " ---" << std::endl;

        int num_aterrissagens = dist_chegada(rng);
        for (int i = 0; i < num_aterrissagens; ++i) {
            int combustivel = dist_combustivel(rng);
            adicionar_aeronave_aterrissagem(combustivel);
        }
        int num_decolagens = dist_chegada(rng);
        for (int i = 0; i < num_decolagens; ++i) {
            adicionar_aeronave_decolagem();
        }

        prateleira1_pista1.decrementar_combustivel();
        prateleira2_pista1.decrementar_combustivel();
        prateleira1_pista2.decrementar_combustivel();
        prateleira2_pista2.decrementar_combustivel();

        No<Aeronave>* atual = prateleira1_pista1.get_cabeca();
        while (atual != nullptr) { atual->dado.tempo_espera++; atual = atual->proximo; }
        atual = prateleira2_pista1.get_cabeca();
        while (atual != nullptr) { atual->dado.tempo_espera++; atual = atual->proximo; }
        atual = prateleira1_pista2.get_cabeca();
        while (atual != nullptr) { atual->dado.tempo_espera++; atual = atual->proximo; }
        atual = prateleira2_pista2.get_cabeca();
        while (atual != nullptr) { atual->dado.tempo_espera++; atual = atual->proximo; }

        atual = fila_decolagem_pista1.get_cabeca();
        while (atual != nullptr) { atual->dado.tempo_espera++; atual = atual->proximo; }
        atual = fila_decolagem_pista2.get_cabeca();
        while (atual != nullptr) { atual->dado.tempo_espera++; atual = atual->proximo; }
        atual = fila_decolagem_pista3.get_cabeca();
        while (atual != nullptr) { atual->dado.tempo_espera++; atual = atual->proximo; }

        std::vector<int> pistas_disponiveis = {1, 2, 3};

        Aeronave* aeronave_emergencia;
        while (!pistas_disponiveis.empty() && (aeronave_emergencia = encontrar_aeronave_emergencia()) != nullptr) {
            int pista_escolhida = pistas_disponiveis.back();
            pistas_disponiveis.pop_back();

            if (pista_escolhida == 3) {
                Aeronave pousada;
                try {
                    pousada = prateleira1_pista1.remover(aeronave_emergencia->id);
                } catch(...) {
                    try {
                        pousada = prateleira2_pista1.remover(aeronave_emergencia->id);
                    } catch(...) {
                        try {
                            pousada = prateleira1_pista2.remover(aeronave_emergencia->id);
                        } catch(...) {
                            pousada = prateleira2_pista2.remover(aeronave_emergencia->id);
                        }
                    }
                }
                processar_aterrissagem(pousada, pista_escolhida);
                aterrissagens_emergencia++;
            } else {
                Aeronave pousada;
                try {
                    pousada = prateleira1_pista1.remover(aeronave_emergencia->id);
                } catch(...) {
                    try {
                        pousada = prateleira2_pista1.remover(aeronave_emergencia->id);
                    } catch(...) {
                        try {
                            pousada = prateleira1_pista2.remover(aeronave_emergencia->id);
                        } catch(...) {
                            pousada = prateleira2_pista2.remover(aeronave_emergencia->id);
                        }
                    }
                }
                processar_aterrissagem(pousada, pista_escolhida);
                aterrissagens_emergencia++;
            }
        }

        while ((pistas_disponiveis.size() >= 2 || (pistas_disponiveis.size() == 1 && pistas_disponiveis[0] != 3)) && encontrar_aterrissagem_regular() != nullptr) {
            Aeronave* aeronave_pousar = encontrar_aterrissagem_regular();
            int pista_escolhida = (pistas_disponiveis[0] == 3) ? pistas_disponiveis[1] : pistas_disponiveis[0];

            Aeronave pousada;
            try {
                pousada = prateleira1_pista1.remover(aeronave_pousar->id);
            } catch(...) {
                try {
                    pousada = prateleira2_pista1.remover(aeronave_pousar->id);
                } catch(...) {
                    try {
                        pousada = prateleira1_pista2.remover(aeronave_pousar->id);
                    } catch(...) {
                        pousada = prateleira2_pista2.remover(aeronave_pousar->id);
                    }
                }
            }

            processar_aterrissagem(pousada, pista_escolhida);

            for(size_t i = 0; i < pistas_disponiveis.size(); ++i) {
                if(pistas_disponiveis[i] == pista_escolhida) {
                    pistas_disponiveis.erase(pistas_disponiveis.begin() + i);
                    break;
                }
            }
        }

        while (!pistas_disponiveis.empty() && encontrar_decolagem_regular() != nullptr) {
            Aeronave* aeronave_decolar = encontrar_decolagem_regular();
            int pista_escolhida = pistas_disponiveis[0];

            Aeronave decolada;
            try {
                decolada = fila_decolagem_pista1.remover(aeronave_decolar->id);
            } catch(...) {
                try {
                    decolada = fila_decolagem_pista2.remover(aeronave_decolar->id);
                } catch(...) {
                    decolada = fila_decolagem_pista3.remover(aeronave_decolar->id);
                }
            }

            processar_decolagem(decolada, pista_escolhida);
            pistas_disponiveis.erase(pistas_disponiveis.begin());
        }

        if (tempo % 10 == 0) {
            std::cout << "\n--- Relatorio do Tempo " << tempo << " ---" << std::endl;

            prateleira1_pista1.imprimir_fila("Aterrissagem Pista 1 (Prat. 1)", true);
            prateleira2_pista1.imprimir_fila("Aterrissagem Pista 1 (Prat. 2)", true);
            prateleira1_pista2.imprimir_fila("Aterrissagem Pista 2 (Prat. 1)", true);
            prateleira2_pista2.imprimir_fila("Aterrissagem Pista 2 (Prat. 2)", true);
            fila_decolagem_pista1.imprimir_fila("Decolagem Pista 1");
            fila_decolagem_pista2.imprimir_fila("Decolagem Pista 2");
            fila_decolagem_pista3.imprimir_fila("Decolagem Pista 3");

            double media_espera_decolagem = (count_decolagens > 0) ? static_cast<double>(tempo_total_espera_decolagem) / count_decolagens : 0.0;
            double media_espera_aterrissagem = (count_aterrissagens > 0) ? static_cast<double>(tempo_total_espera_aterrissagem) / count_aterrissagens : 0.0;

            std::cout << "\n  - Tempo medio de espera para decolagem: " << media_espera_decolagem << "s" << std::endl;
            std::cout << "  - Tempo medio de espera para aterrissagem: " << media_espera_aterrissagem << "s" << std::endl;
            std::cout << "  - Total de aterrissagens de emergencia: " << aterrissagens_emergencia << std::endl;
            std::cout << "------------------------------------------" << std::endl << std::endl;
        }

        std::cout << std::endl;

        // Atraso de 1 segundo entre cada unidade de tempo
        sleep_for(std::chrono::seconds(1));
    }

    return 0;
}