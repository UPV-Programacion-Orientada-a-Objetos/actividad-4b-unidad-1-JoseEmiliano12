#include <iostream>
#include <iomanip>
#include "Simulador2D.h"

template <typename T>
void imprimirFormato(const Simulador2D<T>& sim) {
    std::cout << std::fixed << std::setprecision(1);
    for (int i = 0; i < sim.filas(); ++i) {
        std::cout << "| ";
        for (int j = 0; j < sim.columnas(); ++j) {
            std::cout << std::setw(5) << sim.leerEn(i, j) << " | ";
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "--- Simulador Genérico de Fluidos (Difusión) ---\n\n";

    std::cout << ">> Inicializando Sistema (Tipo FLOAT) <<\n";
    Simulador2D<float> simF(5, 5);
    std::cout << "Creando Grid (FLOAT) de 5x5...\n\n";

    simF.setConstantes(1.0f, 1.0f, 1.0f);

    std::cout << ">> Agregando Fuentes de Concentración <<\n";
    simF.agregarFuente(100.0f);
    std::cout << "Agregar Fuente 1 (Valor 100.0): Éxito.\n";
    simF.agregarFuente(50.0f);
    std::cout << "Agregar Fuente 2 (Valor 50.0): Éxito.\n\n";

    std::cout << ">> Aplicando Valores Iniciales <<\n";
    simF.ponerEn(2, 2, 100.0f);
    simF.ponerEn(4, 0, 50.0f);
    std::cout << "Grid[2,2] = 100.0 (Fuente 1)\n";
    std::cout << "Grid[4,0] = 50.0 (Fuente 2)\n\n";

    std::cout << "--- Grid Inicial (Paso 0) ---\n";
    imprimirFormato(simF);

    std::cout << "\nOpción: Simular 1 Paso\n\n";
    std::cout << "Simulando Propagación...\n\n";
    simF.simularPaso();

    std::cout << "--- Grid Después del Paso 1 ---\n";
    imprimirFormato(simF);
    std::cout << "\n";

    std::cout << "Opción: Redimensionar\n";
    simF.redimensionarGrid(6, 6);
    std::cout << "Redimensionando Grid a 6x6. Datos copiados.\n\n";
    imprimirFormato(simF);
    std::cout << "\n";

    std::cout << "Opción: Salir\n";
    std::cout << "Liberando memoria del Grid y Fuentes... Sistema cerrado.\n";

    return 0;
}
