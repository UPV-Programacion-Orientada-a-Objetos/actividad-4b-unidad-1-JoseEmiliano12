#ifndef SIMULADOR2D_H
#define SIMULADOR2D_H

#include <iostream>

template <typename T>
class Simulador2D {
private:
    // matriz principal del simulador
    T** grid;
    int nfilas;
    int ncols;

    // vector dinamico de fuentes
    T* fuentes;
    int nFuentes;
    int capFuentes;

    // constantes fisicas del sistema
    float K[3]; // [0]=difusion [1]=dx [2]=dt

    // crea una matriz y la llena con ceros
    static T** crearMatriz(int f, int c) {
        if (f < 1) f = 1;
        if (c < 1) c = 1;
        T** m = new T*[f];
        for (int i = 0; i < f; ++i) {
            m[i] = new T[c];
            for (int j = 0; j < c; ++j)
                m[i][j] = T(0);
        }
        return m;
    }

    // libera la memoria usada por la matriz
    static void destruirMatriz(T** m, int f) {
        if (!m) return;
        for (int i = 0; i < f; ++i)
            delete [] m[i];
        delete [] m;
    }

    // copia lo que se pueda entre dos matrices
    static void copiarMin(T** dst, int fd, int cd, T** src, int fs, int cs) {
        int fmin = (fd < fs) ? fd : fs;
        int cmin = (cd < cs) ? cd : cs;
        for (int i = 0; i < fmin; ++i)
            for (int j = 0; j < cmin; ++j)
                dst[i][j] = src[i][j];
    }

    // duplica el tamaño del vector de fuentes si se llena
    void ampliarFuentes() {
        int nuevaCap = (capFuentes == 0) ? 2 : capFuentes * 2;
        T* nuevo = new T[nuevaCap];
        for (int i = 0; i < nFuentes; ++i)
            nuevo[i] = fuentes[i];
        delete [] fuentes;
        fuentes = nuevo;
        capFuentes = nuevaCap;
    }

public:
    // constructor
    Simulador2D(int f, int c)
        : grid(nullptr), nfilas(f), ncols(c),
          fuentes(nullptr), nFuentes(0), capFuentes(0) {
        if (nfilas < 1) nfilas = 1;
        if (ncols < 1) ncols = 1;
        grid = crearMatriz(nfilas, ncols);

        // valores por defecto de las constantes
        K[0] = 1.0f;
        K[1] = 1.0f;
        K[2] = 1.0f;
    }

    // destructor
    ~Simulador2D() {
        destruirMatriz(grid, nfilas);
        delete [] fuentes;
        fuentes = nullptr;
        nFuentes = 0;
        capFuentes = 0;
    }

    // constructor
    Simulador2D(const Simulador2D& o)
        : grid(nullptr), nfilas(o.nfilas), ncols(o.ncols),
          fuentes(nullptr), nFuentes(o.nFuentes), capFuentes(o.capFuentes) {
        grid = crearMatriz(nfilas, ncols);
        copiarMin(grid, nfilas, ncols, o.grid, o.nfilas, o.ncols);

        if (capFuentes > 0) {
            fuentes = new T[capFuentes];
            for (int i = 0; i < nFuentes; ++i)
                fuentes[i] = o.fuentes[i];
        }

        K[0] = o.K[0];
        K[1] = o.K[1];
        K[2] = o.K[2];
    }

    // operador de asignacion
    Simulador2D& operator=(const Simulador2D& o) {
        if (this == &o) return *this;

        destruirMatriz(grid, nfilas);
        delete [] fuentes;

        nfilas = o.nfilas;
        ncols = o.ncols;
        nFuentes = o.nFuentes;
        capFuentes = o.capFuentes;

        grid = crearMatriz(nfilas, ncols);
        copiarMin(grid, nfilas, ncols, o.grid, o.nfilas, o.ncols);

        if (capFuentes > 0) {
            fuentes = new T[capFuentes];
            for (int i = 0; i < nFuentes; ++i)
                fuentes[i] = o.fuentes[i];
        } else {
            fuentes = nullptr;
        }

        K[0] = o.K[0];
        K[1] = o.K[1];
        K[2] = o.K[2];
        return *this;
    }

    // cambia el tamaño del grid y conserva los valores
    void redimensionarGrid(int nuevaF, int nuevaC) {
        if (nuevaF < 1) nuevaF = 1;
        if (nuevaC < 1) nuevaC = 1;

        T** nuevo = crearMatriz(nuevaF, nuevaC);
        copiarMin(nuevo, nuevaF, nuevaC, grid, nfilas, ncols);
        destruirMatriz(grid, nfilas);
        grid = nuevo;
        nfilas = nuevaF;
        ncols = nuevaC;
    }

    // agrega una nueva fuente al vector
    void agregarFuente(T valor) {
        if (nFuentes == capFuentes)
            ampliarFuentes();
        fuentes[nFuentes++] = valor;
    }

    // simula un paso promediando 
    void simularPaso() {
        T** temp = crearMatriz(nfilas, ncols);

        for (int j = 0; j < ncols; ++j) {
            temp[0][j] = grid[0][j];
            temp[nfilas - 1][j] = grid[nfilas - 1][j];
        }
        for (int i = 0; i < nfilas; ++i) {
            temp[i][0] = grid[i][0];
            temp[i][ncols - 1] = grid[i][ncols - 1];
        }

        // cada celda interna toma el promedio de los cuatro 
        for (int i = 1; i < nfilas - 1; ++i) {
            for (int j = 1; j < ncols - 1; ++j) {
                T norte = grid[i - 1][j];
                T sur = grid[i + 1][j];
                T oeste = grid[i][j - 1];
                T este = grid[i][j + 1];
                temp[i][j] = (norte + sur + oeste + este) / T(4);
            }
        }

        // se pasa el resultado al grid principal
        for (int i = 0; i < nfilas; ++i)
            for (int j = 0; j < ncols; ++j)
                grid[i][j] = temp[i][j];

        destruirMatriz(temp, nfilas);
    }

    // cambia las constantes
    void setConstantes(float dif, float dx, float dt) {
        K[0] = dif;
        K[1] = dx;
        K[2] = dt;
    }

    // pone un valor en una posicion del grid
    void ponerEn(int i, int j, T v) {
        if (i < 0 || i >= nfilas || j < 0 || j >= ncols) return;
        grid[i][j] = v;
    }

    // lee un valor de una posicion del grid
    T leerEn(int i, int j) const {
        if (i < 0 || i >= nfilas || j < 0 || j >= ncols)
            return T(0);
        return grid[i][j];
    }

    // getters
    int filas() const { return nfilas; }
    int columnas() const { return ncols; }
};

#endif
